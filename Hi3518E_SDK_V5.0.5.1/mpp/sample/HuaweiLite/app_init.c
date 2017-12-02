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

#include "los_event.h"
#include "los_printf.h"

#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "eth_drv.h"
#include "arch/perf.h"

#include "fcntl.h"
#include "shell.h"
#include "stdio.h"
#include "hisoc/uart.h"

#include "fs/fs.h"
#include "shell.h"
#include "vfs_config.h"
#include "disk.h"
#include "los_cppsupport.h"

//#include "higmac.h"
#include "proc_fs.h"
#include "console.h"
#include "hirandom.h"
#include "nand.h"
#include "spi.h"
#include "dmac_ext.h"
#include "mtd_partition.h"

#include "limits.h"
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



struct netif* pnetif;
extern void SDK_init(void);
extern int app_main(int argc, char* argv[]);

#define CHIP_HI3516C_V200   0x3516C200
#define CHIP_HI3518E_V200   0x3518E200
#define CHIP_HI3518E_V201   0x3518E201

//define, max args ---20
#define  ARGS_SIZE_T         20
#define  ARG_BUF_LEN_T       256
static char *ptask_args[ARGS_SIZE_T];
static char *args_buf_t = NULL;

static int taskid = -1;

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

void app_sample(int argc, char **argv )
{
    int i = 0, ret = 0;
    int len = 0;
    char *pch = NULL;
    TSK_INIT_PARAM_S stappTask;

    if(argc < 1) {
        dprintf("illegal parameter!\n");
    }

    if (taskid != -1) {
        dprintf("There's a app_main task existed.\n");
    }
    args_buf_t = zalloc(ARG_BUF_LEN_T);
    memset(&stappTask, 0, sizeof(TSK_INIT_PARAM_S));
    pch = args_buf_t;
    for(i=0; i<ARGS_SIZE_T; i++) {
        ptask_args[i] = NULL;
    }
    argc++;
    ptask_args[0] = "app_sample";

    for(i = 1; i < argc; i++)
    {
        len =  strlen(argv[i-1]);
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
    stappTask.uwStackSize  = 0x80000;
    stappTask.pcName = "app_sample";
    stappTask.usTaskPrio = 10;
    stappTask.uwResved   = LOS_TASK_STATUS_DETACHED;
    stappTask.auwArgs[0] = argc;
    stappTask.auwArgs[1] = (UINT32)ptask_args;
    ret = LOS_TaskCreate((UINT32 *)&taskid, &stappTask);

    dprintf("camera_Task %d,ret is %d\n", taskid,ret);

    //chdir("/sd0");
    chdir("/nfs");

}

void sample_command(void)
{
    osCmdReg(CMD_TYPE_EX, "sample", 0, (CMD_CBK_FUNC)app_sample);
}

void net_init(void)
{
    (void)secure_func_register();
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

extern char shell_working_directory[PATH_MAX];

extern unsigned int g_uwFatSectorsPerBlock;
extern unsigned int g_uwFatBlockNums;

extern UINT32 g_sys_mem_addr_end;
extern unsigned int g_uart_fputc_en;
void board_config(void)
{
    g_sys_mem_addr_end = SYS_MEM_BASE + SYS_MEM_SIZE_DEFAULT;
    g_uwSysClock = OS_SYS_CLOCK;
    g_uart_fputc_en = 1;
    extern unsigned long g_usb_mem_addr_start;
    g_usb_mem_addr_start = g_sys_mem_addr_end;

    g_uwFatSectorsPerBlock = CONFIG_FS_FAT_SECTOR_PER_BLOCK;
    g_uwFatBlockNums       = CONFIG_FS_FAT_BLOCK_NUMS;
}

void app_init(void)
{
    extern int SD_MMC_Host_init(void);
    extern UINT32 usb_init(void);
    extern int spinor_init(void);
    
    dprintf("os vfs init ...\n");
    proc_fs_init();
    mem_dev_register();
    
    dprintf("uart init ...\n");
    uart_dev_init();

    dprintf("shell init ...\n");
    system_console_init(TTY_DEVICE);
    osShellInit(TTY_DEVICE);
    //shell_cmd_register();

    /*
    dprintf("nand init ...\n");
    if (!nand_init())
    {
        add_mtd_partition("nand", 0x900000, 16 * 0x100000, 0);
        add_mtd_partition("nand", 0x900000 + 16 * 0x100000, 16 * 0x100000, 1);
        mount("/dev/nandblk0", "/yaffs0", "yaffs", 0, NULL);
        //mount("/dev/nandblk1", "/yaffs1", "yaffs", 0, NULL);
    }
    */

    dprintf("spi nor flash init ...\n");
    if (!spinor_init())
    {
        //add_mtd_partition("spinor", 0x100000, 2 * 0x100000, 0);
        //add_mtd_partition("spinor", 3 * 0x100000, 2 * 0x100000, 1);
        add_mtd_partition("spinor",  0x500000, 11 * 0x100000, 0);
	    mount("/dev/spinorblk0", "/jffs0", "jffs", 0, NULL);
        //mount("/dev/spinorblk1", "/jffs1", "jffs", 0, NULL);
    }

    dprintf("spi bus init ...\n");
    spi_dev_init();

    dprintf("i2c bus init ...\n");
    i2c_dev_init();

    dprintf("gpio init ...\n");
    gpio_dev_init();

    dprintf("dmac init\n");
    dmac_init();

    dprintf("random init ...\n");
    ran_dev_register();

    dprintf("net init ...\n");
    net_init();

    dprintf("usb init ...\n");
    usb_init();


    dprintf("sd/mmc host init ...\n");
    size_t part0_start = 16   * (0x100000 / 512);
    size_t part0_count = 1024 * (0x100000 / 512);
    size_t part1_start = 16   * (0x100000 / 512) + part0_count;
    size_t part1_count = 1024 * (0x100000 / 512);
    extern struct disk_divide_info emmc;
    add_mmc_partition(&emmc, part0_start, part0_count);
    add_mmc_partition(&emmc, part1_start, part1_count);
    SD_MMC_Host_init();
    sleep(1);
    mount("/dev/mmcblk0p0", "/sd0", "vfat", 0, 0);
    mount("/dev/mmcblk0p1", "/sd1", "vfat", 0, 0);

    dprintf("Now shell working dir is :%s\n", shell_working_directory);
    dprintf("g_sys_mem_addr_end=0x%08x,\n", g_sys_mem_addr_end);


    CatLogShell();   //cat_logmpp

    dprintf("done init!\n");
    dprintf("Date:%s.\n", __DATE__);
    dprintf("Time:%s.\n", __TIME__);
    SDK_init();
    sample_command();

    //tools_cmd_register();
    return;
}

/* EOF kthread1.c */
