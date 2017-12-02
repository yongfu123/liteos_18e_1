/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : isp_ext.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/07/17
  Description   : 
  History       :
  1.Date        : 2013/07/17
    Author      : n00168968
    Modification: Created file

******************************************************************************/
#ifndef __ISP_EXT_H__
#define __ISP_EXT_H__

#include "hi_type.h"

#include "mod_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum hiISP_BUS_TYPE_E
{
    ISP_BUS_TYPE_I2C = 0,
    ISP_BUS_TYPE_SSP,

    ISP_BUS_TYPE_BUTT,
} ISP_BUS_TYPE_E;

typedef struct hiISP_BUS_CALLBACK_S
{
    HI_S32  (*pfnISPWriteI2CData) (HI_U8 u8DevAddr, HI_U32 u32RegAddr,
        HI_U32 u32RegAddrByteNum, HI_U32 u32Data, HI_U32 u32DataByteNum);
    HI_S32  (*pfnISPWriteSSPData) (HI_U32 u32DevAddr, HI_U32 u32DevAddrByteNum,
        HI_U32 u32RegAddr, HI_U32 u32RegAddrByteNum, HI_U32 u32Data, HI_U32 u32DataByteNum);
} ISP_BUS_CALLBACK_S;

typedef struct hiISP_PIRIS_CALLBACK_S
{
    HI_S32  (*pfn_piris_gpio_update)(HI_S32 *s32Pos);
} ISP_PIRIS_CALLBACK_S;

typedef struct hiISP_EXPORT_FUNC_S
{
    HI_S32  (*pfnISPRegisterBusCallBack) (HI_S32 IspDev, ISP_BUS_TYPE_E enType, ISP_BUS_CALLBACK_S *pstBusCb);
    HI_S32  (*pfnISPRegisterPirisCallBack) (HI_S32 IspDev, ISP_PIRIS_CALLBACK_S *pstPirisCb);
    HI_S32  (*pfnISP_GetDCFInfo)(ISP_DEV IspDev,ISP_DCF_INFO_S *pstIspDCF);
} ISP_EXPORT_FUNC_S;

#define CKFN_ISP_RegisterBusCallBack()\
        (NULL != FUNC_ENTRY(ISP_EXPORT_FUNC_S, HI_ID_ISP)->pfnISPRegisterBusCallBack)
#define CALL_ISP_RegisterBusCallBack(IspDev,enType,pstBusCb)\
        FUNC_ENTRY(ISP_EXPORT_FUNC_S, HI_ID_ISP)->pfnISPRegisterBusCallBack(IspDev,enType,pstBusCb)

#define CKFN_ISP_RegisterPirisCallBack()\
        (NULL != FUNC_ENTRY(ISP_EXPORT_FUNC_S, HI_ID_ISP)->pfnISPRegisterPirisCallBack)
#define CALL_ISP_RegisterPirisCallBack(IspDev,pstPirisCb)\
        FUNC_ENTRY(ISP_EXPORT_FUNC_S, HI_ID_ISP)->pfnISPRegisterPirisCallBack(IspDev,pstPirisCb)

#define CKFN_ISP_GetDCFInfo()\
        (NULL != FUNC_ENTRY(ISP_EXPORT_FUNC_S, HI_ID_ISP)->pfnISP_GetDCFInfo)
#define CALL_ISP_GetDCFInfo(IspDev,pstIspDCF)\
        FUNC_ENTRY(ISP_EXPORT_FUNC_S, HI_ID_ISP)->pfnISP_GetDCFInfo(IspDev,pstIspDCF)

//----------------------------------------------------------------------------------------
// isp sync task    
typedef enum hiISP_SYNC_TSK_METHOD_E
{
    ISP_SYNC_TSK_METHOD_HW_IRQ = 0,
    ISP_SYNC_TSK_METHOD_TSKLET,
    ISP_SYNC_TSK_METHOD_WORKQUE,
    
    ISP_SYNC_TSK_METHOD_BUTT
    
} ISP_SYNC_TSK_METHOD_E;

typedef struct hiISP_SYNC_TASK_NODE_S
{
    ISP_SYNC_TSK_METHOD_E enMethod;
    HI_S32 ( *pfnIspSyncTskCallBack ) (HI_U64 u64Data);
    HI_U64 u64Data;
    const char *pszId;
    struct osal_list_head list;

} ISP_SYNC_TASK_NODE_S;   

extern HI_S32 hi_isp_sync_task_register(ISP_DEV dev, ISP_SYNC_TASK_NODE_S * pstNewNode);
extern HI_S32 hi_isp_sync_task_unregister(ISP_DEV dev, ISP_SYNC_TASK_NODE_S *pstDelNode); 
   
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
