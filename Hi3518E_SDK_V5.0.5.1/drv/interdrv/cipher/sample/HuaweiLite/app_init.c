/******************************************************************************
  Some simple Hisilicon Hi35xx system functions.

  Copyright (C), 2010-2015, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2015-6 Created
******************************************************************************/
#include "sys/types.h"
#include "sys/time.h"
#include "unistd.h"
#include "stdio.h"
#include "shell.h"
#include "hisoc/uart.h"
#include "linux/fb.h"

extern int mem_dev_register(void);
extern void hisi_eth_init(void);
//extern void tools_cmd_register(void);
extern int spi_dev_init(void);
extern int i2c_dev_init(void);
extern int gpio_dev_init(void);
extern int dmac_init(void);
extern int ran_dev_register(void);
extern UINT32 osShellInit(char *);
extern void CatLogShell();
extern void proc_fs_init(void);
extern int uart_dev_init(void);
extern int system_console_init(const char *);
extern int nand_init(void);
extern int add_mtd_partition( char *, UINT32 , UINT32 , UINT32 );
extern int spinor_init(void);
extern void SDK_init(void);
extern int app_main(int argc, char* argv[]);

#define ARGS_SIZE_T		20
#define ARG_BUF_LEN_T	256
static char *ptask_args[ARGS_SIZE_T];
static char *args_buf_t = NULL;
static int taskid = -1;
struct netif* pnetif;

void com_app(unsigned int p0, unsigned int p1, unsigned int p2, unsigned int p3)
{
    int i = 0;
    unsigned int argc = p0;
    char **argv = (char **)p1;

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
        dprintf("There's a app_main task existed.");
    }
    args_buf_t = zalloc(ARG_BUF_LEN_T);
    memset(&stappTask, 0, sizeof(TSK_INIT_PARAM_S));
    pch = args_buf_t;
    for(i=0; i<ARGS_SIZE_T; i++) {
        ptask_args[i] = NULL;
    }
    argc++;
    ptask_args[0] = "sample";

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
    stappTask.uwStackSize = 0x10000;
    stappTask.pcName = "sample";
    stappTask.usTaskPrio = 10;
    stappTask.uwResved   = LOS_TASK_STATUS_DETACHED;
    stappTask.auwArgs[0] = argc;
    stappTask.auwArgs[1] = (UINT32)ptask_args;
    ret = LOS_TaskCreate((UINT32 *)&taskid, &stappTask);
    if (LOS_OK != ret) 
    {
        dprintf("LOS_TaskCreate err, ret:%d\n", ret);
    }
    else
    {
        dprintf("camera_Task %d\n", taskid);
    }

    chdir("/nfs");
}

void sample_command(void)
{
    osCmdReg(CMD_TYPE_EX, "sample", 0, (CMD_CBK_FUNC)app_sample);
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

    dprintf("spi nor flash init ...\n");
    if(!spinor_init()){
        add_mtd_partition("spinor", 0x100000, 2*0x100000, 0);
        add_mtd_partition("spinor", 3*0x100000, 2*0x100000, 1);
        mount("/dev/spinorblk0", "/jffs0", "jffs", 0, NULL);
    }
 
    dprintf("g_sys_mem_addr_end=0x%08x,\n",g_sys_mem_addr_end);

    dprintf("done init!\n");
    dprintf("Date:%s.\n", __DATE__);
    dprintf("Time:%s.\n", __TIME__);
    SDK_init();
    sample_command();

    return;
}

/* EOF kthread1.c */
