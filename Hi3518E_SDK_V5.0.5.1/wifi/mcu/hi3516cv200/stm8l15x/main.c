
#include "boardconfig.h"
#include "kfifo.h"

u8 g_Key_WakeUp_It = 0; //按键唤醒标志
u8 g_Pir_WakeUp_it = 0;//PIR中断唤醒标志
u8 g_Wifi_WakeUp = 0;  //WIFI唤醒标志
u8 g_Pir_Wakeup = 0;
u8 g_IsSystemOn = 0; //dv主控芯片的状态，0关机， 1开机
u8 g_Key_Handle_Flag = 0;
u8 g_Power_Key_Pressed = 0;
u8 g_Wifi_Reg_Det_Flag = 1;
u16 USART_Receive_Timeout = 0;
u8 USART_Send_Buf[UF_MAX_LEN];
u8 USART_Receive_Buf[UF_MAX_LEN];
u32 USART_Receive_Flag = 0;
u8 g_IsDevPwrOn = 0; //device power 的状态，0 关机 ，1 开机
u8 g_IsDevWlanEn = 0; //device wlan 的状态，0 disable ，1 enable

u8 g_Iswkupmsgsend = 0;
u8 g_wkup_reason = 0;
u8 g_forceslp_halt = 0;
u8 g_debounce_cnt = 0;
struct kfifo recvfifo;

static void MDelay(u32 delay_count)
{
	u32 i, j;
	for (i = 0; i < delay_count; i++) {
		for(j = 0; j < 15; j++);
	}
}

/**********************************************************************
 * 函数名称：  XOR_Inverted_Check
 * 功能描述：数据的异或取反校验
 ***********************************************************************/
u8  XOR_Inverted_Check(unsigned char *inBuf,unsigned char inLen)
{
	u8 check = 0,i;

	for(i = 0;i < inLen;i++)
		check ^= inBuf[i];

	return ~check;
}

void USART_Send_Data(unsigned char *Dbuf,unsigned int len)
{
	int i;

	//USART1->CR2 |= (uint8_t)USART_CR2_TEN;
	for(i = 0;i < len;i++) {
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData8(USART1,Dbuf[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
	//USART1->CR2 &= (uint8_t)~USART_CR2_TEN;
}

/**********************************************************************
 * 函数名称：UartHandle
 * 功能描述：串口通信数据处理
 ***********************************************************************/
void Uart_Handle(void)
{
	unsigned char check = 0;
	u8 cmdstr[CMD_LEN];
	u8 len = 0;
	u8 rec_len = __kfifo_len(&recvfifo);
	if(rec_len >= MIN_CMD_LEN)
	{
		if(CMDFLAG_START == recvfifo.buffer[(recvfifo.out + UF_START) & (recvfifo.size - 1)])
		{
			len = recvfifo.buffer[(recvfifo.out + UF_LEN) & (recvfifo.size - 1)];
			if(rec_len >= len)
			{
				__kfifo_get(&recvfifo, cmdstr, len);

				check = XOR_Inverted_Check(cmdstr, len - 1);
				if(check == cmdstr[len - 1])
				{
					if (cmdstr[UF_CMD] < 0x80)
						Response_CMD_Handle();
					else
						Request_CMD_Handle(cmdstr, len);
				}

			}
		}
		else
		{
			__kfifo_get(&recvfifo, cmdstr, 1);//如果当前位置不是命令起始标志，则丢弃
		}
	}
}

void dev_pwr_on(void)
{
	g_IsDevPwrOn = 1;
	GPIO_SetBits(DEV_PWR_GPIO,DEV_PWR_GPIO_Pin);
}


void dev_pwr_off(void)
{
	g_IsDevPwrOn = 0;
	GPIO_ResetBits(DEV_PWR_GPIO, DEV_PWR_GPIO_Pin);
}

void dev_wlan_enable(void)
{
	g_IsDevWlanEn = 1;
	GPIO_SetBits(WL_EN_GPIO, WL_EN_GPIO_Pin);
}


void dev_wlan_disable(void)
{
	g_IsDevWlanEn = 0;
	GPIO_ResetBits(WL_EN_GPIO, WL_EN_GPIO_Pin);
}

void dev_wlan_reset_handle(u8 val)
{
	if  (val && (!g_IsDevWlanEn))
	{
		dev_wlan_enable();
	}
	else if ((!val) && g_IsDevWlanEn)
	{
		dev_wlan_disable();
	}
}

void dev_wlan_power_handle(u8 val)
{
	if  (val && (!g_IsDevPwrOn))
	{
		dev_pwr_on();
	}
	else if ((!val) && g_IsDevPwrOn)
	{
		dev_pwr_off();
	}
}

void host_clr_wkup_reason(void)
{
	GPIO_ResetBits(MCU_GPIO1_GPIO, MCU_GPIO1_GPIO_Pin);
	g_wkup_reason = 0;
}

void dev_wkup_host(void)
{
#if 1
	GPIO_SetBits(MCU_GPIO2_GPIO, MCU_GPIO2_GPIO_Pin);
	MDelay(1);
	GPIO_ResetBits(MCU_GPIO2_GPIO, MCU_GPIO2_GPIO_Pin);
#else
	USART_Send_Buf[UF_START] = 0x7B;
	USART_Send_Buf[UF_LEN] = 0x4;
	USART_Send_Buf[UF_CMD] = 0xbb;
	USART_Send_Buf[USART_Send_Buf[UF_LEN] - 1] = XOR_Inverted_Check(USART_Send_Buf, USART_Send_Buf[UF_LEN] - 1);
	USART_Send_Data(USART_Send_Buf, USART_Send_Buf[UF_LEN]);
#endif
}

void host_set_wkup_reason(u8 val)
{
	if(1 & val)
	{
		GPIO_SetBits(MCU_GPIO1_GPIO, MCU_GPIO1_GPIO_Pin);
	}
	else
	{
		GPIO_ResetBits(MCU_GPIO1_GPIO, MCU_GPIO1_GPIO_Pin);
	}
}

void Request_CMD_Handle(u8* cmdstr, u8 len)
{
	int ret;

	USART_Send_Buf[UF_START] = 0x7B;

	switch (cmdstr[UF_CMD]) {
		case 0x80:
			PWR_OFF();
			MDelay(5);
			g_wkup_reason = 1;
#if 0
			USART_Send_Buf[UF_LEN] = 0x4;
			USART_Send_Buf[UF_CMD] = 0x81;
			USART_Send_Buf[USART_Send_Buf[UF_LEN] - 1] = XOR_Inverted_Check(USART_Send_Buf, USART_Send_Buf[UF_LEN] - 1);
			USART_Send_Data(USART_Send_Buf, USART_Send_Buf[UF_LEN]);
			timeout = 0;
			while (timeout++ < 3000) {
				if (GPIO_ReadInputDataBit(PWROFF_Req_GPIO, PWROFF_Req_GPIO_Pin)) {
					break;
				}
				MDelay(1);
			}
#endif
			break;

		case 0x8a:
			dev_wlan_power_handle(cmdstr[UF_DATA]);
			break;

		case 0x88:
			dev_wlan_reset_handle(cmdstr[UF_DATA]);
			break;

		case 0x8c:
			host_clr_wkup_reason();
			break;

		case 0x84:
			if(cmdstr[UF_DATA] == 0)
				g_Wifi_Reg_Det_Flag = 0;
			else
				g_Wifi_Reg_Det_Flag = 1;
#if 0
			USART_Send_Buf[UF_LEN] = 0x4;
			USART_Send_Buf[UF_CMD] = 0x85;
			USART_Send_Buf[USART_Send_Buf[UF_LEN] - 1] = XOR_Inverted_Check(USART_Send_Buf, USART_Send_Buf[UF_LEN] - 1);
			USART_Send_Data(USART_Send_Buf, USART_Send_Buf[UF_LEN]);
#endif
			break;

		case 0x86:
			ret = RTC_Alarm_Duration_Check(cmdstr[UF_DATA + 1], cmdstr[UF_DATA + 2]);
			if (!ret) {
				RTC_AlarmCmd(DISABLE);
				RTC_ITConfig(RTC_IT_ALRA, DISABLE);
				RTC_ClearITPendingBit(RTC_IT_ALRA);
				g_Alarm_Event.flag = cmdstr[UF_DATA];
				g_Alarm_Event.hours = cmdstr[UF_DATA + 1];
				g_Alarm_Event.minutes = cmdstr[UF_DATA + 2];
				USART_Send_Buf[UF_DATA] = 0;
			} else {
				USART_Send_Buf[UF_DATA] = 1;
			}

			USART_Send_Buf[UF_LEN] = 0x5;
			USART_Send_Buf[UF_CMD] = 0x87;
			USART_Send_Buf[USART_Send_Buf[UF_LEN] - 1] = XOR_Inverted_Check(USART_Send_Buf, USART_Send_Buf[UF_LEN] - 1);
			USART_Send_Data(USART_Send_Buf, USART_Send_Buf[UF_LEN]);
			break;
		default:
			break;
	}
}

void Response_CMD_Handle()
{
	;
}

void CLK_Config(void)
{
	CLK_DeInit();

	CLK_HSICmd(ENABLE);
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_32);//CLK_SYSCLKDiv_32
	while (((CLK->ICKCR)& 0x02)!=0x02);  //HSI准备就绪

	CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
	CLK_SYSCLKSourceSwitchCmd(ENABLE);
	while (((CLK->SWCR)& 0x01)==0x01);  //切换完成

}

void TIM3_Config(void)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);/* Enable TIM3 CLK */
	/* Time base configuration */
	TIM3_TimeBaseInit(TIM3_Prescaler_16,TIM3_CounterMode_Up,125);//20MS 2480------------4ms 499   // 0.5M 4ms 625  1ms - 125
	/* Clear TIM4 update flag */
	TIM3_ClearFlag(TIM3_FLAG_Update);
	/* Enable update interrupt */
	TIM3_ITConfig(TIM3_IT_Update, ENABLE);
	/* Enable TIM4 */
	TIM3_Cmd(ENABLE);
}


void Usart_Config(void)
{
	GPIO_Init(USART1_RX_GPIO,USART1_RX_GPIO_Pin,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(USART1_TX_GPIO,USART1_TX_GPIO_Pin,GPIO_Mode_Out_PP_Low_Fast);
	CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
	USART_Init(USART1,9600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,(USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);  //配置USART1->SR
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);    //配置USART1->CR2的RIEN位
	USART_Cmd(USART1,ENABLE);
}


void PWR_ON(void)
{
	g_IsSystemOn = 1;
	GPIO_SetBits(PWR_GPIO,PWR_GPIO_Pin);
	GPIO_ResetBits(WIFI_LED_GPIO,WIFI_LED_GPIO_Pin);
}


void PWR_OFF(void)
{
	g_IsSystemOn = 0;
	GPIO_ResetBits(PWR_GPIO,PWR_GPIO_Pin);
	GPIO_SetBits(WIFI_LED_GPIO,WIFI_LED_GPIO_Pin);
}

void PWR_Detect_Handle(u8 delay_val)
{
	u8 val;
	static u8 vbus_detect_last = !VBUS_VALID_LEVEL;

	if (g_IsSystemOn == 0) {
		val = !!GPIO_ReadInputDataBit(VBUS_DETECT, VBUS_DETECT_GPIO_Pin);
		if (val == VBUS_VALID_LEVEL) {
			if (vbus_detect_last != VBUS_VALID_LEVEL) {
				MDelay(delay_val);
				PWR_ON();
			}
		}
		vbus_detect_last = val;
	}
	return;
}

void Pin_Det_Handle(void)
{
	u8 val;

#if 0
	/* wlan wake detect handle */

	if(!g_IsSystemOn)
	{
		if(g_Wifi_WakeUp == 1)
		{
			InitExitHaltMode();
			PWR_ON();
			g_Wifi_WakeUp = 0;
		}
	}


	if (g_IsSystemOn && g_Wifi_Reg_Det_Flag)
	{
		val = !!GPIO_ReadInputDataBit(REG_ON_DET_GPIO, REG_ON_DET_GPIO_Pin);
		if (val) {
			GPIO_SetBits(WL_EN_GPIO, WL_EN_GPIO_Pin);
			//GPIO_SetBits(REG_ON_GPIO, REG_ON_GPIO_Pin);
		} else {
			//	GPIO_ResetBits(WL_EN_GPIO, WL_EN_GPIO_Pin);
			//GPIO_ResetBits(REG_ON_GPIO, REG_ON_GPIO_Pin);
		}
	}
#endif

#if 0
	//WLEN DETECT
	val = !!GPIO_ReadInputDataBit(MCU_GPIO1_GPIO, MCU_GPIO1_GPIO_Pin);
	if  (val && (!g_IsDevWlanEn))
		dev_wlan_enable();
	else if ((!val) && g_IsDevWlanEn)
		dev_wlan_disable();

	//PWR DETECT
	val = !!GPIO_ReadInputDataBit(MCU_GPIO2_GPIO, MCU_GPIO2_GPIO_Pin);
	if  (val && (!g_IsDevPwrOn))
		dev_pwr_on();
	else if ((!val) && g_IsDevPwrOn)
		dev_pwr_off();
#endif
	// DEV WAK HOST DETECT
	if(!g_IsSystemOn)
	{
		val = !!GPIO_ReadInputDataBit(D2H_WAK_GPIO, D2H_WAK_GPIO_Pin);
		if(val)
		{
			kfifo_init(&recvfifo);
			g_debounce_cnt++;
			if(GPIO_DEBOUNCE < g_debounce_cnt)
			{
				PWR_ON();
				MDelay(2);
				host_set_wkup_reason(g_wkup_reason);
				InitExitHaltMode();
				GPIO_SetBits(VBAT_DETECT, VBAT_DETECT_GPIO_Pin);
			}
		}
		else
		{
			g_Wifi_WakeUp = 0;
			g_debounce_cnt = 0;
		}
	}
	else
	{
		val = !!GPIO_ReadInputDataBit(D2H_WAK_GPIO, D2H_WAK_GPIO_Pin);
		if(val)
		{
			if(!g_Iswkupmsgsend)
			{
				dev_wkup_host();
				g_Iswkupmsgsend = 1;
			}
		}
		else
		{
			g_Wifi_WakeUp = 0;
			g_Iswkupmsgsend = 0;
		}
	}

}

void Board_Init(void)
{
	g_IsSystemOn = 0;
	g_Key_Handle_Flag = 0;

	USART_Receive_Timeout = 0;
	USART_Receive_Flag = 0;

	GPIO_Init(KEY1_GPIO, KEY1_GPIO_Pin, GPIO_Mode_In_FL_IT);

	GPIO_Init(ADC1_GPIO, ADC1_GPIO_Pin, GPIO_Mode_Out_OD_HiZ_Slow);

	GPIO_Init(LOW_VOL_CHECK_GPIO, LOW_VOL_CHECK_GPIO_Pin, GPIO_Mode_In_FL_IT);

	GPIO_Init(WIFI_LED_GPIO, WIFI_LED_GPIO_Pin, GPIO_Mode_Out_PP_High_Fast);

	GPIO_Init(PWR_GPIO, PWR_GPIO_Pin, GPIO_Mode_Out_PP_Low_Fast);

	GPIO_Init(WL_EN_GPIO, WL_EN_GPIO_Pin, GPIO_Mode_Out_PP_Low_Fast);

	GPIO_Init(DEV_PWR_GPIO, DEV_PWR_GPIO_Pin, GPIO_Mode_Out_PP_Low_Fast);

	GPIO_Init(REG_ON_DET_GPIO, REG_ON_DET_GPIO_Pin, GPIO_Mode_In_PU_No_IT);


#ifdef PIR_SUPPORT
	GPIO_Init(PIR_GPIO, PIR_GPIO_Pin, GPIO_Mode_In_FL_No_IT);
#endif

	GPIO_Init(MCU_GPIO1_GPIO, MCU_GPIO1_GPIO_Pin, GPIO_Mode_Out_PP_Low_Fast);

	GPIO_Init(MCU_GPIO2_GPIO, MCU_GPIO2_GPIO_Pin, GPIO_Mode_Out_PP_Low_Fast);

	//将WIFI开机管脚设为中断模式
	GPIO_Init(WL_WAK_GPIO,WL_WAK_GPIO_Pin,GPIO_Mode_In_FL_IT);
	//然后配置中断1为上升沿低电平触发
	EXTI_SetPinSensitivity(EXTI_Pin_1, EXTI_Trigger_Rising);
	//设置中断的优先级
	ITC_SetSoftwarePriority(EXTI1_IRQn, ITC_PriorityLevel_1);

	CLK_Config();
}


//test RTC
void Param_test_Init(void)
{
	g_Wifi_WakeUp = 0;

	// test enter halt mode  after 1min use rtc wake up STM8
#if ENABLE_RTC

	g_Alarm_Event.flag = 0x03;
	g_Alarm_Event.hours = 0;
	g_Alarm_Event.minutes = 1;

#endif

}

void PullWifiRegon(void)
{

	GPIO_ResetBits(WL_EN_GPIO, WL_EN_GPIO_Pin);

	MDelay(10);

	GPIO_SetBits(WL_EN_GPIO, WL_EN_GPIO_Pin);

}

void init_func(void)
{
	TIM3_Config();

	Usart_Config();
}
/***************************************************************
  LED_Delay
  控制LED灯 闪烁间隔,850 -------- 1S

 ****************************************************************/
void LED_Delay(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin, uint32_t time)
{
	if(time <= 0)
		time = 425;

	GPIO_SetBits(GPIOx,GPIO_Pin);
	MDelay(time);
	GPIO_ResetBits(GPIOx,GPIO_Pin);
	MDelay(time);
}

/***************************************************************
  Pir_Check
  热感红外检测。检测PC4口是否为高电平
 ****************************************************************/
void Pir_Check(void)
{
	static u8 g_pir_state = 0;

	if(g_pir_state != g_Pir_Wakeup){
		g_pir_state = g_Pir_Wakeup;
		if(g_pir_state){
			if(g_IsSystemOn == 0)
				PWR_ON();
		}else {
			if(g_IsSystemOn == 1)
				PWR_OFF();
		}
	}
}

/***************************************************************
  Key_Handle

  按键处理
 ****************************************************************/
void Key_Handle(void)
{
	static u8 power_on_off = 0;

	if (g_Key_Handle_Flag) {
		if (power_on_off == 0) {
			PWR_ON();
			power_on_off = 1;
		} else {
#if 1
			u32 val = 0;
			u32 timeout = 0;
			USART_Send_Buf[UF_START] = 0x7B;
			USART_Send_Buf[UF_LEN] = 0x4;
			USART_Send_Buf[UF_CMD] = 0x0;
			USART_Send_Buf[USART_Send_Buf[UF_LEN] - 1] = XOR_Inverted_Check(USART_Send_Buf, USART_Send_Buf[UF_LEN] - 1);
			USART_Send_Data(USART_Send_Buf, USART_Send_Buf[UF_LEN]);

			MDelay(300);

			while(timeout++ < 2400) {
				//Uart_Handle();
				//MDelay(1);
				//if(g_IsSystemOn == 0) {
				//	power_on_off = 0;
				//	break;
				//}
				// val = GPIO_ReadInputDataBit(MCU_GPIO2_GPIO, MCU_GPIO2_GPIO_Pin);
				//if(val != 0x0) {
				//	break;
				//}

				val = GPIO_ReadInputDataBit(REG_ON_DET_GPIO, REG_ON_DET_GPIO_Pin);
				if(val != 0x0) {
					break;
				}
			}
#endif
			PWR_OFF();
			power_on_off = 0;
		}
		g_Key_Handle_Flag = 0;
	}
}


void main(void)
{
	GPIO_init();

	Board_Init();

	PWR_ON();

	init_func();

	PullWifiRegon();

	enableInterrupts();

	kfifo_init(&recvfifo);

	while(1)
	{
		//LED_Delay(WIFI_LED_GPIO,WIFI_LED_GPIO_Pin,0);

		//Key_Handle();

		Pin_Det_Handle();

#ifdef PIR_SUPPORT
		Pir_Check();
#endif

		Uart_Handle();

		Change_Mode();

		MDelay(1);

	}
}



