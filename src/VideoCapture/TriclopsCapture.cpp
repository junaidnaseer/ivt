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
// Filename:  TriclopsCapture.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "TriclopsCapture.h"
#include "Image/ByteImage.h"



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CTriclopsCapture::CTriclopsCapture(VideoMode mode) : m_mode(mode)
{
	m_nImageWidth = -1;
	m_nImageHeight = -1;

	m_digiclopsContext = 0;
	m_triclopsContext = 0;
}

CTriclopsCapture::~CTriclopsCapture()
{
	CloseCamera();
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CTriclopsCapture::OpenCamera()
{
	if (m_mode != e640x480 && m_mode != e320x240)
		return false;

	digiclopsCreateContext(&m_digiclopsContext);
	digiclopsInitialize(m_digiclopsContext, 0);

	// get the camera module configuration
	digiclopsGetTriclopsContextFromCamera(m_digiclopsContext, &m_triclopsContext);

	// set the digiclops to deliver the left image and right image
	digiclopsSetImageTypes(m_digiclopsContext, LEFT_IMAGE | RIGHT_IMAGE);

	// set the Digiclops resolution
	switch (m_mode)
	{
		case e640x480:
			m_nImageWidth = 640;
			m_nImageHeight = 480;
			digiclopsSetImageResolution(m_digiclopsContext, DIGICLOPS_FULL);
		break;

		case e320x240:
			m_nImageWidth = 320;
			m_nImageHeight = 240;
			digiclopsSetImageResolution(m_digiclopsContext, DIGICLOPS_HALF);
		break;
	}

	//digiclopsSetCameraProperty(m_digiclopsContext, DIGICLOPS_HARDWARE_WHITEBALANCE, 117, 145, false);
	digiclopsSetCameraProperty(m_digiclopsContext, DIGICLOPS_WHITEBALANCE, 117, 145, false);
	digiclopsSetMaxFrameRate(m_digiclopsContext, DIGICLOPS_FRAMERATE_100);

	// start grabbing
	digiclopsStart(m_digiclopsContext);
   
	// set up some stereo parameters
	triclopsSetResolution(m_triclopsContext, m_nImageHeight, m_nImageWidth);

	return true;
}

void CTriclopsCapture::CloseCamera()
{
	if (m_digiclopsContext)
	{
		digiclopsDestroyContext(m_digiclopsContext);
		m_digiclopsContext = 0;
	}

	if (m_triclopsContext)
	{
		triclopsDestroyContext(m_triclopsContext);
		m_triclopsContext = 0;
	}
}

bool CTriclopsCapture::CaptureImage(CByteImage **ppImages)
{
	if (!ppImages || !ppImages[0] || !ppImages[1] ||
		ppImages[0]->width != width || ppImages[0]->height != height ||
		ppImages[1]->width != width || ppImages[1]->height != height ||
		ppImages[0]->type != CByteImage::eRGB24 || ppImages[1]->type != CByteImage::eRGB24)
		return false;
	
	// grab and extract left and right image
	if (digiclopsGrabImage(m_digiclopsContext) != 0)
		return false;

	digiclopsExtractTriclopsInput(m_digiclopsContext, LEFT_IMAGE, &m_colorDataLeft);
	digiclopsExtractTriclopsInput(m_digiclopsContext, RIGHT_IMAGE, &m_colorDataRight);

	triclopsRectifyColorImage(m_triclopsContext, TriCam_LEFT, &m_colorDataLeft, &m_colorImageLeft);
	triclopsRectifyColorImage(m_triclopsContext, TriCam_RIGHT, &m_colorDataRight, &m_colorImageRight);

	ConvertImages(ppImages);

	return true;
}

void CTriclopsCapture::ConvertImages(CByteImage **ppImages)
{
	unsigned char *pLeftData = ppImages[0]->pixels;
	unsigned char *pRightData = ppImages[1]->pixels;
	unsigned char *pLeftRed = (unsigned char *) m_colorImageLeft.red;
	unsigned char *pLeftGreen = (unsigned char *) m_colorImageLeft.green;
	unsigned char *pLeftBlue = (unsigned char *) m_colorImageLeft.blue;
	unsigned char *pRightRed = (unsigned char *) m_colorImageRight.red;
	unsigned char *pRightGreen = (unsigned char *) m_colorImageRight.green;
	unsigned char *pRightBlue = (unsigned char *) m_colorImageRight.blue;
	
	#ifdef __linux
	int offset = 0;
	#else
	int offset = 3 * (m_nImageWidth * m_nImageHeight - m_nImageWidth);
	#endif

	for (int y = 0; y < m_nImageHeight; y++)
	{
		for (int x = 0; x < m_nImageWidth; x++, offset += 3)
		{
			pLeftData[offset] = pLeftRed[x];
			pLeftData[offset + 1] = pLeftGreen[x];
			pLeftData[offset + 2] = pLeftBlue[x];
			pRightData[offset] = pRightRed[x];
			pRightData[offset + 1] = pRightGreen[x];
			pRightData[offset + 2] = pRightBlue[x];
		}
		
		pLeftRed += m_colorImageLeft.rowinc;
		pLeftGreen += m_colorImageLeft.rowinc;
		pLeftBlue += m_colorImageLeft.rowinc;

		pRightRed += m_colorImageLeft.rowinc;
		pRightGreen += m_colorImageLeft.rowinc;
		pRightBlue += m_colorImageLeft.rowinc;

		#ifndef __linux
		offset -= 6 * m_nImageWidth;
		#endif
	}
}
