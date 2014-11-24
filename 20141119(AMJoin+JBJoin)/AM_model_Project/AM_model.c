/*
 * Copyright (c) 2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *    ======== tcpEcho.c ========
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

 /* NDK Header files */
#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/_stack.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>

/* Example/Board Header files */
#include "Board.h"

/* AM_LIB RS-485 Header files */
#include "./AM_LIB/rs485.h"
#include "./Bus_Raw_Protocol/Bus_Raw_Protocol.h"
#include "AM_LIB/TCP_UDP_network.h"


/*    Function Prototype   */
void Create_uartHandler();



unsigned char task_Event;
int JB_Count;
extern int Bus_ID;

UART_Handle uart2;
UART_Params uartParams;

/*
 *  ======== tcpHandler ========
 *  Creates new Task to handle new TCP connections.
 */
Void tcpHandler(UArg arg0, UArg arg1)
{	task_Event = AM_Join;
	unsigned char Join_Flag;
	int JB_Table_Idex;

	TaskSleep(1000);	//wait 5 seconds for DHCP ready
//	network_initial();	//initial network parameter

	while(1){

		switch(task_Event)
		{	case AM_Join:

			Join_Flag = Join_Failed;
			do
			{
				//Join_Flag = AM_JoinRequest();
				//System_printf("Join_Flag: %d  \n",Join_Flag);
				//System_flush();
				TaskSleep(1000);
			}while(0);//Join_Flag != Join_Success);
			task_Event = JB_Join;
			Create_uartHandler();
			break;

			case JB_Join:

			Join_Flag = Join_Failed;
			JB_Table_Idex = 0;
			while(JB_Table_Idex < JB_Count)
			{
				//System_printf("JB_Join_in: %d  \n",JB_Table_Idex);
				//System_flush();
				Join_Flag = JB_JoinRequest(JB_Table_Idex);
				if(Join_Flag == Join_Success){
					//System_printf("JB_Join_Flag: %d  \n",JB_Table_Idex);
					//System_flush();
					JB_Table_Idex++;
				}
				else
				{
					//System_printf("ERROR JB_Join  \n");
					//System_flush();
				}
				Join_Flag = Join_Failed;
				//System_printf("JB_Join_Flag: %d  \n",JB_Table_Idex);
				//System_flush();
				TaskSleep(1000);
			}
			task_Event = Nothing;
			break;

			default:

			//System_printf("tcpHandler Do Nothing T_T \n");
			//System_flush();
			break;

		} /*  end switch  */
		TaskSleep(2000);

	} /* end while  */
}


/*
 *  ======== uartHandler ========
 *  Creates new Task to handle new UART connections.
 */
Void uartHandler(UArg arg0, UArg arg1)
{
	int i=0;
	/*UART_Handle uart;
	UART_Params uartParams;
	*/
	int packet_num;


	/*UART_Params_init(&uartParams);

	uartParams.writeMode = UART_MODE_BLOCKING;
	uartParams.writeDataMode = UART_DATA_BINARY  ;
	uartParams.readDataMode = UART_DATA_BINARY  ;
	uartParams.readReturnMode = UART_RETURN_FULL;
	uartParams.readEcho = UART_ECHO_OFF;
	uartParams.baudRate = 115200;
	uartParams.readTimeout = rs485_timeout;
	uart = UART_open(Board_UART2, &uartParams);
	if (uart == NULL) {
		System_abort("Error opening the UART");
	}
*/
	System_printf("uartHandler in!!!  \n");
	System_flush();
//	rs485_init();

	rs485_write((const uint8*)"RS-485 testing message", 22);
	//UART_write(uart,(char*)"Debug testing message", 21);
	//char broadcast_array[8];

	/*char a = 'a';
	rs485_write((const char*)&a,1);*/

	for(i=0;i<5;i++)					// Broadcast n times
	{
		Broadcast_Packet();
		packet_num = Rs4852Array(total_array);
		Array2Packet(packet_num,uart2);
	}
	JB_Count = Bus_ID;
	task_Event = JB_Join;
	//Request_PV_Value(uart);
	//Packet_Receive(packet_num);
	//rs485_write(Broadcast_Packet(), sizeof(Broadcast_Packet));;

	/*while(1){
		rs485_read(buffer,sizeof(buffer));
		rs485_write(buffer,sizeof(buffer));
		UART_write(uart,(const char *)buffer,sizeof(buffer));
	}*/



}

void Create_uartHandler()
{
		// Create UART task (RS-485)
		Task_Handle UART_taskHandle;
		Task_Params UART_taskParams;
		Error_Block UART_eb;
		Task_Params_init(&UART_taskParams);
		Error_init(&UART_eb);
		UART_taskParams.stackSize = 2048;
		UART_taskParams.priority = 3;
		UART_taskParams.arg0=1000;
		UART_taskHandle = Task_create((Task_FuncPtr)uartHandler, &UART_taskParams, &UART_eb);
		if (UART_taskHandle == NULL) {
			System_printf("main: Failed to create uartHandler Task\n");
		}
		System_printf("Create_uartHandler!!! \n");
		System_flush();
}

/*
 *  ======== main thread ========
 *  Creates main Task to handle another tasks.
 */
Void main_thread(UArg arg0, UArg arg1)
{
	// Create TCP task
	Task_Handle TCP_taskHandle;
	Task_Params TCP_taskParams;
	Error_Block TCP_eb;
	Task_Params_init(&TCP_taskParams);
	Error_init(&TCP_eb);
	TCP_taskParams.stackSize = 8192;
	TCP_taskParams.priority = 2;
	TCP_taskHandle = Task_create((Task_FuncPtr)tcpHandler, &TCP_taskParams, &TCP_eb);
	if (TCP_taskHandle == NULL) {
		System_printf("main: Failed to create tcpHandler Task\n");
	}

	// Create UART task (RS-485)
	/*Task_Handle UART_taskHandle;
	Task_Params UART_taskParams;
	Error_Block UART_eb;
	Task_Params_init(&UART_taskParams);
	Error_init(&UART_eb);
	UART_taskParams.stackSize = 1024;
	UART_taskParams.priority = 1;
	UART_taskParams.arg0=1000;
	UART_taskHandle = Task_create((Task_FuncPtr)uartHandler, &UART_taskParams, &UART_eb);
	if (UART_taskHandle == NULL) {
		System_printf("main: Failed to create uartHandler Task\n");
	}*/
}

/*
 *  ======== main ========
 */
Int main(Void)
{
    Task_Handle taskHandle;
    Task_Params taskParams;
    Error_Block eb;

    /* Call board init functions */
    Board_initGeneral();
	Board_initGPIO();
    Board_initEMAC();
    Board_initUART();
    rs485_init();

    System_printf("Starting the Array Mang\nSystem provider is set to "
                  "SysMin. Halt the target and use ROV to view output.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /*
     *  Create the Task that create and handle another tasks
     *
     */
    Task_Params_init(&taskParams);
    Error_init(&eb);

    taskParams.stackSize = 1024;
    taskParams.priority = 1;
    taskHandle = Task_create((Task_FuncPtr)main_thread, &taskParams, &eb);
    if (taskHandle == NULL) {
        System_printf("main: Failed to create main_thread Task\n");
    }

    /* Start BIOS */
    BIOS_start();

    return (0);
}
