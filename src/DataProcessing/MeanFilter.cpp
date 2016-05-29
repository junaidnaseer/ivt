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
// Filename:  MeanFilter.cpp
// Author:    Pedram Azad
// Date:      2006
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "MeanFilter.h"

#include <stdio.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CMeanFilter::CMeanFilter(int nKernelSize)
{
	m_pValues = 0;
	m_nPosition = 0;
	m_nElementsFilled = 0;
	m_nKernelSize = 0;
	
	SetKernelSize(nKernelSize);
}

CMeanFilter::~CMeanFilter()
{
	if (m_pValues)
		delete [] m_pValues;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CMeanFilter::SetKernelSize(int nKernelSize)
{
	if (nKernelSize <= 0)
	{
		printf("error: nKernelSize must be greater 0 for CMeanFilter::SetKernelSize\n");
		return;
	}
	
	m_nKernelSize = nKernelSize;
	
	if (m_pValues)
		delete [] m_pValues;
	
	m_pValues = new float[nKernelSize];
	
	Reset();
}

void CMeanFilter::Reset()
{
	for (int i = 0; i < m_nKernelSize; i++)
		m_pValues[i] = 0.0f; // this is not really necessary...

	m_nPosition = 0;
	m_nElementsFilled = 0;
}

float CMeanFilter::Filter(float x)
{
	if (m_nKernelSize <= 0)
	{
		printf("error: CMeanFilter::Filter called, but m_nKernelSize is invalid (%i)\n", m_nKernelSize);
		return x;
	}

	m_pValues[m_nPosition++ % m_nKernelSize] = x;
	if (m_nElementsFilled < m_nKernelSize)
		m_nElementsFilled++;
	
	float sum = 0.0f;
	for (int i = 0; i < m_nElementsFilled; i++)
		sum += m_pValues[i];

	return sum / m_nElementsFilled;
}
