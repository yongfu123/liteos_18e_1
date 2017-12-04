#include "halt.h"
#include "boardconfig.h"

void InitEnterHaltMode(void)
{
    disableInterrupts();

 //将开关机管脚设为中断模式
    //GPIO_Init(KEY1_GPIO,KEY1_GPIO_Pin,GPIO_Mode_In_PU_IT);

#ifdef PIR_SUPPORT
 //将PIR管脚设为中断模式
   GPIO_Init(PIR_GPIO,PIR_GPIO_Pin,GPIO_Mode_In_PU_IT);
#endif

 //然后配置中断1为下降沿低电平触发
    //EXTI_SetPinSensitivity(EXTI_Pin_4, EXTI_Trigger_Rising_Falling);

 //设置中断的优先级
    GPIO_Init(D2H_WAK_GPIO,D2H_WAK_GPIO_Pin,GPIO_Mode_In_FL_IT);

    /*然后配置中断1为下降沿低电平触发*/
    EXTI_SetPinSensitivity(EXTI_Pin_3, EXTI_Trigger_Rising);

    /*设置中断的优先级*/
    ITC_SetSoftwarePriority(EXTI3_IRQn, ITC_PriorityLevel_1);

    /*关闭串口*/
    GPIO_Init(USART1_RX_GPIO,USART1_RX_GPIO_Pin,GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(USART1_TX_GPIO,USART1_TX_GPIO_Pin,GPIO_Mode_Out_PP_Low_Slow);
    GPIO_ResetBits(GPIOC, GPIO_Pin_5);
    GPIO_ResetBits(GPIOC, GPIO_Pin_6);
    USART_Cmd(USART1,DISABLE);
    USART_DeInit(USART1);

 	TIM3_DeInit();
    USART_DeInit(USART1);

    enableInterrupts();

}

void InitExitHaltMode(void)
{
    disableInterrupts();

    //将开关机管脚设为普通IO口模式
    GPIO_Init(KEY1_GPIO, KEY1_GPIO_Pin, GPIO_Mode_In_PU_No_IT);

#ifdef  PIR_SUPPORT
    GPIO_Init(PIR_GPIO,PIR_GPIO_Pin,GPIO_Mode_In_FL_No_IT);
#endif

    TIM3_Config();

    Usart_Config();

    enableInterrupts();
}

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
  GPIO_Init(GPIOC, GPIO_Pin_All, GPIO_Mode_Out_PP_Low_Slow);

/* Port D in output push-pull 0 */
  GPIO_Init(GPIOD,GPIO_Pin_All, GPIO_Mode_Out_PP_Low_Slow);

}
extern u8 g_forceslp_halt;

void Change_Mode(void)
{
// key halt mode enter normal mode
      if(g_Key_WakeUp_It)
      {
        InitExitHaltMode();
        g_Key_WakeUp_It = 0;
        PWR_ON();
      }

//Pir halt mode enter normal mode
#ifdef PIR_SUPPORT
	if(g_Pir_WakeUp_it) {
		InitExitHaltMode();
		g_Pir_WakeUp_it = 0;
		PWR_ON();
	}
#endif

//normal mode enter halt mode
    if (!g_Wifi_WakeUp)
    {
    	 if(g_IsSystemOn == 0)
    	 {
    	   InitEnterHaltMode();
    	   Halt_Mode();
    	 }
    }
}

void Halt_Mode(void)
{
  //ENTER ACTIVE HALT CLOSE the main voltage regulator is powered off
  CLK->ICKCR|=CLK_ICKCR_SAHALT;

  // Set STM8 in low power
  PWR->CSR2 = 0x2;

  //  low power fast wake up disable
  PWR_FastWakeUpCmd(DISABLE);

  // Stop RTC Source clock
#if (!ENABLE_RTC)

   CLK_RTCClockConfig(CLK_RTCCLKSource_Off, CLK_RTCCLKDiv_1);
   CLK_PeripheralClockConfig(CLK_Peripheral_RTC, DISABLE);

#endif

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




