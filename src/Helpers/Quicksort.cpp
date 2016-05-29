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
// Filename:  Quicksort.cpp
// Author:    Kai Welke
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "Quicksort.h"



// ****************************************************************************
// Functions
// ****************************************************************************

// quicksort elements of float array by exchanging the elements
void Quicksort::Quicksort(float *pValues, int nLow, int nHigh)
{
	int i = nLow;
	int j = nHigh;
	
	const float fX = pValues[(nLow + nHigh) / 2];

	while (i <= j)
	{    
		while (pValues[i] < fX) i++; 
		while (pValues[j] > fX) j--;
        
		if (i <= j)
		{
			float fTemp = pValues[i];
			pValues[i] = pValues[j];
			pValues[j] = fTemp;
		
			i++; 
			j--;
		}
	}

	if (nLow < j) Quicksort(pValues, nLow, j);
	if (i < nHigh) Quicksort(pValues, i, nHigh);
}


// quicksort elements of float array in inverse order by exchanging the elements
void Quicksort::QuicksortInverse(float *pValues, int nLow, int nHigh)
{
	int i = nLow;
	int j = nHigh;
	
	const float fX = pValues[(nLow + nHigh) / 2];

	while (i <= j)
	{    
		while (pValues[i] > fX) i++; 
		while (pValues[j] < fX) j--;
        
		if (i <= j)
		{
			float fTemp = pValues[i];
			pValues[i] = pValues[j];
			pValues[j] = fTemp;
		
			i++; 
			j--;
		}
	}

	if (nLow < j) QuicksortInverse(pValues, nLow, j);
	if (i < nHigh) QuicksortInverse(pValues, i, nHigh);
}


void Quicksort::QuicksortWithMeta(float *pValues, void **ppMeta, int nLow, int nHigh)
{
	int i = nLow;
	int j = nHigh;
		
	const float fX = pValues[(nLow + nHigh) / 2];

	while (i <= j)
	{    
		while (pValues[i] < fX) i++; 
		while (pValues[j] > fX) j--;
            
		if (i <= j)
		{
			const float fTemp = pValues[i];
			pValues[i] = pValues[j];
			pValues[j] = fTemp;
		
			void *pMetaTemp = ppMeta[i];
			ppMeta[i] = ppMeta[j];
			ppMeta[j] = pMetaTemp;
		
			i++;
			j--;
		}
	}

	if (nLow < j) QuicksortWithMeta(pValues, ppMeta,  nLow, j);
	if (i < nHigh) QuicksortWithMeta(pValues, ppMeta,  i, nHigh);
}

void Quicksort::QuicksortInverseWithMeta(float *pValues, void **ppMeta, int nLow, int nHigh)
{
	int i = nLow;
	int j = nHigh;
	
	const float fX = pValues[(nLow + nHigh) / 2];

	while (i <= j)
	{    
		while (pValues[i] > fX) i++; 
		while (pValues[j] < fX) j--;
        
		if (i <= j)
		{
			float fTemp = pValues[i];
			pValues[i] = pValues[j];
			pValues[j] = fTemp;
			
			void *pMetaTemp = ppMeta[i];
			ppMeta[i] = ppMeta[j];
			ppMeta[j] = pMetaTemp;
		
			i++; 
			j--;
		}
	}

	if (nLow < j) QuicksortInverseWithMeta(pValues, ppMeta, nLow, j);
	if (i < nHigh) QuicksortInverseWithMeta(pValues, ppMeta, i, nHigh);
}


// quicksort vectors (float* in an array) due to the size of the size of the nSortByDimension'th element
// by exchanging the pointers
void Quicksort::QuicksortByElementOfVector(float **ppValues, int nLow, int nHigh, int nSortByDimension)
{
	int i = nLow;
	int j = nHigh;
	
	const float fX = ppValues[(nLow + nHigh) / 2][nSortByDimension];

	while ( i <= j )
	{    
		while (ppValues[i][nSortByDimension] < fX) i++; 
		while (ppValues[j][nSortByDimension] > fX) j--;
	    
		if (i <= j)
		{
			float *pfTemp = ppValues[i];
			ppValues[i]=ppValues[j];
			ppValues[j]=pfTemp;
		
			i++; 
			j--;
		}
	}

	if (nLow < j) QuicksortByElementOfVector(ppValues, nLow, j, nSortByDimension);
	if (i < nHigh) QuicksortByElementOfVector(ppValues, i, nHigh, nSortByDimension);
}
