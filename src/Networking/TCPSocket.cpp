// ****************************************************************************
// This file is part of the Integrating Vision Toolkit (IVT).
//
// The IVT is maintained by the Karlsruhe Institute of Technology (KIT)
// (www.kit.edu) in cooperation with the company Keyetech (www.keyetech.de).
//
// Copyright (C) 2014 Karlsruhe Institute of Technology (KIT).
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the KIT nor the names of its contributors may be
//    used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE KIT AND CONTRIBUTORS “AS IS” AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE KIT OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ****************************************************************************
// ****************************************************************************
// Filename:  TCPSocket.cpp
// Author:    Florian Hecht
// Date:      08.01.2009
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "TCPSocket.h"

#include <stdio.h>
#ifndef __TI_COMPILER_VERSION__
#include <memory.h>
#endif

#include <stdlib.h>
#include <errno.h>


#if defined(WIN32)
#define _WINSOCKAPI_
#include <windows.h>
#include <winsock2.h>

static int socket_ref_count = 0;

void InitWinSock()
{
	socket_ref_count++;

	if (socket_ref_count > 1)
		return;

	WORD wVersionRequested;
	WSADATA wsaData;
	int wsaerr;

	wVersionRequested = MAKEWORD(2, 0);
	wsaerr = WSAStartup(wVersionRequested, &wsaData);
	if (wsaerr != 0)
	{
		// Tell the user that we couldn't find the WinSock DLL
		printf("Error: couldn't initialize WinSock!\n");
	}

	return;
}

void ShutdownWinSock()
{
	socket_ref_count--;
	if (socket_ref_count > 0)
		return;

	WSACleanup();
}

#elif defined(__TI_COMPILER_VERSION__)
#include <string.h>
#include "Networking/VCNet.h"
#else
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#endif



CTCPSocket::CTCPSocket() : m_socket(-1), m_bListening(false)
{
#ifdef WIN32
	InitWinSock();
#endif
}

CTCPSocket::~CTCPSocket()
{
	Close();

#ifdef WIN32
	ShutdownWinSock();
#endif
}

bool CTCPSocket::Listen(const unsigned char *ip, int port)
{
	Close();
	
#ifdef WIN32
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		m_socket = -1;
		return false;
	}

	m_socket = (int)sock;

	// set linger on - helps in closing socket immidiately
	linger li;
	li.l_onoff = true;
	li.l_linger = 0;
	
	if(setsockopt(sock, SOL_SOCKET, SO_LINGER, (const char*)&li, sizeof(li)) == -1)
	{
		printf("setting linger option failed\n");
		Close();
		return false;
	}

	// set non blocking
	u_long v = 1;
	if (ioctlsocket(sock, FIONBIO, &v) == SOCKET_ERROR)
	{
		printf("setting non-blocking option failed\n");
		Close();
		return false;
	}

	struct sockaddr_in serv_addr;
	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	if (ip != NULL)
	{
		memcpy((char *)&serv_addr.sin_addr.s_addr, ip, 4);
	}
	else
	{
		serv_addr.sin_addr.s_addr = INADDR_ANY;
	}
	serv_addr.sin_port = htons(port);
	
	if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		Close();
		return false;
	}
	
	listen(sock, 5);
	m_bListening = true;

