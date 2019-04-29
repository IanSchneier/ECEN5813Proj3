/*
 * dma.h
 *
 *  Created on: Apr 28, 2019
 *      Author: iansc
 */

#ifndef DMA_H_
#define DMA_H_

#include <stdint.h>

#include "MKL25Z4.h"

#define BUFFLEN (128)
#define HALFLEN (BUFFLEN>>1)
#define BYTELEN (BUFFLEN<<1)

extern uint8_t arr_sel;
extern uint16_t arr[BUFFLEN];


void DMA_init(void);
void DMA0_IRQHandler(void);

#endif /* DMA_H_ */
