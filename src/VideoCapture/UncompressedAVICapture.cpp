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
// Filename:  UncompressedAVICapture.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "UncompressedAVICapture.h"
#include "VideoAccess/VideoReader.h"
#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CUncompressedAVICapture::CUncompressedAVICapture(const char *pFilePath, const char *pSecondFilePath)
{
	m_sFilePath = "";
	m_sFilePath += pFilePath;

	m_sSecondFilePath = "";
	m_pVideoReader = new CVideoReader();

	if (pSecondFilePath)
	{
		m_sSecondFilePath += pSecondFilePath;
		m_bStereo = true;
		m_pSecondVideoReader = new CVideoReader();
	}
	else
	{
		m_pSecondVideoReader = 0;
		m_bStereo = false;
	}
}

CUncompressedAVICapture::~CUncompressedAVICapture()
{
	if (m_pVideoReader)
		delete m_pVideoReader;

	if (m_pSecondVideoReader)
		delete m_pSecondVideoReader;
}


// ****************************************************************************
// Methods
// ****************************************************************************

int CUncompressedAVICapture::GetWidth()
{
	return m_bOK ? m_pVideoReader->GetWidth() : -1;
}

int CUncompressedAVICapture::GetHeight()
{
	return m_bOK ? m_pVideoReader->GetHeight() : -1;
}

CByteImage::ImageType CUncompressedAVICapture::GetType()
{
		return m_bOK ? m_pVideoReader->GetType() : (CByteImage::ImageType) -1;
}


bool CUncompressedAVICapture::OpenCamera()
{
	CloseCamera();
	
	m_bOK = false;

	if (!m_pVideoReader->OpenUncompressedAVI(m_sFilePath.c_str()))
		return false;

	if (m_bStereo)
	{
		if (!m_pSecondVideoReader->OpenUncompressedAVI(m_sSecondFilePath.c_str()))
			return false;
			
		if (m_pVideoReader->GetWidth() != m_pSecondVideoReader->GetWidth() || m_pVideoReader->GetHeight() != m_pSecondVideoReader->GetHeight())
			return false;
	}
	
	m_bOK = true;
    
	return true;
}

void CUncompressedAVICapture::CloseCamera()
{
	m_pVideoReader->Close();

	if (m_pSecondVideoReader)
        	m_pSecondVideoReader->Close();
}

bool CUncompressedAVICapture::CaptureImage(CByteImage **ppImages)
{
	CByteImage *pImage = m_pVideoReader->ReadNextFrame();
	if (!pImage)
		return false;

	ImageProcessor::CopyImage(pImage, ppImages[0]);
	
	if (m_bStereo)
	{
		pImage = m_pSecondVideoReader->ReadNextFrame();
		if (!pImage)
			return false;

		ImageProcessor::CopyImage(pImage, ppImages[1]);
	}
	
	return true;
}
