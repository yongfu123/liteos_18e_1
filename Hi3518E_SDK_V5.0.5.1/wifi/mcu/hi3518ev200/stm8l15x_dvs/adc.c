#include "boardconfig.h"

void ADC_Init_adc0()
{
    GPIO_Init(ADCIN_GPIO, ADCIN_GPIO_Pin, GPIO_Mode_In_FL_No_IT);
    //ADC_ConversionMode_TypeDef adc_conversionmode = ADC_ConversionMode_Single;
     ADC_ConversionMode_TypeDef adc_conversionmode =  ADC_ConversionMode_Continuous;
    ADC_Resolution_TypeDef adc_resolution = ADC_Resolution_12Bit;
    ADC_Prescaler_TypeDef adc_prescaler = ADC_Prescaler_2;
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
    ADC_Cmd(ADC1, DISABLE);
    ADC_DMACmd(ADC1, DISABLE);
    ADC_Init(ADC1, adc_conversionmode, adc_resolution, adc_prescaler);
    ADC_ChannelCmd(ADC1, ADC_Channel_18, ENABLE);
    ADC_SamplingTimeConfig(ADC1, ADC_Group_FastChannels, ADC_SamplingTime_4Cycles);
    ADC_Cmd(ADC1, ENABLE);
}

void ADC_Deinit_adc0()
{
    ADC_DMACmd(ADC1, DISABLE);
    ADC_Cmd(ADC1, DISABLE);
    GPIO_Init(ADCIN_GPIO, ADCIN_GPIO_Pin, GPIO_Mode_Out_PP_Low_Slow);
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);
}

uint16_t ADC_GetBatVal()
{   
    uint16_t val = 0;
    ADC_SoftwareStartConv(ADC1);
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    val = ADC_GetConversionValue(ADC1);
    return val;
}


uint16_t ADC_GetBatVal_ori()
{
    uint16_t val = 0;
    ADC_ConversionMode_TypeDef adc_conversionmode = ADC_ConversionMode_Single;
    ADC_Resolution_TypeDef adc_resolution = ADC_Resolution_12Bit;
    ADC_Prescaler_TypeDef adc_prescaler = ADC_Prescaler_2;
    
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
    
    ADC_Cmd(ADC1, DISABLE);
    ADC_DMACmd(ADC1, DISABLE);
    //ADC_Cmd(ADC1, ENABLE);
    ADC_Init(ADC1, adc_conversionmode, adc_resolution, adc_prescaler);
    ADC_ChannelCmd(ADC1, ADC_Channel_18, ENABLE);
    ADC_SamplingTimeConfig(ADC1, ADC_Group_FastChannels, ADC_SamplingTime_4Cycles);
    ADC_Cmd(ADC1, ENABLE);
    ADC_SoftwareStartConv(ADC1);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    val = ADC_GetConversionValue(ADC1);
    
    ADC_Cmd(ADC1, DISABLE);
    
    return val;
}