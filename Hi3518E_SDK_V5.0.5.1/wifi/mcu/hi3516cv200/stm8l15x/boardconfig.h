#ifndef __BOARDCONFIG_H
#define __BOARDCONFIG_H

#include "stm8l15x.h"
#include "rtc.h"
#include "adc.h"
#include "halt.h"

//#define PIR_SUPPORT		1

#define	KEY1_GPIO				GPIOD			//Power key
#define	KEY1_GPIO_Pin			GPIO_Pin_4

#define	PWR_GPIO				GPIOD			//discover板的灯 是PE7  ，正式的是PB6, 2015/06/13 new board 改为PD1
#define	PWR_GPIO_Pin			 GPIO_Pin_2

#define WIFI_LED_GPIO           GPIOD                           //wifi led
#define WIFI_LED_GPIO_Pin       GPIO_Pin_0

#define PIR_GPIO           GPIOC                           //PIR
#define PIR_GPIO_Pin       GPIO_Pin_4


//判断主控芯片是否已启动
#define REG_ON_DET_GPIO			GPIOC
#define REG_ON_DET_GPIO_Pin		GPIO_Pin_5


//WIFI使能脚
#define	WL_EN_GPIO			GPIOB
#define	WL_EN_GPIO_Pin		    GPIO_Pin_6

//WIFI唤醒中断脚
#define	WL_WAK_GPIO			GPIOB
#define	WL_WAK_GPIO_Pin		    GPIO_Pin_1

//WL_EN状态脚
#define	WL_EN_DET_GPIO			GPIOC
#define	WL_EN_DET_GPIO_Pin		GPIO_Pin_5

//POW_ON状态脚
#define	POW_ON_DET_GPIO			GPIOC
#define	POW_ON_DET_GPIO_Pin		GPIO_Pin_6

//MCU_GPIO1
#define	MCU_GPIO1_GPIO			GPIOC
#define	MCU_GPIO1_GPIO_Pin		    GPIO_Pin_5

//MCU_GPIO2
#define	MCU_GPIO2_GPIO			GPIOC
#define	MCU_GPIO2_GPIO_Pin		    GPIO_Pin_6

//此前预留于VBUS
#define	VBUS_DETECT				GPIOB
#define VBUS_DETECT_GPIO_Pin	GPIO_Pin_7
//此前预留于电池电量
#define	VBAT_DETECT				GPIOD
#define VBAT_DETECT_GPIO_Pin	GPIO_Pin_3

//PB2
#define DEV_PWR_GPIO				GPIOB
#define DEV_PWR_GPIO_Pin			GPIO_Pin_2

//PB3
#define D2H_WAK_GPIO				GPIOB
#define D2H_WAK_GPIO_Pin			GPIO_Pin_3

//PB4
#define	WIFI_GPIO2_GPIO			GPIOB
#define	WIFI_GPIO2_GPIO_Pin		GPIO_Pin_4



//PA5
#define	PWROFF_Req_GPIO			GPIOA
#define	PWROFF_Req_GPIO_Pin		GPIO_Pin_5

//PC2
#define	USART1_RX_GPIO			GPIOC
#define	USART1_RX_GPIO_Pin		GPIO_Pin_2//GPIO_Pin_2

//PC3
#define	USART1_TX_GPIO      	GPIOC
#define USART1_TX_GPIO_Pin		GPIO_Pin_3//GPIO_Pin_3



#define	ADC1_GPIO			        GPIOA
#define	ADC1_GPIO_Pin			        GPIO_Pin_4


#define	LOW_VOL_CHECK_GPIO			GPIOD
#define	LOW_VOL_CHECK_GPIO_Pin			GPIO_Pin_1




/* KEY */
#define	KEY_VALID_LEVEL			0x00
#define	KEY_CHECK_CYCLE			5
#define	KEY_JITTER_CHECK_CYCLE	5
#define	PWR_ON_KEY_KEEP_COUNT	100 //unit is 40ms
#define	PWR_OFF_KEY_KEEP_COUNT	100

/* VBUS and VBAT */
#define VBUS_VALID_LEVEL		1
#define VBAT_VALID_LEVEL		1

#define UF_START 0
#define UF_LEN 1
#define UF_CMD 2
#define UF_DATA 3

#define UF_MAX_LEN 64 //此数需2的n次方，以便后续与操作等效取余操作，以优化指令，提高性能

#define GPIO_DEBOUNCE 2
#define MIN_CMD_LEN 4
#define CMD_LEN 8
#define CMDFLAG_START 0x7B

#define ENABLE_RTC 0  //是否编译RTC定时器

extern u8 g_IsSystemOn;
extern u8 g_Key_Handle_Flag;
extern u8 g_Pir_WakeUp_it;
extern u8 g_Key_WakeUp_It; //按键中断唤醒标志
extern u8 g_Wifi_WakeUp ;  //WIFI唤醒标志
extern u8 g_Pir_Wakeup;//PIR唤醒标志
extern u8 g_Power_Key_Pressed;
extern u8 USART_Send_Buf[UF_MAX_LEN];
extern u8 USART_Receive_Buf[UF_MAX_LEN];
extern u32 USART_Receive_Flag;
extern u16 USART_Receive_Timeout;

extern void InitExitHaltMode(void);

void Board_Init(void);
void CLK_Config(void);
void TIM3_Config(void);
void Usart_Config(void);
void PWR_ON(void);
void PWR_OFF(void);
void USART_Send_Data(unsigned char *Dbuf,unsigned int len);
u8  XOR_Inverted_Check(unsigned char *inBuf,unsigned char inLen);
void Response_CMD_Handle();
void Request_CMD_Handle(u8* cmdstr, u8 len);
void Uart_Handle(void);
void Key_Handle(void);
#endif /* __BOARDCONFIG_H */
