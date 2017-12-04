#include "boardconfig.h"

u8 g_RTC_Alarm_Flag = 0;

RTC_Alarm_Event_T g_Alarm_Event = {0};

void RTC_Init_Handle(void)
{
	RTC_InitTypeDef RTC_InitStruct;
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	g_RTC_Alarm_Flag = 0;

	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);

	RTC_DeInit();
	RTC_StructInit(&RTC_InitStruct);
	RTC_InitStruct.RTC_AsynchPrediv = 124;
	RTC_InitStruct.RTC_SynchPrediv = 303;
	RTC_Init(&RTC_InitStruct);

	RTC_TimeStructInit(&RTC_TimeStruct);
	RTC_DateStructInit(&RTC_DateStruct);

	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);

	RTC_WaitForSynchro();
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);

	RTC_AlarmCmd(DISABLE);
}

int RTC_Alarm_Duration_Check(u8 hours, u8 minutes)
{
	int ret;
	if ((minutes >= 60) || (hours >= 24)) {
		ret = -1;
		goto end;
	}

end:
	return ret;
}
int RTC_Alarm_Evetn_Set(RTC_Alarm_Event_T *alarm_event)
{
	int ret;
	RTC_TimeTypeDef rtc_time;

	if ((alarm_event->minutes >= 60) || (alarm_event->hours >= 24)) {
		ret = -1;
		goto end;
	}

	if (alarm_event->flag & 0x1) {
		if (!(alarm_event->flag & 0x2)) {
			alarm_event->flag = 0;
		}

		RTC_WaitForSynchro();
		RTC_GetTime(RTC_Format_BIN, &rtc_time);
		
		alarm_event->rtc_alarm.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
		alarm_event->rtc_alarm.RTC_AlarmTime.RTC_Seconds = rtc_time.RTC_Seconds;
		alarm_event->rtc_alarm.RTC_AlarmTime.RTC_Minutes = rtc_time.RTC_Minutes + alarm_event->minutes;
		alarm_event->rtc_alarm.RTC_AlarmTime.RTC_Hours = rtc_time.RTC_Hours + alarm_event->hours;

		if (alarm_event->rtc_alarm.RTC_AlarmTime.RTC_Minutes >= 60) {
			alarm_event->rtc_alarm.RTC_AlarmTime.RTC_Minutes -= 60;
			alarm_event->rtc_alarm.RTC_AlarmTime.RTC_Hours++;
		}

		if (alarm_event->rtc_alarm.RTC_AlarmTime.RTC_Hours >= 24) {
			alarm_event->rtc_alarm.RTC_AlarmTime.RTC_Hours -= 24;
		}

		RTC_AlarmCmd(DISABLE);
		RTC_SetAlarm(RTC_Format_BIN, &alarm_event->rtc_alarm);
		g_RTC_Alarm_Flag = 0;
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		RTC_ITConfig(RTC_IT_ALRA, ENABLE);
		RTC_AlarmCmd(ENABLE);
		ret = 0;
		goto end;
	} else {
		RTC_AlarmCmd(DISABLE);
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		RTC_ITConfig(RTC_IT_ALRA, DISABLE);
		ret = 0;
		goto end;
	}
end:
	return ret;
}

void RTC_Handle(void)
{
	if (g_RTC_Alarm_Flag) 
        {
          if (g_IsSystemOn == 0)
          {
              InitExitHaltMode();
              PWR_ON();
          }
          g_RTC_Alarm_Flag = 0;
	}
}
