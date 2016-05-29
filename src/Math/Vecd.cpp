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
// Filename:  Vecd.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "Vecd.h"
#include <string.h>
#include <math.h>



// ****************************************************************************
// Constructors / Destructor
// ****************************************************************************

CVecd::CVecd()
{
	m_pElements = 0;
	m_nSize = 0;
}

CVecd::CVecd(int nSize)
{
	m_pElements = 0;

	SetSize(nSize);
}

CVecd::CVecd(double dX1, double dX2)
{
	m_pElements = 0;
	
	SetSize(2);
	
	m_pElements[0] = dX1;
	m_pElements[1] = dX2;
}

CVecd::CVecd(const CVecd &v)
{
	m_pElements = 0;

	SetSize(v.m_nSize);

	memcpy(m_pElements, v.m_pElements, m_nSize * sizeof(double));
}

CVecd::~CVecd()
{
	if (m_pElements)
		delete [] m_pElements;
}


// ****************************************************************************
// Operators
// ****************************************************************************

CVecd& CVecd::operator= (const CVecd &v)
{
	SetSize(v.GetSize());

	memcpy(m_pElements, v.m_pElements, m_nSize * sizeof(double));

	return *this;
}

CVecd CVecd::operator+ (const CVecd &v)
{
	//_ASSERTE(m_nSize == v.m_nSize);

	if (m_nSize != v.m_nSize)
		return CVecd(0);

	
	CVecd result(m_nSize);

	for (int i = 0; i < m_nSize; i++)
		result.m_pElements[i] = m_pElements[i] + v.m_pElements[i];

	
	return result;
}

CVecd CVecd::operator- (const CVecd &v)
{
	//_ASSERTE(m_nSize == v.m_nSize);

	if (m_nSize != v.m_nSize)
		return CVecd(0);

	
	CVecd result(m_nSize);

	for (int i = 0; i < m_nSize; i++)
		result.m_pElements[i] = m_pElements[i] - v.m_pElements[i];

	
	return result;
}

double CVecd::operator* (const CVecd &v)
{
	//_ASSERTE(m_nSize == v.m_nSize);

	if (m_nSize != v.m_nSize)
		return 0.0;

	
	double dResult = 0.0;

	for (int i = 0; i < m_nSize; i++)
		dResult += m_pElements[i] * v.m_pElements[i];

	
	return dResult;
}

double& CVecd::operator[] (const int n) const
{
	//_ASSERTE(n >= 0 && n < m_nSize);

	return m_pElements[n];
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CVecd::SetSize(int nSize)
{
	//_ASSERTE(nSize >= 0);

	// first free memory
	if (m_pElements)
		delete [] m_pElements;

	// allocate memory for vector
	m_pElements = new double[nSize];

	for (int i = 0; i < nSize; i++)
		m_pElements[i] = 0.0;

	// save size of vector
	m_nSize = nSize;
}

double CVecd::Length()
{
	double sum = 0.0;
	
	for (int i = 0; i < m_nSize; i++)
		sum += m_pElements[i] * m_pElements[i];

	return sqrt(sum);
}
