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
// Filename:  PatchFeatureEntry.cpp
// Author:    Pedram Azad
// Date:      24.09.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "PatchFeatureEntry.h"

#include "Image/ByteImage.h"



bool CPatchFeatureEntry::ExtractFeature(const CByteImage *pImage, int nWindowSize, float x, float y)
{
	const int width = pImage->width;
	const int height = pImage->height;
	
	if (x < nWindowSize / 2 || x >= width - nWindowSize / 2 || y < nWindowSize / 2 || y >= height - nWindowSize / 2)
		return false;

	// construct new feature entry
	Math2d::SetVec(point, x, y);
	m_nSize = nWindowSize * nWindowSize;
	if (m_pFeature) delete [] m_pFeature;
	m_pFeature = new float[m_nSize];

	// extract the feature
	const int diff = width - nWindowSize;
	const int offset = (int(y) - nWindowSize / 2) * width + (int(x) - nWindowSize / 2);
	const unsigned char *input = pImage->pixels;
	
	// determine normalized patch in left image
	for (int i = 0, offset2 = offset, offset3 = 0; i < nWindowSize; i++, offset2 += diff)
		for (int j = 0; j < nWindowSize; j++, offset2++, offset3++)
		{
			m_pFeature[offset3] = input[offset2];
		}

	return true;
}
