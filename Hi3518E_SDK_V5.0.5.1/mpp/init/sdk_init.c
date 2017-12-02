/******************************************************************************
  Some simple Hisilicon Hi3516A system functions.

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
#include "hi_type.h"
#include "hi_comm_sys.h"
#include "hi_comm_vi.h"
#include "hi_comm_vpss.h"
#include "hi_comm_vo.h"
#include "hi_comm_venc.h"
#include "hi_comm_vgs.h"
#include "hi_comm_isp.h"

#include "hi_module_param.h"
//#include "hi_venc_param.h"
#include "hi_isp_param.h"
#include "isp_ext.h"
#include "sensor_spi.h"
#include "osal_mmz.h"
#include "hifb.h"

#define CHIP_HI3516C_V200   0x3516C200
#define CHIP_HI3518E_V200   0x3518E200
#define CHIP_HI3518E_V201   0x3518E201


#define himm(address, value)        writel(value, address)
#define M_SIZE (1024*1024)
#define MEM_ALIGN(x) (((x)+ M_SIZE - 1)&(~(M_SIZE - 1)))



HI_CHAR* hi_chip = "hi3518e";                //hi3518e
HI_U32 g_u32vi_vpss_online = 1;         //0, 1
HI_CHAR* sensor_type = "ar0130";     // ov9732 ar0230 ar0130 imx222 9m034 ov9750 
HI_CHAR* vo_type = "BT656";    //BT656 LCD
HI_CHAR* pin_mux_select = "net";  //vo net

#ifndef CHIP_ID
#define MEM_TOTAL_SIZE  64U         /* MB, total mem */
#define MEM_START       0x80000000  /* phy mem start */
#define MEM_OS_SIZE     32U         /* MB, os mem size */
#define MEM_USB_SIZE    1U          /* MB, usb mem size */
#endif

#if (CHIP_ID == CHIP_HI3518E_V200)
#define MEM_TOTAL_SIZE  64U         /* MB, total mem */
#define MEM_START       0x80000000  /* phy mem start */
#define MEM_OS_SIZE     32U         /* MB, os mem size */
#define MEM_USB_SIZE    1U          /* MB, usb mem size */
#elif (CHIP_ID == CHIP_HI3518E_V201)
#define MEM_TOTAL_SIZE  32U         /* MB, total mem */
#define MEM_START       0x80000000  /* phy mem start */
#define MEM_OS_SIZE     20U         /* MB, os mem size */
#define MEM_USB_SIZE    1U          /* MB, usb mem size */
#elif (CHIP_ID == CHIP_HI3516C_V200)
#define MEM_TOTAL_SIZE  256U        /* MB, total mem */
#define MEM_START       0x80000000  /* phy mem start */
#define MEM_OS_SIZE     64U         /* MB, os mem size */
#define MEM_USB_SIZE    1U          /* MB, usb mem size */
#endif


/*disable audio mute*/
static HI_VOID audio_disable_mute(void)
{
	himm(0x200f0078, 0x00000000);
	himm(0x201A0400, 0x00000008);	
	himm(0x201A0020, 0x00000008);
}

static HI_VOID vicap_pin_mux(void)
{
	himm(0x200f0000, 0x00000001);		// 0: GPIO0_4		1: SENSOR_CLK
	himm(0x200f0004, 0x00000000);		// 0: SENSOR_RSTN	1: GPIO0_5		
	himm(0x200f0008, 0x00000001);		// 0: GPIO0_6		1£ºFLASH_TRIG	2: SFC_EMMC_BOOT_MODE	3£ºSPI1_CSN1	4:VI_VS
	himm(0x200f000c, 0x00000001);		// 0£ºGPIO0_7	 	1£ºSHUTTER_TRIG	2£ºSFC_DEVICE_MODE		4: VI_HS
}

//SPI1 -> LCD 
static HI_VOID spi1_pim_mux(void)
{
	himm(0x200f0050, 0x1);				// 001£ºSPI1_SCLK£»
	himm(0x200f0054, 0x1);				// 001£ºSPI1_SDO£»
	himm(0x200f0058, 0x1);				// 001£ºSPI1_SDI£»
	himm(0x200f005c, 0x1);				// 001£ºSPI1_CSN0£»
}

#if 0
//I2C0 -> sensor
static HI_VOID i2c0_pin_mux(void)
{
	himm(0x200f0040, 0x00000002);		// 0: GPIO3_3		1:spi0_sclk		2:i2c0_scl
	himm(0x200f0044, 0x00000002);		// 0: GPIO3_4		1:spi0_sdo		2:i2c0_sda
}
#endif
//I2C1 -> 7179
static HI_VOID i2c1_pin_mux(void)
{
	himm(0x200f0050, 0x00000002);		// 010£ºI2C1_SCL£»
	himm(0x200f0054, 0x00000002);		// 010£ºI2C1_SDA£»
}

