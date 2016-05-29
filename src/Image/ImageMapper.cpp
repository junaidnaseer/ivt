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
// Filename:  ImageMapper.cpp
// Author:    Pedram Azad
// Date:      04.10.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ImageMapper.h"

#include "Image/ByteImage.h"
#include "Image/ImageProcessor.h"
#include "Calibration/Calibration.h"
#include "Helpers/helpers.h"

#include <stdio.h>
#include <math.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CImageMapper::CImageMapper(bool bInterpolate)
{
	m_bInterpolate = bInterpolate;

	m_pOffsetMap = 0;
	m_pCoordinateMap = 0;
	
	width = height = 0;

	m_bMapComputed = false;
}

CImageMapper::~CImageMapper()
{
	if (m_pOffsetMap)
		delete [] m_pOffsetMap;

	if (m_pCoordinateMap)
		delete [] m_pCoordinateMap;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CImageMapper::ComputeMap(int width, int height)
{
	if (width != this->width || height != this->height)
	{
		this->width = width;
		this->height = height;
	
		if (m_pOffsetMap)
			delete [] m_pOffsetMap;
	
		m_pOffsetMap = new int[width * height];
	
		if (m_bInterpolate)
		{
			if (m_pCoordinateMap)
				delete [] m_pCoordinateMap;
			
			m_pCoordinateMap = new MapCoordinates[width * height];
		}
	}
	
	// compute map
	for (int i = 0, offset = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++, offset++)
		{
			const Vec2d newCoordinates = { float(j), float(i) };
			
			// call virtual method
			Vec2d originalCoordinates;
			ComputeOriginalCoordinates(newCoordinates, originalCoordinates);

			const float u = originalCoordinates.x;
			const float v = originalCoordinates.y;

			const int u_int = m_bInterpolate ? int(floor(u)) : my_round(u);
			const int v_int = m_bInterpolate ? int(floor(v)) : my_round(v);

			if (u_int >= 0 && u_int < width - 1 && v_int >= 0 && v_int < height - 1)
			{
				m_pOffsetMap[offset] = v_int * width + u_int;

				if (m_bInterpolate)
				{
					const int u1 = int(floor(u));
					const int v1 = int(floor(v));
					const float x = u - u1;
					const float y = v - v1;
				
					const float f00 = (1 - x) * (1 - y);
					const float f10 = x * (1 - y);
					const float f01 = (1 - x) * y;
					const float f11 = x * y;

					const float sum = f00 + f10 + f01 + f11;

					// 2^22 = 4194304
					m_pCoordinateMap[offset].f00 = int((f00 / sum) * 4194304);
					m_pCoordinateMap[offset].f10 = int((f10 / sum) * 4194304);
					m_pCoordinateMap[offset].f01 = int((f01 / sum) * 4194304);
					m_pCoordinateMap[offset].f11 = int((f11 / sum) * 4194304);
				}
			}
			else
			{
				m_pOffsetMap[offset] = 0;
				
				if (m_bInterpolate)
				{
					m_pCoordinateMap[offset].f00 = 0;
					m_pCoordinateMap[offset].f10 = 0;
					m_pCoordinateMap[offset].f01 = 0;
					m_pCoordinateMap[offset].f11 = 0;
				}
			}
		}
	}

	m_bMapComputed = true;
}

void CImageMapper::PerformMapping(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (!m_bMapComputed)
	{
		printf("error: map has not been computed yet. call CImageMapper::ComputeMap\n");
		return;
	}

	if (pInputImage->type != pOutputImage->type)
	{
		printf("error: input and output image must be of same type for CImageMapper::PerformMapping\n");
		return;
	}

	if (pInputImage->width != width || pInputImage->height != height)
	{
		printf("error: input image does not match calibration file for CImageMapper::PerformMapping\n");
		return;
	}

	if (pOutputImage->width != width || pOutputImage->height != height)
	{
		printf("error: output image does not match calibration file for CImageMapper::PerformMapping\n");
		return;
	}

	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}

	unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	const int nPixels = width * height;

	if (pInputImage->type == CByteImage::eGrayScale)
	{
		const unsigned char g = input[0];
		input[0] = 0;

		if (m_bInterpolate)
		{
			for (int i = 0; i < nPixels; i++)
			{
				const int input_offset = m_pOffsetMap[i];
				const MapCoordinates &m = m_pCoordinateMap[i];
				output[i] = (unsigned char) ((input[input_offset] * m.f00 + input[input_offset + width] * m.f01 + input[input_offset + 1] * m.f10 + input[input_offset + width + 1] * m.f11 + 2097152) >> 22);
			}
		}
		else
		{
			for (int i = 0; i < nPixels; i++)
				output[i] = input[m_pOffsetMap[i]];
		}

		input[0] = g;
	}
	else if (pInputImage->type == CByteImage::eRGB24)
	{
		const unsigned char r = input[0];
		const unsigned char g = input[1];
		const unsigned char b = input[2];
		input[0] = input[1] = input[2] = 0;

		if (m_bInterpolate)
		{
			for (int i = 0, output_offset = 0; i < nPixels; i++, output_offset += 3)
			{
				const int input_offset = 3 * m_pOffsetMap[i];
				const int width3 = 3 * width;

				const MapCoordinates &m = m_pCoordinateMap[i];
				
				output[output_offset] = (unsigned char) ((input[input_offset] * m.f00 + input[input_offset + width3] * m.f01 + input[input_offset + 3] * m.f10 + input[input_offset + width3 + 3] * m.f11 + 2097152) >> 22);
				output[output_offset + 1] = (unsigned char) ((input[input_offset + 1] * m.f00 + input[input_offset + width3 + 1] * m.f01 + input[input_offset + 4] * m.f10 + input[input_offset + width3 + 4] * m.f11 + 2097152) >> 22);
				output[output_offset + 2] = (unsigned char) ((input[input_offset + 2] * m.f00 + input[input_offset + width3 + 2] * m.f01 + input[input_offset + 5] * m.f10 + input[input_offset + width3 + 5] * m.f11 + 2097152) >> 22);
			}
		}
		else
		{
			for (int i = 0, output_offset = 0; i < nPixels; i++, output_offset += 3)
			{
				const int input_offset = 3 * m_pOffsetMap[i];
				output[output_offset] = input[input_offset];
				output[output_offset + 1] = input[input_offset + 1];
				output[output_offset + 2] = input[input_offset + 2];
			}
		}

		input[0] = r;
		input[1] = g;
		input[2] = b;
	}

	if (pSaveOutputImage)
	{
		ImageProcessor::CopyImage(pOutputImage, pSaveOutputImage);
		delete pOutputImage;
	}
}
