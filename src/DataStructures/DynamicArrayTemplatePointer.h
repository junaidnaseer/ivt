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
// Filename:  DynamicArrayTemplatePointer.h
// Author:    Pedram Azad
// Date:      11.02.2010
// ****************************************************************************

#ifndef _DYNAMIC_ARRAY_TEMPLATE_POINTER_H_
#define _DYNAMIC_ARRAY_TEMPLATE_POINTER_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs
#include <stdio.h>



// ****************************************************************************
// CDynamicArrayTemplatePointer
// ****************************************************************************

template <typename T> class CDynamicArrayTemplatePointer
{
public:
	// constructors
	CDynamicArrayTemplatePointer(bool bManageMemory, int nInitialSize = 100)
	{
		m_bManageMemory = bManageMemory;
		
		m_nElements = 0;
		m_nCurrentSize = nInitialSize;
		
		m_ppElements = new T*[m_nCurrentSize];
		for (int i = 0; i < m_nCurrentSize; i++)
			m_ppElements[i] = 0;
	}
	
	// CDynamicArray
	~CDynamicArrayTemplatePointer()
	{
		if (m_bManageMemory)
		{
			for (int i = 0; i < m_nElements; i++)
				delete m_ppElements[i];
		}

		delete [] m_ppElements;
	}

	
	// public methods
	void AddElement(T *pElement)
	{
		if (m_nElements == m_nCurrentSize)
			SetCurrentSize(m_nCurrentSize << 1);
		
		m_ppElements[m_nElements++] = pElement;
	}

	bool DeleteElement(int nIndex)
	{
		if (nIndex < 0 || nIndex >= m_nElements)
			return false;
		
		if (m_bManageMemory)
			delete m_ppElements[nIndex];

		for (int i = nIndex; i < m_nElements - 1; i++)
			m_ppElements[i] = m_ppElements[i + 1];

		m_nElements--;

		return true;
	}

	void Clear()
	{
		if (m_bManageMemory)
		{
			for (int i = 0; i < m_nElements; i++)
				delete m_ppElements[i];
		}

		m_nElements = 0;
	}
	
	void ClearAndResize(int nSize)
	{
		Clear();
		
		m_nCurrentSize = nSize;
		
		delete [] m_ppElements;
		m_ppElements = new T[m_nCurrentSize];
	}

	int GetSize() const { return m_nElements; }
	int GetStorageSize() const { return m_nCurrentSize; }

	// operators
	inline const T* operator[](const int nElement) const { return m_ppElements[nElement]; }
	inline T*& operator[](const int nElement) { return m_ppElements[nElement]; }
	


private:
	// private methods
	void SetCurrentSize(int nCurrentSize)
	{
		if (nCurrentSize <= m_nCurrentSize)
		{
			printf("error: tried to set size smaller than current size in CDynamicArray::SetCurrentSize\n");
			return;
		}
		
		m_nCurrentSize = nCurrentSize;

		T **ppElements = new T*[nCurrentSize];
		
		for (int i = 0; i < m_nElements; i++)
			ppElements[i] = m_ppElements[i];
			
		delete [] m_ppElements;
		m_ppElements = ppElements;
	}
	
	
	// private attribute
	int m_nCurrentSize;
	int m_nElements;
	T **m_ppElements;

	bool m_bManageMemory;
};



#endif /* _DYNAMIC_ARRAY_TEMPLATE_POINTER_H_ */