#if 0
static HI_VOID i2c2_pin_mux(void)
{
	himm(0x200f0060, 0x1);			// i2c2_sda
	himm(0x200f0064, 0x1);			// i2c2_scl
}
#endif
static HI_VOID vo_output_mode(void)
{
	himm(0x200f0010, 0x00000003);		// 3£ºVO_CLK   & 0: GPIO2_0  & 1: RMII_CLK
	himm(0x200f0014, 0x00000000);		// 3£ºVO_VS    & 0: GPIO2_1  & 1: RMII_TX_EN & 4: SDIO1_CARD_DETECT
	himm(0x200f0018, 0x00000003);		// 3£ºVO_DATA5 & 0: GPIO2_2	& 1: RMII_TXD0	& 4: SDIO1_CWPR
	himm(0x200f001c, 0x00000000);		// 3£ºVO_DE    & 0: GPIO2_3	& 1: RMII_TXD1	& 4: SDIO1_CDATA1
	himm(0x200f0020, 0x00000003);		// 3£ºVO_DATA7 & 0: GPIO2_4	& 1: RMII_RX_DV	& 4: SDIO1_CDATA0
	himm(0x200f0024, 0x00000003);		// 3£ºVO_DATA2 & 0: GPIO2_5	& 1: RMII_RXD0	& 4: SDIO1_CDATA3
	himm(0x200f0028, 0x00000003);		// 3£ºVO_DATA3 & 0: GPIO2_6	& 1: RMII_RXD1	& 4: SDIO1_CCMD
	himm(0x200f002c, 0x00000000);		// 3£ºVO_HS    & 0: GPIO2_7	& 1: EPHY_RST	& 2: BOOT_SEL	& 4: SDIO1_CARD_POWER_EN
	himm(0x200f0030, 0x00000003);		// 3£ºVO_DATA0 & 0: GPIO0_3	& 1: SPI1_CSN1	
	himm(0x200f0034, 0x00000003);		// 3£ºVO_DATA1 & 0: GPIO3_0	& 1: EPHY_CLK	& 4: SDIO1_CDATA2
	himm(0x200f0038, 0x00000003);		// 3: VO_DATA6 & 0: GPIO3_1	& 1: MDCK		& 2£ºBOOTROM_SEL
	himm(0x200f003c, 0x00000003);		// 3£ºVO_DATA4 & 0: GPIO3_2	& 1: MDIO
	
}

static HI_VOID bt656_drive_capability(void)
{
	//BT656 drive capability config
	himm(0x200f0810, 0xd0);    		// VO_CLK     
	himm(0x200f0830, 0x90);    		// VO_DATA0 
	himm(0x200f0834, 0xd0);    		// VO_DATA1 
	himm(0x200f0824, 0x90);    		// VO_DATA2 
	himm(0x200f0828, 0x90);    		// VO_DATA3 
	himm(0x200f083c, 0x90);    		// VO_DATA4
	himm(0x200f0818, 0x90);    		// VO_DATA5 
	himm(0x200f0838, 0x90);    		// VO_DATA6
	himm(0x200f0820, 0x90);    		// VO_DATA7
}

static HI_VOID lcd_drive_capability(void)
{
	//LCD drive capability config
	himm(0x200f0810, 0xe0); 
	himm(0x200f0830, 0xa0);  
	himm(0x200f0834, 0xe0); 
	himm(0x200f0824, 0xa0);
	himm(0x200f0828, 0xa0);
	himm(0x200f083c, 0xa0);
	himm(0x200f0818, 0xa0);
	himm(0x200f0838, 0xa0);
	himm(0x200f0820, 0xa0);
	himm(0x200f081c, 0xa0);
}

