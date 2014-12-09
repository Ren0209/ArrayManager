/*
 * TCP_UDP_network.h
 *
 *  Created on: 2014/3/24
 *      Author: SHIN
 */


 /* NDK Header files */
#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/_stack.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/ndk/inc/socket.h>
#include <xdc/runtime/System.h>
//#define server_ip "59.120.25.27"
#define server_ip "140.118.170.189"
#define server_port 9966

//******* for debug *********//
#include <stdio.h>
#include <stdlib.h>

//*****************************************************************************
//
// AM Join Data Protocol Packet - Number of Byte
//
//*  Created on: 2014/7/7
// *      Author: didi Lin
//*****************************************************************************
#define AMJoinPKLength 				221
#define AMJoinResponsePKLength 		8
#define AMJoinResultPKPos			6
#define AMJoinCommand 				0x03
#define CommandPKTailCode 			0x62
#define AMJoin_Period 				5000

#define Port_H						0x26
#define Port_L						0xEE    // port 9966  => 26EE

#define Join_Success 				0x02
#define Join_Deny					0x03 	 // MAC Doesn't Exist
#define Join_Failed 				0x04	 // Retry on Next Period

extern UInt8 macAddress[6];

//*****************************************************************************
//
//				 JB Join Data Protocol Packet - Number of Byte
//
//				 Author: didi Lin
//				 Created on: 2014/9/5
//*****************************************************************************
#define JBJoinPKLength 				124
#define JBJoinResponsePKLength 		14
#define JBJoinCommand 				0x08
#define JBJoinResultPKPos			6


//*****************************************************************************
//
//				 Define Task Event & Flag state
//
//				 Author: didi Lin
//				 Created on: 2014/8/26
//*****************************************************************************
#define AM_Join 		0
#define JB_Join 		1
#define Socket_fail 	2
#define Nothing			20

/* TCP_UDP_network.h */

//*****************************************************************************
//
//				 JB Join Data Protocol Packet - Number of Byte
//
//				 Author: dong Dong
//				 Created on: 2014/12/8
//*****************************************************************************
#define PVPeriodicPKLength			488
#define PVPeriodicResponsePKLength	??
#define PVPeriodicCommand 			0x07
#define PVPeriodicCommandPKTailCode	0x61
#define PVPeriodicUpdatePeriod1		0x00
#define PVPeriodicUpdatePeriod2		0x14

//*****************************************************************************
//
// AM Header code - Number of Byte
//
//*****************************************************************************
#define AMJoinHeader 0x58



extern SOCKET server_sock;
extern struct sockaddr_in server_sin;
extern IPN server_IPAddr;
extern struct timeval server_timeout;


extern int network_initial();
extern int TCP_transmission(char *pbuf, int size);
extern int UDP_transmission();
extern int AM_JoinRequest();
extern int JB_JoinRequest(int Table_Idex);
extern int GenerateAMJoinCommandPK();
extern int GenerateJBJoinCommandPK();

