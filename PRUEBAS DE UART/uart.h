/*******************************************************************************
* File              uart.h
* Owner             TIC
* Version           1.0
* Date              April-27-2016
* Classification    General Business Information
* Brief             Interrupt UART example for K64F
********************************************************************************
* Detailed Description:
********************************************************************************
    UART0 interrupt example for basic send and receive character implementation.
    This project was created for KDS 3.2.0 and FRDM-K64F board. It uses
    default console port UART0 that is using PTB16 and PTB17 as Rx and Tx pins
    respectively.

Revision History:
Version                 1.0
Date                    April-27-2016
Author                  TIC
Description of Changes  First version
*******************************************************************************/
#ifndef _UART_H
#define _UART_H
/*******************************************************************************
* Includes
*******************************************************************************/
#include "MK64F12.h"

/*******************************************************************************
* Constants
*******************************************************************************/

/*******************************************************************************
* Macros
*******************************************************************************/
#define UART0_BAUDRATE              115200u

#define INTERNAL_BUFFER_SIZE        16
/*******************************************************************************
* Types
*******************************************************************************/

/*******************************************************************************
* Global variables
*******************************************************************************/

/*******************************************************************************
* Global functions
*******************************************************************************/
/**************************************************************************//*!
 *
 * @name    UART0_init
 *
 * @brief   Initializes UART0 at baud rate defined in UART0_BAUDRATE macro.
 *          Configures UART0 in its common configuration (8N1) and uses PTB16
 *          and PTB17 as Rx and Tx respectively. Enable Tx/Rx interrupt.
 *
 * @param   none
 *
 * @return  nothing
 *****************************************************************************/
void UART0_init(void);

/**************************************************************************//*!
 *
 * @name    UART0_sendData
 *
 * @brief   Enable TDRE interrupt in order to send data via interrupt. Previous
 *          received data is stored in global variable and this data will be
 *          sent in interrupt service routine.
 *
 * @param   none
 *
 * @return  nothing
 *****************************************************************************/
void UART0_sendData(void);

/******************************************************************************
* Inline functions
******************************************************************************/
#endif /* _UART_H */