//RMII    
static HI_VOID net_rmii_mode(void)
{
	//echo "------net_rmii_mode------"
	himm(0x200f002c, 0x00000001);     // 1: EPHY_RST   & 0: GPIO2_7  & 2: BOOT_SEL & 3£ºVO_HS & 4: SDIO1_CARD_POWER_EN
	himm(0x200f0034, 0x00000001);     // 1: EPHY_CLK   & 0: GPIO3_0  & 3£ºVO_DATA1 & 4: SDIO1_CDATA2
	//
	himm(0x200f0010, 0x00000001);     // 1: RMII_CLK   & 0: GPIO2_0  & 3£ºVO_CLK
	himm(0x200f0014, 0x00000001);     // 1: RMII_TX_EN & 0: GPIO2_1  & 3£ºVO_VS    & 4: SDIO1_CARD_DETECT
	himm(0x200f0018, 0x00000001);     // 1: RMII_TXD0  & 0: GPIO2_2  & 3£ºVO_DATA5 & 4: SDIO1_CWPR
	himm(0x200f001c, 0x00000001);     // 1: RMII_TXD1  & 0: GPIO2_3  & 3£ºVO_DE    & 4: SDIO1_CDATA1
	himm(0x200f0020, 0x00000001);     // 1: RMII_RX_DV & 0: GPIO2_4  & 3£ºVO_DATA7 & 4: SDIO1_CDATA
	himm(0x200f0024, 0x00000001);     // 1: RMII_RXD0  & 0: GPIO2_5  & 3£ºVO_DATA2 & 4: SDIO1_CDATA3
	himm(0x200f0028, 0x00000001);     // 1: RMII_RXD1  & 0: GPIO2_6  & 3£ºVO_DATA3 & 4: SDIO1_CCMD£»
	//
	himm(0x200f0038, 0x00000001);     // 1: MDCK 	    & 0: GPIO3_1  & 2£ºBOOTROM_SEL & 3: VO_DATA6  
	himm(0x200f003c, 0x00000001);     // 1: MDIO       & 0: GPIO3_2  & 3£ºVO_DATA4
	
	//ephy drive capability config
	himm(0x200f0810, 0xd0);			// RMII_CLK
	himm(0x200f0814, 0xa0);			// RMII_TX_EN
	himm(0x200f0818, 0xa0);			// RMII_TXD0
	himm(0x200f081c, 0xa0);			// RMII_TXD1
	himm(0x200f0820, 0xb0);			// RMII_RX_DV
	himm(0x200f0824, 0xb0);			// RMII_RXD0
	himm(0x200f0828, 0xb0);			// RMII_RXD1
	himm(0x200f082c, 0xb0);			// EPHY_RST
	himm(0x200f0834, 0xd0);			// EPHY_CLK
	himm(0x200f0838, 0x90);			// MDCK
	himm(0x200f083c, 0xa0);			// MDIO
}      

#if 0
static HI_VOID i2s_pin_mux(void)
{
	// pin_mux with GPIO1 
	//himm(0x200f007c, 0x3);		    // i2s_bclk_tx
	//himm(0x200f0080, 0x3);		    // i2s_sd_tx
	//himm(0x200f0084, 0x3);		    // i2s_mclk
	//himm(0x200f0088, 0x3);		    // i2s_ws_tx
	//himm(0x200f008c, 0x3);		    // i2s_ws_rx
	//himm(0x200f0090, 0x3);		    // i2s_bclk_rx
	//himm(0x200f0094, 0x3);		    // i2s_sd_rx

	// pin_mux with UART1 
	himm(0x200f00bc, 0x2);		    // i2s_sd_tx
	himm(0x200f00c0, 0x2);		    // i2s_ws_tx
	himm(0x200f00c4, 0x2);		    // i2s_mclk
	himm(0x200f00c8, 0x2);		    // i2s_sd_rx
	himm(0x200f00d0, 0x2);		    // i2s_bclk_tx

	// pin_mux with JTAG
	//himm(0x200f00d4, 0x3);		    // i2s_mclk
	//himm(0x200f00d8, 0x3);		    // i2s_ws_tx
	//himm(0x200f00dc, 0x3);		    // i2s_sd_tx
	//himm(0x200f00e0, 0x3);		    // i2s_sd_rx
	//himm(0x200f00e4, 0x3);		    // i2s_bclk_tx
}
#endif
static HI_VOID pinmux_hi3518e(void)
{
    if (!strcmp(pin_mux_select, "vo"))
    {
        if (!strcmp(vo_type, "BT656"))
        {
            i2c1_pin_mux();         	                  
			vo_output_mode();       	                  
			bt656_drive_capability();

            /* load peripheral equipment  */
        }
        else if (!strcmp(vo_type, "LCD"))
        {
            spi1_pim_mux();                            
			vo_output_mode();                       	 
			lcd_drive_capability(); 				     
			himm(0x200f0014, 0x00000003);	 // 3£ºVO_VS    & 0: GPIO2_1  & 1: RMII_TX_EN & 4: SDIO1_CARD_DETECT
			himm(0x200f002c, 0x00000003);	 // 3£ºVO_HS    & 0: GPIO2_7	& 1: EPHY_RST	& 2: BOOT_SEL	& 4: SDIO1_CARD_POWER_EN
			himm(0x200f001c, 0x00000003);
            /* load peripheral equipment */
        }
        else
        {}
    }
    else if (!strcmp(pin_mux_select, "net"))
    {
        net_rmii_mode();
    }
    else
    {

    }

    //i2c0_pin_mux();
    //i2c2_pin_mux();
    vicap_pin_mux();
    //i2s_pin_mux();
    //vo_bt656_mode();    
}

