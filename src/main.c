/*
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    main.c
 * @brief   Application entry point.
 */

#include <stdio.h>
#include <stdint.h>

#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#include "ADC0.h"
#ifdef DMA_ENABLE
#include "dma.h"
#include "dbfs.h"
#endif


/*
 * @brief   Application entry point.
 */
int main(void)
{

	// Iterator for first half of double buffer
	int i;
	// Iterator for second half of double buffer
	int j;
#ifdef DMA_ENABLE
	// Peak value extracted from ADC doubler buffer
	uint16_t peak;
	// Whole integer of DBFS value
	uint8_t num_res;
	// Decimal part of DBFS value
	uint16_t num_dec;
#endif

	BOARD_InitPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();

	ADC0_init();
#ifdef DMA_ENABLE
	DMA_init();
#endif

	/* Enter an infinite loop */
	while(1)
	{
#ifndef DMA_ENABLE
		PRINTF("ADC Value: %d\r\n",  ADC0_read());
#else
		ADC0_read();
		switch(arr_sel)
		{
		case 0:
			for(j = HALFLEN; j < BUFFLEN; j++)
			{
				//check new value to redetermine peak
				if(peak < arr[j])
				{
					peak = arr[j];
				}
				else peak = peak - ((30 * (peak-arr[i])) >> 5);
			}
			break;
		case 1:
			for(i = 0; i < HALFLEN; i++)
			{
				//check new value to redetermine peak
				if(peak < arr[i])
				{
					peak = arr[i];
				}
				else peak = peak - ((30 * (peak-arr[i])) >> 5);
			}
			break;
		}

		get_log(peak, &num_res, &num_dec);
		if(num_dec < 10)
		{
			PRINTF("Peak Value: %d.00%d, Raw Peak Value: %d\r\n", num_res, num_dec, peak);
		}
		else if(num_dec < 100)
		{
			PRINTF("Peak Value: %d.0%d, Raw Peak Value: %d\r\n", num_res, num_dec, peak);
		}
		else
		{
			PRINTF("Peak Value: %d.%d, Raw Peak Value: %d\r\n", num_res, num_dec, peak);
		}
#endif
	}
	return 0;
}
