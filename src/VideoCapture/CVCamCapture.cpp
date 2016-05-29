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
// Filename:  CVCamCapture.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "CVCamCapture.h"

#include "Image/ByteImage.h"
#include "Image/IplImageAdaptor.h"
#include "Image/ImageProcessor.h"
#include "Threading/Mutex.h"

#include <cvcam.h>
#include <highgui.h>
#include <cv.h>

#include <memory.h>



// ****************************************************************************
// Global and static variables
// ****************************************************************************

CMutex cvcam_critical_section;
static CByteImage *pCapturedImage = 0;
static bool bFirstFrame = false;


// ****************************************************************************
// Static methods
// ****************************************************************************

static void callback(IplImage *pIplImage)
{
	if (!pIplImage)
		return;

	CByteImage *pImage = IplImageAdaptor::Adapt(pIplImage);


	cvcam_critical_section.Lock();

	if (pCapturedImage && (pCapturedImage->width != pImage->width || pCapturedImage->height != pImage->height || pCapturedImage->type != pImage->type))
	{
		delete pCapturedImage;
		pCapturedImage = 0;
	}

	if (!pCapturedImage)
		pCapturedImage = new CByteImage(pImage);

	unsigned char *input = pImage->pixels;
	unsigned char *output = pCapturedImage->pixels;
	const int nBytes = pImage->width * pImage->height * pImage->bytesPerPixel;

	if (pImage->type == CByteImage::eRGB24)
	{
		for (int i = 0; i < nBytes; i += 3)
		{
			output[i] = input[i + 2];
			output[i + 1] = input[i + 1];
			output[i + 2] = input[i];
		}
	}
	else if (pImage->type == CByteImage::eGrayScale)
	{
		memcpy(output, input, nBytes);
	}

	#ifdef WIN32
	ImageProcessor::FlipY(pCapturedImage, pCapturedImage);
	#endif

	cvcam_critical_section.UnLock();

	bFirstFrame = true;

	delete pImage;
}


// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CCVCamCapture::CCVCamCapture()
{
	m_bStarted = false;

	m_pImage = 0;
	pCapturedImage = 0;
}

CCVCamCapture::~CCVCamCapture()
{
	CloseCamera();

	if (m_pImage)
	{
		delete m_pImage;
		delete pCapturedImage;
	}
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CCVCamCapture::OpenCamera()
{
	if (m_bStarted)
		return true;

	m_bStarted = false;

	if (cvcamGetCamerasCount() <=  0)
		return false;

	// selects the 1st found camera
	cvcamSetProperty(0, CVCAM_PROP_ENABLE, CVCAMTRUE);  
	cvcamSetProperty(0, CVCAM_PROP_RENDER, CVCAMTRUE);	
	// set up a callback to process the frames
	cvcamSetProperty(0, CVCAM_PROP_CALLBACK, (void*) callback);
	// start the camera
	cvcamInit();
	cvcamStart();

	m_bStarted = true;

	while (!bFirstFrame)
		cvWaitKey(10);

	return true;
}

void CCVCamCapture::CloseCamera()
{
	cvcamStop();
	cvcamExit();
	m_bStarted = false;
	bFirstFrame = false;
}

bool CCVCamCapture::CaptureImage(CByteImage **ppImages)
{
	if (!ppImages || !ppImages[0])
		return false;

	CByteImage *pImage = ppImages[0];

	cvcam_critical_section.Lock();
	
	if (!pCapturedImage)
	{
		cvcam_critical_section.UnLock();
		return false;
	}
	
	// check if input image matches format
	if (pCapturedImage->width != pImage->width || pCapturedImage->height != pImage->height || pCapturedImage->type != pImage->type)
	{
		cvcam_critical_section.UnLock();
		return false;
	}
	
	ImageProcessor::CopyImage(pCapturedImage, pImage);

	cvcam_critical_section.UnLock();

	return true;
}

int CCVCamCapture::GetWidth()
{
	return pCapturedImage ? pCapturedImage->width : -1;
}

int CCVCamCapture::GetHeight()
{
	return pCapturedImage ? pCapturedImage->height : -1;
}

CByteImage::ImageType CCVCamCapture::GetType()
{
	return pCapturedImage ? pCapturedImage->type : (CByteImage::ImageType) -1;
}
