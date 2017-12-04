#ifndef __HISI_WIFI_H__
#define __HISI_WIFI_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


typedef struct {
    unsigned int wlan_irq;

    unsigned int wifi_data_intr_gpio_group;
    unsigned int wifi_data_intr_gpio_offset;

    unsigned int dev_wak_host_gpio_group;
    unsigned int dev_wak_host_gpio_offset;

    unsigned int host_wak_dev_gpio_group;
    unsigned int host_wak_dev_gpio_offset;

    void (*wifi_power_set)(unsigned char);
    void (*wifi_rst_set)(unsigned char);
    void (*wifi_sdio_detect)(void);
}BOARD_INFO;

#define             WIFI_IRQ                             NUM_HAL_INTERRUPT_GPIO
#define             WIFI_SDIO_INDEX                 (1)

#ifdef HISI_WIFI_PLATFORM_HI3518EV200
#define             WIFI_DATA_INTR_GPIO_GROUP                  (6)
#define             WIFI_DATA_INTR_GPIO_OFFSET                  (1)

#define             HOST_WAK_DEV_GPIO_GROUP                     (5)
#define             HOST_WAK_DEV_GPIO_OFFSET                    (7)

#define             WIFI_WAK_FLAG_GPIO_GROUP                    (6)
#define             WIFI_WAK_FLAG_GPIO_OFFSET                   (2)

#define             DEV_WAK_HOST_GPIO_GROUP                     (6)
#define             DEV_WAK_HOST_GPIO_OFFSET                    (3)

#define REG_MUXCTRL_WIFI_DATA_INTR_GPIO_MAP        (IO_MUX_REG_BASE + 0x0BC)//gpio6_1
#define REG_MUXCTRL_HOST_WAK_DEV_GPIO_MAP        (IO_MUX_REG_BASE + 0x0C4)//gpio5_7
#define REG_MUXCTRL_WIFI_WAK_FLAG_GPIO_MAP        (IO_MUX_REG_BASE + 0x0C8)//gpio6_2
#define REG_MUXCTRL_DEV_WAK_HOST_GPIO_MAP        (IO_MUX_REG_BASE + 0x0CC)//gpio6_3
#else /*default HISI_WIFI_PLATFORM_HI3516CV200*/
#define             WIFI_DATA_INTR_GPIO_GROUP                  (6)
#define             WIFI_DATA_INTR_GPIO_OFFSET                  (5)

#define             HOST_WAK_DEV_GPIO_GROUP                     (6)
#define             HOST_WAK_DEV_GPIO_OFFSET                    (7)

#define             WIFI_WAK_FLAG_GPIO_GROUP                    (7)
#define             WIFI_WAK_FLAG_GPIO_OFFSET                   (0)

#define             DEV_WAK_HOST_GPIO_GROUP                     (7)
#define             DEV_WAK_HOST_GPIO_OFFSET                    (1)

#define REG_MUXCTRL_WIFI_DATA_INTR_GPIO_MAP        (IO_MUX_REG_BASE + 0x0D4)//gpio6_5
#define REG_MUXCTRL_HOST_WAK_DEV_GPIO_MAP        (IO_MUX_REG_BASE + 0x0DC)//gpio6_7
#define REG_MUXCTRL_WIFI_WAK_FLAG_GPIO_MAP        (IO_MUX_REG_BASE + 0x0E0)//gpio7_0
#define REG_MUXCTRL_DEV_WAK_HOST_GPIO_MAP        (IO_MUX_REG_BASE + 0x0E4)//gpio7_1
#endif

#define REG_MUXCTRL_SDIO0_CLK_MAP          (IO_MUX_REG_BASE + 0xA4)
#define REG_MUXCTRL_SDIO0_DETECT_MAP        (IO_MUX_REG_BASE + 0x9C)
#define REG_MUXCTRL_SDIO0_CWPR_MAP        (IO_MUX_REG_BASE + 0xA0)
#define REG_MUXCTRL_SDIO0_CDATA1_MAP        (IO_MUX_REG_BASE + 0xB0)
#define REG_MUXCTRL_SDIO0_CDATA0_MAP        (IO_MUX_REG_BASE + 0xAC)
#define REG_MUXCTRL_SDIO0_CDATA3_MAP        (IO_MUX_REG_BASE + 0xB8)
#define REG_MUXCTRL_SDIO0_CCMD_MAP        (IO_MUX_REG_BASE + 0xA8)
#define REG_MUXCTRL_SDIO0_POWER_EN_MAP        (IO_MUX_REG_BASE + 0X98)
#define REG_MUXCTRL_SDIO0_CDATA2_MAP        (IO_MUX_REG_BASE + 0xB4)


#define REG_MUXCTRL_SDIO1_CLK_MAP          (IO_MUX_REG_BASE + 0x010)
#define REG_MUXCTRL_SDIO1_DETECT_MAP        (IO_MUX_REG_BASE + 0x014)
#define REG_MUXCTRL_SDIO1_CWPR_MAP        (IO_MUX_REG_BASE + 0x018)
#define REG_MUXCTRL_SDIO1_CDATA1_MAP        (IO_MUX_REG_BASE + 0x01C)
#define REG_MUXCTRL_SDIO1_CDATA0_MAP        (IO_MUX_REG_BASE + 0x020)
#define REG_MUXCTRL_SDIO1_CDATA3_MAP        (IO_MUX_REG_BASE + 0x024)
#define REG_MUXCTRL_SDIO1_CCMD_MAP        (IO_MUX_REG_BASE + 0x028)
#define REG_MUXCTRL_SDIO1_POWER_EN_MAP        (IO_MUX_REG_BASE + 0x02C)
#define REG_MUXCTRL_SDIO1_CDATA2_MAP        (IO_MUX_REG_BASE + 0x034)

extern inline BOARD_INFO * get_board_info(void);
extern void wlan_resume_state_set(unsigned int ul_state);
extern void HI_HAL_MCUHOST_WiFi_Clr_Flag(void);
extern void HI_HAL_MCUHOST_WiFi_Power_Set(unsigned char val);
extern void HI_HAL_MCUHOST_WiFi_Rst_Set(unsigned char val);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
#endif
