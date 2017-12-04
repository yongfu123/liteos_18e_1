#include "boardconfig.h"

#if 0
void ADC_Init(void)
{
	ADC_ConversionMode_TypeDef adc_conversionmode = ADC_ConversionMode_Single;
	ADC_Resolution_TypeDef adc_resolution = ADC_Resolution_12Bit;
	ADC_Prescaler_TypeDef adc_prescaler = ADC_Prescaler_2;

	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);

	ADC_Cmd(ADC1, DISABLE);
	ADC_DMACmd(ADC1, DISABLE);
	ADC_Init(ADC1, adc_conversionmode, adc_resolution, adc_prescaler);
	ADC_ChannelCmd(ADC1, ADC_Channel_14, ENABLE); //test
	ADC_SamplingTimeConfig(ADC1, ADC_Group_FastChannels, ADC_SamplingTime_4Cycles);
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
}

uint16_t ADC_GetVal(void)
{
	uint16_t val = 0;
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	val = ADC_GetConversionValue(ADC1);

	return val;
}
#endif

void ADC_Init_Handle()
{
	uint16_t val = 0;
	ADC_ConversionMode_TypeDef adc_conversionmode = ADC_ConversionMode_Single;
	ADC_Resolution_TypeDef adc_resolution = ADC_Resolution_12Bit;
	ADC_Prescaler_TypeDef adc_prescaler = ADC_Prescaler_2;

	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);

	ADC_Cmd(ADC1, DISABLE);
	ADC_DMACmd(ADC1, DISABLE);
	ADC_Init(ADC1, adc_conversionmode, adc_resolution, adc_prescaler);
	ADC_ChannelCmd(ADC1, ADC_Channel_24, ENABLE);
	ADC_SamplingTimeConfig(ADC1, ADC_Group_FastChannels, ADC_SamplingTime_4Cycles);
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	USART_Send_Buf[0] = 0x7b;
	val = ADC_GetConversionValue(ADC1);

	ADC_Cmd(ADC1, DISABLE);

	USART_Send_Buf[0] = 0x7b;
	USART_Send_Buf[1] = 4;
	USART_Send_Buf[2] = val >> 8;
	USART_Send_Buf[3] = val & 0xff;
	USART_Send_Data(USART_Send_Buf, 4);
}
