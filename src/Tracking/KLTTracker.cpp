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

#include "KLTTracker.h"

#include "Image/ByteImage.h"
#include "Image/ImageProcessor.h"
#include "Math/Math2d.h"

#include <stdio.h>
#include <float.h>
#include <math.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define KLT_ITERATIONS	20



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CKLTTracker::CKLTTracker(int width_, int height_, int nLevels, int nHalfWindowSize) : width(width_), height(height_), m_nLevels(nLevels), m_nHalfWindowSize(nHalfWindowSize)
{
	m_ppPyramidI = new CByteImage*[nLevels + 1];
	m_ppPyramidJ = new CByteImage*[nLevels + 1];
	
	int w = width;
	int h = height;
	
	int i;
	
	// allocate pyramid images
	for (i = 0; i <= nLevels; i++)
	{
		m_ppPyramidI[i] = new CByteImage(w, h, CByteImage::eGrayScale);
		m_ppPyramidJ[i] = new CByteImage(w, h, CByteImage::eGrayScale);
		
		w /= 2;
		h /= 2;
	}
	
	// precalculate scale factors
	m_pScaleFactors = new float[m_nLevels + 1];
	for (i = m_nLevels; i >= 0; i--)
		m_pScaleFactors[i] = 1.0f / powf(2.0f, (float) i);
	
	m_bInitialized = false;
}

