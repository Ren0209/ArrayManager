/*
 * TCP_UDP_network.c
 *
 *  Created on: 2014/3/24
 *      Author: SHIN
 */
/*
 * Author: didi Lin
 * Created on: 2014/8/26
 * Description: network reset, TCP_transmission doesn't work.
 *
 *
 *
 */

#include "TCP_UDP_network.h"

static SOCKET server_sock;
static struct sockaddr_in server_sin;
static IPN server_IPAddr;
static struct timeval server_timeout;
extern unsigned char Member_Talbe[20][256];

FILE *error_list;
int error_count = 0;

int network_initial()
{
	server_sock = INVALID_SOCKET;
	server_IPAddr = inet_addr(server_ip);

	// Prepare address for connect
	bzero( &server_sin, sizeof(struct sockaddr_in) );
	server_sin.sin_family = AF_INET;
	server_sin.sin_len = sizeof( server_sin );
	server_sin.sin_addr.s_addr = server_IPAddr;
	server_sin.sin_port = htons(server_port);

	// Configure our Tx and Rx timeout to be 5 seconds
	server_timeout.tv_sec = 5;
	server_timeout.tv_usec = 0;

	return 0;
}

int TCP_transmission(char *pbuf, int size)
{
	// Allocate the file descriptor environment for this Task
	fdOpenSession( (HANDLE)Task_self() );

	server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_sock < 0) {
	        return -1;
	    }

	setsockopt( server_sock, SOL_SOCKET, SO_SNDTIMEO, &server_timeout, sizeof( server_timeout ) );
	setsockopt( server_sock, SOL_SOCKET, SO_RCVTIMEO, &server_timeout, sizeof( server_timeout ) );

	// Connect socket
	while( connect( server_sock, (PSA) &server_sin, sizeof(server_sin) ) < 0 )
		Task_sleep(500);


	send( server_sock, pbuf, size, 0);

	fdClose(server_sock);
	fdCloseSession( (HANDLE)Task_self() );

	return 0;
}

//*  Created on: 2014/9/2
// *      Author: didi
int AM_JoinRequest()
{
	unsigned char *Join_Array = 0;
	unsigned char *Recv_Array = 0;
	int recv_num = 0;
	char Join_Result;

	server_sock = INVALID_SOCKET;
	struct timeval timeout;

	server_IPAddr = inet_addr(server_ip);

	// Allocate the file descriptor environment for this Task
	fdOpenSession( (HANDLE)Task_self() );
	System_printf("\n== Start AM Join Request ==\n");
	System_flush();

	// Create test socket
	server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( server_sock == INVALID_SOCKET )
	{
		System_printf("failed socket create (%d)\n",fdError());
		goto leave;
	}

	// Prepare address for connect
	bzero( &server_sin, sizeof(struct sockaddr_in) );
	server_sin.sin_family = AF_INET;
	server_sin.sin_len = sizeof( server_sin );
	server_sin.sin_addr.s_addr = server_IPAddr;
	server_sin.sin_port = htons(server_port);

	// Configure our Tx and Rx timeout to be 2 seconds
	server_timeout.tv_sec = 5;
	server_timeout.tv_usec = 0;
	setsockopt( server_sock, SOL_SOCKET, SO_SNDTIMEO, &server_timeout, sizeof( server_timeout ) );
	setsockopt( server_sock, SOL_SOCKET, SO_RCVTIMEO, &server_timeout, sizeof( server_timeout ) );

	// Connect socket
	if( connect( server_sock, (PSA) &server_sin, sizeof(server_sin) ) < 0 )
	{
		System_printf("failed connect (%d)\n",fdError());
		goto leave;
	}
	// Allocate a working buffer
	if( !(Join_Array = malloc( AMJoinPKLength )) )
	{
		System_printf("failed Join_Array buffer allocation\n");
		goto leave;
	}
	if( !(Recv_Array = malloc( AMJoinResponsePKLength )) )
	{
		System_printf("failed Recv_Array buffer allocation\n");
		goto leave;
	}

	// generate AM Join Package
	Join_Array[0] = AMJoinHeader;
	Join_Array[1] = 217;
	Join_Array[2] = 0x00;
	Join_Array[3] = rand()%256;
	Join_Array[4] = AMJoinCommand;
	Join_Array[5] = 0x01;
	GenerateAMJoinCommandPK(Join_Array);
	Join_Array[AMJoinPKLength-1] = CommandPKTailCode;

	// Send Join Package
	if( send( server_sock, Join_Array, AMJoinPKLength, 0 ) < 0 )
		{
			System_printf("send failed (%d)\n",fdError());
			goto leave;
		}

	// receive Server's Join Response
	recv_num = recv( server_sock, Recv_Array, AMJoinResponsePKLength, MSG_WAITALL );
	if( recv_num < 0 )
	{
		System_printf("recv failed (%d)\n",fdError());
		goto leave;
	}
	else
	{
		System_printf("Recv: %d bytes \n",recv_num);
		System_flush();

	}


leave:
	switch(Recv_Array[AMJoinResultPKPos])
	{	case Join_Success:
			 Join_Result = Join_Success;
		break;

		case Join_Deny:
			 Join_Result = Join_Deny;
			 	 error_count++;
			 	error_list = fopen(".//error_list.txt","a");
			 	fprintf(error_list,"AM Join Error %d\n",error_count);
			 	fclose(error_list);
		break;

		case Join_Failed:
			 Join_Result = Join_Failed;
			 	 error_count++;
			 	error_list = fopen(".//error_list.txt","a");
			 	fprintf(error_list,"AM Join Error %d\n",error_count);
			 	fclose(error_list);
		break;
		default: Join_Result = Join_Failed;
				 error_count++;
				 error_list = fopen(".//error_list.txt","a");
				 fprintf(error_list,"AM Join Error %d\n",error_count);
				 fclose(error_list);
	}

	// Free Pointer & Socket
	if( Join_Array )
		free( Join_Array );
	if( Recv_Array )
			free( Recv_Array );
	if( server_sock != INVALID_SOCKET )
			fdClose( server_sock );
	System_printf("== End AM Join Request ==\n\n");
	System_flush();
	//Free the file descriptor environment for this Task
	fdCloseSession( (HANDLE)Task_self() );
	return Join_Result;
}


//*  Created on: 2014/9/4 	Continuous modification
// *      Author: Ren

