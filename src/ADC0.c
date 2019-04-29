/*
 * ADC0.c
 *
 *  Created on: Apr 28, 2019
 *      Author: iansc
 */

#include "ADC0.h"

void ADC0_init(void)
{
	adc16_config_t adc16ConfigStruct;
	// Enable clock for PortE
	CLOCK_EnableClock(kCLOCK_PortE);                           /* Port E Clock Gate Control: Clock enabled */
	// Set pin 13 to ADC0 via PTE20
	PORT_SetPinMux(PORTE, 20, kPORT_PinDisabledOrAnalog); /* PORTE20 (pin 13) is configured as ADC0_DP0 */

	adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceVref;
	//TODO: confirm with sorabh how to calculate sample rate and determine best clock source and sample rate
	adc16ConfigStruct.clockSource = kADC16_ClockSourceAlt0;
	adc16ConfigStruct.enableAsynchronousClock = false;
	adc16ConfigStruct.clockDivider = kADC16_ClockDivider8;
	//single ended, 16-bit resolution
	adc16ConfigStruct.resolution = kADC16_ResolutionSE16Bit;
	adc16ConfigStruct.longSampleMode = kADC16_LongSampleDisabled;
	adc16ConfigStruct.enableHighSpeed = false;
	adc16ConfigStruct.enableLowPower = false;
	adc16ConfigStruct.enableContinuousConversion = true;

	ADC16_Init(ADC0, &adc16ConfigStruct);
	ADC16_EnableHardwareTrigger(ADC0, false); /* Make sure the software trigger is used. */
#if defined(FSL_FEATURE_ADC16_HAS_CALIBRATION) && FSL_FEATURE_ADC16_HAS_CALIBRATION
	ADC16_DoAutoCalibration(ADC0);
#endif

	//Configure Pin 13 on J2 to toggle during ADC sampling
	SIM->SCGC5 |= SIM_SCGC5_PORTD(31);
	PORTE->PCR[31] |= PORT_PCR_MUX(1);
	GPIOE->PDDR |= (1 << 31);
}



uint32_t ADC0_read(void)
{
	// Toggle GPIO
	GPIOE->PTOR |= (1<<31);
	// Select channel 0 as the input channel
	ADC0->SC1[0] = (ADC0->SC1[0] & ~ADC_SC1_ADCH_MASK) | ADC_SC1_ADCH(0);
	//ADC16_SetChannelConfig(ADC0, 0, &adc16ChannelConfigStruct);
	// wait for ADC conversion to finish
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
#ifndef DMA_ENABLE
	return ADC0->R[0];
#else
	DMA0->DMA[0].DCR |= DMA_DCR_START_MASK;
	return 0;
#endif
}
