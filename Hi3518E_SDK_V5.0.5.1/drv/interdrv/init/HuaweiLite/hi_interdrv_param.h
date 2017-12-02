#ifndef __HI_INTERDRV_PARAM__
#define __HI_INTERDRV_PARAM__

#include "hi_type.h"

typedef struct tagRTC_MODULE_PARAMS_S
{
    int t_second;
}RTC_MODULE_PARAMS_S;

typedef struct tagWTDG_MODULE_PARAMS_S
{
    int default_margin;
   // int nowayout;
    int nodeamon;
}WTDG_MODULE_PARAMS_S;

#endif

