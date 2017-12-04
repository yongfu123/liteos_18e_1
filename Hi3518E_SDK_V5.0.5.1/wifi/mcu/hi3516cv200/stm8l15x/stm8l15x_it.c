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
INTERRUPT_HANDLER(EXTI0_IRQHandler, 8)
{
	/* In order to detect unexpected events during development,
	   it is recommended to set a breakpoint on the following instruction.
	 */

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

	if(g_IsSystemOn==0)//WIFI��haltģʽ����
	{
		g_Wifi_WakeUp =1;
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
	disableInterrupts();
	if(g_IsSystemOn==0)//WIFI��haltģʽ����
	{
		g_Wifi_WakeUp =1;
	}
	EXTI_ClearITPendingBit(EXTI_IT_Pin3);
	enableInterrupts();
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

	disableInterrupts();

#ifdef PIR_SUPPORT
	if(g_Power_Key_Pressed) {
		u8 pir_state = GPIO_ReadInputDataBit(PIR_GPIO,PIR_GPIO_Pin);

		if(pir_state) {
			if(g_IsSystemOn == 0)//������ ��haltģʽ����
			{
				g_Pir_WakeUp_it = 1; //PIR�жϻ��ѱ�־
			}
		}
	}
#endif

	{
		u8 key_state = GPIO_ReadInputDataBit(KEY1_GPIO,KEY1_GPIO_Pin);
		if(key_state == 0x00) {
			if(g_IsSystemOn == 0) {  //������ ��haltģʽ����
				g_Key_WakeUp_It = 1;
				g_Power_Key_Pressed = 1;
				g_Key_Handle_Flag = 1;
			}
		}
	}

	EXTI_ClearITPendingBit(EXTI_IT_Pin4);

	enableInterrupts();
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
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler, 21)
{
	//static u8 timecount = 0;
	//static u8 key_val_last = !KEY_VALID_LEVEL;
	static u8 key_reless = 1;
	static u16 key_keep_count = 0;
	u8 key_val= 0;;

	key_val = GPIO_ReadInputDataBit(KEY1_GPIO, KEY1_GPIO_Pin);
#if 0
	{
		static u8 key_press = 0;
		//�ɿ�ִ��
		if(key_val == 0x10){
			//�ߵ�ƽ,�����ɿ�
			if(key_press){
				key_reless = 1;
				key_press = 0;
				g_Key_Handle_Flag = 1;
			}
			key_keep_count = 0;
		}
		else {
			//��������,��ʱ����ʱ��
			if(key_keep_count > 100) {//4ms * 100  = 400ms
				if(key_reless){
					key_reless = 0;
					key_press = 1;
				}
			} else {
				key_keep_count++;
			}
		}
	}
#else
	//����ִ��
	if(key_val == 0x10){
		//�ߵ�ƽ,�����ɿ�
		key_reless = 1;
		key_keep_count = 0;
	}
	else {
		//��������,��ʱ����ʱ��
		if(key_keep_count > 625) {//4ms * 150  = 600ms
			if(key_reless){
				g_Key_Handle_Flag = 1;
				key_reless = 0;

				g_Power_Key_Pressed = !g_Power_Key_Pressed;
			}
		} else {
			key_keep_count++;
		}
	}
#endif

#ifdef PIR_SUPPORT
	{
		if(g_Power_Key_Pressed) {//�������ȼ�ֻ�а�������֮�󣬲��������PIR
			u8 pir_val = 0;
			static int low_cnt = 0;

			pir_val = GPIO_ReadInputDataBit(PIR_GPIO,PIR_GPIO_Pin);

			if(pir_val == 0x10){
				low_cnt = 0;
				g_Pir_Wakeup = 1;
			} else {
				if(low_cnt > 5000){
					g_Pir_Wakeup = 0;
					low_cnt = 0;
				} else {
					++low_cnt;
				}
			}
		}
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