#elif defined(__TI_COMPILER_VERSION__)
	uint_32 error;
	
	m_socket = socket_stream();
	if (m_socket == VCRT_SOCKET_ERROR) 
	{
		m_socket = -1;
		return false;
	}
	
	//set linger on - helps in closing socket immidiately

	// SO_LINGER socket option is not available on VCRT.
	// But in Close() the shutdown flag FLAG_ABORT_CONNECTION enforces hard closing of the socket
	// as would SO_LINGER with zero timeout on other platforms.
	
	//set non blocking

	// there is no socket-global (non-)blocking flag on VCRT
	// - accept() is always blocking and therefore guarded by VCRT_selectset().
	// - recv() behavior is set during Recv() depending on bWait flag
	// - send(): don't wait for data to be transmitted, return after buffering. this resembles behavior of Send() on other platforms
	uint_32 opt_value = TRUE;
	error = setsockopt(m_socket, SOL_TCP, OPT_SEND_NOWAIT, &opt_value, sizeof(opt_value));
	if (error != VCRT_OK)
	{
		printf("Error, setsockopt(OPT_SEND_NOWAIT) failed with error code 0x%x\n", error);
		Close();
		return false;
	}
	
	sockaddr_in serv_addr;
	memset( &serv_addr, 0, sizeof(serv_addr));
  	serv_addr.sin_family      = AF_INET;
	if (ip != NULL)
	{
		//never cast ip to an int pointer! this would imply 4byte alignment.
		memcpy((char *)&serv_addr.sin_addr.s_addr, ip, 4);
		// vcrt expects inverse order of ip chars
		// than provided by RemoteApplicationHandler.cpp
		serv_addr.sin_addr.s_addr = revert_byte_order(serv_addr.sin_addr.s_addr);
	}
	else
	{
		serv_addr.sin_addr.s_addr = INADDR_ANY;
	}
  	serv_addr.sin_port = port; //htons(port); VCRT does not want the port in network byte order
	
	error = bind(m_socket, &serv_addr, sizeof(serv_addr));
	if (error!=VCRT_OK)
	{
		printf("\nsocket bind failed, error code: 0x%x\n", error);
		Close();
		return false;
	}
	
	error = listen(m_socket, 5);
	if (error!=VCRT_OK)
	{
		printf("\nsocket listen failed, error code: 0x%x\n", error);
		Close();
		return false;
	}
	m_bListening = true;

#else
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0) 
	{
		m_socket = -1;
		return false;
	}
	
	// set linger on - helps in closing socket immidiately
	linger li;
	li.l_onoff = true;
	li.l_linger = 0;
	
	if(setsockopt(m_socket, SOL_SOCKET, SO_LINGER, &li, sizeof(li)) == -1)
	{
		printf("setting linger option failed\n");
		Close();
		return false;
	}
	
	// set non blocking
	if (fcntl(m_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		printf("setting non-blocking option failed\n");
		Close();
		return false;
	}
	
	struct sockaddr_in serv_addr;
	bzero((char *)&serv_addr, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	if (ip != NULL)
	{
		bcopy(ip, (char *)&serv_addr.sin_addr.s_addr, 4);
	}
	else
	{
		serv_addr.sin_addr.s_addr = INADDR_ANY;
	}
	serv_addr.sin_port = htons(port);
	
	if (bind(m_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		Close();
		return false;
	}
	
	listen(m_socket, 5);
	m_bListening = true;
#endif
	
	return true;
}

CTCPSocket* CTCPSocket::Accept(unsigned char *ip)
{
	if (m_socket == -1 || !m_bListening)
		return NULL;
		
#ifdef WIN32
	SOCKET sock = (SOCKET)m_socket;
	struct sockaddr_in cli_addr;
	int clilen = sizeof(cli_addr);

	SOCKET new_socket = accept(sock, (struct sockaddr *) &cli_addr, &clilen);
	if (new_socket == INVALID_SOCKET)
	{
		return NULL;
	}

	// set non blocking
	u_long v = 1;
	if (ioctlsocket(new_socket, FIONBIO, &v) == SOCKET_ERROR)
	{
		printf("setting non-blocking option failed\n");
		Close();
		return false;
	}
	
	// set no delay for sending
	int v2 = 1;
	if (setsockopt(new_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&v2, sizeof(v2)) == -1)
	{
		printf("newly accepted socket couldn't be set to no delay mode\n");
	}

	if (ip != NULL)
	{
		memcpy(ip, (char *) &cli_addr.sin_addr.s_addr, 4);
	}
	
	CTCPSocket *s = new CTCPSocket();
	s->m_socket = (int) new_socket;

	return s;

#elif defined(__TI_COMPILER_VERSION__)
	uint_32 error;
	
	// acccept is always blocking on vcrt, so we need to test for activity first
	// which is not blocking
	error = VCRT_selectset(&m_socket, 1, (uint_32)-1);
	
	if (error==0) //no new connections, nothing to do
	{
		return NULL;
	}
	if (error==VCRT_SOCKET_ERROR) //an error occured
	{
		printf("Error in accept(): VCRT_selectset failed, error code 0x%x\n", VCRT_geterror(m_socket));
		return NULL;	
	}
	if (error!=m_socket) //should never happen as we check only this socket.
	{
		printf("Error in accept(): VCRT_selectset returned invalid socket");
		return NULL;	
	}
	

	sockaddr_in cli_addr;
	uint_16 clilen = sizeof(cli_addr);
	
	int new_socket = accept(m_socket, &cli_addr, &clilen);
	if (new_socket == VCRT_SOCKET_ERROR)
	{
		error = VCRT_geterror(m_socket);
		if (error == VCRT_OK)
		{
			printf("\nAccept: Connection reset by peer");
		}
		else
		{
			printf("Error, accept() failed with error code 0x%x\n", error);
		}
		return NULL;
	}
	
	
	
	// set non blocking

	// there is no socket-global (non-)blocking flag on VCRT
	// - accept() is always blocking and therefore guarded by VCRT_selectset().
	// - recv() behavior is set during Recv() depending on bWait flag
	// - send(): don't wait for data to be transmitted, return after buffering. this resembles behavior of Send() on other platforms
	uint_32 opt_value;
	
	opt_value = TRUE;
	error = setsockopt(m_socket, SOL_TCP, OPT_SEND_NOWAIT, &opt_value, sizeof(opt_value));
	if (error != VCRT_OK)
	{
		printf("Error, setsockopt(OPT_SEND_NOWAIT) failed with error code 0x%x\n", error);
	}
	
	
	
	// set no delay for sending
	// OPT_NO_NAGLE_ALGORITHM is the VCRT equivalent to TCP_NODELAY
	opt_value = TRUE;
	error = setsockopt(m_socket, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, &opt_value, sizeof(opt_value));
	if (error != VCRT_OK)
	{
		printf("Error, setsockopt(OPT_NO_NAGLE_ALGORITHM) failed with error code 0x%x\n", error);
	}
	
	
	
	if (ip != NULL)
	{
		// vcrt expects inverse order of ip chars
		// than provided by RemoteApplicationHandler.cpp
		cli_addr.sin_addr.s_addr = revert_byte_order(cli_addr.sin_addr.s_addr);
		//never cast ip to an int pointer! this would imply 4byte alignment.
		memcpy( ip, &cli_addr.sin_addr.s_addr, 4);
	}
	
	CTCPSocket *sock = new CTCPSocket();
	sock->m_socket = new_socket;
	
	return sock;

#else
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	
	int new_socket = accept(m_socket, (struct sockaddr *) &cli_addr, &clilen);
	if (new_socket < 0)
	{
		return NULL;
	}
	
	// set non blocking
	if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		printf("newly accepted socket couldn't be set to non blocking mode\n");
	}
	
	// set no delay for sending
	int v = 1;
	if (setsockopt(new_socket, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v)) == -1)
	{
		printf("newly accepted socket couldn't be set to no delay mode\n");
	} 
	
	if (ip != NULL)
	{
		bcopy((char *)&cli_addr.sin_addr.s_addr, ip, 4);
	}
	
	CTCPSocket *sock = new CTCPSocket();
	sock->m_socket = new_socket;
	
	return sock;
#endif
}

