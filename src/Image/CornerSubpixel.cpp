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
// Filename:  KLTTracker.cpp
// Author:    Pedram Azad
// Date:      18.11.2009
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "CornerSubpixel.h"
#include "Image/ByteImage.h"
#include "Math/Math2d.h"

#include <stdio.h>
#include <float.h>
#include <math.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define MAX_CORNER_SUBPIXEL_ITERATIONS 100



// ****************************************************************************
// Methods
// ****************************************************************************

bool CornerSubpixel::Refine(const CByteImage *pImage, const Vec2d &point, Vec2d &resultPoint, int nHalfWindowSize, int nMaxIterations)
{
	if (pImage->type != CByteImage::eGrayScale)
	{
		printf("error: image must be of type eGrayScale for CCornerSubpixel::refine\n");
		return false;
	}
	
	const int width = pImage->width;
	const int height = pImage->height;
	
	const int nWindowSize = 2 * nHalfWindowSize + 1;
	const int nWindowSizePlus2 = nWindowSize + 2;
	const int diff = width - nWindowSizePlus2;
	
	float grayValues[1024];
	float *pGrayValues = nWindowSizePlus2 <= 32 ? grayValues : new float[nWindowSizePlus2 * nWindowSizePlus2];
	
	// track each point
	const float ux = point.x;
	const float uy = point.y;
	
	const unsigned char *pixels = pImage->pixels;
	
	Vec2d v = { 0.0f, 0.0f };
	
	for (int k = 0; k < nMaxIterations; k++)
	{
		const int xb = int(floorf(ux + v.x));
		const int yb = int(floorf(uy + v.y));
		
		if (xb - nHalfWindowSize < 1 || xb + nHalfWindowSize + 3 >= width || yb - nHalfWindowSize < 1 || yb + nHalfWindowSize + 3 >= height)
			break;
		
		const float dx = ux + v.x - float(xb);
		const float dy = uy + v.y - float(yb);
		
		const float f00 = (1.0f - dx) * (1.0f - dy);
		const float f10 = dx * (1.0f - dy);
		const float f01 = (1.0f - dx) * dy;
		const float f11 = dx * dy;
		
		int j, offset, offset2;

		for (j = nWindowSizePlus2, offset = (yb - nHalfWindowSize - 1) * width + xb - nHalfWindowSize - 1, offset2 = 0; j; j--, offset += diff)
			for (int jj = nWindowSizePlus2; jj; jj--, offset++, offset2++)
				pGrayValues[offset2] = f00 * pixels[offset] + f10 * pixels[offset + 1] + f01 * pixels[offset + width] + f11 * pixels[offset + width + 1];
		
		float gxx_sum = 0.0f, gxy_sum = 0.0f, gyy_sum = 0.0f;
		float bx = 0.0f, by = 0.0f;
		
		for (j = 0, offset = nWindowSizePlus2 + 1; j < nWindowSize; j++, offset += 2)
			for (int jj = 0; jj < nWindowSize; jj++, offset++)
			{
				const float ix = pGrayValues[offset + 1] - pGrayValues[offset - 1];
				const float iy = pGrayValues[offset + nWindowSizePlus2] - pGrayValues[offset - nWindowSizePlus2];
				
				const float gxx = ix * ix;
				const float gyy = iy * iy;
				const float gxy = ix * iy;
				
				gxx_sum += gxx;
				gyy_sum += gyy;
				gxy_sum += gxy;
				
				bx += gxx * float(jj - nHalfWindowSize) + gxy * float(j - nHalfWindowSize);
				by += gxy * float(jj - nHalfWindowSize) + gyy * float(j - nHalfWindowSize);
			}
		
		if (fabsf(gxx_sum * gyy_sum - gxy_sum * gxy_sum) <= FLT_EPSILON)
		{
			if (nWindowSizePlus2 > 32)
				delete [] pGrayValues;
			
			Math2d::SetVec(resultPoint, point);
			
			return false;
		}
		
		const Mat2d G = { gxx_sum, gxy_sum, gxy_sum, gyy_sum };
		Mat2d G_;
		Math2d::Invert(G, G_);
		
		const Vec2d b = { bx, by };
		
		Vec2d n;
		Math2d::MulMatVec(G_, b, n);
		
		Math2d::AddToVec(v, n);
		
		if (Math2d::SquaredLength(n) < 0.03f)
			break;
	}
	
	if (nWindowSizePlus2 > 32)
		delete [] pGrayValues;
	
	Math2d::SetVec(resultPoint, ux + v.x, uy + v.y);
		
	return true;
}
