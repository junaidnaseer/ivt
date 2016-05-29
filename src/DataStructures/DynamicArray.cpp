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
// Filename:  DynamicArray.cpp
// Author:    Pedram Azad
// Date:      2006
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "DynamicArray.h"

#include <stdio.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CDynamicArray::CDynamicArray(int nInitialSize)
{
	m_nElements = 0;
	m_nCurrentSize = nInitialSize;
	m_ppElements = new CDynamicArrayElement*[nInitialSize];
	
	for (int i = 0; i < nInitialSize; i++)
		m_ppElements[i] = 0;
}

CDynamicArray::~CDynamicArray()
{
	for (int i = 0; i < m_nElements; i++)
		if (m_ppElements[i]->bDelete)
			delete m_ppElements[i];

	delete [] m_ppElements;
	
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CDynamicArray::DontManageMemory(int nElement)
{
	m_ppElements[nElement]->bDelete = false;
}

void CDynamicArray::Clear()
{
	for (int i = 0; i < m_nElements; i++)
	{
		if (m_ppElements[i]->bDelete)
			delete m_ppElements[i];

		m_ppElements[i] = 0;
	}

	m_nElements = 0;
	
}

void CDynamicArray::SetSize(int nSize)
{
	if (nSize <= m_nCurrentSize)
	{
		printf("error: tried to set size smaller than current size in CDynamicArray::SetSize\n");
		return;
	}
	
	m_nCurrentSize = nSize;

	CDynamicArrayElement **ppElements = new CDynamicArrayElement*[nSize];
	
	int i;
	
	for (i = 0; i < m_nElements; i++)
		ppElements[i] = m_ppElements[i];
		
	for (i = m_nElements; i < nSize; i++)
		ppElements[i] = 0;
	
	delete [] m_ppElements;
	m_ppElements = ppElements;
}

bool CDynamicArray::AddElement(CDynamicArrayElement *pElement, bool bAddUniqueOnly, bool bManageMemory)
{
	if (bAddUniqueOnly && FindFirstMatch(pElement))
		return false;
		
	if (m_nElements > (m_nCurrentSize * 3 / 4))
		SetSize(m_nCurrentSize * 2);
		
	pElement->bDelete = bManageMemory;
	m_ppElements[m_nElements++] = pElement;
		
	return true;
}

bool CDynamicArray::DeleteElement(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nElements)
		return false;

	delete m_ppElements[nIndex];
	m_ppElements[nIndex] = 0;
	
	for (int i = nIndex; i < m_nElements - 1; i++)
		m_ppElements[i] = m_ppElements[i + 1];

	m_nElements--;

	return true;
}

int CDynamicArray::DeleteFirstMatch(const CDynamicArrayElement *pElement)
{
	int nIndex = _FindFirstMatch(pElement);
	
	if (nIndex == -1)
		return 0;
		
	DeleteElement(nIndex);
		
	return 1;
}

int CDynamicArray::DeleteAllMatches(const CDynamicArrayElement *pElement)
{
	int nIndex, nElementsDeleted = 0;
	
	while ((nIndex = _FindFirstMatch(pElement)) != -1)
	{
		DeleteElement(nIndex);
		nElementsDeleted++;
	}
	
	return nElementsDeleted;
}

CDynamicArrayElement* CDynamicArray::FindFirstMatch(const CDynamicArrayElement *pElement)
{
	for (int i = 0; i < m_nElements; i++)
	{
		if (pElement->Equivalent(m_ppElements[i]))
			return m_ppElements[i];
	}
		
	return 0;
}

int CDynamicArray::_FindFirstMatch(const CDynamicArrayElement *pElement)
{
	for (int i = 0; i < m_nElements; i++)
	{
		if (pElement->Equivalent(m_ppElements[i]))
			return i;
	}
		
	return -1;
}

CDynamicArrayElement* CDynamicArray::FindBestMatch(const CDynamicArrayElement *pElement, float &fResultError)
{
	float best_error = 9999999;
	int best_i = -1;
	
	for (int i = 0; i < m_nElements; i++)
	{
		const float error = pElement->Error(m_ppElements[i]);
		
		if (error < best_error)
		{
			best_error = error;
			best_i = i;
		}
	}
	
	if (best_i == -1)
		return 0;
		
	fResultError = best_error;
	
	return m_ppElements[best_i];
}
