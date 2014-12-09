/*
 * Bus_Raw_Protocol.c
 *
 *  Created on: 2014/5/21
 *      Author: Ren
 */





#include "../AM_LIB/rs485.h"
#include "Bus_Raw_Protocol.h"

uint8 packet_buffer[total_array];
Packet packet;
int Bus_ID =0;
char BUS_ID = 0;
int Bus_ID_for_PV_Value =0;
int a = 555;
//------Member Table-------
unsigned char Member_Talbe[20][256];
//row0-row5->MAC
//row6->Valid
//row7->Diode Temperature
//row8~row9->Voltage
//row10~row11->Current
//row12~15->Power/Energy
//row16~19->Alert State
//-------------------------

int Generate_CheckSum (unsigned char *Pack, unsigned char Start_Index,unsigned char Lens)
{
	unsigned char i;
	unsigned long Check_Sum = 0;

	for (i=Start_Index; i< (Lens+Start_Index); i++)
		Check_Sum += Pack[i];

	//
	// Ignaor Saturation
	Check_Sum = (Check_Sum&0xff);

	return Check_Sum;
}

uint8 Broadcast_Packet(void)
{

	unsigned char broadcast_array[8]={0};
	uint8 checksum0 = 0;
	//checksum = (uint8)Generate_CheckSum(broadcast_array,BRD_Ctl_Index,(BRD_Ctl_Byte+BRD_BusID_Byte+BRD_Command_Byte));
	broadcast_array[0] = 0x4A;
	broadcast_array[1] = 0x00;
	broadcast_array[2] = 0x03;
	broadcast_array[3] = 0x07;
	broadcast_array[4] = 0x00;
	broadcast_array[5] = 0x05;
	broadcast_array[7] = 0x3B;
	checksum0 = Generate_CheckSum(broadcast_array,3,broadcast_array[2]);
	broadcast_array[6] = checksum0;
	rs485_write(broadcast_array,8);
	return 0;
}


int Rs4852Array(int size)
{
	int rs485_num=0;
	rs485_num = rs485_read(packet_buffer,size);
	//rs485_write(packet_buffer,30);.
	return rs485_num;
}

int Array2Packet(int rs485_num,UART_Handle uart)
{
		int packet_num=0;
		uint8 *rs485_ptr = packet_buffer;
		uint8 receive_array[128];
		uint8 checksum;
		packet_ptr ptr = &packet;
		int i=0;
		//rs485_write(packet_buffer,30);
		uint8 * old_ptr = rs485_ptr;

		while( rs485_ptr-old_ptr < rs485_num)
		{

		while(*rs485_ptr!=Header_Code && rs485_ptr-old_ptr < rs485_num)
			rs485_ptr++;
		//read length

		packet_num++;
		receive_array[0] = *rs485_ptr;				//header_code
		receive_array[1] = *(rs485_ptr+1);
		receive_array[2] = *(rs485_ptr+2);				//length

		for(i=3;i<=receive_array[2]+4;i++)
			receive_array[i] = *(rs485_ptr+i);
		// Judge packet is correct or not
		if(receive_array[receive_array[2]+4]==Tail_Code)
			{
				checksum = Generate_CheckSum((unsigned char*)receive_array,3,receive_array[2]);
				rs485_ptr += (receive_array[2]+3);			// move ptr to check sum

				if(*(rs485_ptr)==checksum)
				{
					ptr->HeaderCode = receive_array[0];
					ptr->PacketLength = ((receive_array[1]<<8)|receive_array[2]);
					ptr->ControlField = receive_array[3];
					ptr->BusID = receive_array[4];
					ptr->CommandType = receive_array[5];
					ptr->CheckSum = checksum;
					ptr->TailCode = receive_array[receive_array[2]+3];
					//rs485_write(receive_array,ptr->PacketLength+5);
					rs485_ptr+=2;
					//rs485_write(receive_array,14);
					switch(receive_array[5])
					{
					case 0x03:	//Response PV Value
						{
							Response_PV_Value(receive_array);
							//UART_write(uart,"Response PV!!",13);
							break;
						}

					case 0x06:	//Join Request
						{
							int rs485_num=0;
							Assign_BusID(receive_array);
							rs485_num = Rs4852Array(14);
							if(rs485_num==14)
								Array2Packet(rs485_num,uart);
							break;
						}
						/*case 0x06:	//Join Request
						{
							int rs485_num=0;
							int count=0;
							while(count < 3)
							{
								Assign_BusID(receive_array);
								rs485_num = Rs4852Array(14);
								count++;
								if(rs485_num==14)
								{
									Array2Packet(rs485_num);
									break;
								}
							}
							break;
						}*/
						case 0x08:	//ACK Response
						{
							ACKResponse(receive_array,uart);
							break;
						}
					}
				}
				else;
			}
		else
			{
				rs485_ptr += receive_array[2]+5;
			}
		}
		return 0;
}


