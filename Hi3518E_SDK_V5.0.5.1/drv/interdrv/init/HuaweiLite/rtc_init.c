
#include "hi_interdrv_param.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#ifndef NULL
#define NULL  ((void *)0)
#endif

extern int  rtc_init(void);
extern void  rtc_exit(void);

extern int t_second;

int rtc_mod_init(void *pArgs)
{
    //t_second
    if (NULL != pArgs)
    {
        RTC_MODULE_PARAMS_S *pstRtcModParams = (RTC_MODULE_PARAMS_S *)pArgs;
        t_second = pstRtcModParams->t_second;
    }
    return rtc_init();
}

void rtc_mod_exit()
{
    rtc_exit();
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */
