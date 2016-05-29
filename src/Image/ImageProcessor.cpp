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
// Filename:  ImageProcessor.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************
// Changes:   20.05.2008, Florian Hecht
//            * Added Summed Area Table functions 
//
//            07.01.2009, Moritz Hassert
//            * Added new functions:
//              Add, AddAndSaturate, Subtract, AbsoluteDifference,
//              Average, Min, Max
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ImageProcessor.h"

#include "ByteImage.h"
#include "ShortImage.h"
#include "IntImage.h"
#include "FloatImage.h"
#include "PrimitivesDrawer.h"
#include "Color/RGBColorModel.h"
#include "Math/LinearAlgebra.h"
#include "Math/FloatMatrix.h"
#include "Math/DoubleMatrix.h"
#include "Math/Constants.h"
#include "Math/Math3d.h"
#include "Math/Matd.h"
#include "Math/Vecd.h"
#include "Helpers/helpers.h"
#include "Helpers/OptimizedFunctions.h"
#include "Color/ColorParameterSet.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stddef.h>
#include <algorithm>
#include <limits.h>
#include <float.h>



// ****************************************************************************
// Static variables and functions
// ****************************************************************************

// (1 << 20) / i
static const int division_table[] =
{
	0, 1048576, 524288, 349525, 262144, 209715, 174762, 149796, 
	131072, 116508, 104857, 95325, 87381, 80659, 74898, 69905, 
	65536, 61680, 58254, 55188, 52428, 49932, 47662, 45590, 
	43690, 41943, 40329, 38836, 37449, 36157, 34952, 33825, 
	32768, 31775, 30840, 29959, 29127, 28339, 27594, 26886, 
	26214, 25575, 24966, 24385, 23831, 23301, 22795, 22310, 
	21845, 21399, 20971, 20560, 20164, 19784, 19418, 19065, 
	18724, 18396, 18078, 17772, 17476, 17189, 16912, 16644, 
	16384, 16131, 15887, 15650, 15420, 15196, 14979, 14768, 
	14563, 14364, 14169, 13981, 13797, 13617, 13443, 13273, 
	13107, 12945, 12787, 12633, 12483, 12336, 12192, 12052, 
	11915, 11781, 11650, 11522, 11397, 11275, 11155, 11037, 
	10922, 10810, 10699, 10591, 10485, 10381, 10280, 10180, 
	10082, 9986, 9892, 9799, 9709, 9619, 9532, 9446, 
	9362, 9279, 9198, 9118, 9039, 8962, 8886, 8811, 
	8738, 8665, 8594, 8525, 8456, 8388, 8322, 8256, 
	8192, 8128, 8065, 8004, 7943, 7884, 7825, 7767, 
	7710, 7653, 7598, 7543, 7489, 7436, 7384, 7332, 
	7281, 7231, 7182, 7133, 7084, 7037, 6990, 6944, 
	6898, 6853, 6808, 6765, 6721, 6678, 6636, 6594, 
	6553, 6512, 6472, 6432, 6393, 6355, 6316, 6278, 
	6241, 6204, 6168, 6132, 6096, 6061, 6026, 5991, 
	5957, 5924, 5890, 5857, 5825, 5793, 5761, 5729, 
	5698, 5667, 5637, 5607, 5577, 5548, 5518, 5489, 
	5461, 5433, 5405, 5377, 5349, 5322, 5295, 5269, 
	5242, 5216, 5190, 5165, 5140, 5115, 5090, 5065, 
	5041, 5017, 4993, 4969, 4946, 4922, 4899, 4877, 
	4854, 4832, 4809, 4788, 4766, 4744, 4723, 4702, 
	4681, 4660, 4639, 4619, 4599, 4578, 4559, 4539, 
	4519, 4500, 4481, 4462, 4443, 4424, 4405, 4387, 
	4369, 4350, 4332, 4315, 4297, 4279, 4262, 4245, 
	4228, 4211, 4194, 4177, 4161, 4144, 4128, 4112
};

static const int MatrixGaussian5x5[25] =
{
	1,  5,  7,  5, 1,
	5, 20, 33, 20, 5,
	7, 33, 55, 33, 7,
	5, 20, 33, 20, 5,
	1,  5,  7,  5, 1
};

static float sin_table_[380];
static float cos_table_[380];
static float *sin_table = sin_table_ + 10;
static float *cos_table = cos_table_ + 10;

static void InitSinCosTables()
{
	static bool bSinCosTablesInitialized = false;
	
	if (bSinCosTablesInitialized)
		return;
	
	for (int i = -10; i < 370; i++)
	{
		const float theta = i * FLOAT_DEG2RAD;
		sin_table[i] = sinf(theta);
		cos_table[i] = cosf(theta);
	}
	
	bSinCosTablesInitialized = true;
}



// ****************************************************************************
// Functions
// ****************************************************************************

bool ImageProcessor::GeneralFilter(const CByteImage *pInputImage, CByteImage *pOutputImage, const int *pKernel, int nMaskSize, int nDivider, bool bAbsoluteValue)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale || pOutputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::GeneralFilter\n");
		return false;
	}

	if (nMaskSize < 3 || (nMaskSize + 1) % 2 != 0)
	{
		printf("error: nMaskSize must be 2 * k + 1 with k >= 1 for ImageProcessor::GeneralFilter\n");
		return false;
	}

	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}

	ImageProcessor::Zero(pOutputImage);
	
	const int umax = pInputImage->width - nMaskSize + 1;
	const int vmax = pInputImage->height - nMaskSize + 1;

	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	const int diff = (nMaskSize - 1) * (pInputImage->width + 1) / 2;

	if (nDivider == 1)
	{
		if (bAbsoluteValue)
		{
			for (int v = 0, offset = diff; v < vmax; v++, offset += nMaskSize - 1)
			{
				for (int u = 0; u < umax; u++, offset++)
				{
					int sum = 0;
		
					for (int i = 0, offset2 = offset - diff, offset3 = 0; i < nMaskSize; i++, offset2 += umax - 1)
					{
						for (int j = 0;  j < nMaskSize; j++, offset2++, offset3++)
							sum += input[offset2] * pKernel[offset3];
					}
					
					output[offset] = (unsigned char) abs(sum);
				}
			}
		}
		else
		{
			for (int v = 0, offset = diff; v < vmax; v++, offset += nMaskSize - 1)
			{
				for (int u = 0; u < umax; u++, offset++)
				{
					int sum = 0;
		
					for (int i = 0, offset2 = offset - diff, offset3 = 0; i < nMaskSize; i++, offset2 += umax - 1)
					{
						for (int j = 0;  j < nMaskSize; j++, offset2++, offset3++)
							sum += input[offset2] * pKernel[offset3];
					}
					
					output[offset] = (unsigned char) sum;
				}
			}
		}
	}
	else
	{
		if (bAbsoluteValue)
		{
			for (int v = 0, offset = diff; v < vmax; v++, offset += nMaskSize - 1)
			{
				for (int u = 0; u < umax; u++, offset++)
				{
					int sum = 0;
		
					for (int i = 0, offset2 = offset - diff, offset3 = 0; i < nMaskSize; i++, offset2 += umax - 1)
					{
						for (int j = 0;  j < nMaskSize; j++, offset2++, offset3++)
							sum += input[offset2] * pKernel[offset3];
					}
					
					output[offset] = (unsigned char) (abs(sum) / nDivider);
				}
			}
		}
		else
		{
			for (int v = 0, offset = diff; v < vmax; v++, offset += nMaskSize - 1)
			{
				for (int u = 0; u < umax; u++, offset++)
				{
					int sum = 0;
		
					for (int i = 0, offset2 = offset - diff, offset3 = 0; i < nMaskSize; i++, offset2 += umax - 1)
					{
						for (int j = 0;  j < nMaskSize; j++, offset2++, offset3++)
							sum += input[offset2] * pKernel[offset3];
					}
					
					output[offset] = (unsigned char) (sum / nDivider);
				}
			}
		}
	}

	if (pSaveOutputImage)
	{
		CopyImage(pOutputImage, pSaveOutputImage);
		delete pOutputImage;
	}

	return true;
}

bool ImageProcessor::GeneralFilter(const CByteImage *pInputImage, CShortImage *pOutputImage, const int *pKernel, int nMaskSize, int nDivider, bool bAbsoluteValue)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::GeneralFilter\n");
		return false;
	}

	if (nMaskSize < 3 || (nMaskSize + 1) % 2 != 0)
	{
		printf("error: nMaskSize must be 2 * k + 1 with k >= 1 for ImageProcessor::GeneralFilter\n");
		return false;
	}

	ImageProcessor::Zero(pOutputImage);
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;
	const int umax = width - nMaskSize + 1;
	const int vmax = height - nMaskSize + 1;

	const unsigned char *input = pInputImage->pixels;
	short *output = pOutputImage->pixels;

	const int diff = (nMaskSize - 1) * (width + 1) / 2;

	if (nDivider == 1)
	{
		if (bAbsoluteValue)
		{
			for (int v = 0, offset = diff; v < vmax; v++, offset += nMaskSize - 1)
			{
				for (int u = 0; u < umax; u++, offset++)
				{
					int sum = 0;
		
					for (int i = 0, offset2 = offset - diff, offset3 = 0; i < nMaskSize; i++, offset2 += umax - 1)
					{
						for (int j = 0;  j < nMaskSize; j++, offset2++, offset3++)
							sum += input[offset2] * pKernel[offset3];
					}
					
					output[offset] = (short) abs(sum);
				}
			}
		}
		else
		{
			for (int v = 0, offset = diff; v < vmax; v++, offset += nMaskSize - 1)
			{
				for (int u = 0; u < umax; u++, offset++)
				{
					int sum = 0;
		
					for (int i = 0, offset2 = offset - diff, offset3 = 0; i < nMaskSize; i++, offset2 += umax - 1)
					{
						for (int j = 0;  j < nMaskSize; j++, offset2++, offset3++)
							sum += input[offset2] * pKernel[offset3];
					}
					
					output[offset] = (short) sum;
				}
			}
		}
	}
	else
	{
		if (bAbsoluteValue)
		{
			for (int v = 0, offset = diff; v < vmax; v++, offset += nMaskSize - 1)
			{
				for (int u = 0; u < umax; u++, offset++)
				{
					int sum = 0;
		
					for (int i = 0, offset2 = offset - diff, offset3 = 0; i < nMaskSize; i++, offset2 += umax - 1)
					{
						for (int j = 0;  j < nMaskSize; j++, offset2++, offset3++)
							sum += input[offset2] * pKernel[offset3];
					}
					
					output[offset] = (short) (abs(sum) / nDivider);
				}
			}
		}
		else
		{
			for (int v = 0, offset = diff; v < vmax; v++, offset += nMaskSize - 1)
			{
				for (int u = 0; u < umax; u++, offset++)
				{
					int sum = 0;
		
					for (int i = 0, offset2 = offset - diff, offset3 = 0; i < nMaskSize; i++, offset2 += umax - 1)
					{
						for (int j = 0;  j < nMaskSize; j++, offset2++, offset3++)
							sum += input[offset2] * pKernel[offset3];
					}
					
					output[offset] = (short) (sum / nDivider);
				}
			}
		}
	}

	return true;
}

bool ImageProcessor::GeneralFilter(const CByteImage *pInputImage, CFloatMatrix *pOutputMatrix, const int *pKernel, int nMaskSize, int nDivider, bool bAbsoluteValue)
{
	if (pInputImage->width != pOutputMatrix->columns || pInputImage->height != pOutputMatrix->rows ||
		pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image and output matrix do not match for ImageProcessor::GeneralFilter\n");
		return false;
	}

	if (nMaskSize < 3 || (nMaskSize + 1) % 2 != 0)
	{
		printf("error: nMaskSize must be 2 * k + 1 with k >= 1 for ImageProcessor::GeneralFilter\n");
		return false;
	}

	ImageProcessor::Zero(pOutputMatrix);
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;
	const int umax = width - nMaskSize + 1;
	const int vmax = height - nMaskSize + 1;

	const unsigned char *input = pInputImage->pixels;
	float *output = pOutputMatrix->data;

#if 0
	
	const int k = (nMaskSize - 1) / 2;

	for (int v = 0; v < vmax; v++)
	{
		for (int u = 0; u < umax; u++)
		{
			int sum = 0;

			for (int i = 0; i < nMaskSize; i++)
			{
				for (int j = 0;  j < nMaskSize; j++)
					sum += input[(v + i) * width + (u + j)] * pKernel[i * nMaskSize + j];
			}

			output[(v + k) * width + (u + k)] = float(abs(sum)) / nDivider;
		}
	}

#else

	const int diff = (nMaskSize - 1) * (width + 1) / 2;

	if (nDivider == 1)
	{
		if (bAbsoluteValue)
		{
			for (int v = 0, offset = diff; v < vmax; v++, offset += nMaskSize - 1)
			{
				for (int u = 0; u < umax; u++, offset++)
				{
					int sum = 0;
		
					for (int i = 0, offset2 = offset - diff, offset3 = 0; i < nMaskSize; i++, offset2 += umax - 1)
					{
						for (int j = 0;  j < nMaskSize; j++, offset2++, offset3++)
							sum += input[offset2] * pKernel[offset3];
					}
					
					output[offset] = (float) abs(sum);
				}
			}
		}
		else
		{
			for (int v = 0, offset = diff; v < vmax; v++, offset += nMaskSize - 1)
			{
				for (int u = 0; u < umax; u++, offset++)
				{
					int sum = 0;
		
					for (int i = 0, offset2 = offset - diff, offset3 = 0; i < nMaskSize; i++, offset2 += umax - 1)
					{
						for (int j = 0;  j < nMaskSize; j++, offset2++, offset3++)
							sum += input[offset2] * pKernel[offset3];
					}
					
					output[offset] = (float) sum;
				}
			}
		}
	}
	else
	{
		if (bAbsoluteValue)
		{
			for (int v = 0, offset = diff; v < vmax; v++, offset += nMaskSize - 1)
			{
				for (int u = 0; u < umax; u++, offset++)
				{
					int sum = 0;
		
					for (int i = 0, offset2 = offset - diff, offset3 = 0; i < nMaskSize; i++, offset2 += umax - 1)
					{
						for (int j = 0;  j < nMaskSize; j++, offset2++, offset3++)
							sum += input[offset2] * pKernel[offset3];
					}
					
					output[offset] = float(abs(sum)) / nDivider;
				}
			}
		}
		else
		{
			for (int v = 0, offset = diff; v < vmax; v++, offset += nMaskSize - 1)
			{
				for (int u = 0; u < umax; u++, offset++)
				{
					int sum = 0;
		
					for (int i = 0, offset2 = offset - diff, offset3 = 0; i < nMaskSize; i++, offset2 += umax - 1)
					{
						for (int j = 0;  j < nMaskSize; j++, offset2++, offset3++)
							sum += input[offset2] * pKernel[offset3];
					}
					
					output[offset] = float(sum) / nDivider;
				}
			}
		}
	}
#endif

	return true;
}


// Separation:
//
// ( 1  4  6  4  1 )     ( 1 )
// ( 4 16 24 16  4 )     ( 4 )
// ( 6 24 36 24  6 )  =  ( 6 ) * ( 1  4  6  4  1 )
// ( 4 16 24 16  4 )     ( 4 )
// ( 1  4  6  4  1 )     ( 1 )

bool ImageProcessor::GaussianSmooth5x5(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_2(GaussianSmooth5x5, pInputImage, pOutputImage)
	
	if (!pInputImage->IsCompatible(pOutputImage))
	{
		printf("error: input and output image do not match for ImageProcessor::GaussianSmooth5x5\n");
		return false;
	}

	if (pInputImage->width < 5 || pInputImage->height < 5)
	{
		printf("error: image must be at least of size 5x5 for ImageProcessor::GaussianSmooth5x5\n");
		return false;
	}

	const int width = pInputImage->width;
	const int height = pInputImage->height;

	if (pInputImage->type == CByteImage::eRGB24)
	{
		// slow but better than nothing
		CByteImage inputImageRGB24Split(width, height, CByteImage::eRGB24Split);
		CByteImage outputImageRGB24Split(width, height, CByteImage::eRGB24Split);

		ImageProcessor::ConvertImage(pInputImage, &inputImageRGB24Split);

		GaussianSmooth5x5(&inputImageRGB24Split, &outputImageRGB24Split);

		ConvertImage(&outputImageRGB24Split, pOutputImage);

		return true;
	}
	else if (pInputImage->type == CByteImage::eRGB24Split)
	{
		const int nPixels = width * height;

		CByteImage imageHeaderInputGrayscale(width, height, CByteImage::eGrayScale, true);
		CByteImage imageHeaderOutputGrayscale(width, height, CByteImage::eGrayScale, true);

		// red channel
		imageHeaderInputGrayscale.pixels = pInputImage->pixels;
		imageHeaderOutputGrayscale.pixels = pOutputImage->pixels;
		GaussianSmooth5x5(&imageHeaderInputGrayscale, &imageHeaderOutputGrayscale);

		// green channel
		imageHeaderInputGrayscale.pixels = pInputImage->pixels + nPixels;
		imageHeaderOutputGrayscale.pixels = pOutputImage->pixels + nPixels;
		GaussianSmooth5x5(&imageHeaderInputGrayscale, &imageHeaderOutputGrayscale);

		// blue channel
		imageHeaderInputGrayscale.pixels = pInputImage->pixels + 2 * nPixels;
		imageHeaderOutputGrayscale.pixels = pOutputImage->pixels + 2 * nPixels;
		GaussianSmooth5x5(&imageHeaderInputGrayscale, &imageHeaderOutputGrayscale);

		return true;
	}
		
	// create temp image
	CByteImage *pTempImage = new CByteImage(pInputImage);
	
	const int width2 = width << 1;
	
	const unsigned char *input = pInputImage->pixels;
	unsigned char *temp = pTempImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	int x, y, offset;
	
	// x direction
	for (y = 0, offset = 0; y < height; y++)
	{
		temp[offset] = (11 * input[offset] + (input[offset + 1] << 2) + input[offset + 2] + 8) >> 4;
		temp[offset + 1] = (5 * input[offset] + 6 * input[offset + 1] + (input[offset + 2] << 2) + input[offset + 3] + 8) >> 4;
		offset += 2;
		
		// core loop
		for (x = 4; x < width; x++, offset++)
			temp[offset] = (input[offset - 2] + (input[offset - 1] << 2) + 6 * input[offset] + (input[offset + 1] << 2) + input[offset + 2] + 8) >> 4;
		
		temp[offset] = (input[offset - 2] + (input[offset - 1] << 2) + 6 * input[offset] + 5 * input[offset + 1] + 8) >> 4;
		temp[offset + 1] = (input[offset - 1] + (input[offset] << 2) + 11 * input[offset + 1] + 8) >> 4;
		offset += 2;
	}
	
	// y direction
	for (x = 0, offset = 0; x < width; x++, offset++)
	{
		output[offset] = (11 * temp[offset] + (temp[offset + width] << 2) + temp[offset + width2] + 8) >> 4;
		output[offset + width] = (5 * temp[offset] + 6 * temp[offset + width] + (temp[offset + width2] << 2) + temp[offset + 3 * width] + 8) >> 4;
	}
	
	// core loop
	for (y = 4, offset = width << 1; y < height; y++)
		for (x = 0; x < width; x++, offset++)
			output[offset] = (temp[offset - width2] + (temp[offset - width] << 2) + 6 * temp[offset] + (temp[offset + width] << 2) + temp[offset + width2] + 8) >> 4;
		
	for (x = 0, offset = (height - 2) * width; x < width; x++, offset++)
	{
		output[offset] = (temp[offset - width2] + (temp[offset - width] << 2) + 6 * temp[offset] + 5 * temp[offset + width] + 8) >> 4;
		output[offset + width] = (temp[offset - width] + (temp[offset] << 2) + 11 * temp[offset + width] + 8) >> 4;
	}
	
	// free memory
	delete pTempImage;
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


static bool AverageFilter3x3(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale || pOutputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::AverageFilter\n");
		return false;
	}
	
	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}
	
	ImageProcessor::ZeroFrame(pOutputImage);

	const int maxj = pInputImage->width - 1, maxi = pInputImage->height - 1;
	const int width = pInputImage->width;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int i = 1, offset = width + 1; i < maxi; i++, offset += 2)
	{
		for (int j = 1; j < maxj; j++, offset++)
		{
			output[offset] = (
				input[offset - width - 1] + input[offset - width] + input[offset - width + 1] +
				input[offset - 1] + input[offset] + input[offset + 1] + 
				input[offset + width - 1] + input[offset + width] + input[offset + width + 1]
			) / 9;
		}
	}

	if (pSaveOutputImage)
	{
		ImageProcessor::CopyImage(pOutputImage, pSaveOutputImage);
		delete pOutputImage;
	}

	return true;
}

bool ImageProcessor::AverageFilter(const CByteImage *pInputImage, CByteImage *pOutputImage, int nMaskSize)
{
	if (nMaskSize == 3)
	{
		AverageFilter3x3(pInputImage, pOutputImage);
		return false;
	}
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale || pOutputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::AverageFilter\n");
		return false;
	}

	if (nMaskSize < 3 || (nMaskSize + 1) % 2 != 0)
	{
		printf("error: nMaskSize must be 2 * k + 1 with k >= 1 for ImageProcessor::AverageFilter\n");
		return false;
	}
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;
	
	ImageProcessor::Zero(pOutputImage);
	
	CIntImage sat(width, height);
	
	ImageProcessor::CalculateSummedAreaTable(pInputImage, &sat);
	
	const int frame = (nMaskSize - 1) / 2;
	const int area_size = nMaskSize*nMaskSize;
	
	const int maxx = width - 2*frame;
	const int maxy = height - 2*frame;
	const int offset = nMaskSize * width;
	
	const int *input = sat.pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int y = 1; y < maxy; y++)
	{
		const int line_offset = (y + frame)*width + frame;
		for (int x = 1; x < maxx; x++)
		{
			int p1 = (y-1)*width + x - 1;
			int p3 = p1 + offset;
		
			output[line_offset + x] = (input[p3 + nMaskSize] - input[p3] - input[p1 + nMaskSize] + input[p1]) / area_size;
		}
	}

	return true;
}


bool ImageProcessor::GaussianSmooth3x3(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_2(GaussianSmooth3x3, pInputImage, pOutputImage)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type)
	{
		printf("error: input and output image do not match for ImageProcessor::GaussianSmooth3x3\n");
		return false;
	}

	if (pInputImage->width < 3 || pInputImage->height < 3)
	{
		printf("error: image must be at least of size 3x3 for ImageProcessor::GaussianSmooth3x3\n");
		return false;
	}
	
	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}

	const int maxj = pInputImage->width - 1, maxi = pInputImage->height - 1;
	const unsigned char *p = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	if (pInputImage->type == CByteImage::eGrayScale)
	{
		const int width = pInputImage->width;

		for (int i = 1, offset = width + 1; i < maxi; i++, offset += 2)
		{
			for (int j = 1; j < maxj; j++, offset++)
			{
				output[offset] = (
					p[offset - width - 1] + (p[offset - width] << 1) + p[offset - width + 1] +
					(p[offset - 1] << 1) + (p[offset] << 2) + (p[offset + 1] << 1) + 
					p[offset + width - 1] + (p[offset + width] << 1) + p[offset + width + 1] + 8
				) >> 4;
			}
		}
	}
	else if (pInputImage->type == CByteImage::eRGB24)
	{
		const int width = 3 * pInputImage->width;
		int offset = width + 3;

		for (int i = 1; i < maxi; i++)
		{
			for (int j = 1; j < maxj; j++)
			{
				output[offset] = (
					p[offset - width - 3] + (p[offset - width] << 1) + p[offset - width + 3] +
					(p[offset - 3] << 1) + (p[offset] << 2) + (p[offset + 3] << 1) + 
					p[offset + width - 3] + (p[offset + width] << 1) + p[offset + width + 3] + 8
				) >> 4;

				offset++;

				output[offset] = (
					p[offset - width - 3] + (p[offset - width] << 1) + p[offset - width + 3] +
					(p[offset - 3] << 1) + (p[offset] << 2) + (p[offset + 3] << 1) + 
					p[offset + width - 3] + (p[offset + width] << 1) + p[offset + width + 3] + 8
				) >> 4;

				offset++;

				output[offset] = (
					p[offset - width - 3] + (p[offset - width] << 1) + p[offset - width + 3] +
					(p[offset - 3] << 1) + (p[offset] << 2) + (p[offset + 3] << 1) + 
					p[offset + width - 3] + (p[offset + width] << 1) + p[offset + width + 3] + 8
				) >> 4;

				offset++;
			}

			offset += 6;
		}
	}
	
	// copy the border pixels from the original, since the border is not calculated
	if (pSaveOutputImage)
		CopyFrame(pSaveOutputImage, pOutputImage);
	else
		CopyFrame(pInputImage, pOutputImage);

	if (pSaveOutputImage)
	{
		CopyImage(pOutputImage, pSaveOutputImage);
		delete pOutputImage;
	}
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


bool ImageProcessor::SobelX(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bAbsoluteValue)
{
	CShortImage image(pInputImage->width, pInputImage->height);
	
	if (!SobelX(pInputImage, &image, bAbsoluteValue))
		return false;

	return ConvertImage(&image, pOutputImage);
}

bool ImageProcessor::SobelY(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bAbsoluteValue)
{
	CShortImage image(pInputImage->width, pInputImage->height);
	
	if (!SobelY(pInputImage, &image, bAbsoluteValue))
		return false;

	return ConvertImage(&image, pOutputImage);
}

bool ImageProcessor::SobelY(const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue)
{
	OPTIMIZED_FUNCTION_HEADER_3(SobelY, pInputImage, pOutputImage, bAbsoluteValue)

	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::SobelY\n");
		return false;
	}
	
	ZeroFrame(pOutputImage);

	const int maxj = pInputImage->width - 1, maxi = pInputImage->height - 1;
	const int width = pInputImage->width;
	const unsigned char *input = pInputImage->pixels;
	short *output = pOutputImage->pixels;

	if (bAbsoluteValue)
	{
		for (int i = 1, offset = width + 1; i < maxi; i++, offset += 2)
		{
			for (int j = 1; j < maxj; j++, offset++)
			{
				output[offset] =
					abs(input[offset + width - 1] + (input[offset + width] << 1) + input[offset + width + 1] -
					input[offset - width - 1] - (input[offset - width] << 1) - input[offset - width + 1]);
			}
		}
	}
	else
	{
		for (int i = 1, offset = width + 1; i < maxi; i++, offset += 2)
		{
			for (int j = 1; j < maxj; j++, offset++)
			{
				output[offset] =
					input[offset + width - 1] + (input[offset + width] << 1) + input[offset + width + 1] -
					input[offset - width - 1] - (input[offset - width] << 1) - input[offset - width + 1];
			}
		}
	}

	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::SobelX(const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue)
{
	OPTIMIZED_FUNCTION_HEADER_3(SobelX, pInputImage, pOutputImage, bAbsoluteValue)

	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::SobelX\n");
		return false;
	}
	
	ZeroFrame(pOutputImage);

	const int maxj = pInputImage->width - 1, maxi = pInputImage->height - 1;
	const int width = pInputImage->width;
	const unsigned char *input = pInputImage->pixels;
	short *output = pOutputImage->pixels;

	if (bAbsoluteValue)
	{
		for (int i = 1, offset = width + 1; i < maxi; i++, offset += 2)
		{
			for (int j = 1; j < maxj; j++, offset++)
			{
				output[offset] =
					abs(input[offset - width + 1] + (input[offset + 1] << 1) + input[offset + width + 1] -
					input[offset - width - 1] - (input[offset - 1] << 1) - input[offset + width - 1]);
			}
		}
	}
	else
	{
		for (int i = 1, offset = width + 1; i < maxi; i++, offset += 2)
		{
			for (int j = 1; j < maxj; j++, offset++)
			{
				output[offset] =
					input[offset - width + 1] + (input[offset + 1] << 1) + input[offset + width + 1] -
					input[offset - width - 1] - (input[offset - 1] << 1) - input[offset + width - 1];
			}
		}
	}

	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


bool ImageProcessor::PrewittX(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bAbsoluteValue)
{
	CShortImage image(pInputImage->width, pInputImage->height);
	
	if (!PrewittX(pInputImage, &image, bAbsoluteValue))
		return false;

	return ConvertImage(&image, pOutputImage);
}

bool ImageProcessor::PrewittY(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bAbsoluteValue)
{
	CShortImage image(pInputImage->width, pInputImage->height);
	
	if (!PrewittY(pInputImage, &image, bAbsoluteValue))
		return false;

	return ConvertImage(&image, pOutputImage);
}

bool ImageProcessor::PrewittY(const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue)
{
	OPTIMIZED_FUNCTION_HEADER_3(PrewittY, pInputImage, pOutputImage, bAbsoluteValue)

	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::PrewittY\n");
		return false;
	}
	
	ZeroFrame(pOutputImage);

	const int maxj = pInputImage->width - 1, maxi = pInputImage->height - 1;
	const int width = pInputImage->width;
	const unsigned char *input = pInputImage->pixels;
	short *output = pOutputImage->pixels;

	if (bAbsoluteValue)
	{
		for (int i = 1, offset = width + 1; i < maxi; i++, offset += 2)
		{
			for (int j = 1; j < maxj; j++, offset++)
			{
				output[offset] =
					abs(input[offset + width - 1] + input[offset + width] + input[offset + width + 1] -
					input[offset - width - 1] - input[offset - width] - input[offset - width + 1]);
			}
		}
	}
	else
	{
		for (int i = 1, offset = width + 1; i < maxi; i++, offset += 2)
		{
			for (int j = 1; j < maxj; j++, offset++)
			{
				output[offset] =
					input[offset + width - 1] + input[offset + width] + input[offset + width + 1] -
					input[offset - width - 1] - input[offset - width] - input[offset - width + 1];
			}
		}
	}

	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::PrewittX(const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue)
{
	OPTIMIZED_FUNCTION_HEADER_3(PrewittX, pInputImage, pOutputImage, bAbsoluteValue)

	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::PrewittX\n");
		return false;
	}
	
	ZeroFrame(pOutputImage);

	const int maxj = pInputImage->width - 1, maxi = pInputImage->height - 1;
	const int width = pInputImage->width;
	const unsigned char *input = pInputImage->pixels;
	short *output = pOutputImage->pixels;

	if (bAbsoluteValue)
	{
		for (int i = 1; i < maxi; ++i)
		{
			int offset = width * i + 1;
			for (int j = 1; j < maxj; ++j, ++offset)
			{
				output[offset] =
					abs(input[offset - width + 1] + input[offset + 1] + input[offset + width + 1] -
					input[offset - width - 1] - input[offset - 1] - input[offset + width - 1]);
			}
		}
	}
	else
	{
		for (int i = 1; i < maxi; ++i)
		{
			int offset = width * i + 1;
			for (int j = 1; j < maxj; ++j, ++offset)
			{
				output[offset] =
					input[offset - width + 1] + input[offset + 1] + input[offset + width + 1] -
					input[offset - width - 1] - input[offset - 1] - input[offset + width - 1];
			}
		}
	}

	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


bool ImageProcessor::Laplace1(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bAbsoluteValue)
{
	CShortImage image(pInputImage->width, pInputImage->height);
	
	if (!Laplace1(pInputImage, &image, bAbsoluteValue))
		return false;

	ConvertImage(&image, pOutputImage);

	return true;
}

bool ImageProcessor::Laplace2(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bAbsoluteValue)
{
	CShortImage image(pInputImage->width, pInputImage->height);
	
	if (!Laplace2(pInputImage, &image, bAbsoluteValue))
		return false;

	ConvertImage(&image, pOutputImage);

	return true;
}

bool ImageProcessor::Laplace1(const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue)
{
	OPTIMIZED_FUNCTION_HEADER_3(Laplace1, pInputImage, pOutputImage, bAbsoluteValue)
		
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::Laplace1\n");
		return false;
	}
	
	ZeroFrame(pOutputImage);

	const int maxj = pInputImage->width - 1, maxi = pInputImage->height - 1;
	const int width = pInputImage->width;
	const unsigned char *input = pInputImage->pixels;
	short *output = pOutputImage->pixels;

	if (bAbsoluteValue)
	{
		for (int i = 1, offset = width + 1; i < maxi; i++, offset += 2)
		{
			for (int j = 1; j < maxj; j++, offset++)
			{
				output[offset] =
					abs(input[offset - width] +
					input[offset - 1] - (input[offset] << 2) + input[offset + 1] + 
					input[offset + width]);
			}
		}
	}
	else
	{
		for (int i = 1, offset = width + 1; i < maxi; i++, offset += 2)
		{
			for (int j = 1; j < maxj; j++, offset++)
			{
				output[offset] =
					input[offset - width] +
					input[offset - 1] - (input[offset] << 2) + input[offset + 1] + 
					input[offset + width];
			}
		}
	}
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::Laplace2(const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue)
{
	OPTIMIZED_FUNCTION_HEADER_3(Laplace2, pInputImage, pOutputImage, bAbsoluteValue)

	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::Laplace2\n");
		return false;
	}
	
	ZeroFrame(pOutputImage);

	const int maxj = pInputImage->width - 1, maxi = pInputImage->height - 1;
	const int width = pInputImage->width;
	const unsigned char *input = pInputImage->pixels;
	short *output = pOutputImage->pixels;

	if (bAbsoluteValue)
	{
		for (int i = 1, offset = width + 1; i < maxi; i++, offset += 2)
		{
			for (int j = 1; j < maxj; j++, offset++)
			{
				output[offset] =
					abs(input[offset - width - 1] + input[offset - width] + input[offset - width + 1] +
					input[offset - 1] - (input[offset] << 3) + input[offset + 1] + 
					input[offset + width - 1] + input[offset + width] + input[offset + width + 1]);
			}
		}
	}
	else
	{
		for (int i = 1, offset = width + 1; i < maxi; i++, offset += 2)
		{
			for (int j = 1; j < maxj; j++, offset++)
			{
				output[offset] =
					input[offset - width - 1] + input[offset - width] + input[offset - width + 1] +
					input[offset - 1] - (input[offset] << 3) + input[offset + 1] + 
					input[offset + width - 1] + input[offset + width] + input[offset + width + 1];
			}
		}
	}

	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


bool ImageProcessor::CalculateGradientImagePrewitt(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_2(CalculateGradientImagePrewitt, pInputImage, pOutputImage)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::CalculateGradientImagePrewitt\n");
		return false;
	}
	
	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}
	
	ZeroFrame(pOutputImage);

	const int maxu = pInputImage->width - 1, maxv = pInputImage->height - 1;
	const int width = pInputImage->width;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	for (int v = 1, offset = width + 1; v < maxv; v++, offset += 2)
	{
		for (int u = 1; u < maxu; u++, offset++)
		{
			const int value_x = 
				abs(input[offset + width - 1] + input[offset + width] + input[offset + width + 1] -
				input[offset - width - 1] - input[offset - width] - input[offset - width + 1]);
				
			const int value_y = 
				abs(input[offset - width + 1] + input[offset + 1] + input[offset + width + 1] -
				input[offset - width - 1] - input[offset - 1] - input[offset + width - 1]);
			
			const int value = value_x > value_y ? value_x : value_y;

			output[offset] = value > 255 ? 255 : value;
		}
	}
	
	if (pSaveOutputImage)
	{
		CopyImage(pOutputImage, pSaveOutputImage);
		delete pOutputImage;
	}
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::CalculateGradientImageSobel(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_2(CalculateGradientImageSobel, pInputImage, pOutputImage)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::CalculateGradientImageSobel\n");
		return false;
	}
	
	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}
	
	ZeroFrame(pOutputImage);

	const int maxu = pInputImage->width - 1, maxv = pInputImage->height - 1;
	const int width = pInputImage->width;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	for (int v = 1, offset = width + 1; v < maxv; v++, offset += 2)
	{
		for (int u = 1; u < maxu; u++, offset++)
		{
			const int value_x = 
				abs(input[offset + width - 1] + (input[offset + width] << 1) + input[offset + width + 1] -
				input[offset - width - 1] - (input[offset - width] << 1) - input[offset - width + 1]);
				
			const int value_y = 
				abs(input[offset - width + 1] + (input[offset + 1] << 1) + input[offset + width + 1] -
				input[offset - width - 1] - (input[offset - 1] << 1) - input[offset + width - 1]);
			
			const int value = value_x > value_y ? value_x : value_y;
			
			output[offset] = value > 255 ? 255 : value;
		}
	}
	
	if (pSaveOutputImage)
	{
		CopyImage(pOutputImage, pSaveOutputImage);
		delete pOutputImage;
	}
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::CalculateGradientImage(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height)
	{
		printf("error: input and output image do not match for ImageProcessor::CalculateGradientImage\n");
		return false;
	}
	
	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}
	
	ZeroFrame(pOutputImage);

	const int maxu = pInputImage->width - 1, maxv = pInputImage->height - 1;
	const int width = pInputImage->width;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	if (pInputImage->type == CByteImage::eGrayScale)
	{
		for (int v = 1, offset = width + 1; v < maxv; v++, offset += 2)
		{
			for (int u = 1; u < maxu; u++, offset++)
			{
				const int value_x = abs(input[offset + width] - input[offset - width]);
				const int value_y = abs(input[offset + 1] - input[offset - 1]);
				const int value = value_x > value_y ? value_x : value_y;
				output[offset] = value > 255 ? 255 : value;
			}
		}
	}
	else if (pInputImage->type == CByteImage::eRGB24)
	{
		const unsigned char *input_r = pInputImage->pixels;
		const unsigned char *input_g = pInputImage->pixels + 1;
		const unsigned char *input_b = pInputImage->pixels + 2;
		const int width3 = 3 * width;
	
		for (int v = 1, offset = width3 + 3, output_offset = 0; v < maxv; v++, offset += 6, output_offset += 2)
		{
			for (int u = 1; u < maxu; u++, offset += 3, output_offset++)
			{
				const int r1 = abs(input_r[offset + 3] - input_r[offset - 3]);
				const int r2 = abs(input_r[offset + width3] - input_r[offset - width3]);
				const int g1 = abs(input_g[offset + 3] - input_g[offset - 3]);
				const int g2 = abs(input_g[offset + width3] - input_g[offset - width3]);
				const int b1 = abs(input_b[offset + 3] - input_b[offset - 3]);
				const int b2 = abs(input_b[offset + width3] - input_b[offset - width3]);
				const int r = MY_MAX(r1, r2);
				const int g = MY_MAX(g1, g2);
				const int b = MY_MAX(b1, b2);
				const int value = r > g ? (r > b ? r : b) : (g > b ? g : b);
				output[output_offset] = value > 255 ? 255 : value;
			}
		}
	}
	
	if (pSaveOutputImage)
	{
		CopyImage(pOutputImage, pSaveOutputImage);
		delete pOutputImage;
	}

	return true;
}

bool ImageProcessor::CalculateGradientImageBinary(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale || pOutputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::CalculateGradientImageBinary\n");
		return false;
	}
		
	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}
	
	ZeroFrame(pOutputImage);

	const int maxj = pInputImage->width - 1, maxi = pInputImage->height - 1;
	const int width = pInputImage->width;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int i = 0, offset = 0; i < maxi; i++, offset++)
	{
		for (int j = 0; j < maxj; j++, offset++)
		{
			output[offset] = (input[offset + 1] ^ input[offset]) | (input[offset] ^ input[offset + width]);
		}
	}
	
	if (pSaveOutputImage)
	{
		CopyImage(pOutputImage, pSaveOutputImage);
		delete pOutputImage;
	}

	return true;
}


static bool Dilate3x3(const CByteImage *pInputImage, CByteImage *pOutputImage, const MyRegion *pROI)
{
	OPTIMIZED_FUNCTION_HEADER_2_ROI(Dilate3x3, pInputImage, pOutputImage, pROI)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for Dilate3x3\n");
		return false;
	}
		
	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}

	ImageProcessor::Zero(pOutputImage, pROI);

	const int width = pInputImage->width;
	const int height = pInputImage->height;
	
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	int min_u, max_u, min_v, max_v;

	if (pROI)
	{
		min_u = pROI->min_x + 1;
		max_u = pROI->max_x - 1;
		min_v = pROI->min_y + 1;
		max_v = pROI->max_y - 1;

		if (min_u < 1) min_u = 1;
		if (min_u > width - 2) min_u = width - 2;
		if (max_u < 1) max_u = 1;
		if (max_u > width - 2) max_u = width - 2;
		if (min_v < 1) min_v = 1;
		if (min_v > height - 2) min_v = height - 2;
		if (max_v < 1) max_v = 1;
		if (max_v > height - 2) max_v = height - 2;
	}
	else
	{
		min_u = 1;
		max_u = width - 2;
		min_v = 1;
		max_v = height - 2;
	}

	const int diff = width - (max_u - min_u + 1);
	
	bool *bLastY = new bool[width];
	memset(bLastY, false, width * sizeof(bool));

	for (int v = min_v, offset = min_v * width + min_u; v <= max_v; v++, offset += diff)
	{
		bool bLastX = false;
		
		for (int u = min_u; u <= max_u; u++, offset++)
		{
			if (input[offset])
			{
				if (bLastX)
				{
					if (bLastY[u])
					{
						output[offset + width + 1] = 255;
					}
					else
					{
						output[offset - width + 1] = 255;
						output[offset + 1] = 255;
						output[offset + width + 1] = 255;
						
						bLastY[u] = true;
					}
				}
				else
				{
					if (bLastY[u])
					{
						output[offset + width - 1] = 255;
						output[offset + width] = 255;
						output[offset + width + 1] = 255;
					}
					else
					{
						output[offset - width - 1] = 255;
						output[offset - width] = 255;
						output[offset - width + 1] = 255;
						output[offset - 1] = 255;
						output[offset] = 255;
						output[offset + 1] = 255;
						output[offset + width - 1] = 255;
						output[offset + width] = 255;
						output[offset + width + 1] = 255;
						
						bLastY[u] = true;
					}
					
					bLastX = true;
				}
			}
			else
			{
				bLastX = false;
				bLastY[u] = false;
			}
		}
	}
	
	delete [] bLastY;

	if (pSaveOutputImage)
	{
		ImageProcessor::CopyImage(pOutputImage, pSaveOutputImage, pROI, true);
		delete pOutputImage;
	}
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

static bool Erode3x3(const CByteImage *pInputImage, CByteImage *pOutputImage, const MyRegion *pROI)
{
	OPTIMIZED_FUNCTION_HEADER_2_ROI(Erode3x3, pInputImage, pOutputImage, pROI)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for Erode3x3\n");
		return false;
	}
		
	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}

	ImageProcessor::Zero(pOutputImage, pROI);

	const int width = pInputImage->width;
	const int height = pInputImage->height;
	
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	int min_u, max_u, min_v, max_v;

	if (pROI)
	{
		min_u = pROI->min_x + 1;
		max_u = pROI->max_x - 1;
		min_v = pROI->min_y + 1;
		max_v = pROI->max_y - 1;

		if (min_u < 1) min_u = 1;
		if (min_u > width - 2) min_u = width - 2;
		if (max_u < 1) max_u = 1;
		if (max_u > width - 2) max_u = width - 2;
		if (min_v < 1) min_v = 1;
		if (min_v > height - 2) min_v = height - 2;
		if (max_v < 1) max_v = 1;
		if (max_v > height - 2) max_v = height - 2;
	}
	else
	{
		min_u = 1;
		max_u = width - 2;
		min_v = 1;
		max_v = height - 2;
	}

	const int diff = width - (max_u - min_u + 1);

	bool *bLastY = new bool[width];
	memset(bLastY, false, width * sizeof(bool));

	for (int v = min_v, offset = min_v * width + min_u; v <= max_v; v++, offset += diff)
	{
		bool bLastX = false;
		
		for (int u = min_u; u <= max_u; u++, offset++)
		{
			if (bLastX)
			{
				if (bLastY[u])
				{
					if (!input[offset + width + 1])
					{
						bLastX = false;
						bLastY[u] = false;
						continue;
					}
				}
				else
				{
					if (!input[offset - width + 1] || !input[offset + 1] || !input[offset + width + 1])
					{
						bLastX = false;
						continue;
					}
					else
					{
						bLastY[u] = true;
					}
				}
			}
			else
			{
				if (bLastY[u])
				{
					if (!input[offset + width - 1] || !input[offset + width] || !input[offset + width + 1])
					{
						bLastY[u] = false;
						continue;
					}
					else
					{
						bLastX = true;
					}
				}
				else
				{
					if (!input[offset - width - 1] || !input[offset - width] || !input[offset - width + 1] ||
						!input[offset - 1] || !input[offset] || !input[offset + 1] ||
						!input[offset + width - 1] || !input[offset + width] || !input[offset + width + 1])
					{
						continue;
					}
					else
					{
						bLastX = true;
						bLastY[u] = true;
					}
				}
			}
			
			output[offset] = 255;
		}
	}
	
	delete [] bLastY;

	if (pSaveOutputImage)
	{
		ImageProcessor::CopyImage(pOutputImage, pSaveOutputImage, pROI, true);
		delete pOutputImage;
	}
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::Dilate(const CByteImage *pInputImage, CByteImage *pOutputImage, int nMaskSize, const MyRegion *pROI)
{
	if (nMaskSize == 3)
	{
		Dilate3x3(pInputImage, pOutputImage, pROI);
		return false;
	}

	if (nMaskSize < 2)
	{
		printf("error: mask size is too small for ImageProcessor::Dilate\n");
		return false;
	}
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::Dilate\n");
		return false;
	}

	const int k = (nMaskSize - 1) / 2;
		
	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}

	ImageProcessor::Zero(pOutputImage, pROI);

	const int width = pInputImage->width;
	const int height = pInputImage->height;
	
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	int min_u, max_u, min_v, max_v;

	if (pROI)
	{
		min_u = pROI->min_x + 1;
		max_u = pROI->max_x - 1;
		min_v = pROI->min_y + 1;
		max_v = pROI->max_y - 1;

		if (min_u < k) min_u = k;
		if (min_u > width - 1 - k) min_u = width - 1 - k;
		if (max_u < k) max_u = k;
		if (max_u > width - 1 - k) max_u = width - 1 - k;
		if (min_v < k) min_v = k;
		if (min_v > height - 1 - k) min_v = height - 1 - k;
		if (max_v < k) max_v = k;
		if (max_v > height - 1 - k) max_v = height - 1 - k;
	}
	else
	{
		min_u = k;
		max_u = width - 1 - k;
		min_v = k;
		max_v = height - 1 - k;
	}

	const int diff = width - (max_u - min_u + 1);
	
	bool *bLastY = new bool[width];
	memset(bLastY, false, width * sizeof(bool));

	for (int v = min_v, offset = min_v * width + min_u; v <= max_v; v++, offset += diff)
	{
		bool bLastX = false;

		for (int u = min_u; u <= max_u; u++, offset++)
		{
			if (input[offset])
			{
				if (bLastX)
				{
					if (bLastY[u])
					{
						output[offset + k * width + k] = 255;
					}
					else
					{
						for (int i = -k; i <= k; i++)
							output[offset + i * width + k] = 255;
						
						bLastY[u] = true;
					}
				}
				else
				{
					if (bLastY[u])
					{
						for (int i = -k; i <= k; i++)
							output[offset + k * width + i] = 255;
					}
					else
					{
						for (int i = -k; i <= k; i++)
							for (int j = -k; j <= k; j++)
								output[offset + i * width + j] = 255;
						
						bLastY[u] = true;
					}
					
					bLastX = true;
				}
			}
			else
			{
				bLastX = false;
				bLastY[u] = false;
			}
		}
	}
	
	delete [] bLastY;
	
	if (pSaveOutputImage)
	{
		CopyImage(pOutputImage, pSaveOutputImage, pROI, true);
		delete pOutputImage;
	}

	return true;
}

bool ImageProcessor::Erode(const CByteImage *pInputImage, CByteImage *pOutputImage, int nMaskSize, const MyRegion *pROI)
{
	if (nMaskSize == 3)
	{
		Erode3x3(pInputImage, pOutputImage, pROI);
		return false;
	}

	if (nMaskSize < 2)
	{
		printf("error: mask size is too small for ImageProcessor::Erode\n");
		return false;
	}
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::Erode\n");
		return false;
	}

	const int k = (nMaskSize - 1) / 2;
		
	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}

	ImageProcessor::Zero(pOutputImage, pROI);

	const int width = pInputImage->width;
	const int height = pInputImage->height;
	
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	int min_u, max_u, min_v, max_v;

	if (pROI)
	{
		min_u = pROI->min_x + 1;
		max_u = pROI->max_x - 1;
		min_v = pROI->min_y + 1;
		max_v = pROI->max_y - 1;

		if (min_u < k) min_u = k;
		if (min_u > width - 1 - k) min_u = width - 1 - k;
		if (max_u < k) max_u = k;
		if (max_u > width - 1 - k) max_u = width - 1 - k;
		if (min_v < k) min_v = k;
		if (min_v > height - 1 - k) min_v = height - 1 - k;
		if (max_v < k) max_v = k;
		if (max_v > height - 1 - k) max_v = height - 1 - k;
	}
	else
	{
		min_u = k;
		max_u = width - 1 - k;
		min_v = k;
		max_v = height - 1 - k;
	}

	const int diff = width - (max_u - min_u + 1);
	
	bool *bLastY = new bool[width];
	memset(bLastY, false, width * sizeof(bool));

	for (int v = min_v, offset = min_v * width + min_u; v <= max_v; v++, offset += diff)
	{
		bool bLastX = false;

		for (int u = min_u; u <= max_u; u++)
		{
			if (bLastX)
			{
				if (bLastY[u])
				{
					if (!input[offset + k * width + k])
					{
						bLastX = false;
						bLastY[u] = false;
						goto next;
					}
				}
				else
				{
					for (int i = -k; i <= k; i++)
						if (!input[offset + i * width + k])
						{
							bLastX = false;
							goto next;
						}

					bLastY[u] = true;
				}
			}
			else
			{
				if (bLastY[u])
				{
					for (int i = -k; i <= k; i++)
						if (!input[offset + k * width + i])
						{
							bLastY[u] = false;
							goto next;
						}
				}
				else
				{
					for (int i = -k; i <= k; i++)
						for (int j = -k; j <= k; j++)
							if (!input[offset + i * width + j])
								goto next;

					bLastY[u] = true;
				}
				
				bLastX = true;
			}
				
			output[offset] = 255;
			
			next:
			
			offset++;
		}
	}
	
	delete [] bLastY;
	
	if (pSaveOutputImage)
	{
		CopyImage(pOutputImage, pSaveOutputImage, pROI, true);
		delete pOutputImage;
	}

	return true;
}


bool ImageProcessor::HistogramEqualization(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale || pOutputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::HistogramEqualization\n");
		return false;
	}
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	const float fNormalizeConstant = 255.0f / nPixels;
	int histogram[256] = { 0 };
	int i;

	// calculate histogram
	for (i = 0; i < nPixels; i++)
		histogram[input[i]]++;

	// calculate normalized accumulated histogram
	for (i = 1; i < 256; i++)
		histogram[i] += histogram[i - 1];

	for (i = 0; i < 256; i++)
		histogram[i] = int(histogram[i] * fNormalizeConstant + 0.5f);

	// apply normalization
	for (i = 0; i < nPixels; i++)
		output[i] = histogram[input[i]];

	return true;
}


bool ImageProcessor::ApplyAffinePointOperation(const CByteImage *pInputImage, CByteImage *pOutputImage, float a, float b)
{
	OPTIMIZED_FUNCTION_HEADER_4(ApplyAffinePointOperation, pInputImage, pOutputImage, a, b)
	
	if (!pInputImage->IsCompatible(pOutputImage))
	{
		printf("error: input and output image do not match for ImageProcessor::ApplyAffinePointOperation\n");
		return false;
	}
	
	const int nBytes = pInputImage->width * pInputImage->height * pInputImage->bytesPerPixel;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	b += 0.5f;
	
	for (int i = 0; i < nBytes; i++)
	{
		const int v = int(a * input[i] + b);
		output[i] = v < 0 ? 0 : (v > 255 ? 255 : (unsigned char) v);
	}
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


bool ImageProcessor::Spread(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale || pOutputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::Spread\n");
		return false;
	}
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const unsigned char *input = pInputImage->pixels;
	unsigned char min = 255, max = 0;

	// calculate histogram
	for (int i = 0; i < nPixels; i++)
	{
		if (input[i] < min)
			min = input[i];
			
		if (input[i] > max)
			max = input[i];
	}
	
	if (min == max)
	{
		printf("error: input image is homogenous with gray value %i\n", min);
		return false;
	}

	const float a = 255.0f / (max - min);
	const float b = -(min * 255.0f) / (max - min);
	
	ApplyAffinePointOperation(pInputImage, pOutputImage, a, b);

	return true;
}

bool ImageProcessor::HistogramStretching(const CByteImage *pInputImage, CByteImage *pOutputImage, float p_min, float p_max)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale || pOutputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::HistogramStretching\n");
		return false;
	}
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const unsigned char *input = pInputImage->pixels;
	int histogram[256] = { 0 };
	int i;

	// calculate histogram
	for (i = 0; i < nPixels; i++)
		histogram[input[i]]++;

	// calculate accumulated histogram
	for (i = 1; i < 256; i++)
		histogram[i] += histogram[i - 1];

	const float nMinValue = p_min * histogram[255];
	const float nMaxValue = p_max * histogram[255];
	int min = 0, max = 0;
	
	for (i = 0; i < 256; i++)
	{
		if (histogram[i] >= nMinValue)
		{
			min = i;
			break;
		}
	}
	
	for (i = 0; i < 256; i++)
	{
		if (histogram[i] >= nMaxValue)
		{
			max = i;
			break;
		}
	}
	
	if (min == max)
	{
		printf("error: input image is homogenous with gray value %i\n", min);
		return false;
	}
	
	const float a = 255.0f / (max - min);
	const float b = -(min * 255.0f) / (max - min);
	
	ApplyAffinePointOperation(pInputImage, pOutputImage, a, b);

	return true;
}

bool ImageProcessor::Invert(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_2(Invert, pInputImage, pOutputImage)
	
	if (!pInputImage->IsCompatible(pOutputImage))
	{
		printf("error: input and output image do not match for ImageProcessor::Invert\n");
		return false;
	}

	const int nBytes = pInputImage->width * pInputImage->height * pInputImage->bytesPerPixel;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int i = 0; i < nBytes; i++)
		output[i] = 255 - input[i];
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


bool ImageProcessor::ConvertImage(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bFast, const MyRegion *pROI)
{
	OPTIMIZED_FUNCTION_HEADER_3_ROI(ConvertImage, pInputImage, pOutputImage, bFast, pROI)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height)
	{
		printf("error: input and output image do not match for ImageProcessor::ConvertImage\n");
		return false;
	}
	
	if (pInputImage->type == pOutputImage->type)
	{
		CopyImage(pInputImage, pOutputImage, pROI, true);
		return true;
	}
	
	const int nPixels = pInputImage->width * pInputImage->height;

	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	if (pROI)
	{
		const int width = pInputImage->width;
		const int height = pInputImage->height;

		int min_x = pROI->min_x;
		int max_x = pROI->max_x;
		int min_y = pROI->min_y;
		int max_y = pROI->max_y;

		if (min_x < 0) min_x = 0;
		if (min_x > width - 1) min_x = width - 1;
		if (max_x < 0) max_x = 0;
		if (max_x > width - 1) max_x = width - 1;
		if (min_y < 0) min_y = 0;
		if (min_y > height - 1) min_y = height - 1;
		if (max_y < 0) max_y = 0;
		if (max_y > height - 1) max_y = height - 1;

		const int diff = width - (max_x - min_x + 1);
		const int diff_rgb = 3 * diff;

		if (pInputImage->type == CByteImage::eRGB24)
		{
			if (pOutputImage->type == CByteImage::eGrayScale)
			{
				if (bFast)
				{
					for (int y = min_y, offset_output = width * min_y + min_x, offset_input = 3 * offset_output; y <= max_y; y++, offset_output += diff, offset_input += diff_rgb)
						for (int x = min_x; x <= max_x; x++, offset_output++, offset_input += 3)
							output[offset_output] = (input[offset_input] + (input[offset_input + 1] << 1) + input[offset_input + 2] + 2) >> 2;
				}
				else
				{
					for (int y = min_y, offset_output = width * min_y + min_x, offset_input = 3 * offset_output; y <= max_y; y++, offset_output += diff, offset_input += diff_rgb)
						for (int x = min_x; x <= max_x; x++, offset_output++, offset_input += 3)
							output[offset_output] = (9797 * input[offset_input] + 19235 * input[offset_input + 1] + 3736 * input[offset_input + 2] + 16384) >> 15;
				}
			}
			else if (pOutputImage->type == CByteImage::eRGB24Split)
			{
				unsigned char *pHelperR = output;
				unsigned char *pHelperG = pHelperR + nPixels;
				unsigned char *pHelperB = pHelperG + nPixels;
				
				for (int y = min_y, offset_output = width * min_y + min_x, offset_input = 3 * offset_output; y <= max_y; y++, offset_output += diff, offset_input += diff_rgb)
					for (int x = min_x; x <= max_x; x++, offset_output++, offset_input += 3)
					{
						pHelperR[offset_output] = input[offset_input];
						pHelperG[offset_output] = input[offset_input + 1];
						pHelperB[offset_output] = input[offset_input + 2];
					}
			}
		}
		if (pInputImage->type == CByteImage::eRGB24Split)
		{
			const unsigned char *pHelperR = input;
			const unsigned char *pHelperG = pHelperR + nPixels;
			const unsigned char *pHelperB = pHelperG + nPixels;
			
			if (pOutputImage->type == CByteImage::eGrayScale)
			{
				if (bFast)
				{
					for (int y = min_y, offset = width * min_y + min_x; y <= max_y; y++, offset += diff)
						for (int x = min_x; x <= max_x; x++, offset++)
							output[offset] = (pHelperR[offset] + (pHelperG[offset] << 1) + pHelperB[offset] + 2) >> 2;
				}
				else
				{
					for (int y = min_y, offset = width * min_y + min_x; y <= max_y; y++, offset += diff)
						for (int x = min_x; x <= max_x; x++, offset++)
							output[offset] = (9797 * pHelperR[offset] + 19235 * pHelperG[offset] + 3736 * pHelperB[offset] + 16384) >> 15;
				}
			}
			else if (pOutputImage->type == CByteImage::eRGB24)
			{
				for (int y = min_y, input_offset = width * min_y + min_x, output_offset = 3 * input_offset; y <= max_y; y++, input_offset += diff, output_offset += diff_rgb)
					for (int x = min_x; x <= max_x; x++, input_offset++, output_offset += 3)
					{
						output[output_offset] = pHelperR[input_offset];
						output[output_offset + 1] = pHelperG[input_offset];
						output[output_offset + 2] = pHelperB[input_offset];
					}
			}
		}
		else if (pInputImage->type == CByteImage::eGrayScale)
		{
			if (pOutputImage->type == CByteImage::eRGB24)
			{
				for (int y = min_y, offset_input = width * min_y + min_x, offset_output = 3 * offset_input; y <= max_y; y++, offset_output += diff_rgb, offset_input += diff)
					for (int x = min_x; x <= max_x; x++, offset_output += 3, offset_input++)
					{
						output[offset_output + 2] = input[offset_input];
						output[offset_output + 1] = input[offset_input];
						output[offset_output] = input[offset_input];
					}
			}
			else if (pOutputImage->type == CByteImage::eRGB24Split)
			{
				unsigned char *pHelperR = output;
				unsigned char *pHelperG = pHelperR + nPixels;
				unsigned char *pHelperB = pHelperG + nPixels;
				
				for (int y = min_y, offset = width * min_y + min_x; y <= max_y; y++, offset += diff)
					for (int x = min_x; x <= max_x; x++, offset++)
						pHelperR[offset] = pHelperG[offset] = pHelperB[offset] = input[offset];
			}
		}
	}
	else
	{
		if (pInputImage->type == CByteImage::eRGB24)
		{
			if (pOutputImage->type == CByteImage::eGrayScale)
			{
				if (bFast)
				{
					for (int offset = 0, i = 0; i < nPixels; i++, offset += 3)
						output[i] = (input[offset] + (input[offset + 1] << 1) + input[offset + 2] + 2) >> 2;
				}
				else
				{
					for (int offset = 0, i = 0; i < nPixels; i++, offset += 3)
						output[i] = (9797 * input[offset] + 19235 * input[offset + 1] + 3736 * input[offset + 2] + 16384) >> 15;
				}
			}
			else if (pOutputImage->type == CByteImage::eRGB24Split)
			{
				unsigned char *pHelperR = output;
				unsigned char *pHelperG = pHelperR + nPixels;
				unsigned char *pHelperB = pHelperG + nPixels;
				
				for (int offset = 0, i = 0; i < nPixels; i++, offset += 3)
				{
					pHelperR[i] = input[offset];
					pHelperG[i] = input[offset + 1];
					pHelperB[i] = input[offset + 2];
				}
			}
		}
		else if (pInputImage->type == CByteImage::eRGB24Split)
		{
			const unsigned char *pHelperR = input;
			const unsigned char *pHelperG = pHelperR + nPixels;
			const unsigned char *pHelperB = pHelperG + nPixels;
			
			if (pOutputImage->type == CByteImage::eGrayScale)
			{
				if (bFast)
				{
					for (int i = 0; i < nPixels; i++)
						output[i] = (pHelperR[i] + (pHelperG[i] << 1) + pHelperB[i] + 2) >> 2;
				}
				else
				{
					for (int i = 0; i < nPixels; i++)
						output[i] = (9797 * pHelperR[i] + 19235 * pHelperG[i] + 3736 * pHelperB[i] + 16384) >> 15;
				}
			}
			else if (pOutputImage->type == CByteImage::eRGB24)
			{
				for (int offset = 0, i = 0; i < nPixels; i++, offset += 3)
				{
					output[offset] = pHelperR[i];
					output[offset + 1] = pHelperG[i];
					output[offset + 2] = pHelperB[i];
				}
			}
		}
		else if (pInputImage->type == CByteImage::eGrayScale)
		{
			if (pOutputImage->type == CByteImage::eRGB24)
			{
				for (int offset = 0, i = 0; i < nPixels; i++, offset += 3)
				{
					output[offset + 2] = input[i];
					output[offset + 1] = input[i];
					output[offset] = input[i];
				}
			}
			else if (pOutputImage->type == CByteImage::eRGB24Split)
			{
				unsigned char *pHelperR = output;
				unsigned char *pHelperG = pHelperR + nPixels;
				unsigned char *pHelperB = pHelperG + nPixels;
				
				for (int i = 0; i < nPixels; i++)
					pHelperR[i] = pHelperG[i] = pHelperB[i] = input[i];
			}
		}
	}
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


bool ImageProcessor::ConvertImage(const CFloatImage *pInputImage, CByteImage *pOutputImage, bool equalize)
{
    if(pInputImage->numberOfChannels == 1 && pOutputImage->type != CByteImage::eGrayScale)
    {
        printf("error: ImageProcessor::ConvertImage cannot convert a single channel float image into mulitple channel byte image\n");
        return false;
    }

    if(pInputImage->numberOfChannels == 3 && pOutputImage->type == CByteImage::eGrayScale)
    {
        printf("error: ImageProcessor::ConvertImage cannot convert a 3 channel float image into single channel byte image\n");
        return false;
    }

    if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height)
    {
        printf("error: input and output image dimensions do not match for ImageProcessor::ConvertImage\n");
        return false;
    }

    const int nPixels = pInputImage->width * pInputImage->height;
    const float* input = pInputImage->pixels;
    unsigned char* output = pOutputImage->pixels;
    float* min = new float[pInputImage->numberOfChannels];
    float* max = new float[pInputImage->numberOfChannels];
    float* factor = new float[pInputImage->numberOfChannels];
    int i, j;
    int pixelChannelIndex;
    for (i = 0; i < pInputImage->numberOfChannels; i++)
    {
        min[i] = FLT_MAX;
        max[i] = -FLT_MAX;
    }

    for (i = 0; i < nPixels; i++)
    {
        for (j = 0; j < pInputImage->numberOfChannels; j++)
        {
            pixelChannelIndex = i * pInputImage->numberOfChannels + j;

            if (input[pixelChannelIndex] > max[j])
            {
                max[j] = input[pixelChannelIndex];
            }

            if (input[pixelChannelIndex] < min[j])
            {
                min[j] = input[pixelChannelIndex];
            }
        }
    }

    if (equalize)
    {

        for (i = 0; i < pInputImage->numberOfChannels; i++)
        {
            if((max[i] - min[i]) != 0.0f)
            {
                factor[i] = 255.0f / max[i] - min[i];
            }
            else
            {
                factor[i] = 0.0f;
            }
        }

        for (i = 0; i < nPixels; i++)
        {
            for (j = 0; j < pInputImage->numberOfChannels; j++)
            {
                pixelChannelIndex = i * pInputImage->numberOfChannels + j;

                output[pixelChannelIndex] = (unsigned char) ((input[pixelChannelIndex] - min[j]) * factor[j]);
            }
        }
    }
    else
    {
        // verify input value ranges
        for (i = 0; i < pInputImage->numberOfChannels; i++)
        {
            if(min[i] < 0.0f || max[i] > 255.0f)
            {
                printf("error: float input image values exceed value range of output byte image in ImageProcessor::ConvertImage. Equalization required!\n");
                return false;
            }
        }

        for (i = 0; i < nPixels; i++)
        {
            for (j = 0; j < pInputImage->numberOfChannels; j++)
            {
                pixelChannelIndex = i * pInputImage->numberOfChannels + j;

                output[pixelChannelIndex] = (unsigned char) input[pixelChannelIndex];
            }
        }
    }

	return true;
}


bool ImageProcessor::ConvertImage(const CByteImage *pInputImage, CFloatImage *pOutputImage)
{
    if(pOutputImage->numberOfChannels == 1 && pInputImage->type != CByteImage::eGrayScale)
    {
        printf("error: ImageProcessor::ConvertImage cannot convert a single channel byte image into single channel byte image\n");
        return false;
    }

    if(pOutputImage->numberOfChannels == 3 && pInputImage->type == CByteImage::eGrayScale)
    {
        printf("error: ImageProcessor::ConvertImage cannot convert a single channel byte image into 3-channel float image\n");
        return false;
    }

    if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height)
    {
        printf("error: input and output image dimensions do not match for ImageProcessor::ConvertImage\n");
        return false;
    }

    const int nPixels = pInputImage->width * pInputImage->height;
    const unsigned char *input = pInputImage->pixels;
    float *output = pOutputImage->pixels;
    int i, j;
    int pixelChannelIndex;

    for (i = 0; i < nPixels; i++)
    {
        for (j = 0; j < pInputImage->bytesPerPixel; j++)
        {
            pixelChannelIndex = i * pInputImage->bytesPerPixel + j;

            output[pixelChannelIndex] = (float) input[pixelChannelIndex];
        }
    }

	return true;
}


bool ImageProcessor::ConvertImage(const CFloatMatrix *pInputMatrix, CByteImage *pOutputImage)
{
	if (pOutputImage->type != CByteImage::eGrayScale || pInputMatrix->columns != pOutputImage->width || pInputMatrix->rows != pOutputImage->height)
	{
		printf("error: input matrix and output image do not match for ImageProcessor::ConvertImage\n");
		return false;
	}
		
	const int nPixels = pInputMatrix->columns * pInputMatrix->rows;
	const float *input = pInputMatrix->data;
	unsigned char *output = pOutputImage->pixels;
	float min = FLT_MAX, max = -FLT_MAX;
	int i;
	
	for (i = 0; i < nPixels; i++)
	{
		if (input[i] > max)
			max = input[i];
			
		if (input[i] < min)
			min = input[i];
	}
	
	const float divider = max - min;
	
	if (divider == 0.0f)
	{
		Zero(pOutputImage);
	}
	else
	{
		const float factor = 255.0f / divider;
		
		for (i = 0; i < nPixels; i++)
			output[i] = (unsigned char) ((input[i] - min) * factor);
	}

	return true;
}

bool ImageProcessor::ConvertImage(const CByteImage *pInputImage, CFloatMatrix *pOutputMatrix)
{
	if (pInputImage->type != CByteImage::eGrayScale || pOutputMatrix->columns != pInputImage->width || pOutputMatrix->rows != pInputImage->height)
	{
		printf("error: input image and output matrix do not match for ImageProcessor::ConvertImage\n");
		return false;
	}
		
	const int nPixels = pOutputMatrix->columns * pOutputMatrix->rows;
	const unsigned char *input = pInputImage->pixels;
	float *output = pOutputMatrix->data;
	
	for (int i = 0; i < nPixels; i++)
		output[i] = input[i];

	return true;
}

bool ImageProcessor::ConvertImage(const CShortImage *pInputImage, CByteImage *pOutputImage)
{
	if (pOutputImage->type != CByteImage::eGrayScale || pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height)
	{
		printf("error: input matrix and output image do not match for ImageProcessor::ConvertImage\n");
		return false;
	}
		
	const int nPixels = pInputImage->width * pInputImage->height;
	const short *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	short min = SHRT_MAX, max = SHRT_MIN;
	int i;
	
	for (i = 0; i < nPixels; i++)
	{
		if (input[i] > max)
			max = input[i];
		
		if (input[i] < min)
			min = input[i];
	}

	const short divider = max - min;

	if (divider == 0)
	{
		Zero(pOutputImage);
	}
	else
	{
		for (i = 0; i < nPixels; i++)
			output[i] = (unsigned char) ((255 * (input[i] - min)) / divider);
	}

	return true;
}

bool ImageProcessor::ConvertImage(const CByteImage *pInputImage, CShortImage *pOutputImage)
{
	if (pInputImage->type != CByteImage::eGrayScale || pOutputImage->width != pInputImage->width || pOutputImage->height != pInputImage->height)
	{
		printf("error: input image and output matrix do not match for ImageProcessor::ConvertImage\n");
		return false;
	}
		
	const int nPixels = pOutputImage->width * pOutputImage->height;
	const unsigned char *input = pInputImage->pixels;
	short *output = pOutputImage->pixels;
	
	for (int i = 0; i < nPixels; i++)
		output[i] = (short) input[i];

	return true;
}

bool ImageProcessor::ConvertImage(const CByteImage *pInputImage, CIntImage *pOutputImage)
{
	if (pInputImage->type != CByteImage::eGrayScale || pOutputImage->width != pInputImage->width || pOutputImage->height != pInputImage->height)
	{
		printf("error: input image and output matrix do not match for ImageProcessor::ConvertImage\n");
		return false;
	}
	
	const int nPixels = pOutputImage->width * pOutputImage->height;
	const unsigned char *input = pInputImage->pixels;
	int *output = pOutputImage->pixels;
	
	for (int i = 0; i < nPixels; i++)
		output[i] = (int) input[i];

	return true;
}

bool ImageProcessor::ConvertImage(const CIntImage *pInputImage, CByteImage *pOutputImage)
{
	if (pOutputImage->type != CByteImage::eGrayScale || pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height)
	{
		printf("error: input matrix and output image do not match for ImageProcessor::ConvertImage\n");
		return false;
	}
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const int *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	int min = INT_MAX, max = INT_MIN;
	int i;
	
	for (i = 0; i < nPixels; i++)
	{
		if (input[i] > max)
			max = input[i];
		
		if (input[i] < min)
			min = input[i];
	}
	
	const int divider = max - min;
	
	if (divider == 0)
	{
		Zero(pOutputImage);
	}
	else
	{
		for (i = 0; i < nPixels; i++)
			output[i] = (unsigned char) ((255 * (input[i] - min)) / divider);
	}

	return true;
}


bool ImageProcessor::ConvertMatrix(const CFloatMatrix *pInputMatrix, CDoubleMatrix *pOutputMatrix)
{
	if (pInputMatrix->rows != pOutputMatrix->rows || pInputMatrix->columns != pOutputMatrix->columns)
	{
		printf("error: input image and output matrix do not match for ImageProcessor::ConvertMatrix\n");
		return false;
	}
		
	const int nElements = pOutputMatrix->rows * pOutputMatrix->columns;
	const float *input = pInputMatrix->data;
	double *output = pOutputMatrix->data;
	
	for (int i = 0; i < nElements; i++)
		output[i] = (double) input[i];

	return true;
}
	
bool ImageProcessor::ConvertMatrix(const CDoubleMatrix *pInputMatrix, CFloatMatrix *pOutputMatrix)
{
	if (pInputMatrix->rows != pOutputMatrix->rows || pInputMatrix->columns != pOutputMatrix->columns)
	{
		printf("error: input image and output matrix do not match for ImageProcessor::ConvertMatrix\n");
		return false;
	}
		
	const int nElements = pOutputMatrix->rows * pOutputMatrix->columns;
	const double *input = pInputMatrix->data;
	float *output = pOutputMatrix->data;
	
	for (int i = 0; i < nElements; i++)
		output[i] = (float) input[i];

	return true;
}


bool ImageProcessor::CopyImage(const CByteImage *pInputImage, CByteImage *pOutputImage, const MyRegion *pROI, bool bUseSameSize)
{
	if (!pROI || bUseSameSize)
	{
		if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height || pInputImage->type != pOutputImage->type)
		{
			printf("error: input and output image do not match for ImageProcessor::CopyImage\n");
			return false;
		}
	}
	else if (pROI && !bUseSameSize)
	{
		const int w = pROI->max_x - pROI->min_x + 1;
		const int h = pROI->max_y - pROI->min_y + 1;
		
		if (w != pOutputImage->width || pOutputImage->height != h || pInputImage->type != pOutputImage->type)
		{
			printf("error: ROI and output image do not match for ImageProcessor::CopyImage\n");
			return false;
		}
	}

	if (pROI)
	{
		const int width = pInputImage->width;
		const int height = pInputImage->height;

		unsigned char *input = pInputImage->pixels;
		unsigned char *output = pOutputImage->pixels;

		int min_x = pROI->min_x;
		int max_x = pROI->max_x;
		int min_y = pROI->min_y;
		int max_y = pROI->max_y;
		
		if (!bUseSameSize)
		{
			if (min_x < 0 || min_x > width - 1 || max_x < 0 || max_x > width - 1 ||
				min_y < 0 || min_y > height - 1 || max_y < 0 || max_y > height - 1)
			{
				printf("error: ROI is out of image in ImageProcessor::CopyImage\n");
				return false;
			}
		}

		if (min_x < 0) min_x = 0;
		if (min_x > width - 1) min_x = width - 1;
		if (max_x < 0) max_x = 0;
		if (max_x > width - 1) max_x = width - 1;
		if (min_y < 0) min_y = 0;
		if (min_y > height - 1) min_y = height - 1;
		if (max_y < 0) max_y = 0;
		if (max_y > height - 1) max_y = height - 1;

		if (pInputImage->type == CByteImage::eGrayScale)
		{
			const int size = max_x - min_x + 1;
			
			if (bUseSameSize)
			{
				for (int y = min_y, offset = width * min_y + min_x; y <= max_y; y++, offset += width)
					memcpy(output + offset, input + offset, size);
			}
			else
			{
				for (int y = min_y, offset = width * min_y + min_x, output_offset = 0; y <= max_y; y++, offset += width, output_offset += size)
					memcpy(output + output_offset, input + offset, size);
			}
		}
		else if (pInputImage->type == CByteImage::eRGB24)
		{
			const int size = 3 * (max_x - min_x + 1);
			const int width3 = 3 * width;

			if (bUseSameSize)
			{
				for (int y = min_y, offset = 3 * (width * min_y + min_x); y <= max_y; y++, offset += width3)
					memcpy(output + offset, input + offset, size);
			}
			else
			{
				for (int y = min_y, offset = 3 * (width * min_y + min_x), output_offset = 0; y <= max_y; y++, offset += width3, output_offset += size)
					memcpy(output + output_offset, input + offset, size);
			}
		}
		else if (pInputImage->type == CByteImage::eRGB24Split)
		{
			const int size = max_x - min_x + 1;
			
			unsigned char *pHelperR = output;
			unsigned char *pHelperG = pHelperR + width * height;
			unsigned char *pHelperB = pHelperG + width * height;
			
			if (bUseSameSize)
			{
				for (int y = min_y, offset = width * min_y + min_x; y <= max_y; y++, offset += width)
				{
					memcpy(pHelperR + offset, input + offset, size);
					memcpy(pHelperG + offset, input + offset, size);
					memcpy(pHelperB + offset, input + offset, size);
				}
			}
			else
			{
				for (int y = min_y, offset = 3 * (width * min_y + min_x), output_offset = 0; y <= max_y; y++, offset += width, output_offset += size)
				{
					memcpy(pHelperR + output_offset, input + offset, size);
					memcpy(pHelperG + output_offset, input + offset, size);
					memcpy(pHelperB + output_offset, input + offset, size);
				}
			}
		}
	}
	else
	{
		memcpy(pOutputImage->pixels, pInputImage->pixels, pInputImage->width * pInputImage->height * pInputImage->bytesPerPixel);
	}

	return true;
}

bool ImageProcessor::CopyImage(const CShortImage *pInputImage, CShortImage *pOutputImage, const MyRegion *pROI, bool bUseSameSize)
{
	if (!pROI || bUseSameSize)
	{
		if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height)
		{
			printf("error: input and output image do not match for ImageProcessor::CopyImage\n");
			return false;
		}
	}
	else if (pROI && !bUseSameSize)
	{
		const int w = pROI->max_x - pROI->min_x + 1;
		const int h = pROI->max_y - pROI->min_y + 1;
		
		if (w != pOutputImage->width || pOutputImage->height != h)
		{
			printf("error: ROI and output image do not match for ImageProcessor::CopyImage\n");
			return false;
		}
	}

	if (pROI)
	{
		const int width = pInputImage->width;
		const int height = pInputImage->height;

		short *input = pInputImage->pixels;
		short *output = pOutputImage->pixels;

		int min_x = pROI->min_x;
		int max_x = pROI->max_x;
		int min_y = pROI->min_y;
		int max_y = pROI->max_y;
		
		if (!bUseSameSize)
		{
			if (min_x < 0 || min_x > width - 1 || max_x < 0 || max_x > width - 1 ||
				min_y < 0 || min_y > height - 1 || max_y < 0 || max_y > height - 1)
			{
				printf("error: ROI is out of image in ImageProcessor::CopyImage\n");
				return false;
			}
		}

		if (min_x < 0) min_x = 0;
		if (min_x > width - 1) min_x = width - 1;
		if (max_x < 0) max_x = 0;
		if (max_x > width - 1) max_x = width - 1;
		if (min_y < 0) min_y = 0;
		if (min_y > height - 1) min_y = height - 1;
		if (max_y < 0) max_y = 0;
		if (max_y > height - 1) max_y = height - 1;

		if (bUseSameSize)
		{
			const int diff = width - (max_x - min_x + 1);

			for (int y = min_y, offset = width * min_y + min_x; y <= max_y; y++, offset += diff)
				for (int x = min_x; x <= max_x; x++, offset++)
					output[offset] = input[offset];
		}
		else
		{
			const int diff = width - (max_x - min_x + 1);

			for (int y = min_y, offset = width * min_y + min_x, output_offset = 0; y <= max_y; y++, offset += diff)
				for (int x = min_x; x <= max_x; x++, offset++, output_offset++)
					output[output_offset] = input[offset];
		}
	}
	else
	{
		memcpy(pOutputImage->pixels, pInputImage->pixels, pInputImage->width * pInputImage->height * sizeof(short));
	}

	return true;
}


bool ImageProcessor::CopyMatrix(const CFloatMatrix *pInputMatrix, CFloatMatrix *pOutputMatrix)
{
	if (pInputMatrix->rows != pOutputMatrix->rows || pInputMatrix->columns != pOutputMatrix->columns)
	{
		printf("error: input and output matrix do not match for ImageProcessor::CopyMatrix\n");
		return false;
	}

	memcpy(pOutputMatrix->data, pInputMatrix->data, pInputMatrix->rows * pInputMatrix->columns * sizeof(float));

	return true;
}

bool ImageProcessor::CopyMatrix(const CDoubleMatrix *pInputMatrix, CDoubleMatrix *pOutputMatrix)
{
	if (pInputMatrix->rows != pOutputMatrix->rows || pInputMatrix->columns != pOutputMatrix->columns)
	{
		printf("error: input and output matrix do not match for ImageProcessor::CopyMatrix\n");
		return false;
	}

	memcpy(pOutputMatrix->data, pInputMatrix->data, pInputMatrix->rows * pInputMatrix->columns * sizeof(double));

	return true;
}


void ImageProcessor::Zero(CByteImage *pImage, const MyRegion *pROI)
{
	if (pROI)
	{
		const int width = pImage->width;
		const int height = pImage->height;

		unsigned char *pixels = pImage->pixels;

		int min_x = pROI->min_x;
		int max_x = pROI->max_x;
		int min_y = pROI->min_y;
		int max_y = pROI->max_y;

		if (min_x < 0) min_x = 0;
		if (min_x > width - 1) min_x = width - 1;
		if (max_x < 0) max_x = 0;
		if (max_x > width - 1) max_x = width - 1;
		if (min_y < 0) min_y = 0;
		if (min_y > height - 1) min_y = height - 1;
		if (max_y < 0) max_y = 0;
		if (max_y > height - 1) max_y = height - 1;

		if (pImage->type == CByteImage::eGrayScale)
		{
			const int size = max_x - min_x + 1;

			for (int y = min_y, offset = width * min_y + min_x; y <= max_y; y++, offset += width)
				memset(pixels + offset, 0, size);
		}
		else if (pImage->type == CByteImage::eRGB24)
		{
			const int size = 3 * (max_x - min_x + 1);
			const int width3 = 3 * width;

			for (int y = min_y, offset = 3 * (width * min_y + min_x); y <= max_y; y++, offset += width3)
				memset(pixels, 0, size);
		}
		else if (pImage->type == CByteImage::eRGB24Split)
		{
			const int size = max_x - min_x + 1;
			
			unsigned char *pHelperR = pixels;
			unsigned char *pHelperG = pHelperR + width * height;
			unsigned char *pHelperB = pHelperG + width * height;

			for (int y = min_y, offset = width * min_y + min_x; y <= max_y; y++, offset += width)
			{
				memset(pHelperR + offset, 0, size);
				memset(pHelperG + offset, 0, size);
				memset(pHelperB + offset, 0, size);
			}
		}
	}
	else
	{
		memset(pImage->pixels, 0, pImage->width * pImage->height * pImage->bytesPerPixel);
	}
}

void ImageProcessor::Zero(CShortImage *pImage)
{
	memset(pImage->pixels, 0, pImage->width * pImage->height * sizeof(*pImage->pixels));
}

void ImageProcessor::Zero(CIntImage *pImage)
{
	memset(pImage->pixels, 0, pImage->width * pImage->height * sizeof(*pImage->pixels));
}

void ImageProcessor::Zero(CFloatMatrix *pMatrix)
{
	memset(pMatrix->data, 0, pMatrix->columns * pMatrix->rows * sizeof(*pMatrix->data));
}

void ImageProcessor::Zero(CDoubleMatrix *pMatrix)
{
	memset(pMatrix->data, 0, pMatrix->columns * pMatrix->rows * sizeof(*pMatrix->data));
}


void ImageProcessor::ZeroFrame(CByteImage *pImage)
{
	const int width = pImage->width;
	const int height = pImage->height;
	unsigned char *pixels = pImage->pixels;
	
	if (pImage->type == CByteImage::eGrayScale)
	{
		// zero top and bottom row
		memset(pixels, 0, width);
		memset(pixels + width * (height - 1), 0, width);
		
		// zero left and right column
		for (int i = 0, offset = 0; i < height; i++, offset += width)
		{
			pixels[offset] = 0;
			pixels[offset + width - 1] = 0;
		}
	}
	else
	{
		const int width3 = 3 * width;
		
		// zero top and bottom row
		memset(pixels, 0, width3);
		memset(pixels + width3 * (height - 1), 0, width3);
		
		// zero left and right column
		for (int i = 0, offset = 0; i < height; i++, offset += width3)
		{
			pixels[offset] = pixels[offset + 1] = pixels[offset + 2] = 0;
			pixels[offset + width3 - 3] = pixels[offset + width3 - 2] = pixels[offset + width3 - 1] = 0;
		}
	}
}

void ImageProcessor::ZeroFrame(CShortImage *pImage)
{
	const int width = pImage->width;
	const int height = pImage->height;
	short *pixels = pImage->pixels;
	
	// zero top and bottom row
	memset(pixels, 0, width * sizeof(short));
	memset(pixels + width * (height - 1), 0, width * sizeof(short));
	
	// zero left and right column
	for (int i = 0, offset = 0; i < height; i++, offset += width)
	{
		pixels[offset] = 0;
		pixels[offset + width - 1] = 0;
	}
}

void ImageProcessor::ZeroFrame(CIntImage *pImage)
{
	const int width = pImage->width;
	const int height = pImage->height;
	int *pixels = pImage->pixels;
	
	// zero top and bottom row
	memset(pixels, 0, width * sizeof(int));
	memset(pixels + width * (height - 1), 0, width * sizeof(int));
	
	// zero left and right column
	for (int i = 0, offset = 0; i < height; i++, offset += width)
	{
		pixels[offset] = 0;
		pixels[offset + width - 1] = 0;
	}
}

bool ImageProcessor::CopyFrame(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height || pInputImage->type != pOutputImage->type || pInputImage->type == CByteImage::eRGB24Split)
	{
		printf("error: input and output images do not match for ImageProcessor::CopyFrame\n");
		return false;
	}
	
	// copy is not necessary in this case
	if (pInputImage->pixels == pOutputImage->pixels)
		return true;
	
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;
		
	if (pInputImage->type == CByteImage::eGrayScale)
	{
		memcpy(output, input, width);
		memcpy(output + width * (height - 1), input + width * (height - 1), width);
		
		unsigned int offset = width;
		const unsigned int offset2 = width - 1;
		for (int y = 1; y < height-1; y++, offset += width)
		{
			output[offset] = input[offset];
			output[offset + offset2] = input[offset + offset2];
		}
	}
	else if (pInputImage->type == CByteImage::eRGB24)
	{
		memcpy(output, input, 3*width);
		memcpy(output + 3 * width * (height - 1), input + 3 * width * (height - 1), 3 * width);
		
		unsigned int offset = 3 * width;
		const unsigned int offset2 = 3 * (width - 1);
		for (int y = 1; y < height - 1; y++, offset += 3 * width)
		{
			output[offset]   = input[offset];
			output[offset + 1] = input[offset + 1];
			output[offset + 2] = input[offset + 2];
			output[offset + offset2] = input[offset + offset2];
			output[offset + offset2 + 1] = input[offset + offset2 + 1];
			output[offset + offset2 + 2] = input[offset + offset2 + 2];
		}
	}

	return true;
}

bool ImageProcessor::AdaptFrame(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height || pInputImage->type != pOutputImage->type || pInputImage->type == CByteImage::eRGB24Split)
	{
		printf("error: input and output images do not match for ImageProcessor::CopyFrame\n");
		return false;
	}
	
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;
		
	if (pInputImage->type == CByteImage::eGrayScale)
	{
		// adapt the top and bottom row
		memcpy(&output[1], &input[width + 1], width-2);
		memcpy(&output[width*(height-1) + 1], &input[width*(height-2) + 1], width-2);
		
		// adapt the corners
		output[0] = input[width + 1];
		output[width - 1] = input[2*width - 2];
		output[width*(height-1)] = input[width*(height-2) + 1];
		output[width*height - 1] = input[width*height - 2];
		
		// adapt the left and right borders
		unsigned int offset = width;
		const unsigned int offset2 = width-1;
		for (int y = 1; y < height-1; y++, offset += width)
		{
			output[offset] = input[offset+1];
			output[offset + offset2] = input[offset + offset2 - 1];
		}
	}
	else if (pInputImage->type == CByteImage::eRGB24)
	{
		// adapt the top and bottom row
		memcpy(&output[3], &input[3*(width + 1)], 3*(width-2));
		memcpy(&output[3*(width*(height-1) + 1)], &input[3*(width*(height-2) + 1)], 3*(width-2));
		
		// adapt the corners
		output[0] = input[3*(width + 1)];
		output[1] = input[3*(width + 1) + 1];
		output[2] = input[3*(width + 1) + 2];
		output[3*(width - 1)]     = input[3*(2*width - 2)];
		output[3*(width - 1) + 1] = input[3*(2*width - 2) + 1];
		output[3*(width - 1) + 2] = input[3*(2*width - 2) + 2];
		output[3*(width*(height-1))]     = input[3*(width*(height-2) + 1)];
		output[3*(width*(height-1)) + 1] = input[3*(width*(height-2) + 1) + 1];
		output[3*(width*(height-1)) + 2] = input[3*(width*(height-2) + 1) + 2];
		output[3*(width*height - 1)]     = input[3*(width*(height-1) - 2)];
		output[3*(width*height - 1) + 1] = input[3*(width*(height-1) - 2) + 1];
		output[3*(width*height - 1) + 2] = input[3*(width*(height-1) - 2) + 2];
		
		// adapt the left and right borders
		unsigned int offset = 3*width;
		const unsigned int offset2 = 3*(width-1);
		for (int y = 1; y < height-1; y++, offset += 3*width)
		{
			output[offset]     = input[offset + 3];
			output[offset + 1] = input[offset + 3 + 1];
			output[offset + 2] = input[offset + 3 + 2];
			output[offset + offset2]     = input[offset + offset2 - 3];
			output[offset + offset2 + 1] = input[offset + offset2 - 3 + 1];
			output[offset + offset2 + 2] = input[offset + offset2 - 3 + 2];
		}
	}

	return true;
}


bool ImageProcessor::ApplyHomography(const CByteImage *pInputImage, CByteImage *pOutputImage, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, bool bInterpolation)
{
	const Mat3d A = { a1, a2, a3, a4, a5, a6, a7, a8, 1.0f };
	return ApplyHomography(pInputImage, pOutputImage, A, bInterpolation);
}

bool ImageProcessor::ApplyHomography(const CByteImage *pInputImage, CByteImage *pOutputImage, const Mat3d &A, bool bInterpolation)
{
	if (pInputImage->type != pOutputImage->type)
	{
		printf("error: input and output image do not match in ImageProcessor::ApplyHomography\n");
		return false;
	}

	const float a1 = A.r1;
	const float a2 = A.r2;
	const float a3 = A.r3;
	const float a4 = A.r4;
	const float a5 = A.r5;
	const float a6 = A.r6;
	const float a7 = A.r7;
	const float a8 = A.r8;
	const float a9 = A.r9;

	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;
	const int output_width = pOutputImage->width;
	const int output_height = pOutputImage->height;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	const CByteImage::ImageType type = pInputImage->type;
	
	if (bInterpolation)
	{
		if (type == CByteImage::eGrayScale)
		{
			for (int v = 0, offset = 0; v < output_height; v++)
			{
				for (int u = 0; u < output_width; u++, offset++)
				{
					const float u_ = (a1 * u + a2 * v + a3) / (a7 * u + a8 * v + a9);
					const float v_ = (a4 * u + a5 * v + a6) / (a7 * u + a8 * v + a9);

					const int u1 = int(floor(u_));
					const int v1 = int(floor(v_));
					const int u2 = u1 + 1;
					const int v2 = v1 + 1;

					unsigned char f00 = 0, f10 = 0, f01 = 0, f11 = 0;

					const bool v1_ok = v1 >= 0 && v1 < height;
					const bool v2_ok = v2 >= 0 && v2 < height;

					if (u1 >= 0 && u1 < width)
					{
						if (v1_ok)
							f00 = input[v1 * width + u1];

						if (v2_ok)
							f01 = input[v2 * width + u1];
					}

					if (u2 >= 0 && u2 < width)
					{
						if (v1_ok)
							f10 = input[v1 * width + u2];

						if (v2_ok)
							f11 = input[v2 * width + u2];
					}

					const float x = u_ - u1;
					const float y = v_ - v1;

					output[offset] = (unsigned char) (f00 * (1 - x) * (1 - y) + f10 * x * (1 - y) + f01 * (1 - x) * y + f11 * x * y + 0.5f);
				}
			}
		}
		else if (type == CByteImage::eRGB24)
		{
			for (int v = 0, offset = 0; v < height; v++)
			{
				for (int u = 0; u < width; u++, offset += 3)
				{
					const float u_ = (a1 * u + a2 * v + a3) / (a7 * u + a8 * v + a9);
					const float v_ = (a4 * u + a5 * v + a6) / (a7 * u + a8 * v + a9);

					const int u1 = int(floor(u_));
					const int v1 = int(floor(v_));
					const int u2 = u1 + 1;
					const int v2 = v1 + 1;

					unsigned char f00_r = 0, f00_g = 0, f00_b = 0;
					unsigned char f10_r = 0, f10_g = 0, f10_b = 0;
					unsigned char f01_r = 0, f01_g = 0, f01_b = 0;
					unsigned char f11_r = 0, f11_g = 0, f11_b = 0;

					const bool v1_ok = v1 >= 0 && v1 < height;
					const bool v2_ok = v2 >= 0 && v2 < height;

					if (u1 >= 0 && u1 < width)
					{
						if (v1_ok)
						{
							const int x = 3 * (v1 * width + u1);
							f00_r = input[x];
							f00_g = input[x + 1];
							f00_b = input[x + 2];
						}

						if (v2_ok)
						{
							const int x = 3 * (v2 * width + u1);
							f01_r = input[x];
							f01_g = input[x + 1];
							f01_b = input[x + 2];
						}
					}

					if (u2 >= 0 && u2 < width)
					{
						if (v1_ok)
						{
							const int x = 3 * (v1 * width + u2);
							f10_r = input[x];
							f10_g = input[x + 1];
							f10_b = input[x + 2];
						}

						if (v2_ok)
						{
							const int x = 3 * (v2 * width + u2);
							f11_r = input[x];
							f11_g = input[x + 1];
							f11_b = input[x + 2];
						}
					}

					const float x = u_ - u1;
					const float y = v_ - v1;

					output[offset    ] = (unsigned char) (f00_r * (1 - x) * (1 - y) + f10_r * x * (1 - y) + f01_r * (1 - x) * y + f11_r * x * y + 0.5f);
					output[offset + 1] = (unsigned char) (f00_g * (1 - x) * (1 - y) + f10_g * x * (1 - y) + f01_g * (1 - x) * y + f11_g * x * y + 0.5f);
					output[offset + 2] = (unsigned char) (f00_b * (1 - x) * (1 - y) + f10_b * x * (1 - y) + f01_b * (1 - x) * y + f11_b * x * y + 0.5f);
				}
			}
		}
	}
	else
	{
		if (type == CByteImage::eGrayScale)
		{
			for (int v = 0, offset = 0; v < output_height; v++)
			{
				for (int u = 0; u < output_width; u++, offset++)
				{
					const int u_ = (int) ((a1 * u + a2 * v + a3) / (a7 * u + a8 * v + 1.0f) + 0.5f);
					const int v_ = (int) ((a4 * u + a5 * v + a6) / (a7 * u + a8 * v + 1.0f) + 0.5f);
					
					if (u_ < 0 || u_ >= width || v_ < 0 || v_ >= height)
						output[offset] = 0;
					else
						output[offset] = input[v_ * width + u_];
				}
			}
		}
		else if (type == CByteImage::eRGB24)
		{
			for (int v = 0, offset = 0; v < output_height; v++)
			{
				for (int u = 0; u < output_width; u++, offset += 3)
				{
					const int u_ = (int) ((a1 * u + a2 * v + a3) / (a7 * u + a8 * v + 1.0f) + 0.5f);
					const int v_ = (int) ((a4 * u + a5 * v + a6) / (a7 * u + a8 * v + 1.0f) + 0.5f);
					
					if (u_ < 0 || u_ >= width || v_ < 0 || v_ >= height)
						output[offset] =  output[offset + 1] = output[offset + 2] = 0;
					else
					{
						const int offset2 = 3 * (v_ * width + u_);
						output[offset] = input[offset2];
						output[offset + 1] = input[offset2 + 1];
						output[offset + 2] = input[offset2 + 2];
					}
				}
			}
		}
	}

	if (pSaveOutputImage)
	{
		CopyImage(pOutputImage, pSaveOutputImage);
		delete pOutputImage;
	}

	return true;
}

bool ImageProcessor::Resize(const CByteImage *pInputImage, CByteImage *pOutputImage, const MyRegion *pROI, bool bInterpolation)
{
	if (pInputImage->type != pOutputImage->type || pInputImage->type == CByteImage::eRGB24Split)
	{
		printf("error: input and output image do not match in ImageProcessor::Resize\n");
		return false;
	}

	if (pInputImage->width == pOutputImage->width && pInputImage->height == pOutputImage->height)
	{
		CopyImage(pInputImage, pOutputImage);
		return false;
	}

	const CByteImage::ImageType type = pInputImage->type;

	const int output_width = pOutputImage->width;
	const int output_height = pOutputImage->height;
	unsigned char *output = pOutputImage->pixels;

	const int input_width = pInputImage->width;
	const int input_height = pInputImage->height;
	const unsigned char *input = pInputImage->pixels;

	const int start_x = pROI ? pROI->min_x : 0;
	const int start_y = pROI ? pROI->min_y : 0;

	const int width = pROI ? pROI->max_x - pROI->min_x + 1 : input_width;
	const int height = pROI ? pROI->max_y - pROI->min_y + 1 : input_height;

	if (pROI)
	{
		if (pROI->min_x < 0 || pROI->min_y < 0 || pROI->max_x >= input_width || pROI->max_y >= input_height)
		{
			printf("error: provided ROI in ImageProcessor::Resize exceeds input image boundaries\n");
			return false;
		}
	}

	if (width > output_width && width % output_width == 0 &&
		height > output_height && height % output_height == 0)
	{
		if (type == CByteImage::eGrayScale)
		{
			const int a1 = width / output_width;
			const int a4 = height / output_height;
			const int delta = a4 * input_width - width;
			
			for (int v = 0, offset = 0, offset_ = start_y * input_width + start_x; v < output_height; v++, offset_ += delta)
			{
				for (int u = 0; u < output_width; u++, offset++, offset_ += a1)
					output[offset] = input[offset_];
			}
		}
		else if (type == CByteImage::eRGB24)
		{
			const int a1 = 3 * (width / output_width);
			const int a4 = height / output_height;
			const int delta = 3 * (a4 * input_width - width);
			
			for (int v = 0, offset = 0, offset_ = 3 * (start_y * input_width + start_x); v < output_height; v++, offset_ += delta)
			{
				for (int u = 0; u < output_width; u++, offset += 3, offset_ += a1)
				{
					output[offset] = input[offset_];
					output[offset + 1] = input[offset_ + 1];
					output[offset + 2] = input[offset_ + 2];
				}
			}
		}
		
		return true;
	}

	const float a1 = float(width) / output_width;
	const float a4 = float(height) / output_height;

	int *pXOffsets = 0, *pYOffsets = 0;
	int *pXCoordinates = 0, *pYCoordinates = 0;

	const int min_width = MY_MIN(width, output_width);
	const int min_height = MY_MIN(height, output_height);
	int i;

	if (type == CByteImage::eGrayScale)
	{
		pXOffsets = new int[output_width];
		pYOffsets = new int[output_height];
		
		if (bInterpolation)
		{
			pXCoordinates = new int[output_width];
			pYCoordinates = new int[output_height];

			for (i = 0; i < output_width; i++)
			{
				register int x = (i * width) / output_width;

				if (start_x + x < input_width - 1)
				{
					pXOffsets[i] = start_x + x;
					pXCoordinates[i] = int((i * a1 - x) * 1024);
				}
				else
				{
					pXOffsets[i] = input_width - 2;
					pXCoordinates[i] = 1024;
				}
			}

			for (i = 0; i < output_height; i++)
			{
				register int y = (i * height) / output_height;

				if (start_y + y < input_height - 1)
				{
					pYOffsets[i] = input_width * (start_y + y);
					pYCoordinates[i] = int((i * a4 - y) * 1024);
				}
				else
				{
					pYOffsets[i] = input_width * (input_height - 2);
					pYCoordinates[i] = 1024;
				}
			}
		}
		else
		{
			for (i = 0; i < output_width; i++)
				pXOffsets[i] = start_x + (((i * width) << 1) + min_width - 1) / (output_width << 1);

			for (i = 0; i < output_height; i++)
				pYOffsets[i] = input_width * (start_y + (((i * height) << 1) + min_height - 1) / (output_height << 1));
		}
	}
	else
	{
		pXOffsets = new int[3 * output_width];
		pYOffsets = new int[output_height];
		
		int offset = 0;

		if (bInterpolation)
		{
			pXCoordinates = new int[3 * output_width];
			pYCoordinates = new int[output_height];

			for (i = 0; i < output_width; i++, offset += 3)
			{
				register int x = (i * width) / output_width;

				if (start_x + x < input_width - 1)
				{
					pXOffsets[offset] = 3 * (start_x + x);
					pXCoordinates[offset] = int((i * a1 - x) * 1024);
				}
				else
				{
					pXOffsets[offset] = 3 * (input_width - 2);
					pXCoordinates[offset] = 1024;
				}
			}

			for (i = 0; i < output_height; i++)
			{
				register int y = (i * height) / output_height;

				if (start_y + y < input_height - 1)
				{
					pYOffsets[i] = 3 * input_width * (start_y + y);
					pYCoordinates[i] = int((i * a4 - y) * 1024);
				}
				else
				{
					pYOffsets[i] = 3 * input_width * (input_height - 2);
					pYCoordinates[i] = 1024;
				}
			}
		}
		else
		{
			for (i = 0; i < output_width; i++, offset += 3)
				pXOffsets[offset] = 3 * (start_x + (((i * width) << 1) + min_width - 1) / (output_width << 1));

			for (i = 0; i < output_height; i++)
				pYOffsets[i] = 3 * input_width * (start_y + (((i * height) << 1) + min_height - 1) / (output_height << 1));
		}
	}

	if (bInterpolation)
	{
		if (type == CByteImage::eGrayScale)
		{
			const int last_u = MY_MAX(0, output_width - output_width % 4);

			for (int v = 0; v < output_height; v++)
			{
				const unsigned char *input_helper = input + pYOffsets[v];
				const int y = pYCoordinates[v];
				int u;

				for (u = 0; u < last_u; u += 4)
				{
					int offset;
					int x;
					
					offset = pXOffsets[u];
					x = pXCoordinates[u];
					output[u] = (unsigned char) (
						((input_helper[offset] * (1024 - x) * (1024 - y)) +
						(input_helper[offset + 1] * x * (1024 - y)) +
						(input_helper[offset + input_width] * (1024 - x) * y) +
						(input_helper[offset + input_width + 1] * x * y)) >> 20
					);

					offset = pXOffsets[u + 1];
					x = pXCoordinates[u + 1];
					output[u + 1] = (unsigned char) (
						((input_helper[offset] * (1024 - x) * (1024 - y)) +
						(input_helper[offset + 1] * x * (1024 - y)) +
						(input_helper[offset + input_width] * (1024 - x) * y) +
						(input_helper[offset + input_width + 1] * x * y)) >> 20
					);

					offset = pXOffsets[u + 2];
					x = pXCoordinates[u + 2];
					output[u + 2] = (unsigned char) (
						((input_helper[offset] * (1024 - x) * (1024 - y)) +
						(input_helper[offset + 1] * x * (1024 - y)) +
						(input_helper[offset + input_width] * (1024 - x) * y) +
						(input_helper[offset + input_width + 1] * x * y)) >> 20
					);

					offset = pXOffsets[u + 3];
					x = pXCoordinates[u + 3];
					output[u + 3] = (unsigned char) (
						((input_helper[offset] * (1024 - x) * (1024 - y)) +
						(input_helper[offset + 1] * x * (1024 - y)) +
						(input_helper[offset + input_width] * (1024 - x) * y) +
						(input_helper[offset + input_width + 1] * x * y)) >> 20
					);
				}

				for (u = last_u; u < output_width; u++)
				{
					const int offset = pXOffsets[u];
					const int x = pXCoordinates[u];
					output[u] = (unsigned char) (
						((input_helper[offset] * (1024 - x) * (1024 - y)) +
						(input_helper[offset + 1] * x * (1024 - y)) +
						(input_helper[offset + input_width] * (1024 - x) * y) +
						(input_helper[offset + input_width + 1] * x * y)) >> 20
					);
				}

				output += output_width;
			}
		}
		else if (type == CByteImage::eRGB24)
		{
			const int output_width3 = 3 * output_width;
			const int input_width3 = 3 * input_width;

			for (int v = 0; v < output_height; v++)
			{
				const unsigned char *input_helper = input + pYOffsets[v];
				const int y = pYCoordinates[v];

				for (int u = 0; u < output_width3; u += 3)
				{
					const int x = pXCoordinates[u];
					const int f00 = (1024 - x) * (1024 - y);
					const int f10 = x * (1024 - y);
					const int f01 = (1024 - x) * y;
					const int f11 = x * y;

					register int offset;

					offset = pXOffsets[u];
					output[u] = (unsigned char) (
						((input_helper[offset] * f00) +
						(input_helper[offset + 3] * f10) +
						(input_helper[offset + input_width3] * f01) +
						(input_helper[offset + input_width3 + 3] * f11)) >> 20
					);

					offset++;
					output[u + 1] = (unsigned char) (
						((input_helper[offset] * f00) +
						(input_helper[offset + 3] * f10) +
						(input_helper[offset + input_width3] * f01) +
						(input_helper[offset + input_width3 + 3] * f11)) >> 20
					);

					offset++;
					output[u + 2] = (unsigned char) (
						((input_helper[offset] * f00) +
						(input_helper[offset + 3] * f10) +
						(input_helper[offset + input_width3] * f01) +
						(input_helper[offset + input_width3 + 3] * f11)) >> 20
					);
				}

				output += output_width3;
			}
		}
	}
	else
	{
		if (type == CByteImage::eGrayScale)
		{
			const int last_u = MY_MAX(0, output_width - output_width % 4);

			for (int v = 0; v < output_height; v++)
			{
				const unsigned char *input_helper = input + pYOffsets[v];
				int u;

				for (u = 0; u < last_u; u += 4)
				{
					output[u] = input_helper[pXOffsets[u]];
					output[u + 1] = input_helper[pXOffsets[u + 1]];
					output[u + 2] = input_helper[pXOffsets[u + 2]];
					output[u + 3] = input_helper[pXOffsets[u + 3]];
				}

				for (u = last_u; u < output_width; u++)
					output[u] = input_helper[pXOffsets[u]];

				output += output_width;
			}
		}
		else if (type == CByteImage::eRGB24)
		{
			const int output_width3 = 3 * output_width;
			const int last_u = 3 * MY_MAX(0, output_width - output_width % 4);
			
			for (int v = 0; v < output_height; v++)
			{
				const unsigned char *input_helper = input + pYOffsets[v];
				int u;

				for (u = 0; u < last_u; u += 12)
				{
					register int input_offset;
					
					input_offset = pXOffsets[u];
					output[u] = input_helper[input_offset];
					output[u + 1] = input_helper[input_offset + 1];
					output[u + 2] = input_helper[input_offset + 2];

					input_offset = pXOffsets[u + 3];
					output[u + 3] = input_helper[input_offset];
					output[u + 4] = input_helper[input_offset + 1];
					output[u + 5] = input_helper[input_offset + 2];

					input_offset = pXOffsets[u + 6];
					output[u + 6] = input_helper[input_offset];
					output[u + 7] = input_helper[input_offset + 1];
					output[u + 8] = input_helper[input_offset + 2];

					input_offset = pXOffsets[u + 9];
					output[u + 9] = input_helper[input_offset];
					output[u + 10] = input_helper[input_offset + 1];
					output[u + 11] = input_helper[input_offset + 2];
				}

				for (u = last_u; u < output_width3; u += 3)
				{
					register int input_offset = pXOffsets[u];
					output[u] = input_helper[input_offset];
					output[u + 1] = input_helper[input_offset + 1];
					output[u + 2] = input_helper[input_offset + 2];
				}

				output += output_width3;
			}
		}
	}

	delete [] pXOffsets;
	delete [] pYOffsets;

	if (pXCoordinates)
	{
		delete [] pXCoordinates;
		delete [] pYCoordinates;
	}

	return true;
}


bool ImageProcessor::Rotate(const CByteImage *pInputImage, CByteImage *pOutputImage, float mx, float my, float theta, bool bInterpolation)
{
	const float cos_theta = cosf(theta);
	const float sin_theta = sinf(theta);
	
	return ApplyHomography(pInputImage, pOutputImage,
		cos_theta, -sin_theta, mx * (1 - cos_theta) + my * sin_theta,
		sin_theta, cos_theta, my * (1 - cos_theta) - mx * sin_theta,
		0, 0,
		bInterpolation);
}

bool ImageProcessor::Amplify(const CByteImage *pInputImage, CByteImage *pOutputImage, float fFactor)
{
	OPTIMIZED_FUNCTION_HEADER_3(Amplify, pInputImage, pOutputImage, fFactor)
	
	if (!pInputImage->IsCompatible(pOutputImage))
	{
		printf("error: input and output image do not match for ImageProcessor::Amplify\n");
		return false;
	}
		
	const int nBytes = pInputImage->width * pInputImage->height * pInputImage->bytesPerPixel;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int i = 0; i < nBytes; i++)
	{
		const int v = int(fFactor * input[i] + 0.5f);
		output[i] = v < 0 ? 0 : (v > 255 ? 255 : (unsigned char) v);
	}

	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


bool ImageProcessor::CalculateSaturationImage(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		(pInputImage->type != CByteImage::eRGB24 && pInputImage->type != CByteImage::eRGB24Split) || pOutputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::CalculateSaturationImage\n");
		return false;
	}
		
	const int nPixels = pInputImage->width * pInputImage->height;
	unsigned char *output = pOutputImage->pixels;
	
	if (pInputImage->type == CByteImage::eRGB24)
	{
		const unsigned char *input = pInputImage->pixels;
	
		for (int i = 0, offset = 0; i < nPixels; i++)
		{
			const int r = input[offset];
			const int g = input[offset + 1];
			const int b = input[offset + 2];
			const int min = MY_MIN(r, MY_MIN(g, b));
			const int max = MY_MAX(r, MY_MAX(g, b));
			output[i] = (255 * (max - min) * division_table[max]) >> 20;
			offset += 3;
		}
	}
	else
	{
		const unsigned char *input_r = pInputImage->pixels;
		const unsigned char *input_g = input_r + nPixels;
		const unsigned char *input_b = input_g +  nPixels;
		
		for (int i = 0; i < nPixels; i++)
		{
			const int r = input_r[i];
			const int g = input_g[i];
			const int b = input_b[i];
			const int min = MY_MIN(r, MY_MIN(g, b));
			const int max = MY_MAX(r, MY_MAX(g, b));
			output[i] = (255 * (max - min) * division_table[max]) >> 20;
		}
	}

	return true;
}


bool ImageProcessor::ThresholdBinarize(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nThreshold)
{
	OPTIMIZED_FUNCTION_HEADER_3(ThresholdBinarize, pInputImage, pOutputImage, nThreshold)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::ThresholdBinarize\n");
		return false;
	}
		
	const int nPixels = pInputImage->width * pInputImage->height;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	for (int i = 0; i < nPixels; i++)
		output[i] = input[i] >= nThreshold ? 255 : 0;
		
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::ThresholdBinarize(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nMinThreshold, unsigned char nMaxThreshold)
{
	//OPTIMIZED_FUNCTION_HEADER_4(ThresholdBinarize, pInputImage, pOutputImage, nMinThreshold, nMaxThreshold)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::ThresholdBinarize\n");
		return false;
	}
		
	const int nPixels = pInputImage->width * pInputImage->height;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	for (int i = 0; i < nPixels; i++)
		output[i] = input[i] >= nMinThreshold && input[i] <= nMaxThreshold ? 255 : 0;
		
	//OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::ThresholdBinarize(const CFloatMatrix *pInputMatrix, CFloatMatrix *pOutputMatrix, float fThreshold)
{
	if (pInputMatrix->columns != pOutputMatrix->columns || pInputMatrix->rows != pOutputMatrix->rows)
	{
		printf("error: input and output matrix do not match for ImageProcessor::ThresholdBinarize\n");
		return false;
	}
		
	const int n = pInputMatrix->columns * pInputMatrix->rows;
	const float *input = pInputMatrix->data;
	float *output = pOutputMatrix->data;
	
	for (int i = 0; i < n; i++)
		output[i] = input[i] >= fThreshold ? 255.0f : 0;

	return true;
}

bool ImageProcessor::ThresholdBinarizeInverse(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nThreshold)
{
	OPTIMIZED_FUNCTION_HEADER_3(ThresholdBinarizeInverse, pInputImage, pOutputImage, nThreshold)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::ThresholdBinarizeInverse\n");
		return false;
	}
		
	unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	const int nPixels = pInputImage->width * pInputImage->height;

	for (int i = 0; i < nPixels; i++)
		output[i] = input[i] <= nThreshold ? 255 : 0;
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::ThresholdFilter(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nThreshold)
{
	OPTIMIZED_FUNCTION_HEADER_3(ThresholdFilter, pInputImage, pOutputImage, nThreshold)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::ThresholdFilter\n");
		return false;
	}
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	for (int i = 0; i < nPixels; i++)
		output[i] = input[i] >= nThreshold ? input[i] : 0;
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::ThresholdFilterInverse(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nThreshold)
{
	OPTIMIZED_FUNCTION_HEADER_3(ThresholdFilterInverse, pInputImage, pOutputImage, nThreshold)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::ThresholdFilterInverse\n");
		return false;
	}
		
	unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	const int nPixels = pInputImage->width * pInputImage->height;

	for (int i = 0; i < nPixels; i++)
		output[i] = input[i] <= nThreshold ? input[i] : 0;
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


bool ImageProcessor::FilterRGB(const CByteImage *pInputImage, CByteImage *pOutputImage, CRGBColorModel *pColorModel, float fThreshold)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eRGB24 || pOutputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::FilterRGB\n");
		return false;
	}
		
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	const int nPixels = pInputImage->width * pInputImage->height;
	int offset = 0;

	for (int i = 0; i < nPixels; i++, offset += 3)
	{
		Vec3d rgb = { input[offset], input[offset + 1], input[offset + 2] };
		output[i] = pColorModel->CalculateColorProbability(rgb) > fThreshold ? 255 : 0;
	}

	return true;
}

bool ImageProcessor::FilterHSV(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char hue, unsigned char tol_hue, unsigned char min_sat, unsigned char max_sat, unsigned char min_v, unsigned char max_v, const MyRegion *pROI)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		(pInputImage->type != CByteImage::eRGB24 && pInputImage->type != CByteImage::eRGB24Split) || pOutputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::FilterHSV\n");
		return false;
	}
	
	int min_hue = (int) hue - (int) tol_hue;
	int max_hue = (int) hue + (int) tol_hue;

	if (min_hue < 0)
		min_hue += 180;

	if (max_hue >= 180)
		max_hue -= 180;

    if(tol_hue > 89)
    {
        min_hue = 0;
        max_hue = 179;
    }

	return FilterHSV2(pInputImage, pOutputImage, (unsigned char) min_hue, (unsigned char) max_hue, min_sat, max_sat, min_v, max_v, pROI);
}

bool ImageProcessor::FilterHSV2(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char min_hue, unsigned char max_hue, unsigned char min_sat, unsigned char max_sat, unsigned char min_v, unsigned char max_v, const MyRegion *pROI)
{
	OPTIMIZED_FUNCTION_HEADER_8_ROI(FilterHSV2, pInputImage, pOutputImage, min_hue, max_hue, min_sat, max_sat, min_v, max_v, pROI)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		(pInputImage->type != CByteImage::eRGB24 && pInputImage->type != CByteImage::eRGB24Split) || pOutputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::FilterHSV2\n");
		return false;
	}
	
	if (pInputImage->type == CByteImage::eRGB24)
	{
		const unsigned char *input_h = pInputImage->pixels;
		const unsigned char *input_s = pInputImage->pixels + 1;
		const unsigned char *input_v = pInputImage->pixels + 2;
		unsigned char *output = pOutputImage->pixels;
		
		if (pROI)
		{
			const int width = pInputImage->width;
			const int height = pInputImage->height;
			
			int min_x = pROI->min_x;
			int max_x = pROI->max_x;
			int min_y = pROI->min_y;
			int max_y = pROI->max_y;
			
			if (min_x < 0) min_x = 0;
			if (min_x > width - 1) min_x = width - 1;
			if (max_x < 0) max_x = 0;
			if (max_x > width - 1) max_x = width - 1;
			if (min_y < 0) min_y = 0;
			if (min_y > height - 1) min_y = height - 1;
			if (max_y < 0) max_y = 0;
			if (max_y > height - 1) max_y = height - 1;
			
			const int diff = width - (max_x - min_x + 1);
			const int diff_rgb = 3 * diff;
			
			if (max_hue >= min_hue)
			{
				for (int y = min_y, offset = min_y * width + min_x, offset_rgb = 3 * (min_y * width + min_x); y <= max_y; y++, offset += diff, offset_rgb += diff_rgb)
					for (int x = min_x; x <= max_x; x++, offset++, offset_rgb += 3)
						output[offset] = (input_s[offset_rgb] <= max_sat && input_s[offset_rgb] >= min_sat && input_h[offset_rgb] >= min_hue && input_h[offset_rgb] <= max_hue && input_v[offset_rgb] >= min_v && input_v[offset_rgb] <= max_v) * 255;
			}
			else
			{
				for (int y = min_y, offset = min_y * width + min_x, offset_rgb = 3 * (min_y * width + min_x); y <= max_y; y++, offset += diff, offset_rgb += diff_rgb)
					for (int x = min_x; x <= max_x; x++, offset++, offset_rgb += 3)
						output[offset] = (input_s[offset_rgb] <= max_sat && input_s[offset_rgb] >= min_sat && (input_h[offset_rgb] >= min_hue || input_h[offset_rgb] <= max_hue) && input_v[offset_rgb] >= min_v && input_v[offset_rgb] <= max_v) * 255;
			}
		}
		else
		{
			const int nPixels = pInputImage->width * pInputImage->height;
			
			if (max_hue >= min_hue)
			{
				for (int i = 0, offset = 0; i < nPixels; i++, offset += 3)
					output[i] = (input_s[offset] <= max_sat && input_s[offset] >= min_sat && input_h[offset] >= min_hue && input_h[offset] <= max_hue && input_v[offset] >= min_v && input_v[offset] <= max_v) * 255;
			}
			else
			{
				for (int i = 0, offset = 0; i < nPixels; i++, offset += 3)
					output[i] = (input_s[offset] <= max_sat && input_s[offset] >= min_sat && (input_h[offset] >= min_hue || input_h[offset] <= max_hue) && input_v[offset] >= min_v && input_v[offset] <= max_v) * 255;
			}
		}
	}
	else
	{
		const int width = pInputImage->width;
		const int height = pInputImage->height;
		const int nPixels = width * height;
		
		const unsigned char *input_h = pInputImage->pixels;
		const unsigned char *input_s = input_h + nPixels;
		const unsigned char *input_v = input_s + nPixels;
		unsigned char *output = pOutputImage->pixels;
		
		if (pROI)
		{
			int min_x = pROI->min_x;
			int max_x = pROI->max_x;
			int min_y = pROI->min_y;
			int max_y = pROI->max_y;
			
			if (min_x < 0) min_x = 0;
			if (min_x > width - 1) min_x = width - 1;
			if (max_x < 0) max_x = 0;
			if (max_x > width - 1) max_x = width - 1;
			if (min_y < 0) min_y = 0;
			if (min_y > height - 1) min_y = height - 1;
			if (max_y < 0) max_y = 0;
			if (max_y > height - 1) max_y = height - 1;
			
			const int diff = width - (max_x - min_x + 1);
			
			if (max_hue >= min_hue)
			{
				for (int y = min_y, offset = min_y * width + min_x; y <= max_y; y++, offset += diff)
					for (int x = min_x; x <= max_x; x++, offset++)
						output[offset] = (input_s[offset] <= max_sat && input_s[offset] >= min_sat && input_h[offset] >= min_hue && input_h[offset] <= max_hue && input_v[offset] >= min_v && input_v[offset] <= max_v) * 255;
			}
			else
			{
				for (int y = min_y, offset = min_y * width + min_x; y <= max_y; y++, offset += diff)
					for (int x = min_x; x <= max_x; x++, offset++)
						output[offset] = (input_s[offset] <= max_sat && input_s[offset] >= min_sat && (input_h[offset] >= min_hue || input_h[offset] <= max_hue) && input_v[offset] >= min_v && input_v[offset] <= max_v) * 255;
			}
		}
		else
		{
			if (max_hue >= min_hue)
			{
				for (int i = 0; i < nPixels; i++)
					output[i] = (input_s[i] <= max_sat && input_s[i] >= min_sat && input_h[i] >= min_hue && input_h[i] <= max_hue && input_v[i] >= min_v && input_v[i] <= max_v) * 255;
			}
			else
			{
				for (int i = 0; i < nPixels; i++)
					output[i] = (input_s[i] <= max_sat && input_s[i] >= min_sat && (input_h[i] >= min_hue || input_h[i] <= max_hue) && input_v[i] >= min_v && input_v[i] <= max_v) * 255;
			}
		}
	}
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}



bool ImageProcessor::FilterColor(const CByteImage *pInputImage, CByteImage *pOutputImage, ObjectColor cColor, CColorParameterSet* pColorParameterSet, bool bImageIsHSV)
{
    const int* pParams = pColorParameterSet->GetColorParameters(cColor);

    bool bRet;
    if (bImageIsHSV)
    {
        bRet = FilterHSV(pInputImage, pOutputImage, pParams[0], pParams[1], pParams[2], pParams[3], pParams[4], pParams[5]);
    }
    else
    {
        CByteImage* pImage = new CByteImage(pInputImage);
        CalculateHSVImage(pInputImage, pImage);
        bRet = FilterHSV(pImage, pOutputImage, pParams[0], pParams[1], pParams[2], pParams[3], pParams[4], pParams[5]);
        delete pImage;
    }
    return bRet;
}



// Original version of this function by Olaf Fischer
bool ImageProcessor::Rotate180Degrees(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height || pInputImage->type != pOutputImage->type || pInputImage->type == CByteImage::eRGB24Split)
	{
		printf("error: input and output image do not match for ImageProcessor::Rotate180Degrees\n");
		return false;
	}
	
	CByteImage *pSaveOutputImage = 0;
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CByteImage(pInputImage);
	}
	
	const int nBytes = pInputImage->width * pInputImage->height * pInputImage->bytesPerPixel; 
	const int nLastPixel = nBytes - pInputImage->bytesPerPixel;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;

	if (pInputImage->type == CByteImage::eGrayScale)
	{
		for (int i = 0; i < nBytes; i++)
			output[i] = input[nLastPixel - i];
	}
	else if (pInputImage->type == CByteImage::eRGB24)
	{
		for (int i = 0; i < nBytes; i += 3)
		{
			output[i + 2] = input[nLastPixel - i + 2];
			output[i + 1] = input[nLastPixel - i + 1];
			output[i] = input[nLastPixel - i];
		}
	}
  
	if (pSaveOutputImage)
	{
		CopyImage(pOutputImage, pSaveOutputImage);
		delete pOutputImage;
	}

	return true;
}


static int _RegionGrowing(unsigned char *pixels, int width, int offset, int *stack, int *region, MyRegion &resultRegion, int nMinimumPointsPerRegion, int nMaximumPointsPerRegion, bool bCalculateBoundingBox)
{
	resultRegion.nSeedOffset = offset;

	int sp = 0, nPixels = 0;
	
	stack[sp++] = offset;
	pixels[offset] = 254;
	
	while (sp--)
	{
		const int offset = stack[sp];
		
		if (pixels[offset - width] == 255)
		{
			pixels[offset - width] = 254;
			stack[sp++] = offset - width;
		}
		
		if (pixels[offset - 1] == 255)
		{
			pixels[offset - 1] = 254;
			stack[sp++] = offset - 1;
		}
		
		if (pixels[offset + 1] == 255)
		{
			pixels[offset + 1] = 254;
			stack[sp++] = offset + 1;
		}
		
		if (pixels[offset + width] == 255)
		{
			pixels[offset + width] = 254;
			stack[sp++] = offset + width;
		}
		
		region[nPixels++] = offset;
	}
	
	if ((nMinimumPointsPerRegion > 0 && nPixels < nMinimumPointsPerRegion) || (nMaximumPointsPerRegion > 0 && nPixels > nMaximumPointsPerRegion))
		return 0;
	
	int cx = 0, cy = 0;
	
	if (bCalculateBoundingBox)
	{
		const int x = offset % width;
		const int y = offset / width;
		int min_x = x, max_x = x, min_y = y, max_y = y;
	
		for (int i = 0; i < nPixels; i++)
		{
			const int offset = region[i];
			const int x = offset % width;
			const int y = offset / width;
		
			cx += x;
			cy += y;
		
			if (x < min_x)
				min_x = x;
			
			if (x > max_x)
				max_x = x;
			
			if (y < min_y)
				min_y = y;
			
			if (y > max_y)
				max_y = y;
		}
		
		resultRegion.min_x = min_x;
		resultRegion.min_y = min_y;
		resultRegion.max_x = max_x;
		resultRegion.max_y = max_y;
		resultRegion.ratio = float(max_x - min_x + 1) / float(max_y - min_y + 1);
	}
	else
	{
		for (int i = 0; i < nPixels; i++)
		{
			const int offset = region[i];
			
			cx += offset % width;
			cy += offset / width;
		}
		
		resultRegion.min_x = -1;
		resultRegion.min_y = -1;
		resultRegion.max_x = -1;
		resultRegion.max_y = -1;
		resultRegion.ratio = 0.0f;
	}
	
	// fill rest of region struct
	resultRegion.nPixels = nPixels;
	resultRegion.centroid.x = float(cx) / nPixels;
	resultRegion.centroid.y = float(cy) / nPixels;
	
	return nPixels;
}

int ImageProcessor::RegionGrowing(const CByteImage *pImage, MyRegion &resultRegion, int x, int y, int nMinimumPointsPerRegion, int nMaximumPointsPerRegion, bool bCalculateBoundingBox, bool bStorePixels)
{
	if (pImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image should be grayscale for ImageProcessor::RegionGrowing\n");
		return -1;
	}

	const int width = pImage->width;
	const int height = pImage->height;
	
	if (x < 0 || x >= width || y < 0 || y >= height)
	{
		printf("error: x/y is not within bounds of image in ImageProcessor::RegionGrowing\n");
		return -1;
	}

	// create image with additional 1 pixel border
	CByteImage *pTempImage = new CByteImage(width + 2, height + 2, CByteImage::eGrayScale);

	const int temp_width = pTempImage->width;
	const int temp_height = pTempImage->height;

	unsigned char *temp = pTempImage->pixels;
		
	// copy contents
	unsigned char *temp_helper = temp + temp_width + 1;

	for (int i = 0; i < pImage->height; i++)
		memcpy(temp_helper + i * temp_width, pImage->pixels + i * width, width);

	// zero frame
	ImageProcessor::ZeroFrame(pTempImage);

	// allocate memory
	const int nPixels = temp_width * temp_height;

	int *pStack = new int[nPixels];
	int *pRegionPixels = new int[nPixels];

	// perform region growing
	const int nRegionPixels = _RegionGrowing(temp, temp_width, (y + 1) * temp_width + (x + 1), pStack, pRegionPixels, resultRegion, nMinimumPointsPerRegion, nMaximumPointsPerRegion, bCalculateBoundingBox);

	if (resultRegion.pPixels)
	{
		delete [] resultRegion.pPixels;
		resultRegion.pPixels = 0;
	}
	
	if (nRegionPixels > 0)
	{
		if (bStorePixels)
		{
			resultRegion.pPixels = new int[nRegionPixels];

			for (int j = 0; j < nRegionPixels; j++)
			{
				const int x = pRegionPixels[j] % temp_width;
				const int y = pRegionPixels[j] / temp_width;

				resultRegion.pPixels[j] = (y - 1) * width + x - 1;
			}
		}
		
		// correct coordinates
		resultRegion.centroid.x -= 1.0f;
		resultRegion.centroid.y -= 1.0f;

		resultRegion.min_x -= 1;
		resultRegion.min_y -= 1;
		resultRegion.max_x -= 1;
		resultRegion.max_y -= 1;

		resultRegion.nSeedOffset = (resultRegion.nSeedOffset / temp_width - 1) * width + (resultRegion.nSeedOffset % temp_width - 1);
	}

	// free memory
	delete pTempImage;
	delete [] pStack;
	delete [] pRegionPixels;

	return nRegionPixels;
}

bool ImageProcessor::FindRegions(const CByteImage *pImage, RegionList &regionList, int nMinimumPointsPerRegion, int nMaximumPointsPerRegion, bool bCalculateBoundingBox, bool bStorePixels)
{
	// clear result list
	regionList.clear();

	if (pImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image should be grayscale for ImageProcessor::FindRegions\n");
		return false;
	}

	const int width = pImage->width;
	const int height = pImage->height;
	
	// create image with additional 1 pixel border
	CByteImage *pTempImage = new CByteImage(width + 2, height + 2, CByteImage::eGrayScale);

	const int temp_width = pTempImage->width;
	const int temp_height = pTempImage->height;

	unsigned char *temp = pTempImage->pixels;
		
	// copy contents
	unsigned char *temp_helper = temp + temp_width + 1;

	for (int y = 0; y < pImage->height; y++)
		memcpy(temp_helper + y * temp_width, pImage->pixels + y * width, width);

	// zero frame
	ImageProcessor::ZeroFrame(pTempImage);

	// allocate memory
	const int nPixels = temp_width * temp_height;

	int *pStack = new int[nPixels];
	int *pRegionPixels = new int[nPixels];

	// go through image
	for (int i = 0; i < nPixels; i++)
	{
		if (temp[i] == 255)
		{
			MyRegion region;
				
			// do region growing
			const int nRegionPixels = _RegionGrowing(temp, temp_width, i, pStack, pRegionPixels, region, nMinimumPointsPerRegion, nMaximumPointsPerRegion, bCalculateBoundingBox);
			
			if (nRegionPixels > 0)
			{
				// first add
				regionList.push_back(region);

				// then copy (this way a double copy through copy constructor is avoided)
				// and correct coordinates
				MyRegion &addedEntry = regionList.back();

				// correct coordinates
				addedEntry.centroid.x -= 1.0f;
				addedEntry.centroid.y -= 1.0f;

				addedEntry.min_x -= 1;
				addedEntry.min_y -= 1;
				addedEntry.max_x -= 1;
				addedEntry.max_y -= 1;

				if (bStorePixels)
				{
					addedEntry.pPixels = new int[nRegionPixels];

					for (int j = 0; j < nRegionPixels; j++)
					{
						const int x = pRegionPixels[j] % temp_width;
						const int y = pRegionPixels[j] / temp_width;

						addedEntry.pPixels[j] = (y - 1) * width + x - 1;
					}
					
					addedEntry.nSeedOffset = (addedEntry.nSeedOffset / temp_width - 1) * width + (addedEntry.nSeedOffset % temp_width - 1);
				}
			}
		}
	}

	// free memory
	delete pTempImage;
	delete [] pStack;
	delete [] pRegionPixels;

	return true;
}

bool ImageProcessor::FindRegions(const CByteImage *pImage, CRegionArray &regionList, int nMinimumPointsPerRegion, int nMaximumPointsPerRegion, bool bCalculateBoundingBox, bool bStorePixels)
{
	// clear result list
	regionList.Clear();

	if (pImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image should be grayscale for ImageProcessor::FindRegions\n");
		return false;
	}

	const int width = pImage->width;
	const int height = pImage->height;
	
	// create image with additional 1 pixel border
	CByteImage *pTempImage = new CByteImage(width + 2, height + 2, CByteImage::eGrayScale);

	const int temp_width = pTempImage->width;
	const int temp_height = pTempImage->height;

	unsigned char *temp = pTempImage->pixels;
		
	// copy contents
	unsigned char *temp_helper = temp + temp_width + 1;

	for (int y = 0; y < pImage->height; y++)
		memcpy(temp_helper + y * temp_width, pImage->pixels + y * width, width);

	// zero frame
	ImageProcessor::ZeroFrame(pTempImage);

	// allocate memory
	const int nPixels = temp_width * temp_height;

	int *pStack = new int[nPixels];
	int *pRegionPixels = new int[nPixels];

	// go through image
	for (int i = 0; i < nPixels; i++)
	{
		if (temp[i] == 255)
		{
			MyRegion region;
				
			// do region growing
			const int nRegionPixels = _RegionGrowing(temp, temp_width, i, pStack, pRegionPixels, region, nMinimumPointsPerRegion, nMaximumPointsPerRegion, bCalculateBoundingBox);
			
			if (nRegionPixels > 0)
			{
				// first add
				regionList.AddElement(region);

				// then copy (this way a double copy through copy constructor is avoided)
				// and correct coordinates
				MyRegion &addedEntry = regionList[regionList.GetSize() - 1];

				// correct coordinates
				addedEntry.centroid.x -= 1.0f;
				addedEntry.centroid.y -= 1.0f;

				addedEntry.min_x -= 1;
				addedEntry.min_y -= 1;
				addedEntry.max_x -= 1;
				addedEntry.max_y -= 1;

				if (bStorePixels)
				{
					addedEntry.pPixels = new int[nRegionPixels];

					for (int j = 0; j < nRegionPixels; j++)
					{
						const int x = pRegionPixels[j] % temp_width;
						const int y = pRegionPixels[j] / temp_width;

						addedEntry.pPixels[j] = (y - 1) * width + x - 1;
					}
					
					addedEntry.nSeedOffset = (addedEntry.nSeedOffset / temp_width - 1) * width + (addedEntry.nSeedOffset % temp_width - 1);
				}
			}
		}
	}

	// free memory
	delete pTempImage;
	delete [] pStack;
	delete [] pRegionPixels;

	return true;
}


bool ImageProcessor::CalculateHSVImage(const CByteImage *pInputImage, CByteImage *pOutputImage, const MyRegion *pROI)
{
	OPTIMIZED_FUNCTION_HEADER_2_ROI(CalculateHSVImage, pInputImage, pOutputImage, pROI)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || (pInputImage->type != CByteImage::eRGB24 && pInputImage->type != CByteImage::eRGB24Split))
	{
		printf("error: input and output image do not match for ImageProcessor::CalculateHSVImage\n");
		return false;
	}
		
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	if (pROI)
	{
		if (pInputImage->type == CByteImage::eRGB24)
		{
			const int width = pInputImage->width;
			const int height = pInputImage->height;
			
			int min_x = pROI->min_x;
			int max_x = pROI->max_x;
			int min_y = pROI->min_y;
			int max_y = pROI->max_y;
			
			if (min_x < 0) min_x = 0;
			if (min_x > width - 1) min_x = width - 1;
			if (max_x < 0) max_x = 0;
			if (max_x > width - 1) max_x = width - 1;
			if (min_y < 0) min_y = 0;
			if (min_y > height - 1) min_y = height - 1;
			if (max_y < 0) max_y = 0;
			if (max_y > height - 1) max_y = height - 1;
			
			const int diff = 3 * (width - (max_x - min_x + 1));
			
			for (int y = min_y, offset = 3 * (min_y * width + min_x); y <= max_y; y++, offset += diff)
			{
				for (int x = min_x; x <= max_x; x++, offset += 3)
				{
					const int r = input[offset];
					const int g = input[offset + 1];
					const int b = input[offset + 2];
					
					const int max = MY_MAX(MY_MAX(r, g), b);
					const int min = MY_MIN(MY_MIN(r, g), b);
					const int delta = max - min;
				
					// unoptimized: delta * 255 / max;
					const int s = (255 * delta * division_table[max]) >> 20;
					int h;
					
					// unoptimized: 30 * (g - b) / delta (etc.)
					if (r == max)
						h = g > b ? 180 + ((30 * (g - b) * division_table[delta]) >> 20) : 180 - ((30 * (b - g) * division_table[delta]) >> 20);
					else if (g == max)
						h = b > r ? 60 + ((30 * (b - r) * division_table[delta]) >> 20) : 60 - ((30 * (r - b) * division_table[delta]) >> 20);
					else
						h = r > g ? 120 + ((30 * (r - g) * division_table[delta]) >> 20) : 120 - ((30 * (g - r) * division_table[delta]) >> 20);
					
					if (h >= 180) h -= 180;
					
					output[offset] = h;
					output[offset + 1] = s;
					output[offset + 2] = max;
				}
			}
		}
		else
		{
			const int width = pInputImage->width;
			const int height = pInputImage->height;
			
			const int nPixels = width * height;
			
			const unsigned char *input_r = input;
			const unsigned char *input_g = input_r + nPixels;
			const unsigned char *input_b = input_g + nPixels;
			unsigned char *output_r = output;
			unsigned char *output_g = output_r + nPixels;
			unsigned char *output_b = output_g + nPixels;
			
			int min_x = pROI->min_x;
			int max_x = pROI->max_x;
			int min_y = pROI->min_y;
			int max_y = pROI->max_y;
			
			if (min_x < 0) min_x = 0;
			if (min_x > width - 1) min_x = width - 1;
			if (max_x < 0) max_x = 0;
			if (max_x > width - 1) max_x = width - 1;
			if (min_y < 0) min_y = 0;
			if (min_y > height - 1) min_y = height - 1;
			if (max_y < 0) max_y = 0;
			if (max_y > height - 1) max_y = height - 1;
			
			const int diff = width - (max_x - min_x + 1);
			
			for (int y = min_y, offset = min_y * width + min_x; y <= max_y; y++, offset += diff)
			{
				for (int x = min_x; x <= max_x; x++, offset++)
				{
					const int r = input_r[offset];
					const int g = input_g[offset];
					const int b = input_b[offset];
					
					const int max = MY_MAX(MY_MAX(r, g), b);
					const int min = MY_MIN(MY_MIN(r, g), b);
					const int delta = max - min;
					
					// unoptimized: delta * 255 / max;
					const int s = (255 * delta * division_table[max]) >> 20;
					int h;
					
					// unoptimized: 30 * (g - b) / delta (etc.)
					if (r == max)
						h = g > b ? 180 + ((30 * (g - b) * division_table[delta]) >> 20) : 180 - ((30 * (b - g) * division_table[delta]) >> 20);
					else if (g == max)
						h = b > r ? 60 + ((30 * (b - r) * division_table[delta]) >> 20) : 60 - ((30 * (r - b) * division_table[delta]) >> 20);
					else
						h = r > g ? 120 + ((30 * (r - g) * division_table[delta]) >> 20) : 120 - ((30 * (g - r) * division_table[delta]) >> 20);
					
					if (h >= 180) h -= 180;
					
					output_r[offset] = h;
					output_g[offset] = s;
					output_b[offset] = max;
				}
			}
		}
	}
	else
	{
		if (pInputImage->type == CByteImage::eRGB24)
		{
			const int maxi = pInputImage->width * pInputImage->height * 3;
			
			for (int i = 0; i < maxi; i += 3)
			{
				const int r = input[i];
				const int g = input[i + 1];
				const int b = input[i + 2];
				
				const int max = MY_MAX(MY_MAX(r, g), b);
				const int min = MY_MIN(MY_MIN(r, g), b);
				const int delta = max - min;
			
				// unoptimized: delta * 255 / max;
				const int s = (255 * delta * division_table[max]) >> 20;
				int h;
				
				// unoptimized: 30 * (g - b) / delta (etc.)
				if (r == max)
					h = g > b ? 180 + ((30 * (g - b) * division_table[delta]) >> 20) : 180 - ((30 * (b - g) * division_table[delta]) >> 20);
				else if (g == max)
					h = b > r ? 60 + ((30 * (b - r) * division_table[delta]) >> 20) : 60 - ((30 * (r - b) * division_table[delta]) >> 20);
				else
					h = r > g ? 120 + ((30 * (r - g) * division_table[delta]) >> 20) : 120 - ((30 * (g - r) * division_table[delta]) >> 20);
				
				if (h >= 180) h -= 180;
				
				output[i] = h;
				output[i + 1] = s;
				output[i + 2] = max;
			}
		}
		else
		{
			const int nPixels = pInputImage->width * pInputImage->height;
			
			const unsigned char *input_r = input;
			const unsigned char *input_g = input_r + nPixels;
			const unsigned char *input_b = input_g + nPixels;
			unsigned char *output_r = output;
			unsigned char *output_g = output_r + nPixels;
			unsigned char *output_b = output_g + nPixels;
			
			for (int i = 0; i < nPixels; i++)
			{
				const int r = input_r[i];
				const int g = input_g[i];
				const int b = input_b[i];
				
				const int max = MY_MAX(MY_MAX(r, g), b);
				const int min = MY_MIN(MY_MIN(r, g), b);
				const int delta = max - min;
				
				// unoptimized: delta * 255 / max;
				const int s = (255 * delta * division_table[max]) >> 20;
				int h;
				
				// unoptimized: 30 * (g - b) / delta (etc.)
				if (r == max)
					h = g > b ? 180 + ((30 * (g - b) * division_table[delta]) >> 20) : 180 - ((30 * (b - g) * division_table[delta]) >> 20);
				else if (g == max)
					h = b > r ? 60 + ((30 * (b - r) * division_table[delta]) >> 20) : 60 - ((30 * (r - b) * division_table[delta]) >> 20);
				else
					h = r > g ? 120 + ((30 * (r - g) * division_table[delta]) >> 20) : 120 - ((30 * (g - r) * division_table[delta]) >> 20);
				
				if (h >= 180) h -= 180;
				
				output_r[i] = h;
				output_g[i] = s;
				output_b[i] = max;
			}
		}
	}
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


bool ImageProcessor::HoughTransformLines(const CByteImage *pImage, CByteImage *pVisualizationImage, Vec2dList &resultLines, int nLinesToExtract, int nMinHits)
{
	if (pImage->type == CByteImage::eGrayScale)
	{
		printf("error: input image must be of type CByteImage::eGrayScale\n");
		return false;
	}

	const int width = pImage->width;
	const int height = pImage->height;
	const unsigned char *pixels = pImage->pixels;
	
	const int m = 1 + 2 * (int(sqrtf(float(width * width + height * height))) + 1);
	const int m2 = m / 2;
	const float m2f = float(m2) + 0.5f;
	const int n = 180;
	const int nHoughPixels = n * m;
	
	CShortImage houghSpace(m, n);
	
	// reset bins
	ImageProcessor::Zero(&houghSpace);
	short *houghspace = houghSpace.pixels;
	
	// initialize lookup tables for sin and cos
	// function does initialization only for the first call
	InitSinCosTables();
	
	// perform hough transform
	for (int y = 0, offset = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++, offset++)
		{
			if (pixels[offset])
			{
				for (int t = 0; t < n; t++)
				{
					const float r = x * cos_table[t] + y * sin_table[t];
					const int r_ = int(r + m2f);
					
					houghspace[t * m + r_]++;
				}
			}
		}
	}
	
	resultLines.clear();
	
	// find maxima
	for (int nn = 0; nn < nLinesToExtract; nn++)
	{
		int max = nMinHits - 1, best_i = -1;

		for (int i = 0; i < nHoughPixels; i++)
		{
			if (houghspace[i] > max)
			{
				max = houghspace[i];
				best_i = i;
			}
		}

		if (best_i != -1)
		{
			const int t = best_i / m;
			const int r = best_i % m;
			
			const int w = 10;

			for (int j = -w; j <= w; j++)
			{
				for (int k = -w; k <= w; k++)
				{
					const int r_ = r + j;
					int t_ = t + k;
					
					if (t_ >= n)
						t_ -= n;
					
					if (t_ < 0)
						t_ += n;
					
					if (r_ >= 0 && r_ < m)
						houghspace[t_ * m + r_] = 0;
				}
			}

			const float theta = t * FLOAT_DEG2RAD;
			const int r_value = r - m2;
			
			const Vec2d g = { theta, float(r_value) };
			resultLines.push_back(g);
					
			if (pVisualizationImage)
			{
				//printf("%i hits: (theta, r) = (%i, %i)\n", max, t, r_value);
				PrimitivesDrawer::DrawLinePolar(pVisualizationImage, theta, float(r_value), 255, 0, 0);
			}
		}
		else
			break;
	}

	return true;
}

bool ImageProcessor::HoughTransformCircles(const CByteImage *pImage, CByteImage *pVisualizationImage, Vec3dList &resultCircles, int rmin, int rmax, int nCirclesToExtract, int nMinHits)
{
	if (rmin > rmax)
	{
		printf("error: rmin (%i) may not be greater than rmax (%i) for ImageProcessor::HoughTransformCircles\n", rmin, rmax);
		return false;
	}
	
	const int width = pImage->width;
	const int height = pImage->height;
	const unsigned char *pixels = pImage->pixels;

	const int n = width + 2 * rmax;
	const int m = height + 2 * rmax;
	const int nHoughPixels = n * m * (rmax - rmin + 1);
	
	short *houghspace = new short[nHoughPixels];
	
	// reset bins
	memset(houghspace, 0, nHoughPixels * sizeof(short));

	// initialize lookup tables for sin and cos
	// function does initialization only for the first call
	InitSinCosTables();
	
	const int mn = m * n;
	short *helper = houghspace - rmin * mn + rmax * m + rmax;

	// perform hough transform
	for (int v = 0, offset = 0; v < height; v++)
	{
		for (int u = 0; u < width; u++, offset++)
		{
			if (pixels[offset] == 255)
			{
				for (int t = 0; t < 180; t++)
				{
					for (int r = rmin; r <= rmax; r++)
					{
						const int um = int(u - r * cos_table[t] + 0.5f);
						const int vm1 = int(v - r * sin_table[t] + 0.5f);
						const int vm2 = int(v + r * sin_table[t] + 0.5f);
					
						//const int base = (r - rmin) * mn + (rmax + um) * m + rmax;
						//houghspace[base + vm1]++;
						//houghspace[base + vm2]++;
						
						const int base = r * mn + um * m;
						helper[base + vm1]++;
						helper[base + vm2]++;
					}
				}
			}
		}
	}
	
	resultCircles.clear();

	// find maxima
	for (int nn = 0; nn < nCirclesToExtract; nn++)
	{
		int max = nMinHits - 1, best_i = -1;

		for (int i = 0; i < nHoughPixels; i++)
		{
			if (houghspace[i] > max)
			{
				max = houghspace[i];
				best_i = i;
			}
		}

		if (best_i != -1)
		{
			int um = (best_i % mn) / m;
			int vm = (best_i % mn) % m;
			int r = best_i / mn;
			
			const int w = 5;

			for (int j = -w; j <= w; j++)
			{
				for (int k = -w; k <= w; k++)
				{
					for (int l = -w; l <= w; l++)
					{
						const int um_ = um + j;
						const int vm_ = vm + k;
						const int r_ = r + l;
					
						if (um_ >= 0 && um_ < n && vm >= 0 && vm < m && r_ >= 0 && r_ <= rmax - rmin)
							houghspace[r_ * mn + um_ * m + vm_] = 0;
					}
				}
			}

			um -= rmax;
			vm -= rmax;
			r += rmin;

			//printf("%i hits: (um vm) = (%i %i) --  r = = %i\n", max, um, vm, r);

			const Vec3d circle = { float(um), float(vm), float(r) };
			resultCircles.push_back(circle);
					
			if (pVisualizationImage)
			{
				Vec2d center = { float(um), float(vm) };
				PrimitivesDrawer::DrawCircle(pVisualizationImage, center, float(r), 255, 0, 0, 1);
			}
		}
		else
			break;
	}
	
	delete [] houghspace;

	return true;
}

void ImageProcessor::HoughTransformLines(const CVec2dArray &edgePoints, const CVec2dArray &edgeDirections, int width, int height, int nLinesToExtract, int nMinHits, CVec2dArray &resultLines, CIntArray &resultHits, CByteImage *pVisualizationImage)
{
	// clear result list
	resultLines.Clear();

	// perform detection
	CStraightLine2dArray resultLines_;

	HoughTransformLines(edgePoints, edgeDirections, width, height, nLinesToExtract, nMinHits, resultLines_, resultHits, pVisualizationImage);
	
	// fill result list
	const int nResultLines = resultLines_.GetSize();

	for (int i = 0; i < nResultLines; i++)
	{
		const StraightLine2d &line = resultLines_[i];

		Vec2d x = { atan2f(line.normalVector.y, line.normalVector.x), -line.c };

		resultLines.AddElement(x);
	}
}

void ImageProcessor::HoughTransformLines(const CVec2dArray &edgePoints, const CVec2dArray &edgeDirections, int width, int height, int nLinesToExtract, int nMinHits, CStraightLine2dArray &resultLines, CIntArray &resultHits, CByteImage *pVisualizationImage)
{
	// clear result list
	resultLines.Clear();

	const int m = 1 + 2 * (int(sqrtf(float(width * width + height * height))) + 1);
	const int m2 = m / 2;
	const float m2f = float(m2) + 0.5f;
	const int n = 360;
	const int nHoughPixels = n * m;
	
	CShortImage houghSpace(m, n);
	
	// reset bins
	ImageProcessor::Zero(&houghSpace);
	short *houghspace = houghSpace.pixels;
	
	// initialize lookup tables for sin and cos
	// function does initialization only for the first call
	InitSinCosTables();
	
	// perform hough transform
	const int nEdgePoints = edgePoints.GetSize();
	for (int i = 0; i < nEdgePoints; i++)
	{
		const Vec2d &point = edgePoints[i];
		const Vec2d &direction = edgeDirections[i];
		
		const float angle = atan2f(direction.y, direction.x) + FLOAT_PI;
		const int t0 = int(angle * FLOAT_RAD2DEG + 0.5f);
		
		const int t1 = t0 - 5;
		const int t2 = t0 + 5;
		
		for (int t = t1; t <= t2; t++)
		{
			const float r = point.x * cos_table[t] + point.y * sin_table[t];
			const int r_ = int(r + m2f);
			
			houghspace[((t + 360) % 360) * m + r_]++;
		}
	}
	
	resultLines.Clear();
	resultHits.Clear();
	
	// find maxima
	for (int nn = 0; nn < nLinesToExtract; nn++)
	{
		short max = nMinHits - 1;
		int best_i = -1;
		
		for (int i = 0; i < nHoughPixels; i++)
		{
			if (houghspace[i] > max)
			{
				max = houghspace[i];
				best_i = i;
			}
		}
		
		if (best_i != -1)
		{
			const int t = best_i / m;
			const int r = best_i % m;
			
			const int w = 10;
			
			for (int j = -w; j <= w; j++)
			{
				for (int k = -w; k <= w; k++)
				{
					const int r_ = r + j;
					int t_ = t + k;
					
					if (t_ >= n)
						t_ -= n;
					
					if (t_ < 0)
						t_ += n;
					
					if (r_ >= 0 && r_ < m)
						houghspace[t_ * m + r_] = 0;
				}
			}
			
			const float theta = t * FLOAT_DEG2RAD;
			const int r_value = r - m2;
			
			StraightLine2d line(theta, -float(r_value));
			resultLines.AddElement(line);
			
			if (pVisualizationImage)
			{
				//printf("%i hits: (theta, r) = (%i, %i)\n", max, t, r_value);
				PrimitivesDrawer::DrawLine(pVisualizationImage, line, 255, 0, 0);
			}
		}
		else
			break;
	}
}

bool ImageProcessor::HoughTransformCircles(const CVec2dArray &edgePoints, const CVec2dArray &edgeDirections, int width, int height, int rmin, int rmax, int nCirclesToExtract, int nMinHits, CVec3dArray &resultCircles, CIntArray &resultHits, CByteImage *pVisualizationImage)
{
	// clear result list
	resultCircles.Clear();

	// perform detection
	CCircle2dArray resultCircles_;

	if (!HoughTransformCircles(edgePoints, edgeDirections, width, height, rmin, rmax, nCirclesToExtract, nMinHits, resultCircles_, resultHits, pVisualizationImage))
		return false;
	
	// fill result list
	const int nResultCircles = resultCircles_.GetSize();

	for (int i = 0; i < nResultCircles; i++)
	{
		const Circle2d &circle = resultCircles_[i];

		Vec3d x = { circle.center.x, circle.center.y, circle.radius };

		resultCircles.AddElement(x);
	}

	return true;
}

bool ImageProcessor::HoughTransformCircles(const CVec2dArray &edgePoints, const CVec2dArray &edgeDirections, int width, int height, int rmin, int rmax, int nCirclesToExtract, int nMinHits, CCircle2dArray &resultCircles, CIntArray &resultHits, CByteImage *pVisualizationImage)
{
	// clear result list
	resultCircles.Clear();

	if (rmin > rmax)
	{
		printf("error: rmin (%i) may not be greater than rmax (%i) for ImageProcessor::HoughTransformCircles\n", rmin, rmax);
		return false;
	}
	
	const int n = width + 2 * rmax;
	const int m = height + 2 * rmax;
	const int nHoughPixels = n * m * (rmax - rmin + 1);
	
	short *houghspace = new short[nHoughPixels];
	
	// reset bins
	memset(houghspace, 0, nHoughPixels * sizeof(short));
	
	// initialize lookup tables for sin and cos
	// function does initialization only for the first call
	InitSinCosTables();
	
	const int mn = m * n;
	short *helper = houghspace - rmin * mn + rmax * m + rmax;
	
	// perform hough transform
	const int nEdgePoints = edgePoints.GetSize();
	
	for (int i = 0; i < nEdgePoints; i++)
	{
		const Vec2d &point = edgePoints[i];
		const Vec2d &direction = edgeDirections[i];
		
		const float u = point.x + 0.5f;
		const float v = point.y + 0.5f;
		
		const float angle = atan2f(direction.y, direction.x) + FLOAT_PI;
		const int t0 = int(angle * FLOAT_RAD2DEG + 0.5f);
		
		const int t1 = t0 - 3;
		const int t2 = t0 + 3;
		
		for (int t = t1; t <= t2; t++)
		{
			for (int r = rmin; r <= rmax; r++)
			{
				int um, vm;
				
				um = int(u + r * cos_table[t]);
				vm = int(v + r * sin_table[t]);
				helper[r * mn + um * m + vm]++;
				
				um = int(u - r * cos_table[t]);
				vm = int(v - r * sin_table[t]);
				helper[r * mn + um * m + vm]++;
				
				//const int offset = (r - rmin) * mn + (rmax + um) * m + rmax + vm;
			}
		}
	}
	
	resultCircles.Clear();
	
	// find maxima
	for (int nn = 0; nn < nCirclesToExtract; nn++)
	{
		int max = nMinHits - 1, best_i = -1;
		
		for (int i = 0; i < nHoughPixels; i++)
		{
			if (houghspace[i] > max)
			{
				max = houghspace[i];
				best_i = i;
			}
		}
		
		if (best_i != -1)
		{
			int um = (best_i % mn) / m;
			int vm = (best_i % mn) % m;
			int r = best_i / mn;
			
			const int w = 5;
			
			for (int j = -w; j <= w; j++)
			{
				for (int k = -w; k <= w; k++)
				{
					for (int l = -w; l <= w; l++)
					{
						int um_ = um + j;
						int vm_ = vm + k;
						int r_ = r + l;
						
						if (um_ >= 0 && um_ < n && vm >= 0 && vm < m && r_ >= 0 && r_ <= rmax - rmin)
							houghspace[r_ * mn + um_ * m + vm_] = 0;
					}
				}
			}
			
			um -= rmax;
			vm -= rmax;
			r += rmin;
			
			//printf("%i hits: (um vm) = (%i %i) --  r = = %i\n", max, um, vm, r);
			
			Circle2d circle;
			Math2d::SetVec(circle.center, float(um), float(vm));
			circle.radius = float(r);

			resultCircles.AddElement(circle);
			resultHits.AddElement(max);
			
			if (pVisualizationImage)
				PrimitivesDrawer::DrawCircle(pVisualizationImage, circle, 255, 0, 0, 1);
		}
		else
			break;
	}
	
	delete [] houghspace;

	return true;
}



bool ImageProcessor::DetermineHomography(const Vec2d *pSourcePoints, const Vec2d *pTargetPoints, int nPoints,
	float &a1, float &a2, float &a3, float &a4, float &a5, float &a6, float &a7, float &a8)
{
	Mat3d A;
	
	if (!LinearAlgebra::DetermineHomography(pSourcePoints, pTargetPoints, nPoints, A, false))
		return false;
	
	a1 = A.r1;
	a2 = A.r2;
	a3 = A.r3;
	a4 = A.r4;
	a5 = A.r5;
	a6 = A.r6;
	a7 = A.r7;
	a8 = A.r8;

	return true;
}

bool ImageProcessor::DetermineAffineTransformation(const Vec2d *pSourcePoints, const Vec2d *pTargetPoints, int nPoints,
	float &a1, float &a2, float &a3, float &a4, float &a5, float &a6)
{
	Mat3d A;
	
	if (!LinearAlgebra::DetermineAffineTransformation(pSourcePoints, pTargetPoints, nPoints, A, false))
		return false;
	
	a1 = A.r1;
	a2 = A.r2;
	a3 = A.r3;
	a4 = A.r4;
	a5 = A.r5;
	a6 = A.r6;

	return true;
}


bool ImageProcessor::NormalizeColor(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->type == CByteImage::eGrayScale)
	{
		printf("error: input image must be of type CByteImage::eRGB24 or CByteImage::eRGB24Split for ImageProcessor::NormalizeColor\n");
		return false;
	}
	
	if (!pInputImage->IsCompatible(pOutputImage))
	{
		printf("error: input and output image must be of same size and type for ImageProcessor::NormalizeColor\n");
		return false;
	}

	const int nPixels = pInputImage->width * pInputImage->height;
	const unsigned char *input = pInputImage->pixels;

	int redHistogram[256], greenHistogram[256], blueHistogram[256];
	float fNormalizeConstant = 255.0f / nPixels;
	int i;

	// reset bins
	memset(redHistogram, 0, sizeof(redHistogram));
	memset(greenHistogram, 0, sizeof(greenHistogram));
	memset(blueHistogram, 0, sizeof(blueHistogram));

	// calculate histograms
	int offset = 0;
	for (i = 0; i < nPixels; i++)
	{
		redHistogram[input[offset]]++;
		greenHistogram[input[offset + 1]]++;
		blueHistogram[input[offset + 2]]++;
		offset += 3;
	}

	// normalize histograms
	for (i = 1; i < 256; i++)
	{
		redHistogram[i] += redHistogram[i - 1];
		greenHistogram[i] += greenHistogram[i - 1];
		blueHistogram[i] += blueHistogram[i - 1];
	}

	for (i = 0; i < 256; i++)
	{
		redHistogram[i] = int(redHistogram[i] * fNormalizeConstant + 0.5f);
		greenHistogram[i] = int(greenHistogram[i] * fNormalizeConstant + 0.5f);
		blueHistogram[i] = int(blueHistogram[i] * fNormalizeConstant + 0.5f);
	}	

	// apply normalization
	if (pInputImage->type == CByteImage::eRGB24)
	{
		const unsigned char *input_r = input;
		const unsigned char *input_g = input_r + nPixels;
		const unsigned char *input_b = input_g + nPixels;		
		unsigned char *output_r = pOutputImage->pixels;
		unsigned char *output_g = output_r + nPixels;
		unsigned char *output_b = output_g + nPixels;
		
		for (int i = 0; i < nPixels; i++)
		{
			output_r[i] = redHistogram[input_r[i]];
			output_g[i] = greenHistogram[input_g[i]];
			output_b[i] = blueHistogram[input_b[i]];
		}
	}
	else if (pInputImage->type == CByteImage::eRGB24Split)
	{
		unsigned char *output = pOutputImage->pixels;
		
		for (int i = 0, offset = 0; i < nPixels; i++, offset += 3)
		{
			output[offset] = redHistogram[input[offset]];
			output[offset + 1] = greenHistogram[input[offset + 1]];
			output[offset + 2] = blueHistogram[input[offset + 2]];
		}
	}

	return true;
}


bool ImageProcessor::GaussianSmooth5x5(const CFloatMatrix *pInputImage, CFloatMatrix *pOutputImage, float fVariance)
{
	if (pInputImage->columns != pOutputImage->columns || pInputImage->rows != pOutputImage->rows)
	{
		printf("error: input and output matrix do not match for ImageProcessor::GaussianSmooth5x5\n");
		return false;
	}

	if (pInputImage->columns < 5 || pInputImage->rows < 5)
	{
		printf("error: matrices must be at least of size 5x5 for ImageProcessor::GaussianSmooth5x5\n");
		return false;
	}

	const int nKernelSize = 5;
	const int k = 2;//(nKernelSize - 1) / 2;
	
	float pFilter[nKernelSize];
	float sum = 0.0f;
	int i;
	
	// construct Gaussian kernel
	for (i = 0; i < nKernelSize; i++)
	{
		pFilter[i] = expf(-(i - k) * (i - k) / (2.0f * fVariance));
		sum += pFilter[i];
	}
  
	for (i = 0; i < nKernelSize; i++) 
		pFilter[i] /= sum;
	
	// create temp image if necessary
	CFloatMatrix *pTempImage = new CFloatMatrix(pInputImage);
	
	const int width = pInputImage->columns;
	const int height = pInputImage->rows;

	const float *input = pInputImage->data;
	float *temp = pTempImage->data;
	float *output = pOutputImage->data;
	
	int x, y, d, offset;
	
	
	// x direction
	for (y = 0, offset = 0; y < height; y++)
	{
		for (x = 0; x < k; x++, offset++)
		{
			float sum = 0.0f;
      
			for (d = -k; d < -x; d++) 
				sum += input[y * width] * pFilter[k + d];
      
			for (d = -x; d <= k; d++) 
				sum += input[offset + d] * pFilter[k + d];
      
			temp[offset] = sum;
		}
    
		// core loop (optimized)
		for (x = 2 * k; x < width; x++, offset++)
		{
			temp[offset] =
				input[offset - 2] * pFilter[0] +
				input[offset - 1] * pFilter[1] +
				input[offset] * pFilter[2] +
				input[offset + 1] * pFilter[3] +
				input[offset + 2] * pFilter[4];
		}
    
		for (x = width - k; x < width; x++, offset++)
		{
			float sum = 0.0f;
      
			for (d = -k; d < width - x; d++) 
				sum += input[offset + d] * pFilter[k + d];
      
			for (d = width - x; d <= k; d++) 
				sum += input[y * width + width - 1] * pFilter[k + d];
      
			temp[offset] = sum;
		}
	}
	
  
	// y direction
	for (y = 0; y < k; y++)
	{
		for (x = 0; x < width; x++)
		{
			float sum = 0.0f;
      
			for (d = -k; d < -y; d++) 
				sum += temp[x] * pFilter[k + d];
      
			for (d = -y; d <= k; d++) 
				sum += temp[(y + d) * width + x] * pFilter[k + d];
	
			output[y * width + x] = sum;
		}
	}
    
	// core loop (optimized)
	for (y = 2 * k, offset = k * width; y < height; y++)
	{
		for (x = 0; x < width; x++, offset++)
		{
			output[offset] =
				temp[offset - (width << 1)] * pFilter[0] +
				temp[offset - width] * pFilter[1] +
				temp[offset] * pFilter[2] +
				temp[offset + width] * pFilter[3] +
				temp[offset + (width << 1)] * pFilter[4];
		}
    }
	
	for (y = height - k; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			float sum = 0.0f;
		
			for (d = -k; d < height - y; d++) 
				sum += temp[(y + d) * width + x] * pFilter[k + d];
      
			for (d = height - y; d < k; d++) 
				sum += temp[(height - 1) * width + x] * pFilter[k + d];
      
			output[y * width + x] = sum;
		}
	}
	

	delete pTempImage;

	return true;
}

bool ImageProcessor::GaussianSmooth(const CByteImage *pInputImage, CByteImage *pOutputImage, float fVariance, int nKernelSize)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height || pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::GaussianSmooth\n");
		return false;
	}

	if (pInputImage->width < nKernelSize || pInputImage->height < nKernelSize)
	{
		printf("error: image must be at least of size nKernelSize x nKernelSize for ImageProcessor::GaussianSmooth\n");
		return false;
	}

	const int k = (nKernelSize - 1) / 2;
	
	float *pFilter = new float[nKernelSize];
	float sum = 0.0f;
	int i;
	
	// construct Gaussian kernel
	for (i = 0; i < nKernelSize; i++)
	{
		pFilter[i] = expf(-(i - k) * (i - k) / (2.0f * fVariance));
		sum += pFilter[i];
	}
  
	for (i = 0; i < nKernelSize; i++) 
		pFilter[i] /= sum;
	
	// create temp image
	CByteImage *pTempImage = new CByteImage(pInputImage);
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;

	const unsigned char *input = pInputImage->pixels;
	unsigned char *temp = pTempImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	int x, y, d, offset;
	
	
	// x direction
	for (y = 0, offset = 0; y < height; y++)
	{
		for (x = 0; x < k; x++, offset++)
		{
			float sum = 0.0f;
      
			for (d = -k; d < -x; d++) 
				sum += input[y * width] * pFilter[k + d];
      
			for (d = -x; d <= k; d++) 
				sum += input[offset + d] * pFilter[k + d];
      
			temp[offset] = (unsigned char) (sum + 0.5f);
		}
    
		// core loop
		for (x = 2 * k; x < width; x++, offset++)
		{
			float sum = 0.0f;

			for (d = -k; d <= k; d++)
				sum += input[offset + d] * pFilter[d + k];

			temp[offset] = (unsigned char) (sum + 0.5f);
		}
    
		for (x = width - k; x < width; x++, offset++)
		{
			float sum = 0.0f;
      
			for (d = -k; d < width - x; d++) 
				sum += input[offset + d] * pFilter[k + d];
      
			for (d = width - x; d <= k; d++) 
				sum += input[y * width + width - 1] * pFilter[k + d];
      
			temp[offset] = (unsigned char) (sum + 0.5f);
		}
	}
	
  
	// y direction
	for (y = 0; y < k; y++)
	{
		for (x = 0; x < width; x++)
		{
			float sum = 0.0f;
      
			for (d = -k; d < -y; d++) 
				sum += temp[x] * pFilter[k + d];
      
			for (d = -y; d <= k; d++) 
				sum += temp[(y + d) * width + x] * pFilter[k + d];
	
			output[y * width + x] = (unsigned char) (sum + 0.5f);
		}
	}
    
	// core loop
	for (y = 2 * k, offset = k * width; y < height; y++)
	{
		for (x = 0; x < width; x++, offset++)
		{
			float sum = 0.0f;

			for (d = -k; d <= k; d++)
				sum += temp[offset + d * width] * pFilter[d + k];

			output[offset] = (unsigned char) (sum + 0.5f);
		}
    }
	
	for (y = height - k; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			float sum = 0.0f;
		
			for (d = -k; d < height - y; d++) 
				sum += temp[(y + d) * width + x] * pFilter[k + d];
      
			for (d = height - y; d < k; d++) 
				sum += temp[(height - 1) * width + x] * pFilter[k + d];
      
			output[y * width + x] = (unsigned char) (sum + 0.5f);
		}
	}
	

	delete pTempImage;
	delete [] pFilter;

	return true;
}

bool ImageProcessor::GaussianSmooth(const CByteImage *pInputImage, CFloatMatrix *pOutputImage, float fVariance, int nKernelSize)
{
	if (pInputImage->width != pOutputImage->columns || pInputImage->height != pOutputImage->rows || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image and output matrix do not match for ImageProcessor::GaussianSmooth\n");
		return false;
	}

	if (pInputImage->width < nKernelSize || pInputImage->height < nKernelSize)
	{
		printf("error: image must be at least of size nKernelSize x nKernelSize for ImageProcessor::GaussianSmooth\n");
		return false;
	}

	const int k = (nKernelSize - 1) / 2;
	
	float *pFilter = new float[nKernelSize];
	float sum = 0.0f;
	int i;
	
	// construct Gaussian kernel
	for (i = 0; i < nKernelSize; i++)
	{
		pFilter[i] = expf(-(i - k) * (i - k) / (2.0f * fVariance));
		sum += pFilter[i];
	}
  
	for (i = 0; i < nKernelSize; i++) 
		pFilter[i] /= sum;
	
	// create temp image
	CFloatMatrix *pTempImage = new CFloatMatrix(pInputImage->width, pInputImage->height);
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;

	const unsigned char *input = pInputImage->pixels;
	float *temp = pTempImage->data;
	float *output = pOutputImage->data;
	
	int x, y, d, offset;
	
	
	// x direction
	for (y = 0, offset = 0; y < height; y++)
	{
		for (x = 0; x < k; x++, offset++)
		{
			float sum = 0.0f;
      
			for (d = -k; d < -x; d++) 
				sum += input[y * width] * pFilter[k + d];
      
			for (d = -x; d <= k; d++) 
				sum += input[offset + d] * pFilter[k + d];
      
			temp[offset] = sum;
		}
    
		// core loop
		for (x = 2 * k; x < width; x++, offset++)
		{
			float sum = 0.0f;

			for (d = -k; d <= k; d++)
				sum += input[offset + d] * pFilter[d + k];

			temp[offset] = sum;
		}
    
		for (x = width - k; x < width; x++, offset++)
		{
			float sum = 0.0f;
      
			for (d = -k; d < width - x; d++) 
				sum += input[offset + d] * pFilter[k + d];
      
			for (d = width - x; d <= k; d++) 
				sum += input[y * width + width - 1] * pFilter[k + d];
      
			temp[offset] = sum;
		}
	}
	
  
	// y direction
	for (y = 0; y < k; y++)
	{
		for (x = 0; x < width; x++)
		{
			float sum = 0.0f;
      
			for (d = -k; d < -y; d++) 
				sum += temp[x] * pFilter[k + d];
      
			for (d = -y; d <= k; d++) 
				sum += temp[(y + d) * width + x] * pFilter[k + d];
	
			output[y * width + x] = sum;
		}
	}
    
	// core loop
	for (y = 2 * k, offset = k * width; y < height; y++)
	{
		for (x = 0; x < width; x++, offset++)
		{
			float sum = 0.0f;

			for (d = -k; d <= k; d++)
				sum += temp[offset + d * width] * pFilter[d + k];

			output[offset] = sum;
		}
    }
	
	for (y = height - k; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			float sum = 0.0f;
		
			for (d = -k; d < height - y; d++) 
				sum += temp[(y + d) * width + x] * pFilter[k + d];
      
			for (d = height - y; d < k; d++) 
				sum += temp[(height - 1) * width + x] * pFilter[k + d];
      
			output[y * width + x] = sum;
		}
	}
	

	delete pTempImage;
	delete [] pFilter;

	return true;
}

bool ImageProcessor::GaussianSmooth(const CFloatMatrix *pInputImage, CFloatMatrix *pOutputImage, float fVariance, int nKernelSize)
{
	if (pInputImage->columns != pOutputImage->columns || pInputImage->rows != pOutputImage->rows)
	{
		printf("error: input and output matrix do not match for ImageProcessor::GaussianSmooth\n");
		return false;
	}

	if (pInputImage->columns < nKernelSize || pInputImage->rows < nKernelSize)
	{
		printf("error: matrix must be at least of size nKernelSize x nKernelSize for ImageProcessor::GaussianSmooth\n");
		return false;
	}

	const int k = (nKernelSize - 1) / 2;
	
	float *pFilter = new float[nKernelSize];
	float sum = 0.0f;
	int i;
	
	// construct Gaussian kernel
	for (i = 0; i < nKernelSize; i++)
	{
		pFilter[i] = expf(-(i - k) * (i - k) / (2.0f * fVariance));
		sum += pFilter[i];
	}
  
	for (i = 0; i < nKernelSize; i++) 
		pFilter[i] /= sum;
	
	// create temp image
	CFloatMatrix *pTempImage = new CFloatMatrix(pInputImage);
	
	const int width = pInputImage->columns;
	const int height = pInputImage->rows;

	const float *input = pInputImage->data;
	float *temp = pTempImage->data;
	float *output = pOutputImage->data;
	
	int x, y, d, offset;
	
	
	// x direction
	for (y = 0, offset = 0; y < height; y++)
	{
		for (x = 0; x < k; x++, offset++)
		{
			float sum = 0.0f;
      
			for (d = -k; d < -x; d++) 
				sum += input[y * width] * pFilter[k + d];
      
			for (d = -x; d <= k; d++) 
				sum += input[offset + d] * pFilter[k + d];
      
			temp[offset] = sum;
		}
    
		// core loop
		for (x = 2 * k; x < width; x++, offset++)
		{
			float sum = 0.0f;

			for (d = -k; d <= k; d++)
				sum += input[offset + d] * pFilter[d + k];

			temp[offset] = sum;
		}
    
		for (x = width - k; x < width; x++, offset++)
		{
			float sum = 0.0f;
      
			for (d = -k; d < width - x; d++) 
				sum += input[offset + d] * pFilter[k + d];
      
			for (d = width - x; d <= k; d++) 
				sum += input[y * width + width - 1] * pFilter[k + d];
      
			temp[offset] = sum;
		}
	}
	
  
	// y direction
	for (y = 0; y < k; y++)
	{
		for (x = 0; x < width; x++)
		{
			float sum = 0.0f;
      
			for (d = -k; d < -y; d++) 
				sum += temp[x] * pFilter[k + d];
      
			for (d = -y; d <= k; d++) 
				sum += temp[(y + d) * width + x] * pFilter[k + d];
	
			output[y * width + x] = sum;
		}
	}
    
	// core loop
	for (y = 2 * k, offset = k * width; y < height; y++)
	{
		for (x = 0; x < width; x++, offset++)
		{
			float sum = 0.0f;

			for (d = -k; d <= k; d++)
				sum += temp[offset + d * width] * pFilter[d + k];

			output[offset] = sum;
		}
    }
	
	for (y = height - k; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			float sum = 0.0f;
		
			for (d = -k; d < height - y; d++) 
				sum += temp[(y + d) * width + x] * pFilter[k + d];
      
			for (d = height - y; d < k; d++) 
				sum += temp[(height - 1) * width + x] * pFilter[k + d];
      
			output[y * width + x] = sum;
		}
	}
	

	delete pTempImage;
	delete [] pFilter;

	return true;
}

bool ImageProcessor::HighPassX3(const CFloatMatrix *pInputImage, CFloatMatrix *pOutputImage)
{
	if (pInputImage->columns != pOutputImage->columns || pInputImage->rows != pOutputImage->rows)
	{
		printf("error: input and output image do not match for ImageProcessor::HighPassX3\n");
		return false;
	}
	
	CFloatMatrix *pSaveOutputImage = 0;
	if (pInputImage->data == pOutputImage->data)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CFloatMatrix(pInputImage);
	}
	
	const float *input = pInputImage->data;
	float *output = pOutputImage->data;
  
	const int width = pInputImage->columns;
	const int height = pInputImage->rows;
  
	for (int y = 0, offset = 0; y < height; y++, offset += width)
	{
		output[offset] = input[offset + 1] - input[offset];
		
		const int max = offset + width - 1;
		
    
		for (int x = offset + 1; x < max; x++) 
			output[x] = (input[x + 1] - input[x - 1]) * 0.5f;
		
		output[offset + width - 1] = input[offset + width - 1] - input[offset + width - 2];
	}
	
	if (pSaveOutputImage)
	{
		CopyMatrix(pOutputImage, pSaveOutputImage);
		delete pOutputImage;
	}

	return true;
}

bool ImageProcessor::HighPassY3(const CFloatMatrix *pInputImage, CFloatMatrix *pOutputImage)
{
	if (pInputImage->columns != pOutputImage->columns || pInputImage->rows != pOutputImage->rows)
	{
		printf("error: input and output image do not match for ImageProcessor::HighPassY3\n");
		return false;
	}
	
	CFloatMatrix *pSaveOutputImage = 0;
	if (pInputImage->data == pOutputImage->data)
	{
		pSaveOutputImage = pOutputImage;
		pOutputImage = new CFloatMatrix(pInputImage);
	}
	
	const float *input = pInputImage->data;
	float *output = pOutputImage->data;
  
	const int width = pInputImage->columns;
	const int height = pInputImage->rows;
	
	int x;
	
	for (x = 0; x < width; x++)
		output[x] = input[x + width] - input[x];
	
	for (int y = 2, offset = width; y < height; y++)
		for (x = 0; x < width; x++, offset++)
			output[offset] = (input[offset + width] - input[offset - width]) * 0.5f;
			
	for (x = (height - 1) * width; x < width; x++)
		output[x] = input[x] - input[x - width];
		
	if (pSaveOutputImage)
	{
		CopyMatrix(pOutputImage, pSaveOutputImage);
		delete pOutputImage;
	}

	return true;
}

bool ImageProcessor::FlipY(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type)
	{
		printf("error: input and output image do not match for ImageProcessor::FlipY\n");
		return false;
	}
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;
	const int nPixels = width * height;
	
	unsigned char *input = pInputImage->pixels;
	
	if (pInputImage->pixels == pOutputImage->pixels)
	{
		const int h = height / 2;
		
		if (pInputImage->type == CByteImage::eGrayScale)
		{
			unsigned char *temp = new unsigned char[width];
			unsigned char *output = pOutputImage->pixels + nPixels - width;
			
			for (int y = 0; y < h; y++)
			{
				memcpy(temp, output, width);
				memcpy(output, input, width);
				memcpy(input, temp, width);
					
				input += width;
				output -= width;
			}
			
			delete [] temp;
		}
		else if (pInputImage->type == CByteImage::eRGB24)
		{
			const int nWidthBytes = 3 * width;
			
			unsigned char *temp = new unsigned char[nWidthBytes];
			unsigned char *output = pOutputImage->pixels + 3 * nPixels - nWidthBytes;
			
			for (int y = 0; y < h; y++)
			{
				memcpy(temp, output, nWidthBytes);
				memcpy(output, input, nWidthBytes);
				memcpy(input, temp, nWidthBytes);
					
				input += nWidthBytes;
				output -= nWidthBytes;
			}
			
			delete [] temp;
		}
	}
	else
	{
		if (pInputImage->type == CByteImage::eGrayScale)
		{
			unsigned char *output = pOutputImage->pixels + nPixels - width;
			
			for (int y = 0; y < height; y++)
			{
				memcpy(output, input, width);
					
				input += width;
				output -= width;
			}
		}
		else if (pInputImage->type == CByteImage::eRGB24)
		{
			const int nWidthBytes = 3 * width;
			unsigned char *output = pOutputImage->pixels + 3 * nPixels - nWidthBytes;
			
			for (int y = 0; y < height; y++)
			{
				memcpy(output, input, nWidthBytes);
					
				input += nWidthBytes;
				output -= nWidthBytes;
			}
		}
	}

	return true;
}

bool ImageProcessor::CalculateIntegralImage(const CByteImage *pInputImage, CIntImage *pIntegralImage)
{
	if (pInputImage->width != pIntegralImage->width || pInputImage->height != pIntegralImage->height || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::CalculateIntegralImage\n");
		return false;
	}
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;
	
	const unsigned char *input = pInputImage->pixels;
	int *output = pIntegralImage->pixels;
	
	// top left pixel
	*output = *input++;
	output++;
	
	// top row
	for (int x = 1; x < width; x++)
	{
		*output = *input++ + *(output - 1);
		output++;
	}
	
	// the other rows
	for (int y = 1; y < height; y++)
	{
		// first entry in a row
		*output = *input++ + *(output - width);
		output++;
		
		// rest of the row
		for (int x = 1; x < width; x++)
		{
			*output = *input++ + *(output - 1) + *(output - width) - *(output - width - 1);
			output++;
		}
	}

	return true;
}

bool ImageProcessor::CalculateSummedAreaTable(const CByteImage *pInputImage, CIntImage *pSummedAreaTable)
{
	if (pInputImage->width != pSummedAreaTable->width || pInputImage->height != pSummedAreaTable->height || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::CalculateSummedAreaTable\n");
		return false;
	}
	
	return CalculateIntegralImage(pInputImage, pSummedAreaTable);
}

// counts every pixel != 0 as 1
bool ImageProcessor::CalculateBinarizedSummedAreaTable(const CByteImage *pInputImage, CIntImage *pSummedAreaTable)
{
	if (pInputImage->width != pSummedAreaTable->width || pInputImage->height != pSummedAreaTable->height || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::CalculateBinarizedSummedAreaTable\n");
		return false;
	}
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;
	
	unsigned char *input = pInputImage->pixels;
	int *output = pSummedAreaTable->pixels;
	
	// top left pixel
	int c = *input++;
	c = (c != 0 ? 1 : 0);
	
	*output++ = c;
	
	// top row
	for (int x = 1; x < width; x++)
	{
		c = *input++;
		c = (c != 0 ? 1 : 0);
		
		*output = c + *(output-1);
		output++;
	}
	
	// the other rows
	for (int y = 1; y < height; y++)
	{
		// first entry in a row
		c = *input++;
		c = (c != 0 ? 1 : 0);
		
		*output = c + *(output - width);
		output++;
		
		// rest of the row
		for (int x = 1; x < width; x++)
		{
			c = *input++;
			c = (c != 0 ? 1 : 0);
			
			*output = c + *(output-1) + *(output - width) - *(output - width - 1);
			output++;
		}
	}

	return true;
}

bool ImageProcessor::CalculateReverseSummedAreaTable(const CIntImage *pSummedAreaTable, CByteImage *pOutputImage)
{
	if (pOutputImage->width != pSummedAreaTable->width || pOutputImage->height != pSummedAreaTable->height || pOutputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::CalculateReverseSummedAreaTable\n");
		return false;
	}
	
	const int width = pSummedAreaTable->width;
	const int height = pSummedAreaTable->height;
	
	int *input = pSummedAreaTable->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	// top left pixel
	int c = *input++;
	*output++ = c;
	
	// top row
	for (int x = 1; x < width; x++)
	{
		c = *input++;
		*output++ = c - *(input - 2);
	}
	
	// the other rows
	for (int y = 1; y < height; y++)
	{
		// first entry in a row
		c = *input++;
		*output++ = c - *(input - width - 1);
		
		// rest of the row
		for (int x = 1; x < width; x++)
		{
			c = *input++;
			*output++ = c - *(input - 2) - *(input - width - 1) + *(input - width - 2);
		}
	}

	return true;
}

int ImageProcessor::GetAreaSum(const CIntImage *pIntegralImage, int min_x, int min_y, int max_x, int max_y)
{
	const int width = pIntegralImage->width;
	const int height = pIntegralImage->height;

	// to include the min_x column and the min_y row
	min_x--;
	min_y--;
	
	if (min_x >= width) min_x = width - 1;
	if (max_x < 0) max_x = 0;
	if (max_x >= width) max_x = width - 1;
	if (min_y >= height) min_y = height - 1;
	if (max_y < 0) max_y = 0;
	if (max_y >= height) max_y = height - 1;
	
	const int *pixels = pIntegralImage->pixels;
	const int c1 = min_x < 0 || min_y < 0 ? 0 : pixels[min_y * width + min_x];
	const int c2 = min_y < 0 ? 0 : pixels[min_y * width + max_x];
	const int c3 = min_x < 0 ? 0 : pixels[max_y * width + min_x];
	const int c4 = pixels[max_y * width + max_x];
	 
	return c4 - c3 - c2 + c1;
}

inline int ImageProcessor::GetAreaSumNoChecks(const CIntImage *pIntegralImage, int min_x, int min_y, int max_x, int max_y)
{
	// to include the min_x column and the min_y row
	min_x--;
	min_y--;
	
	const int width = pIntegralImage->width;
	const int *pixels = pIntegralImage->pixels;
	
	return pixels[max_y * width + max_x] - pixels[max_y * width + min_x] - pixels[min_y * width + max_x] + pixels[min_y * width + min_x];
}

int ImageProcessor::GetAreaSum(const CIntImage *pIntegralImage, const MyRegion *region)
{
	return GetAreaSum(pIntegralImage, region->min_x, region->min_y, region->max_x, region->max_y);
}


static void track(unsigned char *magnitudes, int *stack, int offset, int width)
{
	int sp = 0;
	
	stack[sp++] = offset;
	
	while (sp--)
	{
		const int offset = stack[sp];

		if (magnitudes[offset - width - 1] == 1)
		{
			stack[sp++] = offset - width - 1;
			magnitudes[offset - width - 1] = 255;
		}

		if (magnitudes[offset + width + 1] == 1)
		{
			stack[sp++] = offset + width + 1;
			magnitudes[offset + width + 1] = 255;
		}

		if (magnitudes[offset - width + 1] == 1)
		{
			stack[sp++] = offset - width + 1;
			magnitudes[offset - width + 1] = 255;
		}

		if (magnitudes[offset + width - 1] == 1)
		{
			stack[sp++] = offset + width - 1;
			magnitudes[offset + width - 1] = 255;
		}

		if (magnitudes[offset - width] == 1)
		{
			stack[sp++] = offset - width;
			magnitudes[offset - width] = 255;
		}

		if (magnitudes[offset + width] == 1)
		{
			stack[sp++] = offset + width;
			magnitudes[offset + width] = 255;
		}

		if (magnitudes[offset - 1] == 1)
		{
			stack[sp++] = offset - 1;
			magnitudes[offset - 1] = 255;
		}
				
		if (magnitudes[offset + 1] == 1)
		{
			stack[sp++] = offset + 1;
			magnitudes[offset + 1] = 255;
		}
	}
}

bool ImageProcessor::Canny(const CByteImage *pInputImage, CByteImage *pOutputImage, int nLowThreshold, int nHighThreshold)
{
	OPTIMIZED_FUNCTION_HEADER_4(Canny, pInputImage, pOutputImage, nLowThreshold, nHighThreshold)
	
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for ImageProcessor::Canny\n");
		return false;
	}

	if (nLowThreshold == 0) nLowThreshold = 1;
	if (nHighThreshold == 0) nHighThreshold = 1;

	const int width = pInputImage->width;
	const int height = pInputImage->height;
	const int nPixels = width * height;

	CShortImage gradientsX(width, height), gradientsY(width, height);
	SobelX(pInputImage, &gradientsX, false);
	SobelY(pInputImage, &gradientsY, false);

	Zero(pOutputImage);
	
	unsigned char *output = pOutputImage->pixels;
	short *magnitudes = gradientsX.pixels; // alias for the gradientX image
	
	int i;
	
	// calculate gradients and sectors
	for (i = 0; i < nPixels; i++)
	{
		const int gx = gradientsX.pixels[i];
		const int gy = gradientsY.pixels[i];
		const int agx = abs(gx);
		const int agy = abs(gy);
		const int g = agx + agy;
		//const int g = short(sqrtf(float(gx * gx + gy * gy)));
		
		magnitudes[i] = g;
		
		if (g >= nLowThreshold)
		{
			if (agx > (agy << 1))
			{
				output[i] = 10;	
			}
			else if ((agx << 1) > agy)
			{
				output[i] = gx * gy >= 0 ? 12 : 13;
			}
			else
			{
				output[i] = 11;
			}	
		}
	}
	
	// apply non maximal supression	
	for (i = 0; i < nPixels; i++)
	{
		const int g = (int) magnitudes[i];

		if (g >= nLowThreshold)
		{
			switch (output[i])
			{
				case 10:
					if (magnitudes[i + 1] <= g && magnitudes[i - 1] < g)
						output[i] = g >= nHighThreshold ? 254 : 1;
				break;

				case 11:
					if (magnitudes[i + width] <= g && magnitudes[i - width] < g)
						output[i] = g >= nHighThreshold ? 254 : 1;
				break;

				case 12:
					if (magnitudes[i + width + 1] <= g && magnitudes[i - width - 1] < g)
						output[i] = g >= nHighThreshold ? 254 : 1;
				break;
			
				case 13:
					if (magnitudes[i + width - 1] <= g && magnitudes[i - width + 1] < g)
						output[i] = g >= nHighThreshold ? 254 : 1;
				break;
			}
		}
	}

	// track
	int *stack = new int[nPixels];

	for (i = 0; i < nPixels; i++)
		if (output[i] == 254)
		{
			track(output, stack, i, width);
			output[i] = 255;
		}
	
	for (i = 0; i < nPixels; i++)
	{
		if (output[i] != 255)
			output[i] = 0;
	}
	
	delete [] stack;
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


static inline void CalculateSubpixel(const unsigned char *input, int width, int sector, Vec2d &point, Vec2d &direction)
{
	int a, b, c, n;
	
	if (sector == 0)
	{
		a = *input - *(input - 2);
		b = *(input + 1) - *(input - 1);
		c = *(input + 2) - *input;
		n = a - 2 * b + c;
		
		if (n != 0)
		{
			const float x = 0.5f * (a - c) / n;

			if (fabsf(x) < 1.0f)
				point.x += x;
		}
	}
	else if (sector == 1)
	{
		a = *input - *(input - (width << 1));
		b = *(input + width) - *(input - width);
		c = *(input + (width << 1)) - *input;
		n = a - 2 * b + c;
		
		if (n != 0)
		{
			const float y = 0.5f * (a - c) / n;

			if (fabsf(y) < 1.0f)
				point.y += y;
		}
	}
	else if (sector == 2)
	{
		a = *input - *(input - (width << 1) - 2);
		b = *(input + width + 1) - *(input - width - 1);
		c = *(input + (width << 1) + 2) - *input;
		n = a - 2 * b + c;
		
		if (n != 0)
		{
			const float xy = 0.35355339059327378637f * (a - c) / n;

			if (fabsf(xy) < 1.0f)
			{
				point.y += xy;
				point.x += xy;
			}
		}
	}
	else if (sector == 3)
	{
		a = *input - *(input - (width << 1) + 2);
		b = *(input + width - 1) - *(input - width + 1);
		c = *(input + (width << 1) - 2) - *input;
		n = a - 2 * b + c;
		
		if (n != 0)
		{
			const float xy = 0.35355339059327378637f * (a - c) / n;

			if (fabsf(xy) < 1.0f)
			{
				point.y += xy;
				point.x -= xy;
			}
		}
	}

	Math2d::SetVec(direction,
		(float) ((input[1] << 1) + input[1 - width] + input[1 + width] - (input[-1] << 1) - input[-1 - width] - input[-1 + width]),
		(float) ((input[width] << 1) + input[width - 1] + input[width + 1] - (input[-width] << 1) - input[-width - 1] - input[-width + 1]));
	Math2d::NormalizeVec(direction);
}

bool ImageProcessor::Canny(const CByteImage *pInputImage, CVec2dArray &resultPoints, CVec2dArray &resultDirections, int nLowThreshold, int nHighThreshold)
{
	OPTIMIZED_FUNCTION_HEADER_5(CannyList, pInputImage, resultPoints, resultDirections, nLowThreshold, nHighThreshold)
	
	if (pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image must be of type eGrayScale for ImageProcessor::Canny (list)\n");
		return false;
	}

	if (nLowThreshold == 0) nLowThreshold = 1;
	if (nHighThreshold == 0) nHighThreshold = 1;

	const int width = pInputImage->width;
	const int height = pInputImage->height;
	const int nPixels = width * height;

	CShortImage gradientsX(width, height), gradientsY(width, height);
	SobelX(pInputImage, &gradientsX, false);
	SobelY(pInputImage, &gradientsY, false);

	CByteImage tempImage(width, height, CByteImage::eGrayScale);
	CByteImage sectorImage(width, height, CByteImage::eGrayScale);
	Zero(&tempImage);
	Zero(&sectorImage);
	
	unsigned char *temp = tempImage.pixels;
	unsigned char *sectors = sectorImage.pixels;
	short *magnitudes = gradientsX.pixels; // alias for the gradientX image
	
	int i;

	// calculate gradients and sectors
	for (i = 0; i < nPixels; i++)
	{
		const int gx = gradientsX.pixels[i];
		const int gy = gradientsY.pixels[i];
		const int agx = abs(gx);
		const int agy = abs(gy);
		const int g = agx + agy;
		//const int g = short(sqrtf(float(gx * gx + gy * gy)));

		magnitudes[i] = g;
		
		if (g >= nLowThreshold)
		{
			if (agx > (agy << 1))
			{
				sectors[i] = 0;	
			}
			else if ((agx << 1) > agy)
			{
				sectors[i] = gx * gy >= 0 ? 2 : 3;
			}
			else
			{
				sectors[i] = 1;
			}	
		}
	}
	
	// apply non maximal supression	
	for (i = 0; i < nPixels; i++)
	{
		const int g = (int) magnitudes[i];

		if (g >= nLowThreshold)
		{
			switch (sectors[i])
			{
				case 0:
					if (magnitudes[i + 1] <= g && magnitudes[i - 1] < g)
						temp[i] = g >= nHighThreshold ? 254 : 1;
				break;

				case 1:
					if (magnitudes[i + width] <= g && magnitudes[i - width] < g)
						temp[i] = g >= nHighThreshold ? 254 : 1;
				break;

				case 2:
					if (magnitudes[i + width + 1] <= g && magnitudes[i - width - 1] < g)
						temp[i] = g >= nHighThreshold ? 254 : 1;
				break;
			
				case 3:
					if (magnitudes[i + width - 1] <= g && magnitudes[i - width + 1] < g)
						temp[i] = g >= nHighThreshold ? 254 : 1;
				break;
			}
		}
	}

	// track
	int *stack = new int[nPixels];

	for (i = 0; i < nPixels; i++)
		if (temp[i] == 254)
		{
			track(temp, stack, i, width);
			temp[i] = 255;
		}
	
	resultPoints.Clear();
	resultDirections.Clear();
	
	int offset;
	for (i = height - 4, offset = 2 * width + 2; i; i--, offset += 4)
		for (int j = width - 4; j; j--, offset++)
			if (temp[offset] == 255)
			{
				Vec2d point;
				Math2d::SetVec(point, (float) (offset % width), (float) (offset / width));

				Vec2d direction;
				CalculateSubpixel(pInputImage->pixels + offset, width, sectors[offset], point, direction);

				resultPoints.AddElement(point);
				resultDirections.AddElement(direction);
			}
	
	delete [] stack;
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}



#define HARRIS_WINDOW_SIZE 3 // don't change this define

bool ImageProcessor::CalculateHarrisMap(const CByteImage *pInputImage, CIntImage *pOutputImage)
{
	if (pInputImage->type != CByteImage::eGrayScale || pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height)
	{
		printf("error: input image and output matrix do not match for ImageProcessor::CalculateHarrisMap\n");
		return false;
	}
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;
	const int nPixels = width * height;
	
	const unsigned char *input = pInputImage->pixels;

	int i, j, offset;

	int *cov = new int[3 * nPixels];
	
	// calculate gradients
	const int stop = nPixels - width - 1;
	for (offset = width + 1; offset < stop; offset++)
	{
		const int p = int(input[offset + 1]) - int(input[offset - 1]);
		const int q = int(input[offset + width]) - int(input[offset - width]);
		const int offset_ = 3 * offset;

		cov[offset_    ] = p * p;
		cov[offset_ + 1] = q * q;
		cov[offset_ + 2] = p * q;
	}

	// zero R image
	Zero(pOutputImage);

	// calculate harris response function
	const int diff = HARRIS_WINDOW_SIZE + 1;
	const int width3 = 3 * width;
	int *R = pOutputImage->pixels + (HARRIS_WINDOW_SIZE / 2) * (width + 1);

	// do not use one-pixel-thick border of cov (gradients are not valid there)
	for (i = diff, offset = width + 1; i < height; i++, offset += diff)
	{
		for (j = diff; j < width; j++, offset++)
		{
			int ppsum = 0;
			int qqsum = 0;
			int pqsum = 0;

			for (int k = 0, offset2 = 3 * offset; k < HARRIS_WINDOW_SIZE; k++, offset2 += width3)
			{
				ppsum += cov[offset2];
				qqsum += cov[offset2 + 1];
				pqsum += cov[offset2 + 2];
				ppsum += cov[offset2 + 3];
				qqsum += cov[offset2 + 4];
				pqsum += cov[offset2 + 5];
				ppsum += cov[offset2 + 6];
				qqsum += cov[offset2 + 7];
				pqsum += cov[offset2 + 8];
			}
			
			// NOTE:
			// while abs()  should not be necessary for most targets (the squared result fits into 32bit),
			// some targets (known:  ti 64x) produce faulty code here.
			pqsum = abs(pqsum);
			
			ppsum >>= 4;
			qqsum >>= 4;
			pqsum >>= 4;
			
			//R[offset] = (ppsum * qqsum - pqsum * pqsum) - 0.04f * ((ppsum + qqsum) * (ppsum + qqsum));
			// approximation: 0.04f = 1 / 25. instead: 1 / 16
			R[offset] = (ppsum * qqsum - pqsum * pqsum) - (((ppsum + qqsum) >> 2) * ((ppsum + qqsum) >> 2));
		}
	}

	delete [] cov;

	return true;
}


static void QuicksortInverse(int *pOffsets, const int *pValues, int nLow, int nHigh)
{
	int i = nLow;
	int j = nHigh;
	
	const int x = pValues[pOffsets[(nLow + nHigh) >> 1]];

	while (i <= j)
	{    
		while (pValues[pOffsets[i]] > x) i++; 
		while (pValues[pOffsets[j]] < x) j--;
        
		if (i <= j)
		{
			const int temp = pOffsets[i];
			pOffsets[i] = pOffsets[j];
			pOffsets[j] = temp;
		
			i++; 
			j--;
		}
	}

	if (nLow < j) QuicksortInverse(pOffsets, pValues, nLow, j);
	if (i < nHigh) QuicksortInverse(pOffsets, pValues, i, nHigh);
}

int ImageProcessor::CalculateHarrisInterestPoints(const CByteImage *pInputImage, Vec2d *pInterestPoints, int nMaxPoints, float fQualityLevel, float fMinDistance)
{
	OPTIMIZED_FUNCTION_HEADER_5_RET(CalculateHarrisInterestPoints, pInputImage, pInterestPoints, nMaxPoints, fQualityLevel, fMinDistance)
		
	if (pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image should be grayscale ImageProcessor::CalculateHarrisInterestPoints\n");
		return -1;
	}
		
	const int width = pInputImage->width;
	const int height = pInputImage->height;
	const int nPixels = width * height;
	
	// calculate harris map
	CIntImage R(pInputImage->width, pInputImage->height);
	CalculateHarrisMap(pInputImage, &R);
	
	int *data = R.pixels;
	int i;

	// determine maximum value
	int max = 0;
	for (i = 0; i < nPixels; ++i)
	{
		if (data[i] > max)
			max = data[i];
	}

	int *pCandidateOffsets = new int[nPixels];
	int nCandidates = 0;
	
	// only accept good pixels
	max = int(max * fQualityLevel + 0.5f);
	for (i = 0; i < nPixels; i++)
	{
		if (data[i] >= max)
			pCandidateOffsets[nCandidates++] = i;
	}

	// sort by quality
	QuicksortInverse(pCandidateOffsets, data, 0, nCandidates - 1);

	// enforce distance constraint
	const int nMinDistance = int(fMinDistance + 0.5f);
	CByteImage image(width, height, CByteImage::eGrayScale);
	Zero(&image);
	int nInterestPoints = 0;
	for (i = 0; i < nCandidates && nInterestPoints < nMaxPoints; i++)
	{
		const int offset = pCandidateOffsets[i];
		
		const int x = offset % width;
		const int y = offset / width;

		bool bTake = true;

		const int minx = x - nMinDistance < 0 ? 0 : x - nMinDistance;
		const int miny = y - nMinDistance < 0 ? 0 : y - nMinDistance;
		const int maxx = x + nMinDistance >= width ? width - 1 : x + nMinDistance;
		const int maxy = y + nMinDistance >= height ? height - 1 : y + nMinDistance;
		const int diff = width - (maxx - minx + 1);

		for (int l = miny, offset2 = miny * width + minx; l <= maxy; l++, offset2 += diff)
			for (int k = minx; k <= maxx; k++, offset2++)
				if (image.pixels[l * width + k])
				{
					bTake = false;
					break;
				}

		if (bTake)
		{
			// store  point
			pInterestPoints[nInterestPoints].x = float(x);
			pInterestPoints[nInterestPoints].y = float(y);
			nInterestPoints++;

			// mark location in grid for distance constraint check
			image.pixels[offset] = 1;
		}
	}

	delete [] pCandidateOffsets;
	
	return nInterestPoints;
}


bool ImageProcessor::And(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_3(And, pInputImage1, pInputImage2, pOutputImage)
	
	if (!pInputImage1->IsCompatible(pInputImage2) || !pInputImage1->IsCompatible(pOutputImage))
	{
		printf("error: input images and output image do not match for ImageProcessor::And\n");
		return false;
	}
	
	const int nBytes = pInputImage1->width * pInputImage1->height * pInputImage1->bytesPerPixel;
	
	const unsigned char *input1 = pInputImage1->pixels;
	const unsigned char *input2 = pInputImage2->pixels;
	unsigned char *output = pOutputImage->pixels;
	for (int i = 0; i < nBytes; i++)
		output[i] = input1[i] & input2[i];
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::Xor(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_3(Xor, pInputImage1, pInputImage2, pOutputImage)
	
	if (!pInputImage1->IsCompatible(pInputImage2) || !pInputImage1->IsCompatible(pOutputImage))
	{
		printf("error: input images and output image do not match for ImageProcessor::Xor\n");
		return false;
	}
	
	const int nBytes = pInputImage1->width * pInputImage1->height * pInputImage1->bytesPerPixel;
	
	const unsigned char *input1 = pInputImage1->pixels;
	const unsigned char *input2 = pInputImage2->pixels;
	unsigned char *output = pOutputImage->pixels;
	for (int i = 0; i < nBytes; i++)
		output[i] = input1[i] ^ input2[i];
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::Or(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_3(Or, pInputImage1, pInputImage2, pOutputImage)
	
	if (!pInputImage1->IsCompatible(pInputImage2) || !pInputImage1->IsCompatible(pOutputImage))
	{
		printf("error: input images and output image do not match for ImageProcessor::Or\n");
		return false;
	}
	
	const int nBytes = pInputImage1->width * pInputImage1->height * pInputImage1->bytesPerPixel;
	
	const unsigned char *input1 = pInputImage1->pixels;
	const unsigned char *input2 = pInputImage2->pixels;
	unsigned char *output = pOutputImage->pixels;
	for (int i = 0; i < nBytes; i++)
		output[i] = input1[i] | input2[i];
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


bool ImageProcessor::Add(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_3(Add, pInputImage1, pInputImage2, pOutputImage)
	
	if (!pInputImage1->IsCompatible(pInputImage2) || !pInputImage1->IsCompatible(pOutputImage))
	{
		printf("error: input images and output image do not match for ImageProcessor::Add\n");
		return false;
	}
	
	const int nBytes = pInputImage1->width * pInputImage1->height * pInputImage1->bytesPerPixel;
	
	const unsigned char *input1 = pInputImage1->pixels;
	const unsigned char *input2 = pInputImage2->pixels;
	unsigned char *output = pOutputImage->pixels;

	for (int i = 0; i < nBytes; i++)
		output[i] = input1[i] + input2[i];
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::AddWithSaturation(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_3(AddWithSaturation, pInputImage1, pInputImage2, pOutputImage)
	
	if (!pInputImage1->IsCompatible(pInputImage2) || !pInputImage1->IsCompatible(pOutputImage))
	{
		printf("error: input images and output image do not match for ImageProcessor::AddAndSaturate\n");
		return false;
	}
		
	const int nBytes = pInputImage1->width * pInputImage1->height * pInputImage1->bytesPerPixel;
	
	const unsigned char *input1 = pInputImage1->pixels;
	const unsigned char *input2 = pInputImage2->pixels;
	unsigned char *output = pOutputImage->pixels;

	for (int i = 0; i < nBytes; i++)
		output[i] = MY_MIN((unsigned int) input1[i] + (unsigned int) input2[i], 255);
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::Subtract(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_3(Subtract, pInputImage1, pInputImage2, pOutputImage)
	
	if (!pInputImage1->IsCompatible(pInputImage2) || !pInputImage1->IsCompatible(pOutputImage))
	{
		printf("error: input images and output image do not match for ImageProcessor::Subtract\n");
		return false;
	}
	
	const int nBytes = pInputImage1->width * pInputImage1->height * pInputImage1->bytesPerPixel;
	
	const unsigned char *input1 = pInputImage1->pixels;
	const unsigned char *input2 = pInputImage2->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int i = 0; i < nBytes; i++)
		output[i] = input1[i] - input2[i];
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::SubtractWithSaturation(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_3(SubtractWithSaturation, pInputImage1, pInputImage2, pOutputImage)
	
	if (!pInputImage1->IsCompatible(pInputImage2) || !pInputImage1->IsCompatible(pOutputImage))
	{
		printf("error: input images and output image do not match for ImageProcessor::SubtractAndSaturate\n");
		return false;
	}
	
	const int nBytes = pInputImage1->width * pInputImage1->height * pInputImage1->bytesPerPixel;
	
	const unsigned char *input1 = pInputImage1->pixels;
	const unsigned char *input2 = pInputImage2->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int i = 0; i < nBytes; i++)
		output[i] = MY_MAX((int) input1[i] - (int) input2[i], 0);
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::AbsoluteDifference(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_3(AbsoluteDifference, pInputImage1, pInputImage2, pOutputImage)
	
	if (!pInputImage1->IsCompatible(pInputImage2) || !pInputImage1->IsCompatible(pOutputImage))
	{
		printf("error: input images and output image do not match for ImageProcessor::AbsoluteDifference\n");
		return false;
	}
		
	const int nBytes = pInputImage1->width * pInputImage1->height * pInputImage1->bytesPerPixel;
	
	const unsigned char *input1 = pInputImage1->pixels;
	const unsigned char *input2 = pInputImage2->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int i = 0; i < nBytes; i++)
		output[i] = abs((int) input1[i] - (int) input2[i]);
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::Average(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_3(Average, pInputImage1, pInputImage2, pOutputImage)
	
	if (!pInputImage1->IsCompatible(pInputImage2) || !pInputImage1->IsCompatible(pOutputImage))
	{
		printf("error: input images and output image do not match for ImageProcessor::Average\n");
		return false;
	}
		
	const int nBytes = pInputImage1->width * pInputImage1->height * pInputImage1->bytesPerPixel;
	
	const unsigned char *input1 = pInputImage1->pixels;
	const unsigned char *input2 = pInputImage2->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int i = 0; i < nBytes; i++)
		output[i] = ((unsigned int) input1[i] + (unsigned int) input2[i] + 1) >> 1;
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::Min(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_3(Min, pInputImage1, pInputImage2, pOutputImage)
	
	if (!pInputImage1->IsCompatible(pInputImage2) || !pInputImage1->IsCompatible(pOutputImage))
	{
		printf("error: input images and output image do not match for ImageProcessor::Min\n");
		return false;
	}
		
	const int nBytes = pInputImage1->width * pInputImage1->height * pInputImage1->bytesPerPixel;
	
	const unsigned char *input1 = pInputImage1->pixels;
	const unsigned char *input2 = pInputImage2->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int i = 0; i < nBytes; i++)
		output[i] = MY_MIN(input1[i], input2[i]);
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

bool ImageProcessor::Max(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
{
	OPTIMIZED_FUNCTION_HEADER_3(Max, pInputImage1, pInputImage2, pOutputImage)
	
	if (!pInputImage1->IsCompatible(pInputImage2) || !pInputImage1->IsCompatible(pOutputImage))
	{
		printf("error: input images and output image do not match for ImageProcessor::Max\n");
		return false;
	}
		
	const int nBytes = pInputImage1->width * pInputImage1->height * pInputImage1->bytesPerPixel;
	
	const unsigned char *input1 = pInputImage1->pixels;
	const unsigned char *input2 = pInputImage2->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int i = 0; i < nBytes; i++)
		output[i] = MY_MAX(input1[i], input2[i]);
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}


unsigned char ImageProcessor::MaxValue(const CByteImage *pInputImage)
{
	unsigned char max;
	
	OPTIMIZED_FUNCTION_HEADER_2(MaxValue, pInputImage, max)
	
	if (pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image must be of type eGrayScale for ImageProcessor::MaxValue\n");
		return 0;
	}
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const unsigned char *input = pInputImage->pixels;
	max = 0;
	
	for (int i = 0; i < nPixels; i++)
	{
		if (input[i] > max)
			max = input[i];
	}
	
	OPTIMIZED_FUNCTION_FOOTER
	
	return max;
}

short ImageProcessor::MaxValue(const CShortImage *pInputImage)
{
	short max;
	
	OPTIMIZED_FUNCTION_HEADER_2(MaxValue_Short, pInputImage, max)
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const short *input = pInputImage->pixels;
	max = SHRT_MIN;
	
	for (int i = 0; i < nPixels; i++)
	{
		if (input[i] > max)
			max = input[i];
	}
	
	OPTIMIZED_FUNCTION_FOOTER
	
	return max;
}

int ImageProcessor::MaxValue(const CIntImage *pInputImage)
{
	int max;
	
	OPTIMIZED_FUNCTION_HEADER_2(MaxValue_Int, pInputImage, max)
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const int *input = pInputImage->pixels;
	max = INT_MIN;
	
	for (int i = 0; i < nPixels; i++)
	{
		if (input[i] > max)
			max = input[i];
	}
	
	OPTIMIZED_FUNCTION_FOOTER
	
	return max;
}

unsigned char ImageProcessor::MinValue(const CByteImage *pInputImage)
{
	unsigned char min;
	
	OPTIMIZED_FUNCTION_HEADER_2(MinValue, pInputImage, min)
	
	if (pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image must be of type eGrayScale for ImageProcessor::MinValue\n");
		return 0;
	}
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const unsigned char *input = pInputImage->pixels;
	min = 255;
	
	for (int i = 0; i < nPixels; i++)
	{
		if (input[i] < min)
			min = input[i];
	}
	
	OPTIMIZED_FUNCTION_FOOTER
	
	return min;
}

short ImageProcessor::MinValue(const CShortImage *pInputImage)
{
	short min;
	
	OPTIMIZED_FUNCTION_HEADER_2(MinValue_Short, pInputImage, min)
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const short *input = pInputImage->pixels;
	min = SHRT_MAX;
	
	for (int i = 0; i < nPixels; i++)
	{
		if (input[i] < min)
			min = input[i];
	}
	
	OPTIMIZED_FUNCTION_FOOTER
	
	return min;
}

int ImageProcessor::MinValue(const CIntImage *pInputImage)
{
	int min;
	
	OPTIMIZED_FUNCTION_HEADER_2(MinValue_Int, pInputImage, min)
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const int *input = pInputImage->pixels;
	min = INT_MAX;
	
	for (int i = 0; i < nPixels; i++)
	{
		if (input[i] < min)
			min = input[i];
	}
	
	OPTIMIZED_FUNCTION_FOOTER
	
	return min;
}

bool ImageProcessor::MinMaxValue(const CByteImage *pInputImage, unsigned char &min, unsigned char &max)
{
	OPTIMIZED_FUNCTION_HEADER_3(MinMaxValue, pInputImage, min, max)
	
	if (pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image must be of type eGrayScale for ImageProcessor::MinMaxValue\n");
		return false;
	}
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const unsigned char *input = pInputImage->pixels;
	min = 255;
	max = 0;
	
	for (int i = 0; i < nPixels; i++)
	{
		if (input[i] < min)
			min = input[i];
		
		if (input[i] > max)
			max = input[i];
	}
	
	OPTIMIZED_FUNCTION_FOOTER

	return true;
}

void ImageProcessor::MinMaxValue(const CShortImage *pInputImage, short &min, short &max)
{
	OPTIMIZED_FUNCTION_HEADER_3(MinMaxValue_Short, pInputImage, min, max)
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const short *input = pInputImage->pixels;
	min = SHRT_MAX;
	max = SHRT_MIN;
	
	for (int i = 0; i < nPixels; i++)
	{
		if (input[i] < min)
			min = input[i];
		
		if (input[i] > max)
			max = input[i];
	}
	
	OPTIMIZED_FUNCTION_FOOTER
}

void ImageProcessor::MinMaxValue(const CIntImage *pInputImage, int &min, int &max)
{
	OPTIMIZED_FUNCTION_HEADER_3(MinMaxValue_Int, pInputImage, min, max)
	
	const int nPixels = pInputImage->width * pInputImage->height;
	const int *input = pInputImage->pixels;
	min = INT_MAX;
	max = INT_MIN;
	
	for (int i = 0; i < nPixels; i++)
	{
		if (input[i] < min)
			min = input[i];
		
		if (input[i] > max)
			max = input[i];
	}
	
	OPTIMIZED_FUNCTION_FOOTER
}


unsigned int ImageProcessor::PixelSum(const CByteImage *pImage)
{
	unsigned int sum = 0;

	OPTIMIZED_FUNCTION_HEADER_2(PixelSum, pImage, sum)

	if (pImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image must be of type eGrayScale for ImageProcessor::PixelSum\n");
		return -1;
	}

	const int nPixels = pImage->width * pImage->height;
	const unsigned char *pixels = pImage->pixels;
	
	for (int i = 0; i < nPixels; i++)
		sum += pixels[i];

	OPTIMIZED_FUNCTION_FOOTER
	
	return sum;
}




////////////////////////////////////////////////////////////////////////////////////////
// The Bayer pattern conversion source code is originally from the OpenCV.
// Below is the copyright notice:
////////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
////////////////////////////////////////////////////////////////////////////////////////

bool ImageProcessor::ConvertBayerPattern(const CByteImage *pBayerImage, CByteImage *pOutputImage, BayerPatternType type)
{
	if (pBayerImage->type != CByteImage::eGrayScale || pOutputImage->type != CByteImage::eRGB24)
	{
		printf("error: input image must be of type eGrayScale and output image of type eRGB24 for ImageProcessor::ConvertBayerPattern\n");
		return false;
	}
	
	const unsigned char *input = pBayerImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	const int width = pBayerImage->width;
	const int height = pBayerImage->height;
	const int width3 = 3 * width;
	
	int blue = type == eBayerGB || type == eBayerBG ? 1 : -1;
	bool bGreenFirst = type == eBayerGR || type == eBayerGB;
	
	memset(output, 0, width);
	memset(output + (height - 1) * width, 0, width);
	
	for (int i = 2, input_offset = 0, output_offset = width3 + 4; i < height; i++, input_offset += width, output_offset += width3)
	{
		const unsigned char *bayer = input + input_offset;
		const unsigned char *bayer_end = bayer + width - 4;
		unsigned char *dst = output + output_offset;
		
		dst[-4] = dst[-3] = dst[-2] = 0;
		
		if (bGreenFirst)
		{
			dst[blue] = (unsigned char) ((bayer[1] + bayer[2 * width + 1] + 1) >> 1);
			dst[0] = bayer[width + 1];
			dst[-blue] = (unsigned char) ((bayer[width] + bayer[width + 2] + 1) >> 1);
			
			bayer++;
			dst += 3;
		}
		
		if (blue > 0)
		{
			for (; bayer <= bayer_end; bayer += 2, dst += 6)
			{
				register int v = bayer[2] + bayer[2 * width + 2] + 1;
				
				dst[4] = (unsigned char) (v >> 1);
				dst[3] = bayer[width + 2];
				dst[2] = (unsigned char) ((bayer[width + 1] + bayer[width + 3] + 1) >> 1);
				
				dst[1] = (unsigned char) ((bayer[0] + bayer[2 * width] + v + 1) >> 2);
				dst[0] = (unsigned char) ((bayer[1] + bayer[width] + bayer[width + 2] + bayer[2 * width + 1] + 2) >> 2);
				dst[-1] = bayer[width + 1];
			}
		}
		else
		{
			for (; bayer <= bayer_end; bayer += 2, dst += 6)
			{
				register int v = bayer[2] + bayer[2 * width + 2] + 1;
				
				dst[2] = (unsigned char) (v >> 1);
				dst[3] = bayer[width + 2];
				dst[4] = (unsigned char) ((bayer[width + 1] + bayer[width + 3] + 1) >> 1);
				
				dst[-1] = (unsigned char) ((bayer[0] + bayer[2 * width] + v + 1) >> 2);
				dst[0] = (unsigned char) ((bayer[1] + bayer[width] + bayer[width + 2] + bayer[2 * width + 1] + 2) >> 2);
				dst[1] = bayer[width + 1];
			}
		}
		
		if (bayer < bayer_end + 2)
		{
			dst[blue] = (unsigned char) ((bayer[0] + bayer[2] + bayer[2 * width] + bayer[2 * width + 2] + 2) >> 2);
			dst[0] = (unsigned char) ((bayer[1] + bayer[width] + bayer[width + 2] + bayer[2 * width + 1] + 2) >> 2);
			dst[-blue] = bayer[width + 1];
			
			bayer++;
			dst += 3;
		}
		
		dst[-1] = dst[0] = dst[1] = 0;
		
		blue = -blue;
		bGreenFirst = !bGreenFirst;
	}
	
	// this removes the black frame of the image by copying the pixels from the inner frame (2 pixels from the border)
	// to the frame pixels
	AdaptFrame(pOutputImage, pOutputImage);

	return true;
}
