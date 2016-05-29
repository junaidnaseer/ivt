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
// Filename:  FloatMatrix.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************

/** \defgroup MathData Mathematic data structures */


#ifndef __FLOAT_MATRIX_H__
#define __FLOAT_MATRIX_H__


// ****************************************************************************
// CFloatMatrix
// ****************************************************************************

/*!
	\brief Data structure for the representation of a matrix of values of the data type float.
	\ingroup MathData
*/
class CFloatMatrix
{
public:
	// constructors
	CFloatMatrix();
	CFloatMatrix(int nColumns, int nRows, bool bHeaderOnly = false);

	// copy constructors (will copy header and allocate memory)
	CFloatMatrix(const CFloatMatrix *pMatrix, bool bHeaderOnly = false);
	CFloatMatrix(const CFloatMatrix &matrix, bool bHeaderOnly = false);

	// destructor
	~CFloatMatrix();

	// public methods
	bool LoadFromFile(const char *pFileName);
	bool SaveToFile(const char *pFileName);
	
	// operators
	inline float& operator() (const int nColumn, const int nRow) { return data[nRow * columns + nColumn]; }
	inline float* operator[] (const int nRow) { return data + nRow * columns; }

	inline const float& operator() (const int nColumn, const int nRow) const { return data[nRow * columns + nColumn]; }
	inline const float* operator[] (const int nRow) const { return data + nRow * columns; }


private:
	// private methods
	void FreeMemory();
	

public:
	// public attributes - not clean OOP design but easy access
	int columns;
	int rows;
	float *data;

private:
	// private attributes - only used internally
	bool m_bOwnMemory;
};



#endif /* __FLOAT_MATRIX_H__ */
