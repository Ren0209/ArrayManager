/*
 * Bus_Raw_Protocol.h
 *
 *  Created on: 2014/5/21
 *      Author: Ren
 */

#ifndef BUS_RAW_PROTOCOL_H_
#define BUS_RAW_PROTOCOL_H_
#define total_array 3584



#endif /* BUS_RAW_PROTOCOL_H_ */

/*************************************************************************************/



/*************************************************************************************/
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTTiva.h>
#include "../DK_TM4C129X.h"
#include "../board.h"
#include <xdc/runtime/System.h>


#ifndef uint32
	#define uint32 unsigned int
#endif
#ifndef uint16
	#define uint16 unsigned short int
#endif
#ifndef uint8
	#define uint8 unsigned char
#endif


typedef struct Packet {
	uint8 HeaderCode;
	uint16 PacketLength;
	uint8 ControlField;
	uint8 BusID;
	uint8 CommandType;
	uint8 *Payload;
	uint8 CheckSum;
	uint8 TailCode;
} Packet;
typedef Packet *packet_ptr;


//*****************************************************************************
//
// Bus Raw Data Protocol Packet - Number of Byte
//
//*****************************************************************************
#define BRD_Head_Byte		1
#define BRD_Lens_Byte		2
#define BRD_Ctl_Byte		1
#define BRD_BusID_Byte		1
#define BRD_Command_Byte	1
#define BRD_Checksum_Byte	1
#define BRD_Tail_Byte		1


//*****************************************************************************
//
// Bus Raw Data Protocol Packet - Index Macro
//
//*****************************************************************************
#define BRD_Lens_Index 			(BRD_Head_Byte)
#define BRD_Ctl_Index 			(BRD_Head_Byte+BRD_Lens_Byte)
#define BRD_BusID_Index 	 	(BRD_Ctl_Index+BRD_Ctl_Byte)
#define BRD_Command_Index 		(BRD_BusID_Index+BRD_BusID_Byte)
#define BRD_PayL_Index 			(BRD_command_Index+BRD_command_Byte)


//*****************************************************************************
//
// Bus Raw Data Protocol - Command Type Table
//
//*****************************************************************************
#define ctPV_Val	(0x03)
#define ctPV_Info	(0x04)
#define ctCH_SCAN	(0x05)
#define ctJOIN_Req	(0x06)
#define ctAssignID	(0x07)
#define ctAsign_Ack	(0x08)
#define ctACK_Resp	(0x09)


#define Header_Code 0x4A
#define Tail_Code 0x3B




int Generate_CheckSum (unsigned char *Pack, unsigned char Start_Index,unsigned char Lens);
uint8 Broadcast_Packet();
int Rs4852Array(int);
int Array2Packet(int,UART_Handle uart);
int Assign_BusID(uint8* Buffer_Array);
int ACKResponse (uint8* Buffer_Array , UART_Handle uart);
int Request_PV_Value(UART_Handle uart);
int Response_PV_Value(uint8* Buffer_Array);

//void packet2array(void);


//packet_ptr Packet_Receive(int);
