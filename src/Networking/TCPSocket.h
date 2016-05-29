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
// Filename:  TCPSocket.h
// Author:    Florian Hecht
// Date:      08.01.2009
// ****************************************************************************

#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_



// this is a simple TCP socket
// the socket is in non-blocking, no linger mode, no tcp delay mode
class CTCPSocket
{
public:
	// constructor
	CTCPSocket();
	
	// destructor
	~CTCPSocket();
	
	// public methods
	
	// activate the socket as a listening socket and
	// bind it to the default ip address or the one supplied
	// in 'ip'
	bool Listen(const unsigned char *ip = 0, int port = 0);
	// accept a connection from a listening socket
	// this is non-blocking, returns NULL if no new
	// client is available
	CTCPSocket* Accept(unsigned char *ip = 0);
	
	// check if the socket is valid
	bool IsOpen() const { return m_socket != -1; }
	
	// open a connection to a remote host at ip:port
	bool Open(const unsigned char *ip, int port);
	// close the socket
	void Close();
	
	// send data. Returns true if send was successful.
	bool Send(const void *pData, int nBytes);
	
	// receive data. Returns the number of received bytes or -1
	// in case of a failure. Since the socket is non-blocking
	// the returned number of bytes can be less than
	// max_num_bytes. If wait is true, the function will return
	// after max_num_bytes have been read.
	int Recv(void *pData, int nMaxBytes, bool bWait = false);

	
private:
	// private attributes
	int	m_socket;
	bool m_bListening;
};



#endif /* _TCP_SOCKET_H_ */
