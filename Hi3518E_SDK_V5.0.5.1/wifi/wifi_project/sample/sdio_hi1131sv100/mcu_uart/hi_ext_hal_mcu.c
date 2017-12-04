
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>
#include "hisoc/uart.h"
#include "hi_type.h"
#include "hi_ext_hal_mcu.h"
#include "asm/delay.h"



#define CHECK_STATE_FLAG(flag, state)\
    do{\
        if(flag != state)\
        {\
            printf("func:%s,line:%d,flag statue fail\n",__FUNCTION__,__LINE__);\
            return HI_FAILURE;\
        }\
    }while(0)

#define CHECK_NULL_PTR(ptr)\
    do{\
        if(NULL == ptr)\
        {\
            printf("func:%s,line:%d,NULL pointer\n",__FUNCTION__,__LINE__);\
            return HI_FAILURE;\
        }\
    }while(0)


HI_BOOL g_bMcuHostInit = HI_FALSE;
HI_BOOL g_bMcuHostState = HI_FALSE;
HI_BOOL g_bMcuHostEnableState = HI_FALSE;
static HI_BOOL s_bMcuHostRegister = HI_FALSE;
static PFN_MCUHOST_NotifyProc s_pfnMcuHostNotifyProc = NULL;
static HI_S32 g_stResponse = 0;
static HI_S32 g_s32WIFIRegDetResponse = 0;
pthread_mutex_t g_ResponseLock;
pthread_mutex_t g_WIFIRegDetResponseLock;
pthread_t g_tMcuHostDetect;
static pthread_mutex_t s_tMcuHostMutex;

HI_S32 mcu_fd;
extern int uart2_fd;
unsigned char power_handle_flag = 0;



#define UF_START 0
#define UF_LEN 1
#define UF_CMD 2
#define UF_DATA 3
#define UF_MAX_LEN 50




#define UART2_BUF_LEN  100

static char uart2_buffer[UART2_BUF_LEN];

#if 0
void debug_dump(char* buffer, HI_U16 length, char* pDescription)
{
    char stream[60];
    char byteOffsetStr[10];
    HI_U32 i;
    HI_U16 offset, count, byteOffset;

    printf("<-----------Dumping %d Bytes : %s -------->\n", length, pDescription);
    //A_PRINTF("%s:%d = \n",  pDescription, length);
    count = 0;
    offset = 0;
    byteOffset = 0;

    for (i = 0; i < length; i++)
    {
        sprintf(stream + offset, "%2.2X ", buffer[i]);
        count ++;
        offset += 3;

        if (count == 16)
        {
            count = 0;
            offset = 0;
            sprintf(byteOffsetStr, "%4.4X", byteOffset);
            printf("[%s]: %s\n", byteOffsetStr, stream);
            memset(stream, 0, 60);
            byteOffset += 16;
        }
    }

    if (offset != 0)
    {
        sprintf(byteOffsetStr, "%4.4X", byteOffset);
        printf("[%s]: %s\n", byteOffsetStr, stream);
    }

    printf("<------------------------------------------------->\n");
}
#else
void debug_dump(char* buffer, HI_U16 length, char* pDescription)
{

}
#endif
unsigned char  XOR_Inverted_Check(unsigned char* inBuf, unsigned char inLen)
{
    unsigned char check = 0, i;

    for (i = 0; i < inLen; i++)
    { check ^= inBuf[i]; }

    return ~check;
}


int USART_Send_Data(HI_S32 fd, HI_U8* sbuf)
{
    int ret, i;

    sbuf[UF_START] = 0x7B;
    sbuf[sbuf[UF_LEN] - 1] = XOR_Inverted_Check(sbuf, sbuf[UF_LEN] - 1);

    //防止mcu丢失host发过来的消息。300us为mcu处理一次接受usart消息中断的最长时间
    for (i = 0; i < sbuf[UF_LEN]; i++){
        ret = write(fd, &sbuf[i], 1);
        udelay(300);
        if (ret != 1)
        {
            printf("write %d fd return %d\n", fd, ret);
            return -1;
        }
    }

    printf("uart send ok!\n");
    return 0;
}



static HI_S32 USART_Receive_Data(HI_S32 fd, HI_U8* rbuf, HI_U8 rbuf_max_len)
{
    HI_S32 ret;
    HI_U8 len, i;
    HI_U32 timeout_cnt;
    HI_U8 bcc;
    //fd_set fs_read;
    struct timeval time;
    HI_S32 fs_sel;

    len = 0;
    timeout_cnt = 0;

    while (((len < 2) || (len < rbuf[UF_LEN])))
    {
        ret = read(fd, rbuf + len, UF_MAX_LEN);

        if (ret < -1)
        {
            perror("receive error!");
            printf("read ret = %d\n", ret);
            return (HI_FAILURE);
        }
        else if (ret == 0)
        {
            usleep(50000);
        }
        else
        {
            len += ret;
        }

        timeout_cnt++;

        if (timeout_cnt > 500)
        {
           // printf("receive timeout!\n");
            return (HI_FAILURE);
        }
    }

    printf("receive buf:");

    for (i = 0; i < len; i++)
    { printf(" %02X ", rbuf[i]); }

    printf("\n");
    bcc = XOR_Inverted_Check(rbuf, rbuf[UF_LEN] - 1);

    if ((rbuf[UF_START] != 0X7B) || (rbuf[rbuf[UF_LEN] - 1] != bcc))
    {
        printf("uart bcc err\n");
        return -1;
    }

    return (HI_SUCCESS);
}