bool CTCPSocket::Open(const unsigned char *ip, int port)
{
	Close();
	
#ifdef WIN32
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		m_socket = -1;
		return false;
	}

	m_socket = (int)sock;

	struct sockaddr_in serv_addr;
	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	memcpy((char *)&serv_addr.sin_addr.s_addr, ip, 4);
	serv_addr.sin_port = htons(port);
	
	if (connect(sock, (const sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		Close();
		return false;
	}
	
	// set non blocking (after connect)
	u_long v = 1;
	if (ioctlsocket(sock, FIONBIO, &v) == SOCKET_ERROR)
	{
		printf("setting non-blocking option failed\n");
		Close();
		return false;
	}
	
	// set no delay for sending
	int v2 = 1;
	if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&v2, sizeof(v2)) == -1)
	{
		printf("socket couldn't be set to no delay mode\n");
	}

	m_bListening = false; 

#elif defined(__TI_COMPILER_VERSION__)
	uint_32 error;

	m_socket = socket_stream();
	if (m_socket == VCRT_SOCKET_ERROR) 
	{
		printf("Error in Open(): socket_stream() failed\n");
		m_socket = -1;
		return false;
	}
	
	sockaddr_in serv_addr;
	memset( &serv_addr, 0, sizeof(serv_addr));
  	serv_addr.sin_family      = AF_INET;
	
	//never cast ip to an int pointer! this would imply 4byte alignment.
	memcpy(&(serv_addr.sin_addr.s_addr), ip, 4);
	// vcrt expects inverse order of ip chars
	// than provided by RemoteApplicationHandler.cpp
	serv_addr.sin_addr.s_addr = revert_byte_order(serv_addr.sin_addr.s_addr);
	
	serv_addr.sin_port = port; //htons(port); VCRT does not want the port in network byte order

	//printf("DEBUG: chars %d,%d,%d,%d,  serv_addr.sin_addr.s_addr: 0x%x\n", ip[0], ip[1], ip[2], ip[3], serv_addr.sin_addr.s_addr);
	
	error = connect(m_socket, &serv_addr, sizeof(serv_addr));
	if (error != VCRT_OK)
	{
		printf("Error in Open(): connect failed, error code 0x%x\n", error);
		Close();
		return false;
	}

	// set non blocking (after connect)

	// there is no socket-global (non-)blocking flag on VCRT
	// - accept() is always blocking and therefore guarded by VCRT_selectset().
	// - recv() behavior is set during Recv() depending on bWait flag
	// - send(): don't wait for data to be transmitted, return after buffering. this resembles behavior of Send() on other platforms
	uint_32 opt_value;
	
	opt_value = TRUE;
	error = setsockopt(m_socket, SOL_TCP, OPT_SEND_NOWAIT, &opt_value, sizeof(opt_value));
	if (error != VCRT_OK)
	{
		printf("Error, setsockopt(OPT_SEND_NOWAIT) failed with error code 0x%x\n", error);
		Close();
		return false;
	}

	
	// set no delay for sending
	// OPT_NO_NAGLE_ALGORITHM is the VCRT equivalent to TCP_NODELAY
	opt_value = TRUE;
	error = setsockopt(m_socket, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, &opt_value, sizeof(opt_value));
	if (error != VCRT_OK)
	{
		printf("Error, setsockopt(OPT_NO_NAGLE_ALGORITHM) failed with error code 0x%x\n", error);
		Close();
		return false;
	}
	
	m_bListening = false;

