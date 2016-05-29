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
// Filename:  FloatVector.h
// Author:    Pedram Azad
// Date:      23.01.2008
// ****************************************************************************


#ifndef __FLOAT_VECTOR_H__
#define __FLOAT_VECTOR_H__


// ****************************************************************************
// CFloatVector
// ****************************************************************************

/** \ingroup MathData
 *  \brief Data structure for the representation of a vector of values of the data type float.
 */
class CFloatVector
{
public:
	// constructors
	CFloatVector();
	CFloatVector(int nDimension, bool bHeaderOnly = false);

	// copy constructors (will copy header and allocate memory)
	CFloatVector(const CFloatVector *pVector, bool bHeaderOnly = false);
	CFloatVector(const CFloatVector &v, bool bHeaderOnly = false);

	// destructor
	~CFloatVector();

	// operators
	inline float& operator[] (const int nElement) { return data[nElement]; }

private:
	// private methods
	void FreeMemory();
	

public:
	// public attributes - not clean OOP design but easy access
	int dimension;
	float *data;

private:
	// private attributes - only used internally
	bool m_bOwnMemory;
};



#endif /* __FLOAT_VECTOR_H__ */
