
#include "hi_interdrv_param.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern int  hiir_init(void);
extern void  hiir_exit(void);

int hiir_mod_init(void *pArgs)
{
    return hiir_init();
}

void hiir_mod_exit()
{
    hiir_exit();
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */

