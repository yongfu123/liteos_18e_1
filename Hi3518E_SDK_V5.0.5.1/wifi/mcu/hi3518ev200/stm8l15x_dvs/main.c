
#include "boardconfig.h"
#include "kfifo.h"

u8 g_WakeUp_Halt_Flag = 0; //按键唤醒标志

u8 g_Wifi_WakeUp = 0;  //WIFI唤醒标志
u8 g_KeyPoweroff_Flag = 0; //按键关机标志
u8 g_Vbus_Check = 0; //VBUS检测

u8 g_PIR_Check = 0; //PIR检测
u8 g_IsSystemOn = 0; //dv主控芯片的状态，0关机， 1开机
u8 g_Key_Handle_Flag = 0;

u8 g_Wifi_Reg_Det_Flag = 1;

u8 g_Bar_Det_Flag = 0; //wifi待机时的电池检测

u8 g_poweroff_check_flg = 0;//检测是否满足断电条件无按键、无pir触发
u8 g_pir_ok_poweroff = 0; // PIR满足关机条件
u8 g_key_ok_poweroff = 0; // 按键满足关机条件
u8 g_host_readly_uart = 0;//标志主控芯片uart准备OK
u16 g_WakeUp_Key_Flag = 0;
u16 USART_Receive_Timeout = 0;
u8 USART_Send_Buf[UF_MAX_LEN];
u8 USART_Receive_Buf[UF_MAX_LEN];
u32 USART_Receive_Flag = 0;
u32 g_time_count = 0;


u8 g_IsDevPwrOn = 0; //device power 的状态，0 关机 ，1 开机
u8 g_IsDevWlanEn = 0; //device wlan 的状态，0 disable ，1 enable
u8 g_Iswkupmsgsend = 0;
u8 g_wkup_reason = 0;

u8 g_debounce_cnt = 0;
#define GPIO_DEBOUNCE 2
struct kfifo recvfifo;

/*设置波特率，默认为9600*/
#define BAUD_RATE 9600

#if (BAUD_RATE == 115200)
#define BAUD_CLK  CLK_SYSCLKDiv_1
#elif (BAUD_RATE == 9600)
#define BAUD_CLK  CLK_SYSCLKDiv_32
#endif


/**********************************************************************
 * 函数名称：  MDelay
 * 功能描述：延时函数，单位为ms
 * para:
delay_count:  延时时长
 ***********************************************************************/
static void MDelay(u32 delay_count)
{
	u32 i, j;

	for (i = 0; i < delay_count; i++)
	{
		for (j = 0; j < 50; j++);
	}
}

/**********************************************************************
 * 函数名称：  PWR_ON
 * 功能描述：给主控上电
 * para:
 void
 ***********************************************************************/
void PWR_ON(void)
{
	GPIO_SetBits(PWR_HOLD_GPIO, PWR_HOLD_GPIO_Pin);
	g_IsSystemOn = 1;
}

/**********************************************************************
 * 函数名称：  PWR_OFF
 * 功能描述：切断主控电源
 * para:
 void
 ***********************************************************************/
void PWR_OFF(void)
{
	GPIO_ResetBits(PWR_HOLD_GPIO, PWR_HOLD_GPIO_Pin);
	GPIO_ResetBits(MCU_GPIO2_GPIO, MCU_GPIO2_GPIO_Pin);
	g_IsSystemOn = 0;
	g_key_ok_poweroff = 0;
	g_pir_ok_poweroff = 0;
	g_host_readly_uart = 0;
}

/**********************************************************************
 * 函数名称：  XOR_Inverted_Check
 * 功能描述：数据的异或取反校验
 * para:
inBuf: 需检测的字符串
inLen: 所需检测的字符串长度
 ***********************************************************************/
u8  XOR_Inverted_Check(unsigned char* inBuf, unsigned char inLen)
{
	u8 check = 0, i;

	for (i = 0; i < inLen; i++) {
		check ^= inBuf[i];
	}

	return ~check;
}

/**********************************************************************
 * 函数名称：  USART_Send_Data
 * 功能描述：发送串口数据接口
 * para:
Dbuf: 发送的数据
len: 所发送的数据长度
 ***********************************************************************/
void USART_Send_Data(unsigned char* Dbuf, unsigned int len)
{
	int i;

	for (i = 0; i < len; i++) {
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

		USART_SendData8(USART1, Dbuf[i]);

		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}

}

/**********************************************************************
 * 函数名称：UartHandle
 * 功能描述：串口通信数据处理
 * para:
 void
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

/******************************************************************
 * 函数名称： Request_CMD_Handle
 *
 * 功能描述：接收串口指令处理
 * para:
 void
 *******************************************************************/
void Request_CMD_Handle(u8* cmdstr, u8 len)
{
	u16 adcvalue = 0;
	int ret;

	USART_Send_Buf[UF_START] = 0x7B;

	switch (cmdstr[UF_CMD]) 
	{
		/*主控通知MCU可进行关机操作*/
		case 0x80:

			GPIO_ResetBits(MCU_GPIO1_GPIO, MCU_GPIO1_GPIO_Pin);
			PWR_OFF();
			MDelay(100);
			g_wkup_reason = 1;
			g_WakeUp_Key_Flag = 0;
			break;
			/*主控控制wifi使能工作IO*/

		case 0x8a:
			dev_wlan_power_handle(cmdstr[UF_DATA]);
			break;

		case 0x8c:
			host_clr_wkup_reason();
			break;
		case 0x84:
			if(cmdstr[UF_DATA] == 0)
			{
				g_Wifi_Reg_Det_Flag = 0;
			} else {
				g_Wifi_Reg_Det_Flag = 1;
				GPIO_ResetBits(MCU_GPIO2_GPIO, MCU_GPIO2_GPIO_Pin);
			}



			USART_Send_Buf[UF_LEN] = 0x4;
			USART_Send_Buf[UF_CMD] = 0x85;
			USART_Send_Buf[USART_Send_Buf[UF_LEN] - 1] = XOR_Inverted_Check(USART_Send_Buf, USART_Send_Buf[UF_LEN] - 1);
			USART_Send_Data(USART_Send_Buf, USART_Send_Buf[UF_LEN]);

			break;
			/*未知*/
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
			/*主控请求进行关机检测*/
		case 0x88:
			dev_wlan_reset_handle(cmdstr[UF_DATA]);

			break;
			/*主控请求ADC采样值*/
		case 0x90:
			adcvalue = ADC_GetBatVal();
			USART_Send_Buf[UF_LEN] = 0x6;
			USART_Send_Buf[UF_LEN] = 0x6;
			USART_Send_Buf[UF_CMD] = 0x91;
			USART_Send_Buf[UF_DATA] = adcvalue>>8;
			USART_Send_Buf[UF_DATA+1] = adcvalue&0xff;
			USART_Send_Buf[USART_Send_Buf[UF_LEN] - 1] = XOR_Inverted_Check(USART_Send_Buf, USART_Send_Buf[UF_LEN] - 1);
			USART_Send_Data(USART_Send_Buf, USART_Send_Buf[UF_LEN]);
			break;
		case 0xee:
			g_host_readly_uart = 1;
			break;
		default:
			break;
	}
}

/******************************************************************
 *函数名称：Response_CMD_Handle
 *
 *功能描述：串口指令请求处理
 * para:
 void
 *******************************************************************/
void Response_CMD_Handle(void)
{}

/******************************************************************
 *函数名称：uart_send_sure_power
 *
 *功能描述：发送确认关机串口指令
 * para:
 void
 *******************************************************************/
void uart_send_cmd(u8 cmd)
{
	USART_Send_Buf[UF_START] = 0x7B;
	USART_Send_Buf[UF_LEN] = 0x4;
	USART_Send_Buf[UF_CMD] = cmd;
	USART_Send_Buf[USART_Send_Buf[UF_LEN] - 1] = XOR_Inverted_Check(USART_Send_Buf, USART_Send_Buf[UF_LEN] - 1);
	USART_Send_Data(USART_Send_Buf, USART_Send_Buf[UF_LEN]);
}

/******************************************************************
 *函数名称：CLK_Config
 *
 *功能描述：串口收发时钟管理初始化
 * para:
 void
 *******************************************************************/
void CLK_Config(void)
{
	CLK_DeInit();

	CLK_HSICmd(ENABLE);
	CLK_SYSCLKDivConfig(BAUD_CLK);//CLK_SYSCLKDiv_32

	while (((CLK->ICKCR) & 0x02) != 0x02); //HSI准备就绪

	CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
	CLK_SYSCLKSourceSwitchCmd(ENABLE);

	while (((CLK->SWCR) & 0x01) == 0x01); //切换完成
}

/******************************************************************
 *函数名称：TIM3_Config
 *
 *功能描述：定时器配置，晶振采用是16Khz
 * para:
 void
 *******************************************************************/
void TIM3_Config(void)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);/* Enable TIM3 CLK */
	/* Time base configuration */

	TIM3_TimeBaseInit(TIM3_Prescaler_16, TIM3_CounterMode_Up, 125); //20MS 2480------------4ms 499   // 0.5M 4ms 625  1ms - 125

	/* Clear TIM4 update flag */
	TIM3_ClearFlag(TIM3_FLAG_Update);
	/* Enable update interrupt */
	TIM3_ITConfig(TIM3_IT_Update, ENABLE);
	/* Enable TIM4 */
	TIM3_Cmd(ENABLE);
}

/******************************************************************
 *函数名称：Usart_Config
 *
 *功能描述：uart初始化配置，波特率默认为9600
 * para:
 void
 *******************************************************************/
void Usart_Config(void)
{
	GPIO_Init(USART_RX_GPIO, USART_RX_GPIO_Pin, GPIO_Mode_In_PU_No_IT);
	GPIO_Init(USART_TX_GPIO, USART_TX_GPIO_Pin, GPIO_Mode_Out_PP_Low_Fast);
	CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
	USART_Init(USART1, BAUD_RATE, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
	USART_ClearITPendingBit(USART1, USART_IT_RXNE); //配置USART1->SR
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //配置USART1->CR2的RIEN位
	USART_Cmd(USART1, ENABLE);
}

/******************************************************************
 *函数名称：Pin_Det_Handle
 *
 *功能描述：wifi中断唤醒处理以及wifi工作使能IO配置
 * para:
 void
 *******************************************************************/
void Pin_Det_Handle(void)
{
	u8 val;

#if 0
	/* wlan wake detect handle */
	if (!g_IsSystemOn) {
		/*  wifi 中断唤醒标志 */
		if (g_Wifi_WakeUp == 1) {
			/* 配置相关IO设置，退出低功耗模式 */
			InitExitHaltMode();

			PWR_ON();
			/* 控制wifi镜像启动IO脚，拉高 */
			GPIO_SetBits(MCU_GPIO2_GPIO, MCU_GPIO2_GPIO_Pin);

			g_Wifi_WakeUp = 0;
		}
	}
	else {
		g_Wifi_WakeUp = 0;
	}


	/* 若已开机，且wifi寄存器标志已置1 */
	if (g_IsSystemOn && g_Wifi_Reg_Det_Flag) {

		/* 主控控制wifi寄存器是否已配置，高电平代表已设置*/
		val = !!GPIO_ReadInputDataBit(REG_ON_DET_GPIO, REG_ON_DET_GPIO_Pin);

		/*控制wifi使能工作IO*/
		if (val) {
			GPIO_SetBits(WIFI_REG_ON_GPIO, WIFI_REG_ON_GPIO_Pin);
		} else {
			GPIO_ResetBits(WIFI_REG_ON_GPIO, WIFI_REG_ON_GPIO_Pin);
		}
	}
#endif
	// DEV WAK HOST DETECT
	if(!g_IsSystemOn)
	{
		val = !!GPIO_ReadInputDataBit(D2H_WAK_GPIO, D2H_WAK_GPIO_Pin);
		if(val)
		{
			g_debounce_cnt++;
			if(GPIO_DEBOUNCE < g_debounce_cnt)
			{
				PWR_ON();
				MDelay(2);
				host_set_wkup_reason(g_wkup_reason);
				InitExitHaltMode();
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

/******************************************************************
 *函数名称：Key_Handle
 *
 *功能描述：按键处理:
 断电状态按下: 开机
 开机状态: 通知主控发送主播报文
 * para:
 void
 *******************************************************************/
void Key_Handle(void)
{
	/*按键检测处理*/
	if (1 == g_Key_Handle_Flag) {
		if (g_IsSystemOn == 0) {
			InitExitHaltMode();
			PWR_ON();
			if(0 == g_Wifi_Reg_Det_Flag) {
				GPIO_SetBits(MCU_GPIO2_GPIO, MCU_GPIO2_GPIO_Pin);
			}
			g_KeyPoweroff_Flag = 0;
			g_Key_Handle_Flag = 0;
		} else {
			/*若已开机，则通知主控再次发生主播包*/
			if(g_host_readly_uart && (g_time_count % 30 == 0)) {
				uart_send_cmd(0x93);
				g_Key_Handle_Flag = 0;
			}
		}
	} else if(2 == g_Key_Handle_Flag) {

		if(g_host_readly_uart)
			uart_send_cmd(0x95);//reset wifi info

		g_Key_Handle_Flag = 0;
	}
}

/******************************************************************
 *函数名称：PWR_Detect_Handle
 *
 *功能描述：检测设备是否是USB供电开机，若是直接开机
 * para:
 void
 *******************************************************************/
void PWR_Detect_Handle()
{
	/*是否检测到是USB供电*/
	if (g_IsSystemOn == 0) {
		if (g_Vbus_Check == 1){
			PWR_ON();
			g_Vbus_Check = 0;
		}
	}
}

/******************************************************************
 *函数名称：PIR_Detect_Handle
 *
 *功能描述：PIR检测处理
 * para:
 void
 *******************************************************************/
void PIR_Detect_Handle()
{
	/*红外检测处理*/
	if (g_PIR_Check == 1) {

		/*如果通过长按键关机则无法通过PIR上电*/
		if (g_IsSystemOn == 0&&g_KeyPoweroff_Flag!=1) {
			InitExitHaltMode();
			PWR_ON();
			GPIO_SetBits(MCU_GPIO2_GPIO, MCU_GPIO2_GPIO_Pin);
		}

		g_PIR_Check = 0;
	}
}

/******************************************************************
 *函数名称：Fun_Init
 *
 *功能描述：功能模块初始化
 * para:
 void
 *******************************************************************/
void Fun_Init(void)
{
	/*定时器初始化*/
	TIM3_Config();

	/*串口初始化*/
	Usart_Config();

	/*ADC采样初始化*/
	ADC_Init_adc0();
}

/******************************************************************
 *函数名称：Board_Init
 *
 *功能描述： 首次上电，变量、GPIO初始化
 * para:
 void
 *******************************************************************/
void Board_Init(void)
{
	/*主控上电标志位*/
	g_IsSystemOn = 0;
	/*按键按下标志位*/
	g_Key_Handle_Flag = 0;

	/*vbus*/
	g_Vbus_Check = 0;

	/*串口接收时间*/
	USART_Receive_Timeout = 0;
	/*串口接收标志*/
	USART_Receive_Flag = 0;

	/*开机键初始化*/
	GPIO_Init(SW_KEY_GPIO, SW_KEY_GPIO_Pin, GPIO_Mode_In_PU_No_IT); //按键检测
	/*控制主控上电IO口*/
	GPIO_Init(PWR_EN_GPIO, PWR_EN_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow); //默认3路电源为关
	/*控制所有电源IO口*/
	GPIO_Init(PWR_HOLD_GPIO, PWR_HOLD_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow); //默认all电源为关
	/*主控设置wifi寄存器标志IO口初始化*/
	GPIO_Init(REG_ON_DET_GPIO, REG_ON_DET_GPIO_Pin, GPIO_Mode_In_FL_No_IT);

#if 0
	/*MCU控制wifi使能工作IO初始化*/
	GPIO_Init(WIFI_REG_ON_GPIO, WIFI_REG_ON_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow);
	/*MCU控制是否镜像启动IO初始化*/
	GPIO_Init(MCU_GPIO2_GPIO, MCU_GPIO2_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow);
#endif

	/*预留IO，连接于主控*/
	GPIO_Init(MCU_GPIO3_GPIO, MCU_GPIO3_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow);
	/*蓝牙唤醒GPIO初始化*/
	GPIO_Init(BT_HOST_WAKE_GPIO, BT_HOST_WAKE_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow);
	/*蓝牙使能工作IO初始化*/
	GPIO_Init(BT_REG_ON_GPIO, BT_REG_ON_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow);
	/*wifi唤醒MCU IO初始化*/
	GPIO_Init(WIFI_GPIO2_GPIO, WIFI_GPIO2_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow);
	/*VBUS IO初始化*/
	//VBAUS关机状态下唤醒MCU
	GPIO_Init(VBUS_DETECT, VBUS_DETECT_GPIO_Pin, GPIO_Mode_In_FL_No_IT);

	/*设置WIFI唤醒检测脚为中断模式*/
	GPIO_Init(WIFI_GPIO1_GPIO, WIFI_GPIO1_GPIO_Pin, GPIO_Mode_In_FL_IT);


	/*wifi上电IO初始化*/
	GPIO_Init(DEV_PWR_GPIO, DEV_PWR_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow);
	/*wifi reset IO初始化*/
	GPIO_Init(WL_EN_GPIO, WL_EN_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow);

	GPIO_Init(MCU_GPIO1_GPIO, MCU_GPIO1_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(MCU_GPIO2_GPIO, MCU_GPIO2_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow);
	/*wifi唤醒主平台IO初始化*/
	GPIO_Init(D2H_WAK_GPIO, D2H_WAK_GPIO_Pin, GPIO_Mode_In_FL_No_IT);



	EXTI_SetPinSensitivity(EXTI_Pin_0, EXTI_Trigger_Rising);
	ITC_SetSoftwarePriority(EXTI0_IRQn, ITC_PriorityLevel_1);

	/*设置电池电量检测脚为中断模式*/
	GPIO_Init(BAT_LOW_DET_GPIO, BAT_LOW_DET_GPIO_Pin, GPIO_Mode_In_FL_IT);
	/*然后配置中断1为下降沿低电平触发*/
	EXTI_SetPinSensitivity(EXTI_Pin_2, EXTI_Trigger_Falling);
	/*设置中断的优先级*/
	ITC_SetSoftwarePriority(EXTI2_IRQn, ITC_PriorityLevel_1);

#if 0
	/*PIR*/
	GPIO_Init(PIR_GPIO, PIR_GPIO_Pin, GPIO_Mode_In_FL_No_IT);
	//GPIO_Init(PIR_GPIO, PIR_GPIO_Pin, GPIO_Mode_In_PU_IT);
	/*然后配置中断1为上升触发*/
	EXTI_SetPinSensitivity(EXTI_Pin_0, EXTI_Trigger_Rising);
	/*设置中断的优先级*/
	ITC_SetSoftwarePriority(EXTI0_IRQn, ITC_PriorityLevel_2);
#endif
	CLK_Config();
}

/******************************************************************
 *函数名称：check_power_action
 *
 *功能描述：检测设备是否满足断电要求
 无按键操作，无pir触发
 * para:
 void
 *******************************************************************/
void check_power_action(void)
{
	if(g_poweroff_check_flg) {
		if(g_key_ok_poweroff || g_pir_ok_poweroff){
			uart_send_cmd(0x00);
			g_poweroff_check_flg = 0;
		}
	}
}

/******************************************************************
 *函数名称：main
 *
 *功能描述：主控程序
 * para:
 void
 *******************************************************************/
void main(void)
{
	GPIO_init();

	Board_Init();

	PWR_ON();

	Fun_Init();

	enableInterrupts();

	kfifo_init(&recvfifo);
	while (1)
	{

		//     Key_Handle();

		Uart_Handle();

		Pin_Det_Handle();

		//PIR_Detect_Handle();//PIR检测

		Change_Mode();

		//    check_power_action();

		MDelay(1);

		g_time_count++;

		//GPIO_SetBits(WIFI_REG_ON_GPIO, WIFI_REG_ON_GPIO_Pin);
	}


}