HI_S32 HI_HAL_MCUHOST_Set_ShutDown_Interval(HI_U32 u32Interval)
{
    HI_U32 u32hour, u32min;

    uart2_buffer[UF_LEN] = 0x4 + 0x3;
    uart2_buffer[UF_CMD] = 0x86;

    if (u32Interval > 0)
    {
        uart2_buffer[UF_DATA] = 0x1;
    }
    else
    {
        uart2_buffer[UF_DATA] = 0x0;
    }

    u32hour = u32Interval / 60;
    u32min = u32Interval % 60;
    uart2_buffer[UF_DATA + 1] = u32hour;
    uart2_buffer[UF_DATA + 2] = u32min;
    USART_Send_Data(mcu_fd, uart2_buffer);



    while (1)
    {
        pthread_mutex_lock(&g_ResponseLock);

        if (-1 == g_stResponse)
        {
            return HI_FAILURE;
        }
        else if (1 == g_stResponse)
        {
            return HI_SUCCESS;
        }

        pthread_mutex_unlock(&g_ResponseLock);
        usleep(10000);
    }

    return HI_SUCCESS;
}

static void Uart_CMD_Handle(int fd, unsigned char* buf, unsigned char buf_max_len)
{
    unsigned char check = 0;
    int ret;

    ret = USART_Receive_Data(fd, buf, buf_max_len);

    if (ret == HI_FAILURE)
    {
        //printf("mcu host get frame fail");
        return;
    }

    //printf("\nUart_CMD_Handle  buf[UF_CMD]: 0x%x\n", buf[UF_CMD]);

    switch (buf[UF_CMD])
    {
        case 0x00:
            printf("poweroff!\n");
#if 0
            system("himm 0x20220400 0x40");
            system("himm 0x20220100 0x40");
#else
            //system("poweroff");
            printf("\n#LPrint file: %s func: %s line: %d ticket: %d\n", __FILE__, __FUNCTION__, __LINE__, LOS_TickCountGet());
            ret = s_pfnMcuHostNotifyProc(MCUHOST_SYSTEMCLOSE);

            if (ret == HI_FAILURE)
            {
                return;
            }

#endif
            break;

        case 0x81:
            printf("receive poweroff response cmd!\n");

#if 0
            system("himm 0x20220400 0x40");
            system("himm 0x20220100 0x40");

#else
            power_handle_flag = 0;
            printf("\n#LPrint file: %s func: %s line: %d ticket: %d\n", __FILE__, __FUNCTION__, __LINE__, LOS_TickCountGet());
            ret = s_pfnMcuHostNotifyProc(MCUHOST_SYSTEMCLOSE);

            if (ret == HI_FAILURE)
            {return;}

#endif
            break;

        case 0x87:
        {
            pthread_mutex_lock(&g_ResponseLock);

            if (0 == buf[UF_DATA])
            {
                g_stResponse = 1;
            }
            else
            {
                g_stResponse = -1;
            }

            pthread_mutex_unlock(&g_ResponseLock);
            break;
        }

        case 0x85:
        {
            pthread_mutex_lock(&g_WIFIRegDetResponseLock);
            g_s32WIFIRegDetResponse = 1;
            printf("receive back of set reg\r\n");
            pthread_mutex_unlock(&g_WIFIRegDetResponseLock);
            break;
        }

        default:
            break;
    }
}


HI_VOID* MCUHOST_Thread(HI_VOID* pPara)
{
    HI_S32 s32Ret;
    prctl(PR_SET_NAME, (HI_SIZE_T)"MCUHOST_Thread", 0, 0, 0);

    while (g_bMcuHostState)
    {
        if (HI_TRUE != g_bMcuHostEnableState)
        {
            usleep(500 * 1000);
            continue;
        }

        Uart_CMD_Handle(mcu_fd, uart2_buffer, UF_MAX_LEN);
    }

    return NULL;
}



HI_S32 HI_HAL_MCUHOST_Init()
{
    HI_S32 s32Ret;

    CHECK_STATE_FLAG(g_bMcuHostInit, HI_FALSE);

    mcu_fd = uart2_fd;

    pthread_mutex_init(&s_tMcuHostMutex, NULL);

    s32Ret = pthread_create(&g_tMcuHostDetect, 0, MCUHOST_Thread, NULL);

    if (s32Ret)
    {
        printf("mcu host create thread fail.\n");
        return s32Ret;
    }


    g_bMcuHostInit = HI_TRUE;
    g_bMcuHostState = HI_TRUE;
    g_bMcuHostEnableState = HI_TRUE;
    pthread_mutex_init(&g_ResponseLock, NULL);
    pthread_mutex_init(&g_WIFIRegDetResponseLock, NULL);
    //s_pfnUsbNotifyProc = NULL;
    printf("mcu host init ok \n");
    return HI_SUCCESS;
}

