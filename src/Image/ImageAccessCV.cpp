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
// Filename:  ImageAccessCV.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ImageAccessCV.h"
#include "IplImageAdaptor.h"
#include "ByteImage.h"

#include <highgui.h>
#include <stdio.h>



bool ImageAccessCV::LoadFromFile(CByteImage *pImage, const char *pFilePath)
{
	IplImage *pIplImage = cvLoadImage(pFilePath);
	if (!pIplImage)
		return false;
		
	if (pIplImage->nChannels != 3 && pIplImage->nChannels != 1)
	{
		cvReleaseImage(&pIplImage);
		return false;
	}
		
	if (pImage->pixels && pImage->m_bOwnMemory)
		delete [] pImage->pixels;
	
	pImage->type = pIplImage->nChannels == 1 ? CByteImage::eGrayScale : CByteImage::eRGB24;
	pImage->width = pIplImage->width;
	pImage->height = pIplImage->height;
	pImage->bytesPerPixel = pIplImage->nChannels;
	pImage->pixels = new unsigned char[pImage->width * pImage->height * pImage->bytesPerPixel];
	pImage->m_bOwnMemory = true;

	const int nPaddingBytes = pIplImage->widthStep - pImage->bytesPerPixel * pIplImage->width;
	const unsigned char *input = (unsigned char *) pIplImage->imageData;
	unsigned char *output = pImage->pixels;
	const int width = pImage->width;
	const int height = pImage->height;
	
	if (pImage->type == CByteImage::eGrayScale)
	{
		for (int y = 0, i_input = 0, i_output = 0; y < height; y++, i_input += nPaddingBytes)
		{
			for (int x = 0; x < width; x++, i_input++, i_output++)
				output[i_output] = input[i_input];
		}
	}
	else if (pImage->type == CByteImage::eRGB24)
	{
		for (int y = 0, i_input = 0, i_output = 0; y < height; y++, i_input += nPaddingBytes)
		{
			for (int x = 0; x < width; x++, i_input += 3, i_output += 3)
			{
				output[i_output] = input[i_input + 2];
				output[i_output + 1] = input[i_input + 1];
				output[i_output + 2] = input[i_input];
			}
		}
	}
	else
	{
		printf("error: CByteImage::eRGB24Split not supported by ImageAccessCV::LoadFromFile\n");
	}
	
	cvReleaseImage(&pIplImage);
		
	return true;
}


bool ImageAccessCV::SaveToFile(const CByteImage *pImage, const char *pFilePath)
{
	int nRet;
	
	if (pImage->type == CByteImage::eGrayScale)
	{
		IplImage *pIplImage = IplImageAdaptor::Adapt(pImage);
		nRet = cvSaveImage(pFilePath, pIplImage);
		cvReleaseImageHeader(&pIplImage);
	}
	else if (pImage->type == CByteImage::eRGB24)
	{
		const int width = pImage->width;
		const int height = pImage->height;
		const int nBytes = width * height * 3;
		
		CByteImage tempImage(width, height, pImage->type);
		
		const unsigned char *input = pImage->pixels;
		unsigned char *output = tempImage.pixels;
		
		for (int i = 0; i < nBytes; i += 3)
		{
			output[i] = input[i + 2];
			output[i + 1] = input[i + 1];
			output[i + 2] = input[i];
		}
		
		IplImage *pIplImage = IplImageAdaptor::Adapt(&tempImage);
		nRet = cvSaveImage(pFilePath, pIplImage);
		cvReleaseImageHeader(&pIplImage);
	}
	else
	{
		printf("error: CByteImage::eRGB24Split not supported by ImageAccessCV::SaveToFile\n");
	}
	
	
	return nRet != 0;
}
