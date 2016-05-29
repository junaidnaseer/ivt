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
// Filename:  Event.cpp
// Author:    Kai Welke
// Date:      13.01.2006
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "Event.h"

#include "Threading.h"

#include <stdio.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CEvent::CEvent()
{
#ifdef WIN32
	InitializeCriticalSection(&m_CSWindowsMutex);
	m_SignalEvent = CreateEvent(NULL,false,false,NULL);
#else
	pthread_mutexattr_init(&m_MutexAttr);
	pthread_mutex_init(&m_PosixMutex ,&m_MutexAttr);
	pthread_condattr_init(&m_ConditionalAttr);
	pthread_cond_init(&m_PosixConditional,&m_ConditionalAttr);
	
	m_bSignaled = false;
#endif
}

CEvent::~CEvent()
{
#ifdef WIN32
	DeleteCriticalSection(&m_CSWindowsMutex);
	CloseHandle(m_SignalEvent);
#else
	pthread_mutex_destroy(&m_PosixMutex);
	pthread_mutexattr_destroy(&m_MutexAttr);
	pthread_cond_destroy(&m_PosixConditional);
	pthread_condattr_destroy(&m_ConditionalAttr);
#endif
}


// ****************************************************************************
// Methods
// ****************************************************************************
void CEvent::Reset()
{
#ifdef WIN32
	ResetEvent(m_SignalEvent);
#else
	pthread_mutex_lock(&m_PosixMutex);
	m_bSignaled = false;
	pthread_mutex_unlock(&m_PosixMutex);

	pthread_cond_signal(&m_PosixConditional);
#endif
}

Threading::EEventStatus CEvent::Wait(int nMS)
{
#ifdef WIN32
	int nWaitMS;
	if (nMS == Threading::WAIT_INFINITE)
		nWaitMS = INFINITE;
	else
		nWaitMS = nMS;

	if (WaitForSingleObject(m_SignalEvent, nWaitMS) == WAIT_OBJECT_0)
		return Threading::eEventSuccess;
	else
		return Threading::eEventTimeout;
#else
	// lock mutex for signal
	pthread_mutex_lock(&m_PosixMutex);
	
	// when we are not signalled, we wait for signal
	if (!m_bSignaled)
	{
		if (nMS == Threading::WAIT_INFINITE)  // wait infinite
		{
			// have to do a loop, because linux sometimes messes other signals to this one 
			while (!m_bSignaled) 
			{
				pthread_cond_wait(&m_PosixConditional, &m_PosixMutex);
			}
			
			// we consumed the signal
			m_bSignaled = false;

			// mutex can be unlocked
			pthread_mutex_unlock(&m_PosixMutex);
			
			// return with success
			return Threading::eEventSuccess;
		}
		else			// wait with timeout
		{
			// set timer structure
			time_t current = time(NULL);
			timespec uni_ts;
			int nSeconds = nMS / 1000;
			if (nSeconds == 0)
			{
				nSeconds = 1;
				nMS = 0;
			}
			else
			{
				nMS -= nSeconds * 1000;
			}
			
			uni_ts.tv_sec = current + nSeconds;
			uni_ts.tv_nsec = nMS * 1000;
			
			
			if (pthread_cond_timedwait(&m_PosixConditional, &m_PosixMutex, &uni_ts) == 0)
			{
				if (m_bSignaled)
				{
					m_bSignaled = false;
					pthread_mutex_unlock(&m_PosixMutex);		
					return Threading::eEventSuccess;
				}
				else
				{
					pthread_mutex_unlock(&m_PosixMutex);		
					return Threading::eEventTimeout;
				}
			}
			else
			{
				m_bSignaled = false;
				pthread_mutex_unlock(&m_PosixMutex);
				
				// perhaps we didnt enter idle so yield
				Threading::YieldThread();
				
				return Threading::eEventTimeout;
			}
		}
		

	}
	else
	{
		// consume signal
		m_bSignaled = false;
		
		// unlock signal mutex
		pthread_mutex_unlock(&m_PosixMutex);
		
		// we didnt enter idle so yield
		Threading::YieldThread();
	}
	
	return Threading::eEventSuccess;
#endif
}

void CEvent::Signal()
{
#ifdef WIN32
	SetEvent(m_SignalEvent);
#else
	pthread_mutex_lock(&m_PosixMutex);
	m_bSignaled = true;
	pthread_mutex_unlock(&m_PosixMutex);
	
	pthread_cond_signal(&m_PosixConditional);
#endif
}