HI_S32 HI_HAL_MCUHOST_Deinit()
{
    HI_S32 s32Ret;

    if (g_bMcuHostInit == HI_TRUE)
    {
        g_bMcuHostState = HI_FALSE;
        pthread_mutex_lock(&s_tMcuHostMutex);
        s32Ret = pthread_join(g_tMcuHostDetect, NULL);

        if (s32Ret != 0)
        {
            printf("g_tMcuHostDetect: pthread_join failed\n");
            return s32Ret;
        }

        pthread_mutex_unlock(&s_tMcuHostMutex);
        pthread_mutex_destroy(&s_tMcuHostMutex);
        memset(&s_tMcuHostMutex, 0, sizeof(s_tMcuHostMutex));

        g_bMcuHostEnableState = HI_FALSE;


        g_bMcuHostInit = HI_FALSE;
    }

    return HI_SUCCESS;
}


HI_S32 HI_HAL_MCUHOST_RegisterNotifyProc(PFN_MCUHOST_NotifyProc pfnNotifyProc)
{
    CHECK_STATE_FLAG(g_bMcuHostInit, HI_TRUE);
    CHECK_STATE_FLAG(s_bMcuHostRegister, HI_FALSE);
    CHECK_NULL_PTR(pfnNotifyProc);
    s_pfnMcuHostNotifyProc = pfnNotifyProc;
    s_bMcuHostRegister = HI_TRUE;
    return HI_SUCCESS;
}



HI_S32 HI_HAL_MCUHOST_RegOn_Control(HI_CHAR u8enable)
{
    HI_U32 u32waitcount = 20;

    if ((u8enable != 0) && (u8enable != 1))
    {
        printf("intput err\r\n");
    }


    uart2_buffer[UF_START] = 0x7b;
    uart2_buffer[UF_LEN] = 0x5;
    uart2_buffer[UF_CMD] = 0x84;
    uart2_buffer[UF_DATA] = u8enable;
    debug_dump(uart2_buffer, 4, "send Wifi_Reg_Det_Control cmd");
    USART_Send_Data(uart2_fd, uart2_buffer);

    do
    {
        usleep(100 * 1000);
    }
    while (u32waitcount-- && (g_s32WIFIRegDetResponse == 0));

    printf("\nHI_Product_Wifi_Reg_Det_Control  g_s32WIFIRegDetResponse: %d u32waitcount: %d\n", g_s32WIFIRegDetResponse, u32waitcount);
    pthread_mutex_lock(&g_WIFIRegDetResponseLock);
    g_s32WIFIRegDetResponse = 0;
    pthread_mutex_unlock(&g_WIFIRegDetResponseLock);
    return HI_SUCCESS;

}


HI_S32 HI_HAL_MCUHOST_PowreOff_Request()
{

    if (power_handle_flag)
    {
		printf("");
        return HI_FAILURE;
    }
	printf("%s------------shutdown-----------------\n",__FUNCTION__);

    power_handle_flag = 1;

    uart2_buffer[UF_START] = 0x7b;
    uart2_buffer[UF_LEN] = 0x4;
    uart2_buffer[UF_CMD] = 0x80;
    debug_dump(uart2_buffer, 4, "send power off cmd");
    USART_Send_Data(mcu_fd, uart2_buffer);

    return HI_SUCCESS;
}


int HI_HAL_MCUHOST_WiFi_ON_Request(int On)
{
    char s_u8buffer[UF_MAX_LEN];
    memset(s_u8buffer, 0, UF_MAX_LEN);
    s_u8buffer[UF_LEN] = 0x5;
    s_u8buffer[UF_CMD] = 0x84;
    s_u8buffer[UF_DATA] = On;
    USART_Send_Data(uart2_fd, s_u8buffer);

}

void HI_HAL_MCUHOST_WiFi_Clr_Flag(void)
{
    uart2_buffer[UF_LEN] = 0x4;
    uart2_buffer[UF_CMD] = 0x8c;
    USART_Send_Data(mcu_fd,uart2_buffer);
}
void HI_HAL_MCUHOST_WiFi_Power_Set(unsigned char val)
{
    uart2_buffer[UF_LEN] = 0x5;
    uart2_buffer[UF_CMD] = 0x8a;
    uart2_buffer[UF_DATA] = val;
    USART_Send_Data(mcu_fd,uart2_buffer);
}

void HI_HAL_MCUHOST_WiFi_Rst_Set(unsigned char val)
{
    uart2_buffer[UF_LEN] = 0x5;
    uart2_buffer[UF_CMD] = 0x88;
    uart2_buffer[UF_DATA] = val;
    USART_Send_Data(mcu_fd,uart2_buffer);
}

