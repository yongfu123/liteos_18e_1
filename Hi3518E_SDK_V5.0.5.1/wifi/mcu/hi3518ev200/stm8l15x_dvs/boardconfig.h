#ifndef __BOARDCONFIG_H
#define __BOARDCONFIG_H

#include "stm8l15x.h"
#include "rtc.h"
#include "adc.h"
#include "halt.h"


//SWIM 烧入IO   PA0
#define SWIM_GPIO           GPIOA
#define SWIM_GPIO_Pin               GPIO_Pin_0

//串口接收IO  PC6
#define USART_RX_GPIO           GPIOC
#define USART_RX_GPIO_Pin       GPIO_Pin_6

//串口发送IO  PC5
#define USART_TX_GPIO               GPIOC
#define USART_TX_GPIO_Pin       GPIO_Pin_5

//开机键PC1
#define SW_KEY_GPIO         GPIOC
#define SW_KEY_GPIO_Pin             GPIO_Pin_1

//主控设置wifi寄存器标志IO  PA2
#define REG_ON_DET_GPIO         GPIOA
#define REG_ON_DET_GPIO_Pin     GPIO_Pin_2

//wifi唤醒标志位IO   PA3
#define MCU_GPIO2_GPIO          GPIOA
#define MCU_GPIO2_GPIO_Pin      GPIO_Pin_3

//未发现使用  PC4
#define MCU_GPIO3_GPIO          GPIOC
#define MCU_GPIO3_GPIO_Pin      GPIO_Pin_4

#define PWR_EN_GPIO         GPIOB
#define PWR_EN_GPIO_Pin             GPIO_Pin_0

#define PWR_HOLD_GPIO           GPIOD
#define PWR_HOLD_GPIO_Pin           GPIO_Pin_0

#define BAT_LOW_DET_GPIO        GPIOB
#define BAT_LOW_DET_GPIO_Pin            GPIO_Pin_2

#define BT_HOST_WAKE_GPIO       GPIOB
#define BT_HOST_WAKE_GPIO_Pin           GPIO_Pin_3

#define BT_REG_ON_GPIO          GPIOB
#define BT_REG_ON_GPIO_Pin          GPIO_Pin_4

#define WIFI_REG_ON_GPIO        GPIOB
#define WIFI_REG_ON_GPIO_Pin            GPIO_Pin_5

#define WIFI_GPIO2_GPIO         GPIOB
#define WIFI_GPIO2_GPIO_Pin         GPIO_Pin_6

//#define   WIFI_GPIO1_GPIO         GPIOB
//#define   WIFI_GPIO1_GPIO_Pin         GPIO_Pin_7
#define WIFI_GPIO1_GPIO         GPIOC
#define WIFI_GPIO1_GPIO_Pin         GPIO_Pin_0

#define VBUS_DETECT         GPIOB
#define VBUS_DETECT_GPIO_Pin            GPIO_Pin_1

#define ADCIN_GPIO          GPIOB
#define ADCIN_GPIO_Pin                  GPIO_Pin_0

//#define   PIR_GPIO            GPIOC
//#define   PIR_GPIO_Pin                    GPIO_Pin_0

#define PIR_GPIO            GPIOB
#define PIR_GPIO_Pin                    GPIO_Pin_0

#define KEY_OUT_GPIO       GPIOB
#define KEY_OUT_GPIO_Pin                GPIO_Pin_7


//PB6
#define WL_EN_GPIO          GPIOB
#define WL_EN_GPIO_Pin          GPIO_Pin_6

//PB5
#define DEV_PWR_GPIO                GPIOB
#define DEV_PWR_GPIO_Pin            GPIO_Pin_5

//PC0
#define D2H_WAK_GPIO                GPIOC
#define D2H_WAK_GPIO_Pin            GPIO_Pin_0

#define MCU_GPIO1_GPIO          GPIOA
#define MCU_GPIO1_GPIO_Pin      GPIO_Pin_2

//此前预留于电池电量
#define	VBAT_DETECT				GPIOD
#define VBAT_DETECT_GPIO_Pin	GPIO_Pin_3


/* KEY */
#define KEY_VALID_LEVEL         0
#define KEY_CHECK_CYCLE         5
#define KEY_JITTER_CHECK_CYCLE  5
#define PWR_ON_KEY_KEEP_COUNT   500 //unit is 4ms
#define PWR_OFF_KEY_KEEP_COUNT  300

/* VBUS and VBAT */
#define VBUS_VALID_LEVEL        1
#define VBAT_VALID_LEVEL        1

#define UF_START 0
#define UF_LEN 1
#define UF_CMD 2
#define UF_DATA 3

#define UF_MAX_LEN 64 //此数需2的n次方，以便后续与操作等效取余操作，以优化指令，提高性能

#define GPIO_DEBOUNCE 2
#define MIN_CMD_LEN 4
#define CMD_LEN 8
#define CMDFLAG_START 0x7B
#define ENABLE_RTC 0  //是否编译RTC定时器

extern u8 g_IsSystemOn;
extern u8 g_Key_Handle_Flag;

extern u8 g_WakeUp_Halt_Flag ; //唤醒标志
extern u8 g_Wifi_WakeUp ;  //WIFI唤醒标志
extern u8 g_Bar_Det_Flag;
extern u8 g_Vbus_Check;


extern u8 USART_Send_Buf[UF_MAX_LEN];
extern u8 USART_Receive_Buf[UF_MAX_LEN];
extern u32 USART_Receive_Flag;
extern u16 USART_Receive_Timeout;

extern u16 g_WakeUp_Key_Flag;  //halt模式延时开机计数


extern void InitExitHaltMode(void);

void Board_Init(void);
void CLK_Config(void);
void TIM3_Config(void);
void Usart_Config(void);
void PWR_ON(void);
void PWR_OFF(void);
void USART_Send_Data(unsigned char *Dbuf,unsigned int len);
u8  XOR_Inverted_Check(unsigned char *inBuf,unsigned char inLen);
void Response_CMD_Handle();
void Request_CMD_Handle(u8* cmdstr, u8 len);
void Uart_Handle(void);
void Key_Handle(void);
#endif /* __BOARDCONFIG_H */