static HI_VOID clkcfg_hi3518e(void)
{
    himm(0x2003002c, 0xc4003);       // VICAP, ISP unreset & clock enable, Sensor clock enable, clk reverse
	himm(0x20030034, 0x64ff4);       // 6bit LCD
	himm(0x20030034, 0x164ff4);      // 8bit LCD
	himm(0x20030034, 0xff4);         // bt656
	
	himm(0x20030040, 0x2000);        // AVC unreset, code also config
	himm(0x20030048, 0x2);           // VPSS unreset, code also config
	himm(0x20030058, 0x2);           // TDE  unreset
	himm(0x2003005c, 0x2);           // VGS
	himm(0x20030060, 0x2);           // JPGE unreset
	himm(0x20030068, 0x02000000);    // LCD 27M:0x04000000, 13.5M:0x02000000
	himm(0x2003006c, 0xa);           // IVE/HASH  unreset
	himm(0x2003007c, 0x2);           // Cipher
	himm(0x200300d4, 0x7);           // GZIP
	himm(0x200300d8, 0x2a);          // DDRT¡¢Efuse¡¢DMA 
	
	himm(0x2003008c, 0x2);           // AIO unreset and clock enable,m/f/bclk config in code.
	himm(0x20030100, 0x20);          // RSA
	himm(0x20030104, 0x0);           // AVC-148.5M VGS-148.5M VPSS-99M
}

static HI_VOID sysctl_hi3518e(void)
{
    //# msic config
    himm(0x201200E0, 0xd);				// internal codec£¬AIO MCLK out, CODEC AIO TX MCLK 
    //himm(0x201200E0, 0xe);			        // external codec: AIC31£¬AIO MCLK out, CODEC AIO TX MCLK
#if 1           
    //write command priority
	himm(0x201100c0, 0x76543210);     // ports0         
	himm(0x201100c4, 0x76543210);     // ports1         
	himm(0x201100c8, 0x76543210);     // ports2
	himm(0x201100cc, 0x76543210);     // ports3
	himm(0x201100d0, 0x76543210);     // ports4
	himm(0x201100d4, 0x76543210);     // ports5
	himm(0x201100d8, 0x76543210);     // ports6

	//read command priority               
	himm(0x20110100, 0x76543210);     // ports0         
	himm(0x20110104, 0x76543210);     // ports1         
	himm(0x20110108, 0x76543210);     // ports2
	himm(0x2011010c, 0x76543210);     // ports3
	himm(0x20110110, 0x76543210);     // ports4
	himm(0x20110114, 0x76543210);     // ports5
	himm(0x20110118, 0x76543210);     // ports6

	//write command timeout
	himm(0x20110140, 0x08040200);     // ports0 
	himm(0x20110144, 0x08040100);     // ports1 
	himm(0x20110148, 0x08040200);     // ports2 
	himm(0x2011014c, 0x08040200);     // ports3 
	himm(0x20110150, 0x08040200);     // ports4 
	himm(0x20110154, 0x08040200);     // ports5 
	himm(0x20110158, 0x08040200);     // ports6 

	//read command timeout                 
	himm(0x20110180, 0x08040200);     // ports0 
	himm(0x20110184, 0x08040200);     // ports1 
	himm(0x20110188, 0x08040200);     // ports2 
	himm(0x2011018c, 0x08040200);     // ports3 
	himm(0x20110190, 0x08040200);     // ports4 
	himm(0x20110194, 0x08040200);     // ports5 
	himm(0x20110198, 0x08040200);     // ports6

	//map mode
	himm(0x20110040, 0x01001000);   // ports0 
	himm(0x20110044, 0x01001000);   // ports1 
	himm(0x20110048, 0x01001000);   // ports2 
	himm(0x2011004c, 0x01001000);   // ports3 
	himm(0x20110050, 0x01001000);   // ports4 
	himm(0x20110054, 0x01001000);   // ports5 
	himm(0x20110058, 0x01001000);   // ports6
#endif

    if (g_u32vi_vpss_online)
    {
        //echo "==============vi_vpss_online==============";
        himm(0x20120004, 0x40001000);			// online, SPI1 CS0; [12]-ive
		//#pri config
		himm(0x20120058, 0x26666401);			// each module 4bit£ºvedu       ddrt_md  ive  aio    jpge    tde   vicap  vdp
		himm(0x2012005c, 0x66666103);			// each module 4bit£ºsfc_nand   sfc_nor  nfc  sdio1  sdio0   a7    vpss   vgs 
		himm(0x20120060, 0x66266666);			// each module 4bit£ºreserve    reserve  avc  usb    cipher  dma2  dma1   gsf
		//timeout config                
		himm(0x20120064, 0x00000011);			// each module 4bit£ºvedu       ddrt_md  ive  aio    jpge    tde   vicap  vdp
		himm(0x20120068, 0x00000010);			// each module 4bit£ºsfc_nand   sfc_nor  nfc  sdio1  sdio0   a7    vpss   vgs 
		himm(0x2012006c, 0x00000000);			// each module 4bit£ºreserve    reserve  avc  usb    cipher  dma2  dma1   gsf 
    }
    else
    {
        //echo "==============vi_vpss_offline==============";
		himm(0x20120004, 0x1000);		   		// offline, mipi SPI1 CS0; [12]-ive
        //# pri config
		himm(0x20120058, 0x26666400);			// each module 4bit£ºvedu       ddrt_md  ive  aio    jpge    tde   vicap  vdp
		himm(0x2012005c, 0x66666123);			// each module 4bit£ºsfc_nand   sfc_nor  nfc  sdio1  sdio0   a7    vpss   vgs 
		himm(0x20120060, 0x66266666);		// each module 4bit£ºreserve    reserve  avc  usb    cipher  dma2  dma1   gsf
        //# timeout config              		
		himm(0x20120064, 0x00000011);			// each module 4bit£ºvedu       ddrt_md  ive  aio    jpge    tde   vicap  vdp
		himm(0x20120068, 0x00000000);			// each module 4bit£ºsfc_nand   sfc_nor  nfc  sdio1  sdio0   a7    vpss   vgs 
		himm(0x2012006c, 0x00000000);			// each module 4bit£ºreserve    reserve  avc  usb    cipher  dma2  dma1   gsf
    }

    // ive utili 
    himm(0x206A0000, 0x2);				       // Open utili statistic
    himm(0x206A0080, 0x11E1A300);  		       // Utili peri,default 0x11E1A300 cycle
}

