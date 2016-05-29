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
// Filename:  Mutex.cpp
// Author:    Kai Welke
// Date:      13.01.2006
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "Mutex.h"



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CMutex::CMutex()
{
#ifdef WIN32
	InitializeCriticalSection(&m_CSWindowsMutex);
#else
	pthread_mutexattr_init(&m_MutexAttr);
	pthread_mutex_init(&m_PosixMutex ,&m_MutexAttr);
#endif
}

CMutex::~CMutex()
{
#ifdef WIN32
	DeleteCriticalSection(&m_CSWindowsMutex);
#else
	pthread_mutex_destroy(&m_PosixMutex);
	pthread_mutexattr_destroy(&m_MutexAttr);
#endif
}


// ****************************************************************************
// Methods
// ****************************************************************************

// try to lock the mutex. If another thread is owner of mutex, wait till mutex is unlocked
Threading::EMutexStatus CMutex::Lock()
{
#ifdef WIN32
	EnterCriticalSection(&m_CSWindowsMutex);
	return Threading::eMutexSuccess;
#else
	if (pthread_mutex_lock(&m_PosixMutex) == 0)
		return Threading::eMutexSuccess;
	else
		return Threading::eMutexError;
#endif
}

// try to lock the mutex. If another thread is owner of mutex, return EBusy, otherwise lock mutex
Threading::EMutexStatus CMutex::TryLock()
{
#ifdef WIN32
	if (TryEnterCriticalSection(&m_CSWindowsMutex) == 0)
	{
		return Threading::eMutexBusy;
	}
	else
	{
		return Threading::eMutexSuccess;
	}
#else
	const int nResult = pthread_mutex_trylock(&m_PosixMutex);
	
	if (nResult == 0)
	{
		return Threading::eMutexSuccess;
	}
	else if (nResult == EBUSY)
	{
		return Threading::eMutexBusy;
	}
	else
	{
		return Threading::eMutexError;
	}
#endif
}

// unlock mutex
Threading::EMutexStatus CMutex::UnLock()
{
#ifdef WIN32
	LeaveCriticalSection(&m_CSWindowsMutex);
	return Threading::eMutexSuccess;
#else
	if (pthread_mutex_unlock(&m_PosixMutex) == 0)
	{
		return Threading::eMutexSuccess;
	}
	else
	{
		return Threading::eMutexError;
	}
#endif
}
