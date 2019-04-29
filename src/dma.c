/*
 * dma.c
 *
 *  Created on: Apr 28, 2019
 *      Author: iansc
 */

#include "dma.h"

uint8_t arr_sel;
uint16_t arr[BUFFLEN];

void DMA_init(void)
{

	//configure clock for dma mux
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	//configure clock for dma
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;

	//disable DMA MUX
	DMAMUX0->CHCFG[0] = 0;

	// Set adc source address
	DMA0->DMA[0].SAR = (uint32_t)&ADC0->R[0];

	arr_sel = 0;
	DMA0->DMA[0].DAR = (uint32_t)&arr;

	// Number of bytes to transfer
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(BYTELEN);

	// Clear Source size and Destination size fields.
	DMA0->DMA[0].DCR &= ~(DMA_DCR_SSIZE_MASK | DMA_DCR_DSIZE_MASK);

	// Set DMA
	DMA0->DMA[0].DCR |= (DMA_DCR_EINT_MASK		// Int. enable... used if FREEDOM macro is set
			//			| DMA_DCR_ERQ_MASK			//Enable Peripheral request
			| DMA_DCR_CS_MASK			//Single read/write per request
			//			| DMA_DCR_EADREQ_MASK		//Enable Async. DMA Requests
			| DMA_DCR_SSIZE(2)			//Source size is 16-bit
			| DMA_DCR_DINC_MASK			//Destination address increments
			| DMA_DCR_DSIZE(2)			//Destination size is 16-bit
			//			| DMA_DCR_DMOD(2)			//32-bytes circular buffer enabled
			| DMA_DCR_D_REQ_MASK		//DMA request is cleared
	);

	// Enables the DMA channel and set the DMA Channel Source to ADC0
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(40);
	//Enable interrupt
	__enable_irq();
	NVIC_EnableIRQ(DMA0_IRQn);
	//Start DMA transfer to initiate ISR
	DMA0->DMA[0].DCR |= DMA_DCR_START_MASK;
}


void DMA0_IRQHandler(void)
{
	__disable_irq();

	if(DMA_DSR_BCR_DONE_MASK)
	{
		DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
		//Switch Double buffer in use
		switch(arr_sel)
		{
		case 0:
			//switch double buffer
			arr_sel = 1;
			DMA0->DMA[0].DAR = (uint32_t)&arr;
			break;
		case 1:
			arr_sel = 0;
			DMA0->DMA[0].DAR = (uint32_t)&arr[HALFLEN];
			break;
		}
		//Restate number of bytes to transfer
		DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(BYTELEN);
		//Restart DMA operation
		DMA0->DMA[0].DCR |= DMA_DCR_ERQ_MASK;
	}
	//	PRINTF("Exiting DMA ISR\r\n");
	__enable_irq();
}
