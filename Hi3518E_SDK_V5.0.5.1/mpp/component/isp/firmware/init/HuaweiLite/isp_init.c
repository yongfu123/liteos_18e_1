
#include "hi_type.h"
#include "hi_common.h"

#include "hi_isp_param.h"

extern HI_U32 pwm_num;
extern HI_U32 proc_param;
extern HI_U32 update_pos;
extern HI_BOOL lsc_update_mode;


extern int ISP_ModInit(void);
extern void ISP_ModExit(void);

int isp_mod_init(void *pArgs)
{
    ISP_MODULE_PARAMS_S* pstIspModuleParam = (ISP_MODULE_PARAMS_S*)pArgs;

    if(NULL != pstIspModuleParam)
    {
        pwm_num    = pstIspModuleParam->u32PwmNum;
        proc_param = pstIspModuleParam->u32ProcParam;
        update_pos = pstIspModuleParam->u32UpdatePos;
        lsc_update_mode = pstIspModuleParam->u32LscUpdateMode;
    }

	ISP_ModInit();
	return 0;
}

void isp_mod_exit(void)
{
	ISP_ModExit();
}