static HI_VOID insert_sns(void)
{

    if (!strcmp(sensor_type, "9m034") || !strcmp(sensor_type, "ar0130"))
    {
		himm(0x200f0040, 0x2);    			// I2C0_SCL
		himm(0x200f0044, 0x2);    			// I2C0_SDA
		
		//cmos pinmux
		himm(0x200f007c, 0x1);    			// VI_DATA13
		himm(0x200f0080, 0x1);    			// VI_DATA10
		himm(0x200f0084, 0x1);    			// VI_DATA12
		himm(0x200f0088, 0x1);    			// VI_DATA11
		himm(0x200f008c, 0x2);    			// VI_VS
		himm(0x200f0090, 0x2);    			// VI_HS
		himm(0x200f0094, 0x1);    			// VI_DATA9
		
		himm(0x2003002c, 0xb4001);			// sensor unreset, clk 27MHz, VI 99MHz
    }
    else if (!strcmp(sensor_type, "ar0230"))
    {
		himm(0x200f0040, 0x2);    			// I2C0_SCL
		himm(0x200f0044, 0x2);    			// I2C0_SDA

        himm(0x20030104, 0x1);           // AVC-148.5M VGS-148.5M VPSS-99M
		himm(0x2003002c, 0xb4005);			// sensor unreset, clk 27MHz, VI 148.5MHz
    }
    else if (!strcmp(sensor_type, "imx222"))
    {
		himm(0x200f0040, 0x1);    				// SPI0_SCLK
		himm(0x200f0044, 0x1);    				// SPI0_SDO
		himm(0x200f0048, 0x1);    				// SPI0_SDI
		himm(0x200f004c, 0x1);    				// SPI0_CSN
		
		//cmos pinmux
		himm(0x200f007c, 0x1);    			// VI_DATA13
		himm(0x200f0080, 0x1);    			// VI_DATA10
		himm(0x200f0084, 0x1);    			// VI_DATA12
		himm(0x200f0088, 0x1);    			// VI_DATA11
		himm(0x200f008c, 0x2);    			// VI_VS
		himm(0x200f0090, 0x2);    			// VI_HS
		himm(0x200f0094, 0x1);    			// VI_DATA9

		himm(0x2003002c, 0x94001);			// sensor unreset, clk 37.125MHz, VI 99MHz	
		//insmod extdrv/sensor_spi.ko;
    }
    else if (!strcmp(sensor_type, "ov9712"))
    {
		himm(0x200f0040, 0x2);    			// I2C0_SCL
		himm(0x200f0044, 0x2);    			// I2C0_SDA
		
		//cmos pinmux
		himm(0x200f007c, 0x1);    			// VI_DATA13
		himm(0x200f0080, 0x1);    			// VI_DATA10
		himm(0x200f0084, 0x1);    			// VI_DATA12
		himm(0x200f0088, 0x1);    			// VI_DATA11
		himm(0x200f008c, 0x2);    			// VI_VS
		himm(0x200f0090, 0x2);    			// VI_HS
		himm(0x200f0094, 0x1);    			// VI_DATA9
		
		himm(0x2003002c, 0xc4001);			// sensor unreset, clk 24MHz, VI 99MHz
    }
	else if (!strcmp(sensor_type, "ov9752") || !strcmp(sensor_type, "ov9750"))
	{
		himm(0x200f0040, 0x2);    			// I2C0_SCL
		himm(0x200f0044, 0x2);    			// I2C0_SDA
		
		himm(0x2003002c, 0xc4001);			// sensor unreset, clk 24MHz, VI 99MHz
	}
    else if (!strcmp(sensor_type, "mn34220"))
    {
		himm(0x200f0040, 0x2);    			// I2C0_SCL
		himm(0x200f0044, 0x2);    			// I2C0_SDA
		
		himm(0x2003002c, 0xc4001);			// sensor unreset, clk 24MHz, VI 99MHz
    }
    else if (!strcmp(sensor_type, "mn34222"))
    {
		himm(0x200f0040, 0x2);    			// I2C0_SCL
		himm(0x200f0044, 0x2);    			// I2C0_SDA
		
		himm(0x2003002c, 0x94001);			// sensor unreset, clk 37.125MHz, VI 99MHz
    }
	else if (!strcmp(sensor_type, "ov4682"))	
	{
		himm(0x200f0040, 0x2);    			// I2C0_SCL
		himm(0x200f0044, 0x2);    			// I2C0_SDA
		
		himm(0x2003002c, 0xc4001);			// sensor unreset, clk 24MHz, VI 99MHz
	}
    else if (!strcmp(sensor_type, "ov9732"))
	{
		himm(0x200f0040, 0x2);    			// I2C0_SCL
		himm(0x200f0044, 0x2);    			// I2C0_SDA
		
		//cmos pinmux
		himm(0x200f007c, 0x1);    			// VI_DATA13
		himm(0x200f0080, 0x1);    			// VI_DATA10
		himm(0x200f0084, 0x1);    			// VI_DATA12
		himm(0x200f0088, 0x1);    			// VI_DATA11
		himm(0x200f008c, 0x2);    			// VI_VS
		himm(0x200f0090, 0x2);    			// VI_HS
		himm(0x200f0094, 0x1);    			// VI_DATA9
		
		himm(0x2003002c, 0xc4001);			// sensor unreset, clk 24MHz, VI 99MHz
	}
    else if (!strcmp(sensor_type, "ov2718"))
	{
		himm(0x200f0040, 0x2);    			// I2C0_SCL
		himm(0x200f0044, 0x2);    			// I2C0_SDA
		
		himm(0x2003002c, 0xc4001);			// sensor unreset, clk 24MHz, VI 99MHz
	}
    else if (!strcmp(sensor_type, "bt1120"))
    {
    }
    else
    {
        printf("sensor_type '%s' is error!!!\n", sensor_type);
    }
}

