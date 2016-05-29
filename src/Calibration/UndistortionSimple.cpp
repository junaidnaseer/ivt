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
// Filename:  UndistortionSimple.cpp
// Author:    Pedram Azad
// Date:      11.10.2011
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "UndistortionSimple.h"

#include "Image/ByteImage.h"
#include "Math/Math2d.h"

#include <stdio.h>



// ****************************************************************************
// class CUndistortion::CUndistortionMapper
// ****************************************************************************

void CUndistortionSimple::CUndistortionMapper::Init(int width, int height, float d)
{
	this->d = d;
	this->cx = 0.5f * float(width - 1);
	this->cy = 0.5f * float(height - 1);
	
	ComputeMap(width, height);
}

void CUndistortionSimple::CUndistortionMapper::ComputeOriginalCoordinates(const Vec2d &newCoordinates, Vec2d &originalCoordinates)
{
	// radial distortion with one parameter d
	const float x = newCoordinates.x - cx;
	const float y = newCoordinates.y - cy;
	
	const float rr = x * x + y * y;
	const float factor = 1.0f + d * rr;
	
	originalCoordinates.x = cx + factor * x;
	originalCoordinates.y = cy + factor * y;
}



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CUndistortionSimple::CUndistortionSimple(int width, int height, float d, bool bInterpolate)
{
	this->width = width;
	this->height = height;
	
	m_pUndistortionMapper = new CUndistortionMapper(bInterpolate);
	m_pUndistortionMapper->Init(width, height, d);
}

CUndistortionSimple::~CUndistortionSimple()
{
	delete m_pUndistortionMapper;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CUndistortionSimple::Undistort(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	m_pUndistortionMapper->PerformMapping(pInputImage, pOutputImage);
}
