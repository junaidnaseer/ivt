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
// Filename:  IplImageAdaptor.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "IplImageAdaptor.h"
#include "Image/ByteImage.h"

#include <stdio.h>



// ****************************************************************************
// Functions
// ****************************************************************************

IplImage* IplImageAdaptor::Adapt(const CByteImage *pImage, bool bAllocateMemory)
{
	if (pImage->type == CByteImage::eRGB24Split)
	{
		printf("error: CByteImage::eRGB24Split is not supported by IplImageAdaptor::Adapt\n");
		return 0;
	}

	IplImage *pIplImage;
	
	if (bAllocateMemory)
	{
		pIplImage = cvCreateImage(cvSize(pImage->width, pImage->height), IPL_DEPTH_8U, pImage->bytesPerPixel);
		memcpy(pIplImage->imageData, pImage->pixels, pImage->width * pImage->height * pImage->bytesPerPixel);
	}
	else
	{
		pIplImage = cvCreateImageHeader(cvSize(pImage->width, pImage->height), IPL_DEPTH_8U, pImage->bytesPerPixel);
		cvSetImageData(pIplImage, (char *) pImage->pixels, pImage->width * pImage->bytesPerPixel);
	}

	return pIplImage;
}
	
CByteImage* IplImageAdaptor::Adapt(const IplImage *pIplImage, bool bAllocateMemory)
{
	CByteImage *pImage;
	
	if (bAllocateMemory)
	{
		pImage = new CByteImage(pIplImage->width, pIplImage->height, pIplImage->nChannels == 1 ? CByteImage::eGrayScale : CByteImage::eRGB24);
		memcpy(pImage->pixels, pIplImage->imageData, pImage->width * pImage->height * pImage->bytesPerPixel);
	}
	else
	{
		pImage = new CByteImage(pIplImage->width, pIplImage->height, pIplImage->nChannels == 1 ? CByteImage::eGrayScale : CByteImage::eRGB24, true);
		pImage->pixels = (unsigned char *) pIplImage->imageData;
	}

	return pImage;
}