CKLTTracker::~CKLTTracker()
{
	for (int i = 0; i <= m_nLevels; i++)
	{
		delete m_ppPyramidI[i];
		delete m_ppPyramidJ[i];
	}
	
	delete [] m_ppPyramidI;
	delete [] m_ppPyramidJ;
	
	delete [] m_pScaleFactors;
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CKLTTracker::Track(const CByteImage *pImage, const Vec2d *pPoints, int nPoints, Vec2d *pResultPoints)
{
	if (pImage->type != CByteImage::eGrayScale)
	{
		printf("error: image must be of type eGrayScale for CKLTTracker::Track\n");
		return false;
	}
	
	if (!m_bInitialized)
	{
		// copy image to become level 0 of pyramid
		ImageProcessor::CopyImage(pImage, m_ppPyramidI[0]);
		
		// compute levels 1 to m_nLevels - 1 of pyramid
		for (int i = 1; i <= m_nLevels; i++)
			ImageProcessor::Resize(m_ppPyramidI[i - 1], m_ppPyramidI[i]);
		
		m_bInitialized = true;
	}
	
	int i;
	
	// copy image to become level 0 of pyramid
	ImageProcessor::CopyImage(pImage, m_ppPyramidJ[0]);
	
	// compute levels 1 to m_nLevels - 1 of pyramid
	for (i = 1; i <= m_nLevels; i++)
		ImageProcessor::Resize(m_ppPyramidJ[i - 1], m_ppPyramidJ[i]);
	
	const int nWindowSize = 2 * m_nHalfWindowSize + 1;
	const int nWindowSizePlus2 = nWindowSize + 2;
	
	Vec2d d;
	
	float *IX = new float[nWindowSizePlus2 * nWindowSizePlus2];
	float *IY = new float[nWindowSizePlus2 * nWindowSizePlus2];
	float *pGrayValues = new float[nWindowSizePlus2 * nWindowSizePlus2];
	
	// track each point
	for (i = 0; i < nPoints; i++)
	{
		const float ux = pPoints[i].x;
		const float uy = pPoints[i].y;
		
		if (ux < 0.0f && uy < 0.0f)
		{
			Math2d::SetVec(pResultPoints[i], -1.0f, -1.0f);
			continue;
		}
		
		Vec2d g = { 0.0f, 0.0f };
		
		for (int l = m_nLevels; l >= 0; l--)
		{
			const int w = m_ppPyramidI[l]->width;
			const int h = m_ppPyramidI[l]->height;
			
			const float px_ = m_pScaleFactors[l] * ux;
			const float py_ = m_pScaleFactors[l] * uy;
			
			const int px = int(floorf(px_));
			const int py = int(floorf(py_));
			
			if (px - m_nHalfWindowSize < 1 || px + m_nHalfWindowSize + 3 > w || py - m_nHalfWindowSize < 1 || py + m_nHalfWindowSize + 3 > h)
			{
				Math2d::MulVecScalar(g, 2.0f, g);
				continue;
			}
			
			const unsigned char *pixelsI = m_ppPyramidI[l]->pixels;
			
			{
				const int diff = w - nWindowSizePlus2;
				
				const float dx = px_ - floorf(px_);
				const float dy = py_ - floorf(py_);
				
				const float f00 = (1.0f - dx) * (1.0f - dy);
				const float f10 = dx * (1.0f - dy);
				const float f01 = (1.0f - dx) * dy;
				const float f11 = dx * dy;
				
				for (int j = nWindowSizePlus2, offset = (py - m_nHalfWindowSize - 1) * w + px - m_nHalfWindowSize - 1, offset2 = 0; j; j--, offset += diff)
					for (int jj = nWindowSizePlus2; jj; jj--, offset++, offset2++)
						pGrayValues[offset2] = f00 * pixelsI[offset] + f10 * pixelsI[offset + 1] + f01 * pixelsI[offset + w] + f11 * pixelsI[offset + w + 1];
			}
			
			float gxx = 0.0f, gxy = 0.0f, gyy = 0.0f;
			
			for (int j = nWindowSize, offset = nWindowSizePlus2 + 1; j; j--, offset += 2)
				for (int jj = nWindowSize; jj; jj--, offset++)
				{
					const float ix = pGrayValues[offset + 1] - pGrayValues[offset - 1];
					const float iy = pGrayValues[offset + nWindowSizePlus2] - pGrayValues[offset - nWindowSizePlus2];
					
					IX[offset] = ix;
					IY[offset] = iy;
					
					gxx += ix * ix;
					gyy += iy * iy;
					gxy += ix * iy;
				}
			
			Vec2d v = { 0.0f, 0.0f };
			
			if (fabsf(gxx * gyy - gxy * gxy) > FLT_EPSILON) // determinant check
			{
				const Mat2d G = { gxx, gxy, gxy, gyy };
				Mat2d G_;
				Math2d::Invert(G, G_);
				
				const unsigned char *pixelsJ = m_ppPyramidJ[l]->pixels;
				
				for (int k = 0; k < KLT_ITERATIONS; k++)
				{
					const float dxJ = g.x + v.x;
					const float dyJ = g.y + v.y;
					
					const int xb = int(floorf(px_ + dxJ));
					const int yb = int(floorf(py_ + dyJ));
					
					if (xb - m_nHalfWindowSize < 1 || xb + m_nHalfWindowSize + 3 >= w || yb - m_nHalfWindowSize < 1 || yb + m_nHalfWindowSize + 3 >= h)
						break;
					
					float bx = 0.0f, by = 0.0f;
					
					{
						const int diff = w - nWindowSize;
						
						const float dx = px_ + dxJ - floorf(px_ + dxJ);
						const float dy = py_ + dyJ - floorf(py_ + dyJ);
						
						const float f00 = (1.0f - dx) * (1.0f - dy);
						const float f10 = dx * (1.0f - dy);
						const float f01 = (1.0f - dx) * dy;
						const float f11 = dx * dy;
						
						for (int j = nWindowSize, offset = (yb - m_nHalfWindowSize) * w + xb - m_nHalfWindowSize, offset2 = nWindowSizePlus2 + 1; j; j--, offset += diff, offset2 += 2)
							for (int jj = nWindowSize; jj; jj--, offset++, offset2++)
							{
								const float dI = pGrayValues[offset2] - (f00 * pixelsJ[offset] + f10 * pixelsJ[offset + 1] + f01 * pixelsJ[offset + w] + f11 * pixelsJ[offset + w + 1]);
								
								bx += IX[offset2] * dI;
								by += IY[offset2] * dI;
							}
					}
					
					const Vec2d b = { bx, by };
					
					Vec2d n;
					Math2d::MulMatVec(G_, b, n);
					
					Math2d::MulVecScalar(n, 2.0f, n); // take into account leaving out factor 0.5 for gradient computation
					
					Math2d::AddToVec(v, n);
					
					if (Math2d::SquaredLength(n) < 0.03f)
						break;
				}
			}
			
			Math2d::SetVec(d, v);
			
			if (l != 0)
			{
				g.x = 2.0f * (g.x + d.x);
				g.y = 2.0f * (g.y + d.y);
			}
		}
		
		Math2d::AddToVec(d, g);
		
		const float x = ux + d.x;
		const float y = uy + d.y;
		
		const int x_ = int(floorf(x));
		const int y_ = int(floorf(y));
		
		if (x_ > m_nHalfWindowSize && x_ < width - m_nHalfWindowSize - 3 && y_ > m_nHalfWindowSize && y_ < height - m_nHalfWindowSize - 3)
			Math2d::SetVec(pResultPoints[i], x, y);
		else
			Math2d::SetVec(pResultPoints[i], -1.0f, -1.0f);
	}
	
	// swap pyramids
	CByteImage **ppTemp = m_ppPyramidI;
	m_ppPyramidI = m_ppPyramidJ;
	m_ppPyramidJ = ppTemp;
	
	// free memory
	delete [] IX;
	delete [] IY;
	delete [] pGrayValues;
	
	return true;
}

