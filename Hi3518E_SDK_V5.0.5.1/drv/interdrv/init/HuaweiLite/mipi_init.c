 

#include "hi_type.h"
#include "hi_mipi.h"

extern int mipi_init(void);
extern void  mipi_exit(void);

int mipi_mod_init(void)
{
	return mipi_init();
}
void mipi_mod_exit(void)
{
	mipi_exit();
}
 

/****************************************************************************
 * Export symbol                                                            *
 ****************************************************************************/
extern int mipi_set_combo_dev_attr(combo_dev_attr_t* p_attr);
extern int mipi_drv_set_phy_reg_start(HI_BOOL en);


