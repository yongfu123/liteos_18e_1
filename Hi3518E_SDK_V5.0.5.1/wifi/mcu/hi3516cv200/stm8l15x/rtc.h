#ifndef __RTC_H
#define __RTC_H

typedef struct
{
	RTC_AlarmTypeDef rtc_alarm;
	u8 flag;
	u8 hours;
	u8 minutes;
}
RTC_Alarm_Event_T;

extern u8 g_RTC_Alarm_Flag;
extern RTC_Alarm_Event_T g_Alarm_Event;

void RTC_Init_Handle(void);
int RTC_Alarm_Duration_Check(u8 hours, u8 minutes);
int RTC_Alarm_Evetn_Set(RTC_Alarm_Event_T *alarm_event);
void RTC_Handle(void);

#endif
