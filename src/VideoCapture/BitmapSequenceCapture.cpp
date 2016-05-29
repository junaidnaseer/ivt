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
// Filename:  BitmapSequenceCapture.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "BitmapSequenceCapture.h"
#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"

#include <stdio.h>
#include <stdlib.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CBitmapSequenceCapture::CBitmapSequenceCapture(const char *pFirstFilePathLeft, const char *pFirstFilePathBaseRight)
{
	m_sFilePathBaseLeft = "";
	m_sFilePathBaseRight = "";
	m_sFirstFilePathLeft = "";
	m_sFirstFilePathRight = "";

	m_sFirstFilePathLeft += pFirstFilePathLeft;
	m_pLeftImage = new CByteImage();

	if (pFirstFilePathBaseRight)
	{
		m_sFirstFilePathRight += pFirstFilePathBaseRight;
		m_pRightImage = new CByteImage();
		m_bStereo = true;
	}
	else
	{
		m_pRightImage = 0;
		m_bStereo = false;
	}

	m_bOK = false;
	n = 0;
	m_nFiguresLeft = 0;
	m_nFiguresRight = 0;

	m_sFileExtention = "";
	nFirstImage = 0;
}

CBitmapSequenceCapture::~CBitmapSequenceCapture()
{
	if (m_pLeftImage)
		delete m_pLeftImage;

	if (m_pRightImage)
		delete m_pRightImage;
}


// ****************************************************************************
// Methods
// ****************************************************************************

int CBitmapSequenceCapture::GetWidth()
{
	return m_bOK ? m_pLeftImage->width : -1;
}

int CBitmapSequenceCapture::GetHeight()
{
	return m_bOK ? m_pLeftImage->height : -1;
}

CByteImage::ImageType CBitmapSequenceCapture::GetType()
{
	return m_bOK ? m_pLeftImage->type : (CByteImage::ImageType) -1;
}


bool CBitmapSequenceCapture::OpenCamera()
{
	m_bOK = false;
	m_nFiguresLeft = 0;
	m_nFiguresRight = 0;
	n = 0;
	nFirstImage = 0;

	const char *pFirstFilePathLeft = m_sFirstFilePathLeft.c_str();
	bool bLookForFigures = false;
	
	const int nLengthMinusOne = int(m_sFirstFilePathLeft.length()) - 1;
	for (int i = nLengthMinusOne; i >= 0; i--)
	{
		if (pFirstFilePathLeft[i] == '.')
		{
			bLookForFigures = true;
			m_sFileExtention = "";
			m_sFileExtention += m_sFirstFilePathLeft.substr(i + 1, 3);
		}
		else if (bLookForFigures)
		{
			if (pFirstFilePathLeft[i] < '0' || pFirstFilePathLeft[i] > '9')
			{
				m_sFilePathBaseLeft = "";
				m_sFilePathBaseLeft += pFirstFilePathLeft;
				m_sFilePathBaseLeft[i + 1] = '\0';
				n = atoi(pFirstFilePathLeft + i + 1);
				nFirstImage = n;
				break;
			}
			else
				m_nFiguresLeft++;
		}
	}

	if (!m_pLeftImage->LoadFromFile(m_sFirstFilePathLeft.c_str()))
		return false;

	if (m_bStereo)
	{
		const char *pFirstFilePathRight = m_sFirstFilePathRight.c_str();
		bool bLookForFigures = false;

		const int nLengthMinusOne = int(m_sFirstFilePathRight.length()) - 1;
		for (int i = nLengthMinusOne; i >= 0; i--)
		{
			if (pFirstFilePathRight[i] == '.')
				bLookForFigures = true;
			else if (bLookForFigures)
			{
				if (pFirstFilePathRight[i] < '0' || pFirstFilePathRight[i] > '9')
				{
					m_sFilePathBaseRight = "";
					m_sFilePathBaseRight += pFirstFilePathRight;
					m_sFilePathBaseRight[i + 1] = '\0';
					if (n != atoi(pFirstFilePathRight + i + 1))
						return false;
					break;
				}
				else
					m_nFiguresRight++;
			}
		}
	
		if (!m_pRightImage->LoadFromFile(m_sFirstFilePathRight.c_str()))
			return false;
			
		if (m_nFiguresLeft != m_nFiguresRight || m_pLeftImage->width != m_pRightImage->width || m_pLeftImage->height != m_pRightImage->height)
			return false;
	}
	
	m_bOK = true;
    
	return true;
}

void CBitmapSequenceCapture::CloseCamera()
{
}

bool CBitmapSequenceCapture::CaptureImage(CByteImage **ppImages)
{
	if (!m_bOK)
		return false;
	
	char szTemp[100];
	char szTemp2[1000];
	
	sprintf(szTemp, "%%s%%.%ii.%s", m_nFiguresLeft, m_sFileExtention.c_str());
	sprintf(szTemp2, szTemp, m_sFilePathBaseLeft.c_str(), n);

	if (!m_pLeftImage->LoadFromFile(szTemp2))
		return false;
	
	if (!m_pLeftImage->IsCompatible(ppImages[0]))
	{
		printf("error: read image is not compatible in CBitmapSequenceCapture::CaptureImage\n");
		return false;
	}
	
	ImageProcessor::CopyImage(m_pLeftImage, ppImages[0]);
	
	if (m_bStereo)
	{
		sprintf(szTemp2, szTemp, m_sFilePathBaseRight.c_str(), n);
		if (!m_pRightImage->LoadFromFile(szTemp2))
			return false;
		
		if (!m_pRightImage->IsCompatible(ppImages[1]))
		{
			printf("error: read image is not compatible in CBitmapSequenceCapture::CaptureImage (right image)\n");
			return false;
		}
		
		ImageProcessor::CopyImage(m_pRightImage, ppImages[1]);
	}
	
	n++;
	
	return true;
}

void CBitmapSequenceCapture::Rewind()
{
	n = nFirstImage;
}
