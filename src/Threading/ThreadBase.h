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
// Filename:  ThreadBase.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


#ifndef _THREAD_BASE_H_
#define _THREAD_BASE_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Helpers/helpers.h"
#include <stdio.h>


// ****************************************************************************
// Typedefs
// ****************************************************************************

typedef int (*ThreadMethodType)(void *pParameter);
 


// ****************************************************************************
// CThreadBase
// ****************************************************************************

class CThreadBase
{
public:
	// constructor
	CThreadBase()
	{
		m_bExit = true;
		m_pThreadMethod = 0;
	}

	// destructor
	virtual ~CThreadBase() { }
	
	
	// public methods
	void Start(void *pParameter, ThreadMethodType pThreadMethod, int nKillThreadTimeout = 5000)
	{
		Stop();

		m_bExit = false;
		m_pParameter = pParameter;
		m_pThreadMethod = pThreadMethod;
		m_nKillThreadTimeout = nKillThreadTimeout;
		m_bCompletelyDone = false;
		_Start();
	}
	
	void Start(int nKillThreadTimeout = 5000)
	{
		Stop();

		m_bExit = false;
		m_pParameter = 0;
		m_pThreadMethod = 0;
		m_nKillThreadTimeout = nKillThreadTimeout;
		m_bCompletelyDone = false;
		_Start();
	}

	void Stop()
	{
		if (!IsRunning())
			return;

		m_bExit = true;
	
		StopThreadCallback();

		_Stop();

		for (int i = 0; i < 100; i++)
		{
			if (m_bCompletelyDone)
				break;

			sleep_ms(1);
		}
	}

	bool IsRunning() { return !m_bExit; }
	bool GetExit() { return m_bExit; }

	
protected:
	// virtual thread method: can be implemented if C++ style is desired
	virtual int ThreadMethod()
	{
		if (!m_pThreadMethod)
		{
			printf("error: thread function not implemented, but using thread function variant.\n");
			return -1;
		}
		
		return m_pThreadMethod(m_pParameter);
	}
	
	// use this if you need a callback after stop has been called and m_bExit set to true
	virtual void StopThreadCallback() { }

	virtual void _Start() = 0;
	virtual void _Stop() = 0;

	int m_nKillThreadTimeout;


private:
	// private methods
	virtual void ThreadMethodFinished() { }

	// private attributes
	ThreadMethodType m_pThreadMethod;
	void *m_pParameter;
	bool m_bExit;
	

public:
	// thread function access
	int _ThreadMethod()
	{
		const int nRet = ThreadMethod();
		
		ThreadMethodFinished();

		return nRet;
	}

	bool m_bCompletelyDone;
};



#endif /* _THREAD_BASE_H_ */
