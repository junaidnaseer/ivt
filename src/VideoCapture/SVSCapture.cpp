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
// Filename:  SVSCapture.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************
// Changes:   13.07.2006, Martin Loesch
//            * Fixed compatiblity issues for newer versions of the SVS
//            13.06.2008, Miguel Bernal Marin
//            * Optimized RGBA2RGB conversion
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "SVSCapture.h"

#include "Image/ByteImage.h"

#include <memory.h>
#include <iostream>

#include <svsclass.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CSVSCapture::CSVSCapture(VideoMode mode, int nIndex) : m_nIndex(nIndex), m_mode(mode)
{
	svs_video = getVideoObject();
	
	m_nBytesPerPixel = 0;
	m_bColor = false;

	width = -1;
	height = -1;
}

CSVSCapture::~CSVSCapture()
{
	CloseCamera();
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CSVSCapture::CaptureImage(CByteImage **ppImages)
{
	if (!ppImages || !ppImages[0] || !ppImages[1] ||
		ppImages[0]->width != width || ppImages[0]->height != height ||
		ppImages[1]->width != width || ppImages[1]->height != height )
	{
		return false;
	}

	if ((m_bColor && ( ppImages[0]->type != CByteImage::eRGB24 || ppImages[1]->type != CByteImage::eRGB24) ) ||
	   (!m_bColor && ( ppImages[0]->type != CByteImage::eGrayScale || ppImages[1]->type != CByteImage::eGrayScale) ) )
	{
		return false;
	}

	// get image
	svsStereoImage *svs_image = svs_video->GetImage(10);
	if (!svs_image)
		return false;
		
	if (m_bColor)
	{
		// destination images
		unsigned int *dstL = (unsigned int *) ppImages[0]->pixels;
		unsigned int *dstR = (unsigned int *) ppImages[1]->pixels;

		// source images
		unsigned int *srcL = (unsigned int *) svs_image->color;
		unsigned int *srcR = (unsigned int *) svs_image->color_right;

		// BEGIN: Miguel Bernal Marin
		int max = width * height / 4;
		
		while (max--)
		{
			// copy four left images
			*dstL++ = (srcL[0] & 0x00FFFFFF)       | (srcL[1] & 0x000000FF) << 24;
			*dstL++ = (srcL[1] & 0x00FFFF00) >>  8 | (srcL[2] & 0x0000FFFF) << 16;
			*dstL++ = (srcL[2] & 0x00FF0000) >> 16 | (srcL[3] & 0x00FFFFFF) <<  8;
			
			// copy four right images
			*dstR++ = (srcR[0] & 0x00FFFFFF)       | (srcR[1] & 0x000000FF) << 24;
			*dstR++ = (srcR[1] & 0x00FFFF00) >>  8 | (srcR[2] & 0x0000FFFF) << 16;
			*dstR++ = (srcR[2] & 0x00FF0000) >> 16 | (srcR[3] & 0x00FFFFFF) <<  8;

			srcL += 4;
			srcR += 4;
		}
		// END
	}
	else
	{
		memcpy(ppImages[1]->pixels, svs_image->left, width * height * m_nBytesPerPixel);
		memcpy(ppImages[0]->pixels, svs_image->right, width * height * m_nBytesPerPixel);
	}

	return true;
}

void CSVSCapture::CloseCamera()
{
	if (svs_video)
	{
		svs_video->Stop();
		svs_video->Close();
	}
}

bool CSVSCapture::OpenCamera()
{
	int resolution = -1;

	// BEGIN INSERTED
	int maxSizeFactor = 0;  // 1=1280x960, 2=640x480
	// END INSERTED
	
	switch (m_mode)
	{
		case e320x240:
			width = 320;
			height = 240;
			resolution = 1;
		break;
		
		case e640x480:
			width = 640;
			height = 480;
			resolution = 4;
		break;
		
		case e1280x960:
			width = 1280;
			height = 960;
			resolution = 5;
		break;
		
		default:
			return false;
	}

	m_nBytesPerPixel = 3;
	m_bColor = true;
  
	if (!svs_video->Open(m_nIndex))
		return false;

	// idea: try to open cam with resolution and sampling that is dependent of the possible max resolution of the cam
	svs_video->SetSize(320, 240);
	svs_video->SetSample(2, 2);
	
	if (svs_video->Start())
	{
		svs_video->Stop();
		maxSizeFactor = 1; // cam has max resolution 1280x960
	}
	else
	{
		maxSizeFactor = 2; // cam has max resolution 640x480
	}

	svsImageParams *ip = 0;
  
	switch (resolution)
	{
		case 0:
			// 320x240
			svs_video->SetSize(320, 240);
			svs_video->SetFrameDiv(1);
			svs_video->SetSample(1, 4 / maxSizeFactor);
		break;

		case 1:
			// 320x240
			svs_video->SetSize(320, 240);
			svs_video->SetFrameDiv(1);
			svs_video->SetSample(2/maxSizeFactor, 2);
		break;
		
		case 2:
			// 320x240
			svs_video->SetSize(320, 240);
			svs_video->SetFrameDiv(1);
			svs_video->SetSample(4 / (maxSizeFactor * maxSizeFactor), maxSizeFactor); // some kind of evil hack, because for some reason, SetSample(2,1) will not work with the 640x480 videre Cam, it has to be SetSample(1,2)...
		break;

		case 3:
			// 640x480
			svs_video->SetSize(640, 480);
			svs_video->SetFrameDiv(1);
			svs_video->SetSample(1, 2 / maxSizeFactor);
		break;
		
		case 4:
			// 640x480
			svs_video->SetSize(640, 480);
			svs_video->SetFrameDiv(1);
			svs_video->SetSample(2 / maxSizeFactor, 1);
		break;
		
		case 5:
			// 1280x960
			svs_video->SetSize(1280, 960);
			svs_video->SetFrameDiv(1);
			svs_video->SetSample(1, 1);
			ip->ix = 0;
			ip->iy = 0;
			ip->vergence = 0;
		break;
	}
  
	// set default values
	svs_video->SetBalance(false, 20, 10);
	svs_video->SetColor(true, true);
	svs_video->SetRect(false);
	svs_video->exposure = 100;
	svs_video->gain = 30;
	svs_video->SetRate(30);
	svs_video->SetDigitization();

	// start video acquisition
	if (!svs_video->Start())
	{
		CloseCamera();
		return false;
	}
	
	return true;
}

void CSVSCapture::SetRed(int val)
{
	if (val < -40 || val > 40)
		return;
		
	svs_video->red = val;
	svs_video->SetDigitization();
}

void CSVSCapture::SetBlue(int val)
{
	if (val < -40 || val > 40)
		return;
	
	svs_video->blue = val;
	svs_video->SetDigitization();
}

void CSVSCapture::SetExposure(int val)
{
	if (val < 0 || val > 100)
		return;

	svs_video->exposure = val;
	svs_video->SetDigitization();
}

void CSVSCapture::SetGain(int val)
{
	if (val < 0 || val > 100)
		return;
		
	svs_video->gain = val;
	svs_video->SetDigitization();
}

void CSVSCapture::SetColor(bool bColor)
{	
	svs_video->SetColor(bColor, bColor);
	svs_video->SetDigitization();
		
	if (bColor)
	{
		m_nBytesPerPixel = 3;
	}
	else
	{
		m_nBytesPerPixel = 1;
	}
	
	m_bColor = bColor;
}

void CSVSCapture::SetRectify(bool bRectify)
{
	svs_video->SetRect(bRectify);
	svs_video->SetDigitization();
}