#else
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket < 0) 
	{
		m_socket = -1;
		return false;
	}
	
	struct sockaddr_in serv_addr;
	bzero((char *)&serv_addr, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	bcopy(ip, (char *)&serv_addr.sin_addr.s_addr, 4);
	serv_addr.sin_port = htons(port);
	
	if (connect(m_socket, (const sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		Close();
		return false;
	}
	
	// set non blocking (after connect)
	if (fcntl(m_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		Close();
		return false;
	}
	
	// set no delay for sending
	int v = 1;
	if (setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v)) == -1)
	{
		Close();
		return false;
	} 
	
	m_bListening = false;
#endif
	
	return true;
}

void CTCPSocket::Close()
{
	if (m_socket == -1)
		return;
		
#ifdef WIN32
	SOCKET sock = (SOCKET)m_socket;
	closesocket(sock);

#elif defined(__TI_COMPILER_VERSION__)
	// FLAG_ABORT_CONNECTION enforces hard closing of the socket as specified
	// by the SO_LINGER socket option with zero timeout on other platforms.
	// SO_LINGER is not available on VCRT.
	uint_32 error = shutdown(m_socket, FLAG_ABORT_CONNECTION);
	if(error!= VCRT_OK)
	{
		printf("\nsocket shutdown failed, error code: 0x%x\n", error);
	}
#else
	close(m_socket);
#endif

	m_socket = -1;
}

bool CTCPSocket::Send(const void *pData, int nBytes)
{
	if (m_socket == -1 || m_bListening)
		return false;
	
#ifdef WIN32
	SOCKET sock = (SOCKET)m_socket;
	int size = 0;
	unsigned char *ptr = (unsigned char*) pData;
	while (true)
	{
		int s = send(sock, (const char *) ptr, nBytes, 0);
		if (s == SOCKET_ERROR)
		{
			int errnum = WSAGetLastError();
			if (errnum == WSAEWOULDBLOCK)
				continue;
			
			return false;
		}
		
		size += s;
		ptr += s;
		nBytes -= s;
		if (nBytes == 0)
			break;
		
	}

#elif defined(__TI_COMPILER_VERSION__)
	
	char *ptr = (char *) pData;
	while (nBytes>0)
	{
		// as OPT_SEND_NOWAIT is set above, this blocks until data is buffered
		// but does not wait for data to be send and acknowledged.
		// Send() therefore blocks until all data is passed on to the OS.
		int_32 count = send(m_socket, ptr, nBytes, 0);

		if (count==VCRT_ERROR)
		{
			printf("Error in Send(): send failed, error code 0x%x\n", VCRT_geterror(m_socket));
			return false;
		}
		
		ptr += count;
		nBytes -= count;
	}

#else
	int size = 0;
	unsigned char *ptr = (unsigned char *) pData;
	while (true)
	{
		int s = write(m_socket, ptr, nBytes);
		if (s == -1)
		{
			int errnum = errno;
			if (errnum == EAGAIN)
				continue;
			
			return false;
		}
		
		size += s;
		ptr += s;
		nBytes -= s;
		if (nBytes == 0)
			break;
		
	}
#endif
	
	return true;
}

int CTCPSocket::Recv(void *pData, int nMaxBytes, bool bWait)
{
	if (m_socket == -1 || m_bListening)
		return -1;
	
#ifdef WIN32
	SOCKET sock = (SOCKET) m_socket;

	if (bWait)
	{
		int size = 0;
		unsigned char *ptr = (unsigned char*) pData;
		while (true)
		{
			int s = recv(sock, (char *) ptr, nMaxBytes, 0);
			if (s == SOCKET_ERROR)
			{
				int errnum = WSAGetLastError();
				if (errnum == WSAEWOULDBLOCK)
					continue;
			
				// this is really bad, the connection is probably dead
				return -1;
			}
			
			ptr += s;
			size += s;
			nMaxBytes -= s; 
			
			if (nMaxBytes == 0)
				return size;
				
		}
	}
	else
	{
		int size = recv(sock, (char *) pData, nMaxBytes, 0);
		if (size == SOCKET_ERROR)
		{
			int errnum = WSAGetLastError();
			if (errnum == WSAEWOULDBLOCK)
				return 0;
			
			// this is really bad, the connection is probably dead
		
			return -1;
		}
		else if (size == 0)
		{
			return -1;
		}
		
		return size;
	}

#elif defined(__TI_COMPILER_VERSION__)
	//set OPT_RECV_NOWAIT depending on bWait
	// if bWait==false: behaves like a non-blocking socket on other platforms
	// else: recv() blocks, avoiding polling with busy waiting.
	//       recv() may non the less return early in case of a PUSH flag.
	uint_32 opt_value = bWait? FALSE : TRUE;
	uint_32 error = setsockopt(m_socket, SOL_TCP, OPT_RECEIVE_NOWAIT, &opt_value, sizeof(opt_value));
	if (error != VCRT_OK)
	{
		printf("Error in Recv(), setsockopt(OPT_RECEIVE_NOWAIT) failed with error code 0x%x\n", error);
		return -1;
	}

	if (bWait)
	{
		int size = 0;
		char *ptr = (char *) pData;
		while (nMaxBytes>0)
		{
			int_32 count = recv(m_socket, ptr, nMaxBytes, 0); //is set to blocking above
			
			if (count == VCRT_ERROR)
			{
				printf("Error in Recv(): recv failed, error code 0x%x\n", VCRT_geterror(m_socket));
				// this is really bad, the connection is probably dead
				return -1;
			}
			
			ptr += count;
			size += count;
			nMaxBytes -= count; 
		}
		return size;
	}
	else
	{
		//receive whatever data is available and return immediately
		int_32 count = recv(m_socket, (char*)pData, nMaxBytes, 0); //is set to non-blocking above

		if (count == VCRT_ERROR)
		{
			printf("Error in Recv(): recv failed, error code 0x%x\n", VCRT_geterror(m_socket));
			// this is really bad, the connection is probably dead
			return -1;
		}
		// on vcrt, count==0 means zero bytes received and not connection closed by peer.
		// so in this case 0 is returned and not -1.
		
		return count;
	}

#else
	if (bWait)
	{
		int size = 0;
		unsigned char *ptr = (unsigned char *) pData;
		while (true)
		{
			int s = (int) recv(m_socket, ptr, nMaxBytes, 0);
			if (s == -1)
			{
				int errnum = errno;
				if (errnum == EAGAIN)
					continue;
			
				// this is really bad, the connection is probably dead
		
				return -1;
			}
			
			ptr += s;
			size += s;
			nMaxBytes -= s; 
			
			if (nMaxBytes == 0)
				return size;
				
		}
	}
	else
	{
		int size = (int) recv(m_socket, pData, nMaxBytes, 0);
		if (size == -1)
		{
			int errnum = errno;
			if (errnum == EAGAIN)
				return 0;
			
			// this is really bad, the connection is probably dead
		
			return -1;
		}
		else if (size == 0)
		{
			return -1;
		}
		
		return size;
	}
#endif
	
	return -1;
}
