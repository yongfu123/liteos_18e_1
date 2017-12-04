/**
  ******************************************************************************
  * @file    TIM4/TIM4_TimeBase/stm8l15x_it.c
  * @author  MCD Application Team
  * @version V1.5.2
  * @date    30-September-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x_it.h"
#include "boardconfig.h"
#include "kfifo.h"


/** @addtogroup STM8L15x_StdPeriph_Examples
  * @{
  */

/** @addtogroup TIM4_TimeBase
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
    extern u32 couter;
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

#ifdef _COSMIC_
/**
  * @brief Dummy interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(NonHandledInterrupt, 0)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif

/**
  * @brief TRAP interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief FLASH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(FLASH_IRQHandler, 1)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief DMA1 channel0 and channel1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL0_1_IRQHandler, 2)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief DMA1 channel2 and channel3 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL2_3_IRQHandler, 3)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief RTC / CSS_LSE Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler, 4)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
	g_RTC_Alarm_Flag = 1;

	RTC_AlarmCmd(DISABLE);
	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
	RTC_ClearITPendingBit(RTC_IT_ALRA);
}
/**
  * @brief External IT PORTE/F and PVD Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIE_F_PVD_IRQHandler, 5)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PORTB / PORTG Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIB_G_IRQHandler, 6)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PORTD /PORTH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTID_H_IRQHandler, 7)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PIN0 Interrupt routine.
  * @param  None
  * @retval None
  */


 extern u8 g_PIR_Check;
INTERRUPT_HANDLER(EXTI0_IRQHandler, 8)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */

  disableInterrupts();

#if 0
  if(g_IsSystemOn==0)  //PIR从halt模式唤醒
  {
      g_PIR_Check = 1;
  }
#endif

  if(g_IsSystemOn==0)//WIFI从halt模式唤醒
  {
    g_Wifi_WakeUp =1;
  }

  EXTI_ClearITPendingBit(EXTI_IT_Pin0);

  enableInterrupts();

}

/**
  * @brief External IT PIN1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI1_IRQHandler, 9)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
   disableInterrupts();

  if(g_IsSystemOn==0)//开机键 从halt模式唤醒
  {
    g_WakeUp_Halt_Flag =1;
  }

  EXTI_ClearITPendingBit(EXTI_IT_Pin1);

  enableInterrupts();
}

/**
  * @brief External IT PIN2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI2_IRQHandler, 10)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
   u8 key_val;
   disableInterrupts();

  //WIFI待机中电池电量低 检测
  if(g_Bar_Det_Flag==0)
  {
    key_val = GPIO_ReadInputDataBit(BAT_LOW_DET_GPIO, BAT_LOW_DET_GPIO_Pin);
    if(!key_val)//检测到为低
    {
      g_Bar_Det_Flag = 1;
    }
  }

  EXTI_ClearITPendingBit(EXTI_IT_Pin2);
  enableInterrupts();

}

/**
  * @brief External IT PIN3 Interrupt routine.  PB
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI3_IRQHandler, 11)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */

}

/**
  * @brief External IT PIN4 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External IT PIN5 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI5_IRQHandler, 13)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */

}

/**
  * @brief External IT PIN6 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI6_IRQHandler, 14)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */

}

/**
  * @brief External IT PIN7 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI7_IRQHandler, 15)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */

  disableInterrupts();

  if(g_IsSystemOn==0)//WIFI从halt模式唤醒
  {
    g_Wifi_WakeUp =1;
  }

  EXTI_ClearITPendingBit(EXTI_IT_Pin7);

  enableInterrupts();

}
/**
  * @brief LCD /AES Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(LCD_AES_IRQHandler, 16)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief CLK switch/CSS/TIM1 break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SWITCH_CSS_BREAK_DAC_IRQHandler, 17)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief ADC1/Comparator Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(ADC1_COMP_IRQHandler, 18)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief TIM2 Update/Overflow/Trigger/Break /USART2 TX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Timer2 Capture/Compare / USART2 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}


/**
  * @brief Timer3 Update/Overflow/Trigger/Break Interrupt routine.
  * @param  None
  * @retval None
  */
extern u8 g_poweroff_check_flg;//检测是否满足断电条件无按键、无pir触发
extern u8 g_pir_ok_poweroff; // PIR满足关机条件
extern u8 g_key_ok_poweroff; // 按键满足关机条件
extern u8 g_Wifi_Reg_Det_Flag;
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler, 21)
{
#if 0
	u8 key_val;
	static u16 pir_conut = 0;
	static u16 key_keep_count = 0;
	static u16 key_release_count = 0;

        key_val = GPIO_ReadInputDataBit(SW_KEY_GPIO, SW_KEY_GPIO_Pin);
        if (key_val != KEY_VALID_LEVEL) // no key press
        {
              if(g_WakeUp_Key_Flag)
              {
                g_WakeUp_Key_Flag --;
              }

			  if(g_poweroff_check_flg) {
				  key_release_count++;
				  if(key_release_count > 1250 && !g_key_ok_poweroff){
					g_key_ok_poweroff = 1;
				  }
			  }
              key_keep_count = 0;
        }
        else
        {
              if (!g_IsSystemOn) //按键按下  唤醒时计数
              {
                  g_WakeUp_Key_Flag = 1 ;
              }

              //if (!g_IsSystemOn)
              {
				  if(key_keep_count >= 1750){
						g_Key_Handle_Flag = 2;
                    key_keep_count = 0 ;
				  } else if (key_keep_count >=  25 )// PWR_ON_KEY_KEEP_COUNT)   800ms
                  {
                    	g_Key_Handle_Flag = 1;
                    //key_keep_count = 0 ;
                  }
              }

              key_keep_count ++ ;
			  key_release_count = 0;
			  g_key_ok_poweroff = 0;
        }

	key_val = GPIO_ReadInputDataBit(PIR_GPIO, PIR_GPIO_Pin);
	if(key_val == KEY_VALID_LEVEL) //无触发
	{
		if(g_poweroff_check_flg) {
			pir_conut ++;
			if(pir_conut > 1250 && !g_pir_ok_poweroff)
				g_pir_ok_poweroff = 1;
		}
	}
	else {//有触发
		pir_conut = 0;
		g_pir_ok_poweroff = 0;
	}
#endif
	if(USART_Receive_Timeout < U16_MAX)
		USART_Receive_Timeout++;

	TIM3_ClearITPendingBit(TIM3_IT_Update);
}
/**
  * @brief Timer3 Capture/Compare /USART3 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM3_CC_USART3_RX_IRQHandler, 22)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief TIM1 Update/Overflow/Trigger/Commutation Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler, 23)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief TIM1 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_CC_IRQHandler, 24)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief TIM4 Update/Overflow/Trigger Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief SPI1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SPI1_IRQHandler, 26)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief USART1 TX / TIM5 Update/Overflow/Trigger/Break Interrupt  routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQHandler, 27)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}



extern void dev_wlan_power_handle(u8 val);
extern void dev_wlan_reset_handle(u8 val);
extern void host_clr_wkup_reason(void);

/**
  * @brief USART1 RX / Timer5 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler, 28)
{
	unsigned char  temdata;

	/* In order to detect unexpected events during development,
	   it is recommended to set a breakpoint on the following instruction.
	 */
	temdata  = USART_ReceiveData8(USART1);
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	__kfifo_put_singlebyte(&recvfifo, temdata);
}


/**
  * @brief I2C1 / SPI2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler, 29)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
