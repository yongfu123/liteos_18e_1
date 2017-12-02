/******************************************************************************
  Some simple Hisilicon Hi35xx system functions.

  Copyright (C), 2010-2015, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2015-6 Created
******************************************************************************/
#include "sys/types.h"

#include "sys/time.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/statfs.h"
#include "limits.h"

#include "los_event.h"
#include "los_printf.h"

#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "eth_drv.h"
#include "arch/perf.h"

#include "fcntl.h"
#include "fs/fs.h"

#include "stdio.h"

#include "shell.h"
#include "hisoc/uart.h"
#include "vfs_config.h"
#include "disk.h"

#include "los_cppsupport.h"

#include "linux/fb.h"
#include "adaptsecure.h"

extern int mem_dev_register(void);
extern UINT32 osShellInit();
extern void CatLogShell();
extern void hisi_eth_init(void);
//extern void tools_cmd_register(void);
extern void proc_fs_init(void);
extern int uart_dev_init(void);
extern int system_console_init(const char *);
extern int nand_init(void);
extern int add_mtd_partition( char *, UINT32 , UINT32 , UINT32 );
extern int spinor_init(void);
extern int spi_dev_init(void);
extern int i2c_dev_init(void);
extern int gpio_dev_init(void);
extern int dmac_init(void);
extern int ran_dev_register(void);

int secure_func_register(void)
{
    int ret;
    STlwIPSecFuncSsp stlwIPSspCbk= {0};
    stlwIPSspCbk.pfMemset_s = Stub_MemSet;
    stlwIPSspCbk.pfMemcpy_s = Stub_MemCpy;
    stlwIPSspCbk.pfStrNCpy_s = Stub_StrnCpy;
    stlwIPSspCbk.pfStrNCat_s = Stub_StrnCat;
    stlwIPSspCbk.pfStrCat_s = Stub_StrCat;
    stlwIPSspCbk.pfMemMove_s = Stub_MemMove;
    stlwIPSspCbk.pfSnprintf_s = Stub_Snprintf;
    stlwIPSspCbk.pfRand = rand;
    ret = lwIPRegSecSspCbk(&stlwIPSspCbk);
    if (ret != 0)
    {
        PRINT_ERR("\n***lwIPRegSecSspCbk Failed***\n");
        return -1;
    }

    PRINTK("\nCalling lwIPRegSecSspCbk\n");
    return ret;
}

extern UINT32 osShellInit();

struct netif *pnetif;

//define, max args ---20
#define ARGS_SIZE_T 20
#define ARG_BUF_LEN_T 256
static char *ptask_args[ARGS_SIZE_T];
static char *args_buf_t = NULL;

static int taskid = -1;

static char *s_testname = "rtc_test";

void com_app(unsigned int p0, unsigned int p1, unsigned int p2, unsigned int p3)
{
    int i = 0;
    unsigned int argc = p0;
    char **argv = (char **)p1;
    //Set_Interupt(0);
    dprintf("\ninput command:\n");
    for(i=0; i<argc; i++) {
    dprintf("%s ", argv[i]);
    }
    dprintf("\n");
    app_main(argc,argv);
    dprintf("\nmain out\n");
    dprintf("[END]:app_test finish!\n");
    free(args_buf_t);
    args_buf_t = NULL;
    taskid = -1;
}


void app_test(int argc, char **argv )
{
    int i = 0, ret = 0;
    int len = 0;
    char *pch = NULL;
    TSK_INIT_PARAM_S stappTask;

    if(argc < 1) {
    dprintf("illegal parameter!\n");
    }

    if (taskid != -1) {
    dprintf("There's a app_main task existed.");
    }
    args_buf_t = zalloc(ARG_BUF_LEN_T);
    memset(&stappTask, 0, sizeof(TSK_INIT_PARAM_S));
    pch = args_buf_t;
    for(i=0; i<ARGS_SIZE_T; i++) {
    ptask_args[i] = NULL;
    }
    argc++;
    ptask_args[0] = s_testname;

    for(i = 1; i < argc; i++)
    {
    len = strlen(argv[i-1]);
    memcpy(pch , argv[i-1], len);
    ptask_args[i] = pch;
    //keep a '\0' at the end of a string.
    pch = pch + len + 1;
    if (pch >= args_buf_t +ARG_BUF_LEN_T) {
    dprintf("args out of range!\n");
    break;
    }
    }
    memset(&stappTask, 0, sizeof(TSK_INIT_PARAM_S));
    stappTask.pfnTaskEntry = (TSK_ENTRY_FUNC)com_app;
    stappTask.uwStackSize = 0x10000;
    stappTask.pcName = s_testname;
    stappTask.usTaskPrio = 10;
    stappTask.uwResved = LOS_TASK_STATUS_DETACHED;
    stappTask.auwArgs[0] = argc;
    stappTask.auwArgs[1] = (UINT32)ptask_args;
    ret = LOS_TaskCreate(&taskid, &stappTask);

    dprintf("camera_Task %d\n", taskid);

    //chdir("/sd0");
    chdir("/nfs");

}