/*static HI_VOID ir_light_off(void)
{
    // # pin_mux
    himm(0x200f00e0, 0x0);     //# GPIO0_3

    himm(0x20140400, 0xa);      //dir
    himm(0x20140020, 0x8);      //data
}*/

static HI_VOID SYS_cfg(void)
{
    pinmux_hi3518e();
    clkcfg_hi3518e();
    sysctl_hi3518e();
    //ir_light_off();
}

static HI_S32 BASE_init(void)
{
    extern int base_mod_init(void);
    
    return base_mod_init();
}


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
static HI_S32 SYS_init(void)
{
    extern int sys_mod_init(void *pArgs);
    SYS_MODULE_PARAMS_S stSYS_Param;

    stSYS_Param.u32VI_VPSS_online = g_u32vi_vpss_online;
    stSYS_Param.u32SensorNum      = 1;
    snprintf(stSYS_Param.cSensor[0], sizeof(stSYS_Param.cSensor[0]), sensor_type);

    return sys_mod_init(&stSYS_Param);
}

static HI_S32 TDE_init(void)
{
    extern int TDE_DRV_ModInit(void);
    
    return TDE_DRV_ModInit();
}

static HI_S32 REGION_init(void)
{
    extern int rgn_mod_init(void);
    return rgn_mod_init();
}

