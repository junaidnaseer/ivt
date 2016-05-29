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
// Filename:  BitmapCapture.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "BitmapCapture.h"

#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CBitmapCapture::CBitmapCapture(const char *pFilePath, const char *pSecondFilePath)
{
	m_sFilePath = "";
	m_sFilePath += pFilePath;

	m_sSecondFilePath = "";
	m_pLeftImage = new CByteImage();

	if (pSecondFilePath)
	{
		m_sSecondFilePath += pSecondFilePath;
		m_bStereo = true;
		m_pRightImage = new CByteImage();
	}
	else
	{
		m_pRightImage = 0;
		m_bStereo = false;
	}
	
	m_pLeftImageRGB24Split = 0;
	m_pRightImageRGB24Split = 0;

	m_bOK = false;
}

CBitmapCapture::~CBitmapCapture()
{
	if (m_pLeftImage)
		delete m_pLeftImage;

	if (m_pRightImage)
		delete m_pRightImage;
	
	if (m_pLeftImageRGB24Split)
		delete m_pLeftImageRGB24Split;
	
	if (m_pRightImageRGB24Split)
		delete m_pRightImageRGB24Split;
}


// ****************************************************************************
// Methods
// ****************************************************************************

int CBitmapCapture::GetWidth()
{
	return m_bOK ? m_pLeftImage->width : -1;
}

int CBitmapCapture::GetHeight()
{
	return m_bOK ? m_pLeftImage->height : -1;
}

CByteImage::ImageType CBitmapCapture::GetType()
{
	return m_bOK ? m_pLeftImage->type : (CByteImage::ImageType) -1;
}


bool CBitmapCapture::OpenCamera()
{
	m_bOK = false;

	if (!m_pLeftImage->LoadFromFile(m_sFilePath.c_str()))
		return false;
	
	if (m_pLeftImage->type == CByteImage::eRGB24)
	{
		if (m_pLeftImageRGB24Split)
			delete m_pLeftImageRGB24Split;
		
		m_pLeftImageRGB24Split = new CByteImage(m_pLeftImage->width, m_pLeftImage->height, CByteImage::eRGB24Split);
		ImageProcessor::ConvertImage(m_pLeftImage, m_pLeftImageRGB24Split);
	}

	if (m_bStereo)
	{
		if (!m_pRightImage->LoadFromFile(m_sSecondFilePath.c_str()))
			return false;
			
		if (m_pLeftImage->width != m_pRightImage->width || m_pLeftImage->height != m_pRightImage->height)
			return false;
		
		if (m_pRightImage->type == CByteImage::eRGB24)
		{
			if (m_pRightImageRGB24Split)
				delete m_pRightImageRGB24Split;
			
			m_pRightImageRGB24Split = new CByteImage(m_pRightImage->width, m_pRightImage->height, CByteImage::eRGB24Split);
			ImageProcessor::ConvertImage(m_pRightImage, m_pRightImageRGB24Split);
		}
	}
	
	m_bOK = true;
    
	return true;
}

void CBitmapCapture::CloseCamera()
{
}

bool CBitmapCapture::CaptureImage(CByteImage **ppImages)
{
	if (!m_bOK)
		return false;
	
	if (ppImages[0]->IsCompatible(m_pLeftImage))
		ImageProcessor::CopyImage(m_pLeftImage, ppImages[0]);
	else if (ppImages[0]->IsCompatible(m_pLeftImageRGB24Split))
		ImageProcessor::CopyImage(m_pLeftImageRGB24Split, ppImages[0]);
	else
	{
		printf("error: images are not compatible in CBitmapCapture::CaptureImage\n");
		return false;
	}
		
	if (m_bStereo)
	{
		if (ppImages[1]->IsCompatible(m_pRightImage))
			ImageProcessor::CopyImage(m_pRightImage, ppImages[1]);
		else if (ppImages[1]->IsCompatible(m_pRightImageRGB24Split))
			ImageProcessor::CopyImage(m_pRightImageRGB24Split, ppImages[1]);
		else
		{
			printf("error: images are not compatible in CBitmapCapture::CaptureImage\n");
			return false;
		}
	}
	
	return true;
}
