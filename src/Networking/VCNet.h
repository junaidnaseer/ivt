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
// Filename:  VCNet.h
// Author:    Moritz Hassert
// Date:      2009
// ****************************************************************************


#ifndef _VC_NET_H_
#define _VC_NET_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include <stdint.h>
#include <vcrt.h>



// ****************************************************************************
// Functions
// ****************************************************************************

// replacement for missing built-in function for SWAP2 pseudo operation
inline uint32_t swap_words(uint32_t dword)
{
	return _packlh2(dword,dword);
}

inline uint32_t revert_byte_order(uint32_t dword)
{
	return swap_words(_swap4(dword));
}

inline uint32_t htonl(uint32_t hostlong)
{
#if defined(__little_endian__)
	return revert_byte_order(hostlong);
#else
	return hostlong;
#endif
}

inline uint16_t htons(uint16_t hostshort)
{
#if defined(__little_endian__)
	return _swap4(hostshort);
#else
	return hostshort;
#endif
}

inline uint32_t ntohl(uint32_t netlong)
{
#if defined(__little_endian__)
	return revert_byte_order(netlong);
#else
	return netlong;
#endif
}

inline uint16_t ntohs(uint16_t netshort)
{
#if defined(__little_endian__)
	return _swap4(netshort);
#else
	return netshort;
#endif
}



#endif /* _VC_NET_H_ */
