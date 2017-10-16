
#include "App.h"
#include "UART.h"

#include "PORT.h"
#include "SysTick.h"
#include "UART.h"


void send_UART_Package(unsigned int rotate_r, unsigned int rotate_f, unsigned char id );

static unsigned int rotate_r = 0;
static unsigned int rotate_f = 0;
static unsigned char id = 1;


void App_Init (void)
{
	PORT_Init();
	SysTick_Init();
	UART_Init();

}

void App_Run (void)
{
	if(id > 6)
		id = 1;
	if(rotate_r > 366)
		rotate_r= 0;
	if(rotate_f > 360)
		rotate_f = 0;
	// ESTO ES PARA ESCRIBIR LA FIFO
	/* Write data into the FIFO */
	//    UART_D_REG(tx_module) = 0;		//donde esta el cero va la data;

	send_UART_Package(rotate_r,rotate_f,id);
	id++;
	rotate_r++;
	rotate_f++;
}
/*
void send_UART_Package(unsigned int rotate_r, unsigned int rotate_f, unsigned char id ){
	if(rotate_r <= 360 && rotate_f <= 360 && id <= 6 && id > 0){
//		MANDO PAQUETES PARA SINCRONIZACION
		UART_Send_Data(255);
		UART_Send_Data(255);

//		MANDO ID
		UART_Send_Data(id);

// 		MANDO ROTATE_R
		if(rotate_r > 255){
			UART_Send_Data(255);
			UART_Send_Data(rotate_r - 255);
		}
		if(rotate_r <= 255){
			UART_Send_Data(rotate_r);
			UART_Send_Data(0);
		}
// 		MANDO ROTATE_F
		if(rotate_f > 255){
			UART_Send_Data(255);
			UART_Send_Data(rotate_f - 255);
		}
		if(rotate_f <= 255){
			UART_Send_Data(rotate_f);
			UART_Send_Data(0);
		}
	}
}
*/


void send_UART_Package(unsigned int rotate_r, unsigned int rotate_f, unsigned char id ){
	if(rotate_r <= 360 && rotate_f <= 360 && id <= 6 && id > 0){
//		MANDO PAQUETES PARA SINCRONIZACION
		UART_D_REG(tx_module) = 255;
		UART_D_REG(tx_module) = 255;
//		MANDO ID
		UART_D_REG(tx_module) = id;

// 		MANDO ROTATE_R
		if(rotate_r > 255){
			UART_D_REG(tx_module) = 255;
			UART_D_REG(tx_module) = rotate_r - 255;
		}
		if(rotate_r <= 255){
			UART_D_REG(tx_module) = rotate_r;
			UART_D_REG(tx_module) = 0;
		}
// 		MANDO ROTATE_F
		if(rotate_f > 255){
			UART_D_REG(tx_module) = 255;
			UART_D_REG(tx_module) = rotate_f - 255;
		}
		if(rotate_f <= 255){
			UART_D_REG(tx_module) = rotate_f;
			UART_D_REG(tx_module) = 0;
		}
//		TERMINADOR! APROVECHO LA ULTIMA POSICION DE LA FIFO PARA LA INTERRUPCION
		UART_D_REG(tx_module) = 0;

	}
}

