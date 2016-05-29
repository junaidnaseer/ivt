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
// Filename:  FloatImage.cpp
// Author:    Jan Issac
// Date:      2011
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "FloatImage.h"
#include "ByteImage.h"
#include "Image/ImageProcessor.h"

#include "Helpers/helpers.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>




// ****************************************************************************
// Defines
// ****************************************************************************

// ****************************************************************************
// Constructors / Destructor
// ****************************************************************************

CFloatImage::CFloatImage()
{
	width = 0;
	height = 0;
	bytesPerPixel = 0;
	pixels = 0;
	numberOfChannels = 1;
	m_bOwnMemory = false;
}

CFloatImage::CFloatImage(int nImageWidth, int nImageHeight, int nNumberOfChannels, bool bHeaderOnly)
{
    bytesPerPixel = nNumberOfChannels * sizeof(float);

	width = nImageWidth;
	height = nImageHeight;
	numberOfChannels = nNumberOfChannels;

	if (bHeaderOnly)
	{
		pixels = 0;
		m_bOwnMemory = false;
	}
	else
	{
		pixels = new float[width * height * numberOfChannels];
		m_bOwnMemory = true;
	}
}

CFloatImage::CFloatImage(const CFloatImage &image, bool bHeaderOnly)
{
	width = image.width;
	height = image.height;
	bytesPerPixel = image.bytesPerPixel;
	numberOfChannels = image.numberOfChannels;
	
	if (bHeaderOnly)
	{
		pixels = 0;
		m_bOwnMemory = false;
	}
	else
	{
	    pixels = new float[width * height * numberOfChannels];
		m_bOwnMemory = true;
	}
}

CFloatImage::CFloatImage(const CFloatImage *pImage, bool bHeaderOnly)
{
	width = pImage->width;
	height = pImage->height;
	bytesPerPixel = pImage->bytesPerPixel;
	numberOfChannels = pImage->numberOfChannels;
	
	if (bHeaderOnly)
	{
		pixels = 0;
		m_bOwnMemory = false;
	}
	else
	{
	    pixels = new float[width * height * numberOfChannels];
		m_bOwnMemory = true;
	}
}

CFloatImage::~CFloatImage()
{
    FreeMemory();
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CFloatImage::FreeMemory()
{
	if (pixels)
	{
		if (m_bOwnMemory)
		delete [] pixels;

		pixels = 0;
		m_bOwnMemory = false;
	}
}

bool CFloatImage::IsCompatible(const CFloatImage *pImage) const
{
	return width == pImage->width && height == pImage->height && numberOfChannels == pImage->numberOfChannels;
}


bool CFloatImage::LoadFromFile(const char *pFileName)
{
    // first free memory, in any case
    FreeMemory();

    CByteImage im;
    if (im.LoadFromFile(pFileName))
    {
        switch(im.type)
        {
        case CByteImage::eGrayScale:
            numberOfChannels = 1;
            break;
        case CByteImage::eRGB24:
            numberOfChannels = 3;
            break;
        case CByteImage::eRGB24Split:
            printf("error: CByteImage::eRGB24Split to 3-Channel CFloatImage not supported.");
            return false;
        }

        width = im.width;
        height = im.height;
        pixels = new float[width * height * numberOfChannels];
        m_bOwnMemory = true;

        ImageProcessor::ConvertImage(&im, this);
        return true;
    }

	return false;
}


bool CFloatImage::SaveToFile(const char *pFileName) const
{
    CByteImage::ImageType imType;

    switch(numberOfChannels)
    {
    case 1:
        imType = CByteImage::eGrayScale;
        break;
    case 3:
        imType = CByteImage::eRGB24;
        break;
    }

    CByteImage im(width, height, imType);

    ImageProcessor::ConvertImage(this, &im);

    return im.SaveToFile(pFileName);
}