int JB_JoinRequest(int Table_Index)
{
	unsigned char *Join_Array = 0;
	unsigned char *Recv_Array = 0;
	int recv_num = 0;
	char Join_Result;

	server_sock = INVALID_SOCKET;
	struct timeval timeout;

	server_IPAddr = inet_addr(server_ip);

	// Allocate the file descriptor environment for this Task
	fdOpenSession( (HANDLE)Task_self() );
	System_printf("\n== Start JB Join Request ==\n");
	System_flush();

	// Create test socket
	server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( server_sock == INVALID_SOCKET )
	{
		System_printf("failed socket create (%d)\n",fdError());
		goto leave;
	}
	System_printf("== Fin JB socket Join Request ==\n\n");
	System_flush();
	// Prepare address for connect
	bzero( &server_sin, sizeof(struct sockaddr_in) );
	server_sin.sin_family = AF_INET;
	server_sin.sin_len = sizeof( server_sin );
	server_sin.sin_addr.s_addr = server_IPAddr;
	server_sin.sin_port = htons(server_port);

	// Configure our Tx and Rx timeout to be 2 seconds
	server_timeout.tv_sec = 5;
	server_timeout.tv_usec = 0;
	setsockopt( server_sock, SOL_SOCKET, SO_SNDTIMEO, &server_timeout, sizeof( server_timeout ) );
	setsockopt( server_sock, SOL_SOCKET, SO_RCVTIMEO, &server_timeout, sizeof( server_timeout ) );

	System_printf("== Fin JB socket set Join Request ==\n\n");
	System_flush();
	// Connect socket
	if( connect( server_sock, (PSA) &server_sin, sizeof(server_sin) ) < 0 )
	{
		System_printf("failed connect (%d)\n",fdError());
		goto leave;
	}

	System_printf("== Fin JB socket connect Join Request ==\n\n");
	System_flush();
	// Allocate a working buffer
	if( !(Join_Array = malloc( JBJoinPKLength )) )
	{
		System_printf("failed Join_Array buffer allocation\n");
		goto leave;
	}
	if( !(Recv_Array = malloc( JBJoinResponsePKLength )) )
	{
		System_printf("failed Recv_Array buffer allocation\n");
		goto leave;
	}
	System_printf("== Fin JB array malloc Join Request ==\n\n");
	System_flush();
	// generate JB Join Package
	Join_Array[0] = AMJoinHeader;
	Join_Array[1] = 120;
	Join_Array[2] = 0x00;
	Join_Array[3] = rand()%256;
	Join_Array[4] = JBJoinCommand;
	Join_Array[5] = 0x01;
	GenerateJBJoinCommandPK(Join_Array,Table_Index);
	Join_Array[JBJoinPKLength-1] = CommandPKTailCode;

	System_printf("== Fin JB PK set Join Request ==\n\n");
	System_flush();
	// Send Join Package
	if( send( server_sock, Join_Array, JBJoinPKLength, 0 ) < 0 )
		{
			System_printf("send failed (%d)\n",fdError());
			goto leave;
		}
	System_printf("== Fin JB socket send Join Request ==\n\n");
	System_flush();
	// receive Server's Join Response
	recv_num = recv( server_sock, Recv_Array, JBJoinResponsePKLength, MSG_WAITALL );

	System_printf("== Fin JB socket recv Join Request ==\n\n");
	System_flush();

	if( recv_num < 0 )
	{
		System_printf("recv failed (%d)\n",fdError());
		goto leave;
	}
	else
	{
		System_printf("Recv: %d bytes \n",recv_num);
		System_flush();

	}

leave:
	switch(Recv_Array[JBJoinResultPKPos])
	{	case Join_Success:
			 Join_Result = Join_Success;
		break;

		default: Join_Result = Join_Failed;
	}

	// Free Pointer & Socket
	if( Join_Array )
		free( Join_Array );
	if( Recv_Array )
			free( Recv_Array );
	if( server_sock != INVALID_SOCKET )
			fdClose( server_sock );
	System_printf("== End JB Join Request ==\n\n");
	System_flush();
	//Free the file descriptor environment for this Task
	fdCloseSession( (HANDLE)Task_self() );
	return Join_Result;
}



int GenerateAMJoinCommandPK(unsigned char *Join_Array)
{	int i;
	for(i=0;i<6;i++)
		Join_Array[i+6] = macAddress[i];    // Join_Array[6]~Join_Array[11]
	for(i=12;i<16;i++)
		Join_Array[i] = 0xff;				// Reserved
	Join_Array[16] = Port_L;
	Join_Array[17] = Port_H;
	for(i=18;i<AMJoinPKLength-1;i++)
		Join_Array[i] = 0xff;				// ignore port number
	return 0;
}

int GenerateJBJoinCommandPK(unsigned char *Join_Array,int Table_Index)
{	int i;
	for(i=0;i<6;i++)
		Join_Array[i+6] = macAddress[i];    // Join_Array[6]~Join_Array[11]
	Join_Array[12] = Table_Index;
	for(i=0;i<6;i++)
		Join_Array[i+13] = Member_Talbe[i][Table_Index];				// ignore port number
	for(i=19;i<JBJoinPKLength-1;i++)
		Join_Array[i] = 0xff;
	return 0;
}


