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
// Filename:  Mutex.h
// Author:    Kai Welke
// Date:      13.01.2006
// ****************************************************************************


#ifndef _MUTEX_H_
#define _MUTEX_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <errno.h>
#endif

#include "Threading.h"



// ****************************************************************************
// CMutex
// ****************************************************************************

/*!
	\ingroup Threading
	\brief Implementation of mutexes for synchronization.
*/
class CMutex
{
public:
	// constructor
	CMutex();

	// destructor
	~CMutex();

	// locks the mutex
	Threading::EMutexStatus Lock();

	// tries to lock mutex
	Threading::EMutexStatus TryLock();

	// unlocks a mutex
	Threading::EMutexStatus UnLock();
	

private:
#ifdef WIN32
	CRITICAL_SECTION m_CSWindowsMutex;
#else
	pthread_mutex_t m_PosixMutex;
	pthread_mutexattr_t	m_MutexAttr;
#endif
};



#endif /* _MUTEX_H_ */
