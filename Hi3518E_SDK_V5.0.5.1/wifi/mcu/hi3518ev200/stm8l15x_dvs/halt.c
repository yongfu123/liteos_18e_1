#include "halt.h"
#include "boardconfig.h"

/******************************************************************
*函数名称：InitEnterHaltMode
*
*功能描述：进入低功耗模式前的配置
* para:
	    void
*******************************************************************/
void InitEnterHaltMode(void)
{
	/*关闭中断*/
    disableInterrupts();
#if 0
    /*将开关机管脚设为中断模式*/
    GPIO_Init(SW_KEY_GPIO,SW_KEY_GPIO_Pin,GPIO_Mode_In_PU_IT);

    /*然后配置中断1为下降沿低电平触发*/
    EXTI_SetPinSensitivity(EXTI_Pin_1, EXTI_Trigger_Falling);

    /*设置中断的优先级*/
    ITC_SetSoftwarePriority(EXTI1_IRQn, ITC_PriorityLevel_1);
#endif
    GPIO_Init(D2H_WAK_GPIO,D2H_WAK_GPIO_Pin,GPIO_Mode_In_FL_IT);

    /*然后配置中断1为下降沿低电平触发*/
    EXTI_SetPinSensitivity(EXTI_Pin_0, EXTI_Trigger_Rising);

    /*设置中断的优先级*/
    ITC_SetSoftwarePriority(EXTI0_IRQn, ITC_PriorityLevel_1);


 	/*关闭定时器中断*/
    TIM3_DeInit();
    /*关闭串口*/
    GPIO_Init(USART_RX_GPIO,USART_RX_GPIO_Pin,GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(USART_TX_GPIO,USART_TX_GPIO_Pin,GPIO_Mode_Out_PP_Low_Slow);

    GPIO_ResetBits(USART_TX_GPIO, USART_TX_GPIO_Pin);
    GPIO_ResetBits(USART_RX_GPIO, USART_RX_GPIO_Pin);

    USART_Cmd(USART1,DISABLE);
    USART_DeInit(USART1);

	/*关闭ADC采样*/
    ADC_Deinit_adc0();
    
    /*若需开启PIR中断唤醒，则设置PIR为中断模式*/
    extern u8 g_Wifi_Reg_Det_Flag;
    if(g_Wifi_Reg_Det_Flag==0) {
      GPIO_Init(PIR_GPIO, PIR_GPIO_Pin, GPIO_Mode_In_FL_IT);
    } else {
      GPIO_Init(PIR_GPIO, PIR_GPIO_Pin, GPIO_Mode_Out_OD_HiZ_Slow);
    }
    /*开启中断*/
    enableInterrupts();
}

/******************************************************************
*函数名称：InitExitHaltMode
*
*功能描述：退出低功耗模式相关配置
* para:
	    void
*******************************************************************/
void InitExitHaltMode(void)
{
  disableInterrupts();

  GPIO_Init(PIR_GPIO,PIR_GPIO_Pin,GPIO_Mode_In_FL_No_IT);

  TIM3_Config();

  ADC_Init_adc0();

  Usart_Config();

  enableInterrupts();
}

/******************************************************************
*函数名称：GPIO_init
*
*功能描述：GPIO初始化
* para:
	    void
*******************************************************************/
void GPIO_init(void)
{
  GPIO_DeInit(GPIOA);

  GPIO_DeInit(GPIOB);

  GPIO_DeInit(GPIOC);

  GPIO_DeInit(GPIOD);

/* Port A in output push-pull 0 */
  GPIO_Init(GPIOA,GPIO_Pin_All,GPIO_Mode_Out_PP_Low_Slow);

/* Port B in output push-pull 0 */
  GPIO_Init(GPIOB, GPIO_Pin_All, GPIO_Mode_Out_PP_Low_Slow);

/* Port C in output push-pull 0 */
   GPIO_Init(GPIOC,GPIO_Pin_All, GPIO_Mode_Out_PP_Low_Slow);

/* Port D in output push-pull 0 */
  GPIO_Init(GPIOD,GPIO_Pin_All, GPIO_Mode_Out_PP_Low_Slow);
}

/**********************************************************************
* 函数名称：  ALL_PWR_ON
* 功能描述：给所有功能器件上电，包括主控和wifi
* para:
		void
***********************************************************************/
void ALL_PWR_ON(void)
{

    GPIO_SetBits(PWR_EN_GPIO, PWR_EN_GPIO_Pin);
    //MDelay(10);
    GPIO_SetBits(PWR_HOLD_GPIO, PWR_HOLD_GPIO_Pin);

    g_IsSystemOn = 1;

}
/**********************************************************************
* 函数名称：  ALL_PWR_OFF
* 功能描述：主控和wifi切断电源，只保留MCU低功耗，
				     进入高节能低功耗模式
* para:
		void
***********************************************************************/
void ALL_PWR_OFF(void)
{
    GPIO_ResetBits(PWR_HOLD_GPIO, PWR_HOLD_GPIO_Pin);

    g_IsSystemOn = 0;
}

/******************************************************************
*函数名称：BAR_Detect
*
*功能描述：待机状态下电池电量过低关闭WIFI供电进入深入待机
* para:
	    void
*******************************************************************/
void BAR_Detect(void)
{
     u8 key_val = 0;

    /*WIFI待机中电池电量低 检测*/
    key_val = GPIO_ReadInputDataBit(BAT_LOW_DET_GPIO, BAT_LOW_DET_GPIO_Pin); //检测到为低

    if(g_Bar_Det_Flag||(!key_val))
    {
        ALL_PWR_OFF();
        g_Bar_Det_Flag=0;
    }
}

/******************************************************************
*函数名称：Change_Mode
*
*功能描述：正常模式和低功耗模式切换
* para:
	  void
*******************************************************************/
void Change_Mode(void)
{
	/*当检测到电池电量< 3v进入深度待机*/
	//BAR_Detect();

	/*  normal mode enter halt mode*/
	/*延时开机用，持续按键时不使MCU进入低功耗 */
    if(!g_Wifi_WakeUp)
    {
        if(g_IsSystemOn==0)	{
        /*按键无持续按下或无按键按下*/
            if(!g_WakeUp_Key_Flag) {
                /*配置相关IO模式*/
                InitEnterHaltMode();
                /*进入低功耗模式*/
                Halt_Mode();
            }
		}
    }
	// halt mode enter normal mode
	/*按键中断*/
	if(g_WakeUp_Halt_Flag||g_Vbus_Check) {
		/*确保是关机状态下退出halt模式*/
		if(g_IsSystemOn==0) {
			/*退出低功耗的相关配置*/
			InitExitHaltMode();
			/*按键唤醒*/
			g_WakeUp_Halt_Flag=0;
			/*检测是否持续按下按键标识*/
			g_WakeUp_Key_Flag = 10;
		}
	}
}


/******************************************************************
*函数名称：Halt_Mode
*
*功能描述：低功耗模式接口
* para:
	  void
*******************************************************************/
void Halt_Mode(void)
{
	/*ENTER ACTIVE HALT CLOSE the main voltage regulator is powered off*/
	CLK->ICKCR|=CLK_ICKCR_SAHALT;

	/* Set STM8 in low power */
	PWR->CSR2 = 0x2;

	/*  low power fast wake up disable*/
	PWR_FastWakeUpCmd(DISABLE);

	CLK_RTCClockConfig(CLK_RTCCLKSource_Off, CLK_RTCCLKDiv_1);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, DISABLE);

	CLK_LSICmd(DISABLE);
	while ((CLK->ICKCR & 0x04) != 0x00);

	CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_TIM5, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_USART2, DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_USART3, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_BEEP, DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_DAC, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_LCD, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_BOOTROM, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_COMP, DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_AES, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_SPI2, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_CSSLSE, DISABLE);


	CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE);

	CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);

	enableInterrupts();

	halt(); //system go to halt mode;
}