int Assign_BusID(uint8* Buffer_Array)
{
	uint8 Send_Array[14];
	uint8 *rs485_ptr = Send_Array;
	uint8 *buffer_ptr = Buffer_Array;
	int i;

	*rs485_ptr = 0x4A;
	rs485_ptr++;
	*rs485_ptr = 0x00;
	rs485_ptr++;
	*rs485_ptr = 0x0A;
	rs485_ptr++;
	*rs485_ptr = 0x03;
	rs485_ptr++;
	*rs485_ptr = Bus_ID;
	rs485_ptr++;
	*rs485_ptr = 0x07;
	rs485_ptr++;
	*rs485_ptr = 0x00;
	rs485_ptr++;
	buffer_ptr+=6; //Read Payload
	for(i=0;i<6;i++)
	{
		*rs485_ptr = *buffer_ptr;
		rs485_ptr++;
		buffer_ptr++;
	}
	*rs485_ptr = Generate_CheckSum((unsigned char*)Send_Array,3,10);
	rs485_ptr++;
	*rs485_ptr = 0x3B;
	rs485_ptr++;
	rs485_write(Send_Array,15);
	return 0;
}


int ACKResponse (uint8* Buffer_Array , UART_Handle uart)
{
	uint8 ACK_buffer[8];

	ACK_buffer[0] = 0x4A;
	ACK_buffer[1] = 0x00;
	ACK_buffer[2] = 0x03;
	ACK_buffer[3] = 0x02;
	ACK_buffer[4] = Bus_ID;
	ACK_buffer[5] = 0x09;
	ACK_buffer[6] = Generate_CheckSum(ACK_buffer,3,ACK_buffer[2]);
	ACK_buffer[7] = 0x3B;
	rs485_write(ACK_buffer,8);
	Member_Talbe[0][Bus_ID] = Buffer_Array[6];
	Member_Talbe[1][Bus_ID] = Buffer_Array[7];
	Member_Talbe[2][Bus_ID] = Buffer_Array[8];
	Member_Talbe[3][Bus_ID] = Buffer_Array[9];
	Member_Talbe[4][Bus_ID] = Buffer_Array[10];
	Member_Talbe[5][Bus_ID] = Buffer_Array[11];
	Member_Talbe[6][Bus_ID] = 0x03;
	//BUS_ID = (char)(Bus_ID+0x30);

	/*
	UART_write(uart,(char*)"Junction box ID ", 16);
	char* id = &BUS_ID;
	UART_write(uart,(char*)id , 1);
	UART_write(uart,(char*)" is Joined!", 11);
	*/
	char temp[50];
	char buf[50];
	int n=0,m=0;
	int i=0;
	for(i=0;i<=6;i++)
		temp[i] = (char)Buffer_Array[i];

	n = sprintf(buf,"Junction box ID %d is Joined!\t",Bus_ID);
	m = sprintf(temp,"MAC = %02X%02X%02X%02X%02X%02X!\r\n",Buffer_Array[6],Buffer_Array[7],Buffer_Array[8],Buffer_Array[9],Buffer_Array[10],Buffer_Array[11]);
	UART_write(uart, buf,n);
	UART_write(uart, temp,m);
	//UART_write(uart, "abc",3);
	/*UART_write(uart, (char*)Buffer_Array+7,1);
	UART_write(uart, (char*)Buffer_Array+8,1);
	UART_write(uart, (char*)Buffer_Array+9,1);
	UART_write(uart, (char*)Buffer_Array+10,1);
	UART_write(uart, (char*)Buffer_Array+11,1);*/
	Bus_ID++;
	return 0;
}

