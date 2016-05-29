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
// Filename:  OpenCVWindow.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "OpenCVWindow.h"
#include "Image/IplImageAdaptor.h"
#include "Image/ByteImage.h"

#include <highgui.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

COpenCVWindow::COpenCVWindow(int nWidth, int nHeight, const char *pWindowName)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	
	m_sWindowName = "";
	m_sWindowName += pWindowName;
}

COpenCVWindow::~COpenCVWindow()
{
	cvDestroyWindow(m_sWindowName.c_str());
}


// ****************************************************************************
// Methods
// ****************************************************************************

void COpenCVWindow::Show()
{
	cvNamedWindow(m_sWindowName.c_str(), CV_WINDOW_AUTOSIZE);
}

void COpenCVWindow::Hide()
{
}

void COpenCVWindow::DrawImage(const CByteImage *pImage, int x, int y)
{
	IplImage *pIplImage = IplImageAdaptor::Adapt(pImage);
	
	if (pImage->type == CByteImage::eRGB24)
	{
		const int max = pImage->width * pImage->height * 3;
		unsigned char *pixels = pImage->pixels;
		for (int i = 0; i < max; i += 3)
		{
			unsigned char temp = pixels[i];
			pixels[i] = pixels[i + 2];
			pixels[i + 2] = temp;
		}
	}

	cvShowImage(m_sWindowName.c_str(), pIplImage);

	if (pImage->type == CByteImage::eRGB24)
	{
		const int max = pImage->width * pImage->height * 3;
		unsigned char *pixels = pImage->pixels;
		for (int i = 0; i < max; i += 3)
		{
			unsigned char temp = pixels[i];
			pixels[i] = pixels[i + 2];
			pixels[i + 2] = temp;
		}
	}
	
	cvReleaseImageHeader(&pIplImage);
}
