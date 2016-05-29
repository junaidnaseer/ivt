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
// Filename:  OpenCVCapture.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "OpenCVCapture.h"
#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"

#include <highgui.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

COpenCVCapture::COpenCVCapture(int nIndex, const char *pFilename) : m_nIndex(nIndex)
{
	m_pCapture = 0;
	m_pIplImage = 0;

	m_sFilename = "";

	if (pFilename)
		m_sFilename += pFilename;
}

COpenCVCapture::~COpenCVCapture()
{
	CloseCamera();
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool COpenCVCapture::OpenCamera()
{
	CloseCamera();

	if (m_sFilename.length() > 0)
		m_pCapture = cvCaptureFromFile(m_sFilename.c_str());
	else
		m_pCapture = cvCaptureFromCAM(m_nIndex);
	if (!m_pCapture)
		return false;

	cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_FPS, 30);
	cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_FRAME_WIDTH, 640);
	cvSetCaptureProperty(m_pCapture, CV_CAP_PROP_FRAME_HEIGHT, 480);

	m_pIplImage = cvQueryFrame(m_pCapture);
	
	return true;
}

void COpenCVCapture::CloseCamera()
{
	cvReleaseCapture(&m_pCapture);
	m_pIplImage = 0;
}

bool COpenCVCapture::CaptureImage(CByteImage **ppImages)
{
	m_pIplImage = cvQueryFrame(m_pCapture);
	if (!m_pIplImage)
		return false;

	CByteImage *pImage = ppImages[0];
	if (!pImage || pImage->width != m_pIplImage->width || pImage->height != m_pIplImage->height)
		return false;

	if (m_pIplImage->depth != IPL_DEPTH_8U)
		return false;
	else if (m_pIplImage->nChannels != 1 && m_pIplImage->nChannels != 3)
		return false;
	else if (m_pIplImage->nChannels == 1 && pImage->type != CByteImage::eGrayScale)
		return false;
	else if (m_pIplImage->nChannels == 3 && pImage->type != CByteImage::eRGB24)
		return false;
	
	const int nBytes = pImage->width * pImage->height * pImage->bytesPerPixel;
	unsigned char *input = (unsigned char *) m_pIplImage->imageData;
	unsigned char *output = pImage->pixels;

	if (pImage->type == CByteImage::eGrayScale)
	{
		memcpy(output, input, nBytes);
	}
	else if (pImage->type == CByteImage::eRGB24)
	{
		for (int i = 0; i < nBytes; i += 3)
		{
			output[i] = input[i + 2];
			output[i + 1] = input[i + 1];
			output[i + 2] = input[i];
		}
	}

	#ifdef WIN32
	ImageProcessor::FlipY(pImage, pImage);
	#endif
	
	return true;
}


int COpenCVCapture::GetWidth()
{
	return m_pIplImage ? m_pIplImage->width : -1;
}

int COpenCVCapture::GetHeight()
{
	return m_pIplImage ? m_pIplImage->height : -1;
}

CByteImage::ImageType COpenCVCapture::GetType()
{
	return m_pIplImage ? (m_pIplImage->nChannels == 3 ? CByteImage::eRGB24 : CByteImage::eGrayScale) : (CByteImage::ImageType) -1;
}
