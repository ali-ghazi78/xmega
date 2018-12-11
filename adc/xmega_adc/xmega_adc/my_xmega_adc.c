/*
 * my_xmega_adc.c
 *
 * Created: 12/11/2018 6:44:11 PM
 *  Author: ali
 */ 
#include "avr/io.h"
void adc_init()
{
	ADCA_CTRLB |= ADC_CURRLIMIT1_bm|ADC_CONMODE_bm; //, maximum sampling speed 1msps, resolution 12-bit right adjusted, signed mode
	ADCA_REFCTRL |= ADC_REFSEL0_bm; //INTVCC2 i.e. Vcc/1.6 is selected as reference voltage
	ADCA_EVCTRL = 0x00; //no event channel input is selected
	ADCA_PRESCALER |= ADC_PRESCALER2_bm; //prescaler 64 is selected and peripheral clock is 8MHz
	ADCA_INTFLAGS = ADC_CH0IF_bm; //clear interrupt flag by writing one to it
	ADCA_CH0_CTRL |= ADC_CH_INPUTMODE0_bm; //signed mode, single ended positive input selected
	ADCA_CH0_MUXCTRL |= ADC_CH_MUXPOS0_bm; //select PIN1 as input for ADC
	ADCA_CTRLA=ADC_ENABLE_bm; //enable ADC
}
unsigned int read_adc()
{
	unsigned int adc_val;
	ADCA_CTRLA|=ADC_CH0START_bm;			//start adc
	
	 while(!(ADCA_INTFLAGS & ADC_CH0IF_bm));
	 
	 ADCA_INTFLAGS=ADC_CH0IF_bm;  //clear flag
	 
	 adc_val=ADCA_CH0_RES;
	return adc_val;
	
}
