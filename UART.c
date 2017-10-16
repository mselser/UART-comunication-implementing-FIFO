
#include "UART.h"
#include "PORT.h"

#define UART_HAL_DEFAULT_BAUDRATE 9600

#define UART0_TX_PIN 	17   //PTB17
#define UART0_RX_PIN 	16   //PTB16
#define PORT_UART0 PORTB
#define GPIO_UART0 PTB
#define FIFO_SIZE 8

static unsigned char rx_flag=false;
static unsigned char rx_data;
UART_MemMapPtr tx_module, rx_module;


void UART_Init (void)
{
	PCRstr UserPCR;
	UserPCR.PCR=false;

	// APUNTO EL PUNTERO A LA MEMORIA A EL UART0
    tx_module = UART0_BASE_PTR;

    // HABILITO LA FIFO
    UART_PFIFO_REG(tx_module) |= UART_PFIFO_TXFE_MASK;

    // DESHABILITO TEMPORALMENTE EL TRANSMISOR HASTA QUE SE LLENE LA FIFO!
    UART_C2_REG(tx_module) &= ~UART_C2_TE_MASK;


// Note: 5.6 Clock Gating page 192
// Any bus access to a peripheral that has its clock disabled generates an error termination.

	    SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
		SIM_SCGC4 |= SIM_SCGC4_UART1_MASK;
		SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
		SIM_SCGC4 |= SIM_SCGC4_UART3_MASK;
		SIM_SCGC1 |= SIM_SCGC1_UART4_MASK;
		SIM_SCGC1 |= SIM_SCGC1_UART5_MASK;

		NVIC_EnableIRQ(UART0_RX_TX_IRQn);
		NVIC_EnableIRQ(UART1_RX_TX_IRQn);
		NVIC_EnableIRQ(UART2_RX_TX_IRQn);
		NVIC_EnableIRQ(UART3_RX_TX_IRQn);
		NVIC_EnableIRQ(UART4_RX_TX_IRQn);
		NVIC_EnableIRQ(UART5_RX_TX_IRQn);


		UART_SetBaudRate(UART0, UART_HAL_DEFAULT_BAUDRATE);

	//UART0 PINS setup

		UserPCR.FIELD.MUX=PORT_mAlt3;		// UART0
		UserPCR.FIELD.IRQC=PORT_eDisabled;  // No Irqs from port

		PORT_Configure2(PORT_UART0,UART0_TX_PIN,UserPCR); //Setup Tx and Rx pins
		PORT_Configure2(PORT_UART0,UART0_RX_PIN,UserPCR);

	//UART0 Baudrate Setup

		UART_SetBaudRate (UART0, 9600);

	//UART0 Setup

		//UART0->C2=UART_C2_TE_MASK | UART_C2_RE_MASK;
		UART0->C2=UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_RIE_MASK;

		/* Enable the FIFOs */
		    UART_PFIFO_REG(UART0 -> PFIFO) |= UART_PFIFO_TXFE_MASK;
		    UART_PFIFO_REG(UART0 -> PFIFO) |= UART_PFIFO_RXFE_MASK;

			UART_TWFIFO_REG(UART0 -> PFIFO) = i;

}


void UART_SetBaudRate (UART_Type *uart, uint32_t baudrate)
{
	uint16_t sbr, brfa;
	uint32_t clock;

	clock = ((uart == UART0) || (uart == UART1))?(__CORE_CLOCK__):(__CORE_CLOCK__ >> 1);

	baudrate = ((baudrate == 0)?(UART_HAL_DEFAULT_BAUDRATE):
			((baudrate > 0x1FFF)?(UART_HAL_DEFAULT_BAUDRATE):(baudrate)));

	sbr = clock / (baudrate << 4);               // sbr = clock/(Baudrate x 16)
	brfa = (clock << 1) / baudrate - (sbr << 5); // brfa = 2*Clock/baudrate - 32*sbr

	uart->BDH = UART_BDH_SBR(sbr >> 8);
	uart->BDL = UART_BDL_SBR(sbr);
	uart->C4 = (uart->C4 & ~UART_C4_BRFA_MASK) | UART_C4_BRFA(brfa);
}


__ISR__ UART0_RX_TX_IRQHandler (void)		// ACA NECESITO PREGUNTAR QUE ONDA
{
	// ACA ESTOY SI LLEGA UNA INTERRUPCION DE RX O TX, ES DECIR, QUE SE LLENO EL BUFFER

	/* Now re-enable the transmitter. This should cause all 8 bytes to transmit
	 * back to back.
	 */
	//    UART_C2_REG(tx_module) |= UART_C2_TE_MASK;

	unsigned char tmp;
	tmp=UART0->S1;
	rx_flag=true;
	rx_data=UART0->D;

	/* Flush the TxFIFO */
	//    UART_CFIFO_REG(tx_module) |= UART_CFIFO_TXFLUSH_MASK;

	/* Wait for RDRF flag to set indicating transmit is complete */
	//    while( !(UART_S1_REG(rx_module) & UART_S1_RDRF_MASK));

	// DESHABILITO TEMPORALMENTE EL TRANSMISOR HASTA QUE SE LLENE LA FIFO!
	//UART_C2_REG(tx_module) &= ~UART_C2_TE_MASK;
}

// NO USO RX
/*
unsigned char UART_Get_Status(void)
{
	return(rx_flag);
}

unsigned char UART_Get_Data(void)
{
	rx_flag=false;
	return(rx_data);
}
*/

// SOLO MANDO CUANDO SE LLENA LA FIFO PA
/*
void UART_Send_Data(unsigned char tx_data)
{
	while(((UART0->S1)& UART_S1_TDRE_MASK) ==0);

				UART0->D = tx_data;
}
*/



// ESTO ES PARA ESCRIBIR LA FIFO
/* Write data into the FIFO */
//    UART_D_REG(tx_module) = 0;		//donde esta el cero va la data;



/* Test to make sure TCFIFO is 8 (FIFO full) */
//    if ( UART_TCFIFO_REG(tx_module) != 8)
//    {
//        error++;
//        printf("\nERR! TCFIFO value is incorrect. Expected: 0x8 Read: 0x%02X", UART_TCFIFO_REG(tx_module));
//    }


/* Test to make sure TCFIFO is 0 now (FIFO flushed) */
//    if ( UART_TCFIFO_REG(tx_module) != 0)
//    {
//        error++;
//        printf("\nERR! TCFIFO value is incorrect. Expected: 0x0 Read: 0x%02X", UART_TCFIFO_REG(tx_module));
//    }


