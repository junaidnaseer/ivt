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
// Filename:  VCCapture.cpp
// Author:    Moritz Hassert
// Date:      2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "Helpers/helpers.h"

#define NEW_IMAGE_VAR
#include <vcrt.h>
#include <macros.h>
#include <sysvar.h>

#include "VCCapture.h"

#include <algorithm>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CVCCapture::CVCCapture() : m_width(ScrGetColumns), m_height(ScrGetRows), m_pitch(ScrGetPitch)
{
}

CVCCapture::~CVCCapture()
{
	CloseCamera();
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CVCCapture::OpenCamera()
{
	//no cyclic image aquisition, keep current overlay status
	vmode( getvar(OVLY_ACTIVE) ? vmOvlStill : vmStill);
	tpwait(); //wait for vmode change. else first call to tpp() may fail
	return true;
}

void CVCCapture::CloseCamera()
{
	//restore live mode, keep current overlay status
	//setvar(IMODE, 0); does not work. WHY?
	vmode( getvar(OVLY_ACTIVE) ? vmOvlLive : vmLive);
}

bool CVCCapture::CaptureImage(CByteImage ** ppImages)
{
	//TODO: allow background image capture
	if (tpp() == -1)
		return false;

	//get the capture buffer address here and not in constructor as someone might have changed the location
	unsigned char* restrict scr_pixels = (unsigned char*) ScrGetCaptPage;
	unsigned char* restrict img_pixels = ppImages[0]->pixels;
	
	int h = std::min(m_height, ppImages[0]->height);
	int w = std::min(m_width,  ppImages[0]->width);

	//copy row by row as width may differ and pitch most likely will
	for (int row=0; row<h; row++) {
		std::copy(scr_pixels, scr_pixels+w, img_pixels);

		scr_pixels += m_pitch;
		img_pixels += ppImages[0]->width;
	}

	return true;
}

int CVCCapture::GetWidth()
{
	return m_width;
}

int CVCCapture::GetHeight()
{
	return m_height;
}

CByteImage::ImageType CVCCapture::GetType()
{
	//TODO: what about rgb vc cams?
	return CByteImage::eGrayScale;
}

int CVCCapture::GetNumberOfCameras()
{
	return 1;
}
