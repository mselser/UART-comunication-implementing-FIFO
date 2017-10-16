/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
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

/*******************************************************************************
* File              main.c
* Owner             TIC
* Version           1.0
* Date              April-27-2016
* Classification    General Business Information
* Brief             interrupt UART example for K64F
*
********************************************************************************
* Detailed Description:
********************************************************************************
    UART0 interrupt example for basic send and receive characters implementation.
    This project was created for KDS 3.2.0 and FRDM-K64F board. It uses
    default console port UART0 that is using PTB16 and PTB17 as Rx and Tx pins
    respectively.

Revision History:
Version                 1.0
Date                    April-27-2016
Author                  TIC
Description of Changes  First version
*******************************************************************************/

#include "MK64F12.h"
#include "uart.h"


int main(void)
{

    /* initialize UART0 module */
    UART0_init();


    /* Clear UART0 interrupt pending, set priority and enable interrupt */
    NVIC_ClearPendingIRQ(UART0_RX_TX_IRQn);
    NVIC_SetPriority(UART0_RX_TX_IRQn, 8);
    NVIC_EnableIRQ(UART0_RX_TX_IRQn);

    /* Clear UART0 error interrupt pending, set priority and enable interrupt */
    NVIC_ClearPendingIRQ(UART0_ERR_IRQn);
    NVIC_SetPriority(UART0_ERR_IRQn, 7);
    NVIC_EnableIRQ(UART0_ERR_IRQn);

    for (;;) {
        /* Send data once Rx FIFO has available data (data received is equal or
         * greater than Rx FIFO's watermark) */
        UART0_sendData();
    }
    /* Never leave main */
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
