#include "stm32f4xx_adc.h"

/**
* Funkcija incijalizira ADC i povezuje ga s temperaturnim senzorom.
*/
void init_adc() {
	
	/* Struktura za inicijalizaciju ADC-a */
	ADC_InitTypeDef ADC_InitStruct;
	/* Struktura za inicijalizaciju svega sto je potrebno za rad s bilo kojim ADC-om */
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	
	ADC_DeInit(); //postavlja sve vrijednosti adc-a na defaultne postavke
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //omogucava clock za ADC
	
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div8;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	
	/* Inicjalizacij uz dane parametre */
	
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	/* Postavljanje razlicitih postavki u ADC strukturu */

	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfConversion = 1;
	
	/* ADC 1 inicijalizacija uz danu strukturu */
	ADC_Init(ADC1, &ADC_InitStruct);
	
	/* ADC1 povezivanje s temperaturnim senzorom */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_144Cycles);
	
	/* Omogucavanje internog temperaturnog senzora */
	ADC_TempSensorVrefintCmd(ENABLE);
	
	/* Omogucavanje ADC1 */
	ADC_Cmd(ADC1, ENABLE);
}

double getTempValue() {
	double TemperatureValue;
	ADC_SoftwareStartConv(ADC1); //Start the conversion
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //Processing the conversion
	
	
	TemperatureValue = ADC_GetConversionValue(ADC1); 
	TemperatureValue *= 3300;
  TemperatureValue /= 0xfff; //Reading in mV
  TemperatureValue /= 1000.0; //Reading in Volts
  TemperatureValue -= 0.760; // Subtract the reference voltage at 25°C
  TemperatureValue /= .0025; // Divide by slope 2.5mV
  TemperatureValue -= 11.0; // Add the 25°C
	return TemperatureValue;
}
