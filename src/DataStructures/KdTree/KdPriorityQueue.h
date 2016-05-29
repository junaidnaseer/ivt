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
// Filename:  KdPriorityQueue.h
// Author:    Kai Welke
// Date:      14.04.2005
// ****************************************************************************

#ifndef _KD_PRIORITY_QUEUE_H_
#define _KD_PRIORITY_QUEUE_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs



// ****************************************************************************
// CKdPriorityQueue
// ****************************************************************************

class CKdPriorityQueue
{
private:
	// structure for queue entry
	struct KdQueueEntry
	{
		float fValue;
		void *pMeta;
	};

	
public:
	// constructor
	CKdPriorityQueue(int nMaxSize)
	{
		m_nMaxSize = nMaxSize;
		m_nSize = 0;
		
		// queue is array [1..max] of nodes
		m_pQueue = new KdQueueEntry[nMaxSize + 1];
	}

	// destructor
	~CKdPriorityQueue()
	{ 
		delete [] m_pQueue;
	}
	

	// public methods
	void Empty()
	{ 
		m_nSize = 0; 
	}
	
	int GetSize()
	{
		return m_nSize;
	}
	
	inline void Push(float fValue, void* pMeta)
	{
		// check for oveflow
		if (++m_nSize > m_nMaxSize) 
		{
			//printf("CKdPriorityQueue: Overflow!!\n");
			return;
		}
		
		register int nPos = m_nSize;
		
		while (nPos > 1) {
			register int nHalf = nPos / 2;
			
			// stop iterating, if queue value is smaller than insert position
			if (m_pQueue[nHalf].fValue <= fValue)
				break;
			
			// swap elements in queue
			m_pQueue[nPos] = m_pQueue[nHalf];
			nPos = nHalf;
		}
		
		// insert element
		m_pQueue[nPos].fValue = fValue;
		m_pQueue[nPos].pMeta = pMeta;
	}

	inline void Pop(float& fValue,void*& pMeta)
	{
		// read minimum value
		fValue = m_pQueue[1].fValue;
		pMeta = m_pQueue[1].pMeta;
		
		register float fLast = m_pQueue[m_nSize--].fValue;
		
		register int nPos = 1;
		register int nDouble = nPos << 1;
		
		// while r is still within the heap
		while (nDouble <= m_nSize) {
			// set r to smaller child of p
			
			if (nDouble < m_nSize  && m_pQueue[nDouble].fValue > m_pQueue[nDouble + 1].fValue) 
				nDouble++;
				
			// stop if we arrived at last
			if (fLast <= m_pQueue[nDouble].fValue)
				break;
			
			// swap elements
			m_pQueue[nPos] = m_pQueue[nDouble];
			
			nPos = nDouble;
			nDouble = nPos << 1;
		}
		
		// adjust last item
		m_pQueue[nPos] = m_pQueue[m_nSize + 1];
	}

		
private:
	// private attributes
	int m_nSize;
	int m_nMaxSize;
	
	KdQueueEntry *m_pQueue;
};



#endif /* _KD_PRIORITY_QUEUE_H_ */
