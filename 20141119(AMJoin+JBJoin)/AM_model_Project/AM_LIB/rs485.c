/*
 * rs485.c
 *
 *  Created on: 2014/3/14
 *      Author: SHIN
 *  How to ues: Reference to rs485.h file
 *
 */
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTTiva.h>
#include "rs485.h"
#include "../DK_TM4C129X.h"
#include "../board.h"
#include <ti/drivers/GPIO.h>
/*
UART_Handle uart2;
UART_Params uartParams;
*/
extern  UART_Handle uart2;
int rs485_write(const unsigned char *buffer, UInt size)
{
	int i=0;
	int j=0;
	int num;
	int a=5;
	GPIO_write(Board_RS485_DE_RE_PIN, RS485_WRITE);
	UART_init();
//	num = UART_writePolling(uart2, (char *)buffer, size);

	while(j<size)
	{	//UART_init();
		num = UART_writePolling(uart2, (char *)(buffer+j), 1);
		for(i=0;i<=1000;i++);
		j++;
	}
//	for(i=0;i<500;i++);
/*
	if(num == UART_ERROR)
	{
		System_printf("write error!!!: %d  \n",num);
		System_flush();
	}*/
//	System_printf(" ");
//	System_flush();
	System_printf("write: %d bytes \n",num);
//	System_flush();

	//while(num < size) ;
	//for(i=0;i<size*1000;i++);
	GPIO_write(Board_RS485_DE_RE_PIN, RS485_READ);
	return num;
}
int rs485_read(unsigned char *buffer, UInt size)
{
	int num;
	GPIO_write(Board_RS485_DE_RE_PIN, RS485_READ);
	num = UART_read(uart2, (char *)buffer, size);
	return num;
}
int rs485_init()
{	UART_init();
	UART_Params_init(&uartParams);
	uartParams.writeMode = UART_MODE_BLOCKING;
	uartParams.readMode = UART_MODE_BLOCKING;
	uartParams.writeTimeout = rs485_timeout;
	uartParams.writeDataMode = UART_DATA_BINARY  ;
	uartParams.readDataMode = UART_DATA_BINARY  ;
	uartParams.readReturnMode = UART_RETURN_FULL;
	uartParams.readEcho = UART_ECHO_OFF;
	uartParams.baudRate = rs485_baudRate;
	uartParams.readTimeout = rs485_timeout;

	UART_init();
	uart2 = UART_open(Board_UART0, &uartParams);

	if (uart2 == NULL ) {
		System_abort("Error opening the UART2");
		return 1;
		}

	return 0;
}

