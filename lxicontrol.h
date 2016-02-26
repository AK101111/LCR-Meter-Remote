/* Include file to control LXI instruments, version 0.21, Jun 16 2010.
 * Copyright (C) 2010, Shouri Chatterjee
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 *
 * This include file is to make controlling and operating
 * LAN based instruments easy.
 * 
 * In the program, first create a connection.
 * Then issue control commands.
 * Then read the response / measurement.
 * 
 * You can operate multiple instruments with multiple IP 
 * addresses.
*/

#ifndef _lxicontrol
#define _lxicontrol
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <bits/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/time.h>

#define SOCKETS_TIMEOUT	10

#ifndef _commonroutines
#include "commonroutines.h"
#endif

/* Create a TCP/IP socket connection to the desired IP
   address and port number. This routine also kills
   Nagel's algorithm and allows packets of small size. */

int CreateConnection(char* IPAddress, int PortNumber)
{
	int CreatedSocket;
	struct sockaddr_in MyAddress;

	if((CreatedSocket=socket(PF_INET,SOCK_STREAM,0))==-1) 
	{ printf("Could not create socket!\n"); exit(1);}

	memset(&MyAddress,0,sizeof(struct sockaddr_in)); //Initialize.
	MyAddress.sin_family = PF_INET; // IPv4
	MyAddress.sin_port=htons(PortNumber); // Port number
	MyAddress.sin_addr.s_addr = inet_addr(IPAddress); //IP

	// Establish TCP connection
	if(connect(CreatedSocket, (struct sockaddr *)&MyAddress, sizeof(struct sockaddr_in))==-1) 
			{
				printf("Error %i : Could not connect!\n", errno);
				perror("connect");
				exit(1);
			}
	
	// Allow packets of small size.
	int StateNODELAY = 1;
	setsockopt(CreatedSocket,IPPROTO_TCP,TCP_NODELAY, (void *)&StateNODELAY, sizeof StateNODELAY);

	return(CreatedSocket);
}

/* Write a command into an LXI instrument. This routine should
   be used after creating a connection to the instrument. */

int IssueCommand(int MySocket, char* string)
{
	int retval;

	string = FixNewlineProblemIfAny(string);
	
	if((retval=send(MySocket,string,strlen(string),0))==-1) {
		printf("Error: Unable to send message (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		exit(1);
	}
	return retval;
}

/* Routine to check if data is waiting at
   the instrument. */

int WaitForData(int MySocket)
{
	fd_set MyFDSet;
	struct timeval tv;
	int retval;
	
	// Wait for data to become available
	FD_ZERO(&MyFDSet); // Initialize fd_set structure
	FD_SET(MySocket,&MyFDSet); // Add socket to "watch list"
	tv.tv_sec=SOCKETS_TIMEOUT; tv.tv_usec=0; // Set timeout
	retval=select(MySocket+1,&MyFDSet,NULL,NULL,&tv); // Wait for change
	
	// Interpret return value
	if(retval==-1) {
		printf("Error: Problem with select (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		exit(1);
	}
	return retval; // 0 = timeout, 1 = socket status has changed
}

/* Routine to check if we need to time-out */

int CheckTimedOut(int ReadSocket)
{
	if(WaitForData(ReadSocket)==0)
	{
		// Timeout
		printf("Warning: Timeout...\n");
		return 0;
	}
}

/* Routine to read a string from the instrument.
   Don't use this for really long strings. */
char* ReadResponse(int ReadSocket, int MaxLength)
{
	char* InputBuffer = NULL;

	InputBuffer = calloc(MaxLength, sizeof(char)); // initialize
	CheckTimedOut(ReadSocket);
	if(recv(ReadSocket, InputBuffer, MaxLength, 0) == -1)
	{
		printf("Error: Unable to read message (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		printf("Did not receive any information!\n");
	}
	return(InputBuffer);
}

/* This is the routine you need to use to read
   very long strings from the instrument. */
char* ReadLongResponse(int ReadSocket, long int MinimumLength)
{
/* Note: The maximum length of the string should be less than
   2x the MinimumLength. You should guarantee that the length
   of the response be more than the MinimumLength. */
/* 
   BIG FLAW! 
   NEEDS TO BE FIXED IN THE NEXT VERSION.
   This can be fixed if I work with the header which indicates the
   number of bytes that follow. Some IEEE standard it seems...
*/
	char* InputBuffer = NULL;
	int LengthRead;

	InputBuffer = calloc(MinimumLength * 2, sizeof(char)); // initialize
	CheckTimedOut(ReadSocket);
	LengthRead = recv(ReadSocket, InputBuffer, MinimumLength, MSG_WAITALL);
	if(LengthRead == -1)
	{
		printf("Error: Unable to read message (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		printf("Did not receive any information!\n");
	}
	CheckTimedOut(ReadSocket);
	recv(ReadSocket, InputBuffer+LengthRead, MinimumLength, MSG_DONTWAIT);
	return(InputBuffer);
}

/* Routine to terminate a connection. */
int TerminateConnection(int SocketToBeClosed)
{
	if(close(SocketToBeClosed)==-1)
	{
		printf("Error: Unable to close connection (%i)...\n",errno);
		perror("sockets"); // Print error message based on errno
		exit(1);
	}
	return(0);
}