static HI_S32 VGS_init(void)
{
    extern int vgs_mod_init(void *pArgs);

    return vgs_mod_init(NULL);
}

static HI_S32 ISP_init(void)
{
    extern int isp_mod_init(void *pArgs);
    ISP_MODULE_PARAMS_S stIsp_Param;

    stIsp_Param.u32PwmNum = 1;
    stIsp_Param.u32ProcParam = 0;
    stIsp_Param.u32UpdatePos = 0;
    stIsp_Param.u32LscUpdateMode = 0;

    return isp_mod_init(&stIsp_Param);
}

static HI_S32 VI_init(void)
{
    extern int viu_mod_init(void *pArgs);
    VI_MODULE_PARAMS_S stVI_Param;
    
    stVI_Param.u32DetectErrFrame = 10;
    stVI_Param.u32DropErrFrame = 0;
    stVI_Param.u32StopIntLevel = 0;
    stVI_Param.u32DiscardInt = 0;
    stVI_Param.u32IntdetInterval = 10;
    stVI_Param.bCscTvEnable = HI_FALSE;
    stVI_Param.bCscCtMode = HI_FALSE;
    stVI_Param.bYuvSkip = HI_FALSE;

    return viu_mod_init(&stVI_Param);
}

static HI_S32 VPSS_init(void)
{
    extern int vpss_mod_init(void *pArgs);
    
    return vpss_mod_init(NULL);
}

static HI_S32 VO_init(void)
{
    extern int vou_mod_init(void *pArgs);
    
    return vou_mod_init(NULL);
}

static HI_S32 RC_init(void)
{
    extern int  rc_mod_init(void);
    
    return rc_mod_init();
}

static HI_S32 VENC_init(void)
{
    extern int  venc_mod_init(void * pArgs);
    
    return venc_mod_init(NULL);
}

static HI_S32 CHNL_init(void)
{
    extern int chnl_mod_init(void);

    return chnl_mod_init();
}

static HI_S32 H264E_init(void)
{
    extern int h264e_mod_init(void * pArgs);

    return h264e_mod_init(NULL);
}

static HI_S32 JPEGE_init(void)
{
    extern int jpege_mod_init(void * pArgs);

    return jpege_mod_init(NULL);
}

static HI_S32 SENSOR_I2C_init(void)
{
    extern int hi_dev_init(void);

    return hi_dev_init();
}

static HI_S32 SENSOR_SPI_init(void)
{
    extern int sensor_spi_dev_init(void *pArgs);
    
    SPI_MODULE_PARAMS_S stSpiModuleParam;

    snprintf(stSpiModuleParam.cSensor, 64, sensor_type);
    stSpiModuleParam.u32csn = 0;
    stSpiModuleParam.u32BusNum = 0;

    return sensor_spi_dev_init(&stSpiModuleParam);

}

static HI_S32 PWM_init(void)
{
    extern int pwm_init(void);

    return pwm_init();
}

static HI_S32 Piris_init(void)
{
    extern int piris_init(void);

    return piris_init();
}
static HI_S32 MIPI_init(void)
{
    extern int mipi_init(void);

    return mipi_init();
}

static HI_S32 CIPHER_init(void)
{
    extern int CIPHER_DRV_ModInit(void);

    return CIPHER_DRV_ModInit();
}

static HI_S32 AiaoMod_init(void)
{
    extern  int aiao_mod_init(void);
    
    return aiao_mod_init();
}

static HI_S32 AiMod_init(void)
{
    extern int  ai_mod_init(void);
    
    return ai_mod_init();
}

static HI_S32 AoMod_init(void)
{
    extern int ao_mod_init(void * pArgs);
    
    return ao_mod_init(NULL);
}

static HI_S32 AencMod_init(void)
{
    extern int  aenc_mod_init(void * pArgs);
    
    return aenc_mod_init(NULL);
}

static HI_S32 AdecMod_init(void)
{
    extern int adec_mod_init(void * pArgs);
    
    return adec_mod_init(NULL);
}

#ifdef HI_ACODEC_TYPE_INNER
static HI_S32 AcodecMod_init(void)
{
    extern int acodec_mod_init(void * pArgs);
    
    ACODEC_MODULE_PARAMS_S stModParam;
    stModParam.u32InitDelayTimeMs = 10;
    return acodec_mod_init((void *)&stModParam);
}
#endif

#ifdef HI_ACODEC_TYPE_TLV320AIC31
static HI_S32 tlv320aic31Mod_init(void)
{
    extern int tlv320aic31_init(void);

    return tlv320aic31_init();
}
#endif