void sample_command(void) 
{
    osCmdReg(CMD_TYPE_EX,s_testname, 0, (CMD_CBK_FUNC)app_test);
}

void net_init()
{
    tcpip_init(NULL, NULL);
#ifdef LOSCFG_DRIVERS_HIGMAC
    extern struct los_eth_driver higmac_drv_sc;
    pnetif = &(higmac_drv_sc.ac_if);
    higmac_init();
#endif

#ifdef LOSCFG_DRIVERS_HIETH_SF
    extern struct los_eth_driver hisi_eth_drv_sc;
    pnetif = &(hisi_eth_drv_sc.ac_if);
    hisi_eth_init();
#endif

    dprintf("cmd_startnetwork : DHCP_BOUND finished\n");

    netif_set_up(pnetif);
}

extern unsigned int g_uwFatSectorsPerBlock;
extern unsigned int g_uwFatBlockNums;
#define SUPPORT_FMASS_PARITION
#ifdef SUPPORT_FMASS_PARITION
extern int fmass_register_notify(void(*notify)(void* context, int status), void* context);
extern int fmass_partition_startup(char* path);
void fmass_app_notify(void* conext, int status)
{
    if(status == 1)/*usb device connect*/
    {
        char *path = "/dev/mmcblk0p0";
        //startup fmass access patition
        fmass_partition_startup(path);
    }
}
#endif

#include "board.h"
extern UINT32 g_sys_mem_addr_end;
extern unsigned int g_uart_fputc_en;
void board_config(void)
{
    g_sys_mem_addr_end = SYS_MEM_BASE + SYS_MEM_SIZE_DEFAULT;
    g_uwSysClock = OS_SYS_CLOCK;
    g_uart_fputc_en = 1;
    extern unsigned long g_usb_mem_addr_start;
    extern unsigned long g_usb_mem_size;
    g_usb_mem_addr_start = g_sys_mem_addr_end;
    g_usb_mem_size = USB_MEM_SIZE;

    g_uwFatSectorsPerBlock = CONFIG_FS_FAT_SECTOR_PER_BLOCK;
    g_uwFatBlockNums       = CONFIG_FS_FAT_BLOCK_NUMS;



}

void app_init(void)
{
    UINT32 uwRet;

    dprintf("uart init ...\n");
    uart_dev_init();

    dprintf("spi bus init ...\n");
    spi_dev_init();
    
    dprintf("dmac init\n");
    dmac_init();

    dprintf("i2c bus init ...\n");
    i2c_dev_init();
    
    dprintf("random dev init ...\n");
    ran_dev_register();
	
    dprintf("mem dev init ...\n");
	mem_dev_register();
	
	dprintf("porc fs init ...\n");
    proc_fs_init();

    dprintf("cxx init ...\n");
    extern char __init_array_start__, __init_array_end__;
    LOS_CppSystemInit((unsigned long)&__init_array_start__, (unsigned long)&__init_array_end__, NO_SCATTER);


    dprintf("nand init ...\n");
    if(!nand_init()) {
        add_mtd_partition("nand", 0x200000, 32*0x100000, 0);
        add_mtd_partition("nand", 0x200000 + 32*0x100000, 32*0x100000, 1);
        mount("/dev/nandblk0", "/yaffs0", "yaffs", 0, NULL);
        //mount("/dev/nandblk1", "/yaffs1", "yaffs", 0, NULL);
    }

    dprintf("spi nor flash init ...\n");
    if(!spinor_init()){
        add_mtd_partition("spinor", 0x100000, 2*0x100000, 0);
        add_mtd_partition("spinor", 3*0x100000, 2*0x100000, 1);
#ifndef HI3559
        mount("/dev/spinorblk0", "/jffs0", "jffs", 0, NULL);
#endif
        //mount("/dev/spinorblk1", "/jffs1", "jffs", 0, NULL);
    }
    
    dprintf("gpio init ...\n");
    gpio_dev_init();

    dprintf("net init ...\n");
    (void)secure_func_register();
    net_init();

    dprintf("shell init ...\n");
    system_console_init(TTY_DEVICE);
    osShellInit(TTY_DEVICE);

    dprintf("g_sys_mem_addr_end=0x%08x,\n",g_sys_mem_addr_end);
    dprintf("done init!\n");
    dprintf("Date:%s.\n", __DATE__);
    dprintf("Time:%s.\n", __TIME__);

    //tools_cmd_register();
    SDK_init();
    sample_command();

    printf("###### AUTOTEST_RUN running #######\n");
    return;
}

/* EOF kthread1.c */