int Request_PV_Value(UART_Handle uart)
{
	uint8 Request_PV_Value_Buffer[8];
	int num=0;

	Request_PV_Value_Buffer[0] = 0x4A;
	Request_PV_Value_Buffer[1] = 0x00;
	Request_PV_Value_Buffer[2] = 0x03;
	Request_PV_Value_Buffer[3] = 0x02;
	Request_PV_Value_Buffer[5] = 0x03;
	Request_PV_Value_Buffer[7] = 0x3B;

	for(Bus_ID_for_PV_Value=0;Bus_ID_for_PV_Value<Bus_ID;Bus_ID_for_PV_Value++)
	{
		Request_PV_Value_Buffer[4] = Bus_ID_for_PV_Value;
		Request_PV_Value_Buffer[6] = Generate_CheckSum(Request_PV_Value_Buffer,3,Request_PV_Value_Buffer[2]);
		rs485_write(Request_PV_Value_Buffer,8);
		num = Rs4852Array(21);
		if(num==21)
			Array2Packet(num,uart);
		else;
	}
	return 0;
}

int Response_PV_Value(uint8* Buffer_Array)
{
	Member_Talbe[7][Bus_ID_for_PV_Value] = Buffer_Array[6];
	Member_Talbe[8][Bus_ID_for_PV_Value] = Buffer_Array[7];
	Member_Talbe[9][Bus_ID_for_PV_Value] = Buffer_Array[8];
	Member_Talbe[10][Bus_ID_for_PV_Value] = Buffer_Array[9];
	Member_Talbe[11][Bus_ID_for_PV_Value] = Buffer_Array[10];
	Member_Talbe[12][Bus_ID_for_PV_Value] = Buffer_Array[11];
	Member_Talbe[13][Bus_ID_for_PV_Value] = Buffer_Array[12];
	Member_Talbe[14][Bus_ID_for_PV_Value] = Buffer_Array[13];
	Member_Talbe[15][Bus_ID_for_PV_Value] = Buffer_Array[14];
	Member_Talbe[16][Bus_ID_for_PV_Value] = Buffer_Array[15];
	Member_Talbe[17][Bus_ID_for_PV_Value] = Buffer_Array[16];
	Member_Talbe[18][Bus_ID_for_PV_Value] = Buffer_Array[17];
	Member_Talbe[19][Bus_ID_for_PV_Value] = Buffer_Array[18];
	return 0;
}


/*packet_ptr Packet_Receive(int packet_num)
{

			char checksum;
			packet_ptr ptr = &packet;
			char receive_array[14];
			int j=0,i=0;

	while(1)
	{
		if(j==packet_num)
			return ptr;
		for(i=0;i<14;i++)		//14 is number of receive packet
		{
			receive_array[i] = packet_buffer[i+j*14];
		}
		checksum = Generate_CheckSum((unsigned char*)receive_array,3,receive_array[2]);
		// Judge checksum is correct or not
		if(checksum!=receive_array[receive_array[2]+3])
			return (packet_ptr)-1;

		ptr->HeaderCode = receive_array[0];
		ptr->PacketLength = ((receive_array[1]<<8)|receive_array[2]);
		ptr->ControlField = receive_array[3];
		ptr->BusID = receive_array[4];
		ptr->CommandType = receive_array[5];
		ptr->CheckSum = checksum;
		ptr->TailCode = receive_array[receive_array[2]+3];

		rs485_write(receive_array,ptr->PacketLength+5);
		j++;
	}
}
*/