static HI_S32 HIFB_init(void)
{
	extern HI_S32 __init hifb_init(void* pArgs);
    HIFB_MODULE_PARAMS_S stHIFB_Param;
    
    snprintf(stHIFB_Param.video, 64, "hifb:vram0_size:1620");
    snprintf(stHIFB_Param.softcursor, 8, "off");

    return hifb_init(&stHIFB_Param);
}
static HI_S32 IveMod_init(void)
{
    extern int ive_mod_init(void *pvArg);
    
    IVE_MODULE_PARAMS_S stParam = {0};	
    stParam.bSavePowerEn = HI_FALSE; 
	
    return ive_mod_init(&stParam);
}

#if 0
static HI_S32 Adv7179Mod_init(void)
{
    ADV7179_MODULE_PARAMS_S stAdv7179Param;
    stAdv7179Param.Norm_mode = 0;

    return adv7179_init(&stAdv7179Param);
}
#endif

extern void osal_proc_init(void);

HI_VOID SDK_init(void)
{
    HI_S32 ret = 0;

    osal_proc_init();

    SYS_cfg();

    ret = MMZ_init();
    if (ret != 0)
    {
        printf("mmz init error.\n");
    }
    
    ret = BASE_init();
    if (ret != 0)
    {
        printf("base init error.\n");
    }

    ret = SYS_init();
    if (ret != 0)
    {
        printf("sys init error.\n");
    }

    ret = TDE_init();
    if (ret != 0)
    {
        printf("tde init error.\n");
    }
    
    ret = REGION_init();
    if (ret != 0)
    {
        printf("region init error.\n");
    }
    
    ret = VGS_init();
    if (ret != 0)
    {
        printf("vgs init error.\n");
    }

    ret = VI_init();
    if (ret != 0)
    {
        printf("vi init error.\n");
    }
    
    ret = ISP_init();
    if (ret != 0)
    {
        printf("isp init error.\n");
    }
    
    ret = VPSS_init();
    if (ret != 0)
    {
        printf("vpss init error.\n");
    }
    
    ret = VO_init();
    if (ret != 0)
    {
        printf("vo init error.\n");
    }
    
    ret = HIFB_init();
    if (ret != 0)
    {
        printf("hifb init error.\n");
    }

    ret = RC_init();
    if (ret != 0)
    {
        printf("rc init error.\n");
    }
    
    ret = VENC_init();
    if (ret != 0)
    {
        printf("venc init error.\n");
    }

    ret = CHNL_init();
    if (ret != 0)
    {
        printf("chnl init error.\n");
    }

    ret = H264E_init();
    if (ret != 0)
    {
        printf("h264e init error.\n");
    }

    ret = JPEGE_init();
    if (ret != 0)
    {
        printf("jpege init error.\n");
    }
    
    ret = SENSOR_I2C_init();
    if (ret != 0)
    {
        printf("sensor'i2c init error.\n");
    }

    ret = SENSOR_SPI_init();
    if (ret != 0)
    {
        printf("sensor'spi init error.\n");
    }
    
    ret = PWM_init();
    if (ret != 0)
    {
        printf("pwm init error.\n");
    }
    ret = Piris_init();
    if (ret != 0)
    {
        printf("piris init error.\n");
    }

    ret = CIPHER_init();
    if (ret != 0)
    {
        printf("cipher init error.\n");
    }
    
    insert_sns();

    ret = MIPI_init();
    if (ret != 0)
    {
        printf("mipi init error.\n");
    }

    ret = AiaoMod_init();
    if (ret != 0)
    {
        printf("aiao init error.\n");
    }
    ret = AiMod_init();
    if (ret != 0)
    {
        printf("ai init error.\n");
    }
    ret = AoMod_init();
    if (ret != 0)
    {
        printf("ao init error.\n");
    }
    ret = AencMod_init();
    if (ret != 0)
    {
        printf("aenc init error.\n");
    }
    ret = AdecMod_init();
    if (ret != 0)
    {
        printf("adec init error.\n");
    }

#ifdef HI_ACODEC_TYPE_INNER
    ret = AcodecMod_init();
    if (ret != 0)
    {
        printf("acodec init error.\n");
    }
#endif

#ifdef HI_ACODEC_TYPE_TLV320AIC31
    ret = tlv320aic31Mod_init();
       if (ret != 0)
       {
           printf("tlv320aic31 init error.\n");
       }
#endif

	audio_disable_mute();
	
    ret = IveMod_init();
    if (ret != 0)
    {
    	printf("Ive  init error.\n");
    }

	//sample config as demo board, don't need this ko.
	/*	
	    ret = Adv7179Mod_init();
	    if (ret != 0)
	    {
	        printf("adv7179 init error.\n");
	    }
	*/
	
    printf("SDK init ok...\n");
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
