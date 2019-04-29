/*
 * ADC0.h
 *
 *  Created on: Apr 28, 2019
 *      Author: iansc
 */

#ifndef ADC0_H_
#define ADC0_H_

#if 1
#define DMA_ENABLE
#endif

#include <stdint.h>

#include "fsl_adc16.h"
#include "fsl_port.h"

void ADC0_init(void);

uint32_t ADC0_read(void);

#endif /* ADC0_H_ */
