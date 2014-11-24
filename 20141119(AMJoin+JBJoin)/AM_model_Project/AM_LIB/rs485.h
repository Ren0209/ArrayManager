/*
 * rs485.h
 *
 *  Created on: 2014/3/14
 *      Author: SHIN
 *  How to ues: call the initial function - rs485_init() to initial the hardware.There is no parameter required for this function.
 *  			call the send function - rs485_write to transmit data.The first parameter is the buffer address that contain the data.
 *										 The second parameter is the size of the data in buffer(bytes)
 *				call the receive function - rs485_read to receive data.The two parameter is the same as send function.
 */

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTTiva.h>
#include "../DK_TM4C129X.h"
#include "../board.h"
#include <xdc/runtime/System.h>

#ifndef RS485_H_
#define RS485_H_

#define rs485_baudRate 115200
#define rs485_timeout 2000 //ms

extern UART_Handle uart2;
extern UART_Params uartParams;

extern int rs485_write(const unsigned char *buffer, UInt size);
extern int rs485_read(unsigned char *buffer, UInt size);
extern int rs485_init();

#endif /* RS485_H_ */
