/******************************************************************************
  Some simple Hisilicon Hi35xx system functions.

  Copyright (C), 2010-2015, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2015-6 Created
******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <asm/io.h>
#include <string.h>
#include "hi_type.h"
#include "hi_comm_sys.h"
#include "hi_module_param.h"
#include "osal_mmz.h"

#define himm(address, value)        writel(value, address)


#define M_SIZE (1024*1024)
#define MEM_ALIGN(x) (((x)+ M_SIZE - 1)&(~(M_SIZE - 1)))

#define MEM_TOTAL_SIZE  64U        /* MB, total mem */

/* calculate the MMZ info */
extern unsigned long g_usb_mem_size;
extern unsigned int g_sys_mem_addr_end;

static HI_S32 MMZ_init(void)
{
    extern int media_mem_init(void *);
    MMZ_MODULE_PARAMS_S stMMZ_Param;
    HI_U32 u32MmzStart, u32MmzSize;

    u32MmzStart = g_sys_mem_addr_end + g_usb_mem_size;
    u32MmzSize = (SYS_MEM_BASE + MEM_TOTAL_SIZE*M_SIZE - u32MmzStart)/M_SIZE;

    snprintf(stMMZ_Param.mmz, MMZ_SETUP_CMDLINE_LEN, "anonymous,0,0x%x,%dM", u32MmzStart, u32MmzSize);

    stMMZ_Param.anony = 1;

    dprintf("mem_start=0x%x, MEM_OS_SIZE=%dM, MEM_USB_SIZE=%dM, mmz_start=0x%x, mmz_size=%dM\n", 
        SYS_MEM_BASE, (g_sys_mem_addr_end-SYS_MEM_BASE)/M_SIZE, MEM_ALIGN(g_usb_mem_size)/M_SIZE, u32MmzStart, u32MmzSize);
    dprintf("mmz param= %s\n", stMMZ_Param.mmz);
    return media_mem_init(&stMMZ_Param);
}

static HI_S32 CIPHER_init(void)
{
    extern int CIPHER_DRV_ModInit(void);

    return CIPHER_DRV_ModInit();
}

extern void osal_proc_init(void);

HI_VOID SDK_init(void)
{
    HI_S32 ret = 0;

    ret = MMZ_init();
    if (ret != 0)
    {
        printf("mmz init error.\n");
    }
    
    osal_proc_init();

    ret = CIPHER_init();
    if (ret != 0)
    {
        printf("cipher init error.\n");
    }

    printf("SDK init ok...\n");
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

