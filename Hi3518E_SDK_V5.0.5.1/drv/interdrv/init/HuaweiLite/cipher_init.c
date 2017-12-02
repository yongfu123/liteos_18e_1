#include <linux/module.h>
#include <linux/kernel.h>

#include "hi_type.h"

extern int CIPHER_DRV_ModInit(void);
extern void  CIPHER_DRV_ModExit(void);

int cipher_mod_init(void)
{
	return CIPHER_DRV_ModInit();
}

void cipher_mod_exit(void)
{
	CIPHER_DRV_ModExit();
}


