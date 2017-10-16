/*******************************************************************************
 * File              uart.c
 * Owner             TIC
 * Version           1.0
 * Date              August-25-2016
 * Classification    General Business Information
 * Brief             Interrupt UART Rx FIFO example for K64F
 *
 ********************************************************************************
 * Detailed Description:
 ********************************************************************************
    UART0 Rx FIFO example for basic send and receive characters implementation.
    This project was created for KDS 3.2.0 and FRDM-K64F board. It uses
    default console port UART0 that is using PTB16 and PTB17 as Rx and Tx pins
    respectively. It enables UART0's Rx FIFO and set watermark to FIFO/2, when
    the RDRF flag is set, Rx FIFO is read and data is stored in internal buffer,
    then, these data will be sent back.

Revision History:
Version                 1.0
Date                    August-25-2016
Author                  TIC
Description of Changes  First version
 *******************************************************************************/
#include "uart.h"
/*******************************************************************************
 * External objects
 *******************************************************************************/
/*******************************************************************************
 * Global variables
 *******************************************************************************/
#ifndef false
#define false   0
#endif

#ifndef true
#define true    1
#endif

/* Variable to store received character that will be sent in Tx Interrupt */
static int8_t s8_g_uart_data[INTERNAL_BUFFER_SIZE] = {0};

static int8_t s8_g_availableData = 0;
/* Variable to signal when received data is available to be sent */
static uint8_t bPendingData = false;

/* Variables to count Rx FIFO underflow and overflow errors */
static uint32_t g_rxuf_error = 0;
static uint32_t g_rxof_error = 0;
/*******************************************************************************
 * Constants and macros
 *******************************************************************************/
#define CORE_CLOCK_HZ   (20970000u)
#define SBR_VALUE       (CORE_CLOCK_HZ / (16 * UART0_BAUDRATE))
#define BRFA_VALUE      ((32 * CORE_CLOCK_HZ / (16 * UART0_BAUDRATE)) - \
        (32 * SBR_VALUE))

/*******************************************************************************
 * Local types
 *******************************************************************************/
/*******************************************************************************
 * Local function prototypes
 *******************************************************************************/
/* Function to send data that is stored in internal buffer */
static void UART0_sendReceivedData(void);
/* Function to read data from Rx FIFO */
static inline void UART0_readRxFIFO(void);
/*******************************************************************************
 * Local variables
 *******************************************************************************/
/*******************************************************************************
 * Local functions
 *******************************************************************************/
static inline void UART0_readRxFIFO(void)
{
    /* Get available data in RX FIFO */
    while (0 < UART0_RCFIFO)
    {
        /* Read data and stored in global buffer */
        s8_g_uart_data[s8_g_availableData++] = UART0_D;
    }
}

static void UART0_sendReceivedData(void)
{
    uint8_t u8_temp = 0;
    while (u8_temp < s8_g_availableData)
    {
        /* wait for empty space */
        while (!(UART0_S1 & UART_S1_TDRE_MASK));
        /* Send next available data */
        UART0_D = s8_g_uart_data[u8_temp++];
    }
    /* Reset available data counter */
    s8_g_availableData = 0;
}


/*******************************************************************************
 * Global functions
 *******************************************************************************/
void UART0_init(void)
{
    uint8_t u8_fifoSize = 0;

    /* Clock UART0 module */
    SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;

    /* Use PTB16 and PTB17 as Tx and Rx for UART0 */
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
    PORTB_PCR16 = PORT_PCR_MUX(3);
    PORTB_PCR17 = PORT_PCR_MUX(3);

    /* Disable Transmitter and Receptor */
    UART0_C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

    /* UART baud rate = UART clock / (16 * (SBR + (BRFA/32))) */
    /* When running in FEI mode, Core clock is 20.97 MHz, this core clock is
     * used to feed UART module, so, in FEI mode and baud rate equals to 115200
     * bps, SBR value should be 11 and BRFA 12 */
    UART0_BDH = UART_BDH_SBR(SBR_VALUE >> 8);
    UART0_BDL = UART_BDL_SBR(SBR_VALUE);
    UART0_C4 = UART_C4_BRFA(BRFA_VALUE);

    /* Get fifo size */
    u8_fifoSize = (UART0_PFIFO & UART_PFIFO_RXFIFOSIZE_MASK) >> UART_PFIFO_RXFIFOSIZE_SHIFT;
    if (0 != u8_fifoSize)
    {
        /* Buffer size is 2 ^ (TXFIFOSIZE + 1) */
        u8_fifoSize = (1 << (u8_fifoSize + 1));
        /* Set FIFO watermark to FIFO_SIZE / 2 */
        UART0_RWFIFO = u8_fifoSize >> 1;

        /* Enable Rx FIFO feature */
        UART0_PFIFO |= UART_PFIFO_RXFE_MASK;
        /* Enable interrupt for Rx FIFO overflow and Rx FIFO underflow */
        UART0_CFIFO |= UART_CFIFO_RXOFE_MASK | UART_CFIFO_RXUFE_MASK;
        /* Clear FIFO overflow and underflow flags */
        UART0_SFIFO |= UART_SFIFO_RXOF_MASK | UART_SFIFO_RXUF_MASK;
        /* Flush RX FIFO */
        UART0_CFIFO |= UART_CFIFO_RXFLUSH_MASK;
        /* Clear underflow flag */
        UART0_SFIFO |= UART_SFIFO_RXUF_MASK;
    }
    else
    {
        /* FIFO size is 1, do not enable FIFO feature */
    }

    /* Enable Receiver interrupt, Enable Transmitter and Receptor*/
    UART0_C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_RIE_MASK);
}

/* Send data only when there is data in internal buffer */
void UART0_sendData (void)
{
    if (bPendingData)
    {
        bPendingData = false;
        UART0_sendReceivedData();
    }
}

void UART0_RX_TX_IRQHandler(void)
{
    /* Check if it is a receive interrupt */
    if (UART0_S1 & UART_S1_RDRF_MASK)
    {
        /* Read received characters, RDRF will be cleared */
        UART0_readRxFIFO();
        /* Set pending data variable */
        bPendingData = true;
    }
}

void UART0_ERR_IRQHandler (void)
{
    if (UART0_SFIFO & UART_SFIFO_RXOF_MASK)
    {
        /* Increment error counters */
        g_rxof_error++;
        /* Clear FIFO overflow flag */
        UART0_SFIFO |= UART_SFIFO_RXOF_MASK;
        /* Flush RX FIFO */
        UART0_CFIFO |= UART_CFIFO_RXFLUSH_MASK;
    }
    if (UART0_SFIFO & UART_SFIFO_RXUF_MASK)
    {
        /* Increment error counters */
        g_rxuf_error++;
        /* Clear FIFO overflow flag */
        UART0_SFIFO |= UART_SFIFO_RXUF_MASK;
        /* Flush RX FIFO */
        UART0_CFIFO |= UART_CFIFO_RXFLUSH_MASK;
    }
}
