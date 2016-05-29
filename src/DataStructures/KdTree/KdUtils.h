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
// Filename:  KdUtils.h
// Author:    Kai Welke
// Date:      14.04.2005
// ****************************************************************************


#ifndef _KD_UTILS_H_
#define _KD_UTILS_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs
#include <math.h>
#include "KdStructs.h"



// ****************************************************************************
// Functions
// ****************************************************************************

// find smallest enclosing rectangle
// used to find initial bounding box for recursion in build process
KdBoundingBox CalculateEnclosingBoundingBox(float **ppValues, int nDimension, int nSize)
{
	KdBoundingBox EnclosingBox;
	EnclosingBox.nDimension = nDimension;
	EnclosingBox.pfLow = new float[nDimension];
	EnclosingBox.pfHigh = new float[nDimension];
	
	// find boundings for each dimension
	for (int d = 0; d < nDimension; d++)
	{
		// init lower and upper bound
		float fLow = ppValues[0][d];
		float fHigh = ppValues[0][d];
		
		// find smallest and highest element
		for (int i = 0; i < nSize; i++)
		{
			if (ppValues[i][d] < fLow) 
			{
				fLow = ppValues[i][d];
			} 
			else if (ppValues[i][d] > fHigh)
			{
				fHigh = ppValues[i][d];
			}
		}
		
		// store in Box
		EnclosingBox.pfLow[d] = fLow;
		EnclosingBox.pfHigh[d] = fHigh;
	}
	
	return EnclosingBox;
}


// calculate distance form query point to bounding box
// used once to initiate recursion for BBF search
float GetDistanceFromBox(KdBoundingBox BoundingBox, const float *pValues, int nDimension)
{
	register float fDistance = 0.0f;

	for (register int d = 0; d < nDimension; d++)
	{
		if (pValues[d] < BoundingBox.pfLow[d])
		{
			// point is below lower bound so adjust distance
			const float fTemp = BoundingBox.pfLow[d] - pValues[d];
			fDistance += powf(fTemp, 2);
		}
		else if (pValues[d] > BoundingBox.pfHigh[d])
		{
			// point is above upper bound so adjust distance
			const float fTemp = pValues[d] - BoundingBox.pfHigh[d];
			fDistance += powf(fTemp, 2);
		}
	}

	return fDistance;
}



#endif /* _KD_UTILS_H_ */
