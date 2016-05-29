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
// Filename:  DynamicArray.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************

#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;



// ****************************************************************************
// CDynamicArrayElement
// ****************************************************************************

class CDynamicArrayElement
{
public:
	CDynamicArrayElement()
	{
		bDelete = true;
	}

	virtual ~CDynamicArrayElement() { }

	virtual bool Equivalent(const CDynamicArrayElement *pElement) const { return true; }
	virtual float Error(const CDynamicArrayElement *pElement) const { return 0; }

	bool bDelete;
};


// ****************************************************************************
// CDynamicArray
// ****************************************************************************

class CDynamicArray
{
public:
	// constructor
	CDynamicArray(int nInititalSize);

	// CDynamicArray
	~CDynamicArray();

	
	// public methods
	bool AddElement(CDynamicArrayElement *pElement, bool bAddUniqueOnly = false, bool bManageMemory = true);
	inline CDynamicArrayElement* GetElement(int nElement) { return (nElement < 0 || nElement >= m_nElements) ? 0 : m_ppElements[nElement]; }
	inline const CDynamicArrayElement* GetElement(int nElement) const { return (nElement < 0 || nElement >= m_nElements) ? 0 : m_ppElements[nElement]; }
	inline CDynamicArrayElement* GetElementNoCheck(int nElement) { return m_ppElements[nElement]; }
	inline const CDynamicArrayElement* GetElementNoCheck(int nElement) const { return m_ppElements[nElement]; }
	inline CDynamicArrayElement* operator[] (const int nElement) { return m_ppElements[nElement]; }
	CDynamicArrayElement* FindFirstMatch(const CDynamicArrayElement *pElement);
	CDynamicArrayElement* FindBestMatch(const CDynamicArrayElement *pElement, float &fResultError);
	bool DeleteElement(int nIndex);
	int DeleteFirstMatch(const CDynamicArrayElement *pElement);
	int DeleteAllMatches(const CDynamicArrayElement *pElement);
	void DontManageMemory(int nElement);
	void Clear();
	int GetSize() const { return m_nElements; }


private:
	// private methods
	int _FindFirstMatch(const CDynamicArrayElement *pElement);
	void SetSize(int nSize);
	
	
	// private attribute
	int m_nCurrentSize;
	int m_nElements;
	CDynamicArrayElement **m_ppElements;
};



#endif /* _DYNAMIC_ARRAY_H_ */
