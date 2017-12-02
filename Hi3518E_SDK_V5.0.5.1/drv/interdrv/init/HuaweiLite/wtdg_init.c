#include "hi_interdrv_param.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern int  watchdog_init(void);
extern void watchdog_exit(void);

extern int default_margin;
//extern int nowayout;
extern int nodeamon;

int wtdg_mod_init(void *pArgs)
{
    WTDG_MODULE_PARAMS_S *pstWtdgModParam = (WTDG_MODULE_PARAMS_S*)pArgs;
	default_margin = pstWtdgModParam->default_margin;
//	nowayout = pstWtdgModParam->nowayout;
	nodeamon = pstWtdgModParam->nodeamon;
    return watchdog_init();
}

void wtdg_mod_exit()
{
    watchdog_exit();
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */
