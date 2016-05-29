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
// Filename:  StereoVision.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "StereoVision.h"

#include "ImageProcessor.h"
#include "ByteImage.h"
#include "Calibration/StereoCalibration.h"
#include "Math/FloatMatrix.h"
#include "Helpers/OptimizedFunctions.h"
#include "Helpers/helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>



// *****************************************************************
// Constructor / Destructor
// *****************************************************************

CStereoVision::CStereoVision()
{
}

CStereoVision::~CStereoVision()
{
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CStereoVision::Process(const CByteImage *pLeftImage, const CByteImage *pRightImage, CByteImage *pDepthImage,
	int nWindowSize, int d1, int d2, int d_step, int nErrorThreshold)
{
	if (pLeftImage->width != pRightImage->width || pLeftImage->height != pRightImage->height ||
		pLeftImage->type != CByteImage::eGrayScale || pRightImage->type != CByteImage::eGrayScale ||
		pDepthImage->width != pLeftImage->width || pDepthImage->height != pLeftImage->height ||
		pDepthImage->type != CByteImage::eGrayScale)
	{
		printf("error: input images and/or output image do not match for CStereoVision::ProcessSSD\n");
		return false;
	}
	
	if (d_step == 0)
	{
		printf("error: d_step must not be zero for CStereoVision::ProcessSSD\n");
		return false;
	}
	
	if (d_step > 0 && d1 >= d2)
	{
		printf("error: if d_step > 0 then d2 must be greater than d1 for CStereoVision::ProcessSSD\n");
		return false;
	}
	
	if (d_step < 0 && d1 <= d2)
	{
		printf("error: if d_step < 0 then d1 must be greater than d2 for CStereoVision::ProcessSSD\n");
		return false;
	}
	
	ImageProcessor::Zero(pDepthImage);
	
	nErrorThreshold *= nWindowSize * nWindowSize;
	
	// adjust d2 so we can use != instead of <= or >=
	int d;
	if (d_step > 0)
		for (d = d1; d <= d2; d += d_step);
	else if (d_step < 0)
		for (d = d1; d >= d2; d += d_step);
	d2 = d + d_step;
	
	const int width = pLeftImage->width;
	const int height = pLeftImage->height;

	const unsigned char *pLeftImageData = pLeftImage->pixels;
	const unsigned char *pRightImageData = pRightImage->pixels;
	unsigned char *pDepthImageData = pDepthImage->pixels + (nWindowSize / 2) * (width + 1);
	
	const int max_i = height - nWindowSize + 1;
	const int max_j = width - nWindowSize + 1;
	
	const int start_j = MY_MAX(nWindowSize, d2);
	const int diff = width - (max_j - start_j);
	const int diff2 = width - nWindowSize;

	for (int i = 0, offset = start_j; i < max_i; i++, offset += diff)
	{
		for (int j = start_j; j < max_j; j++, offset++)
		{
			int best_error = INT_MAX;
			int best_d = 0;

			// find correlation
			for (int d = d1; d != d2; d += d_step)
			{
				int error = 0;

				for (int y = 0, offset2 = offset; y < nWindowSize; y++, offset2 += diff2)
					for (int x = 0; x < nWindowSize; x++, offset2++)
						error += abs(pLeftImageData[offset2] - pRightImageData[offset2 - d]);

				if (error < best_error)
				{
					best_error = error;
					best_d = d;
				}
			}

			pDepthImageData[i * width + j] = best_error < nErrorThreshold ? best_d : 0;
		}
	}

	return true;
}


bool CStereoVision::ProcessFast(const CByteImage *pLeftImage, const CByteImage *pRightImage, CByteImage *pDepthImage,
	int nWindowSize, int d1, int d2, int d_step, int nErrorThreshold)
{
	if (pLeftImage->width != pRightImage->width || pLeftImage->height != pRightImage->height ||
		pLeftImage->type != CByteImage::eGrayScale || pRightImage->type != CByteImage::eGrayScale ||
		pDepthImage->width != pLeftImage->width || pDepthImage->height != pLeftImage->height ||
		pDepthImage->type != CByteImage::eGrayScale)
	{
		printf("error: intput images and output image do not match in CStereoVision::ProcessFast\n");
		return false;
	}

	if (d_step == 0)
	{
		printf("error: d_step must not be zero for CStereoVision::ProcessFast\n");
		return false;
	}
	
	if (d_step > 0 && d1 >= d2)
	{
		printf("error: if d_step > 0 then d2 must be greater than d1 for CStereoVision::ProcessFast\n");
		return false;
	}
	
	if (d_step < 0 && d1 <= d2)
	{
		printf("error: if d_step < 0 then d1 must be greater than d2 for CStereoVision::ProcessFast\n");
		return false;
	}
	
	//OPTIMIZED_FUNCTION_HEADER_8(StereoVision, pLeftImage, pRightImage, pDepthImage, nWindowSize, d1, d2, d_step, nErrorThreshold)
	
	_ProcessFast(pLeftImage, pRightImage, pDepthImage, nWindowSize, d1, d2, d_step, nErrorThreshold);
	
	//OPTIMIZED_FUNCTION_FOOTER
	
	return true;
}


void CStereoVision::_ProcessFast(const CByteImage *pLeftImage, const CByteImage *pRightImage, CByteImage *pDepthImage,
	int nWindowSize, int d1, int d2, int d_step, int nErrorThreshold)
{
	ImageProcessor::Zero(pDepthImage);

	nErrorThreshold *= nWindowSize * nWindowSize;
	
	// adjust d2 so we can use != instead of <= or >=
	int d;
	if (d_step > 0)
		for (d = d1; d <= d2; d += d_step);
	else if (d_step < 0)
		for (d = d1; d >= d2; d += d_step);
	d2 = d + d_step;
		
	const int width = pLeftImage->width;
	const int height = pLeftImage->height;
	const int nPixels = width * height;
	
	// allocate memory
	int *H_SUM = new int[nWindowSize * width];
	int *V_SUM = new int[width];
	int *SAD = new int[width];
	int *MIN_SSAD = new int[width * height];
	
	for (int i = 0; i < nPixels; i++)
		MIN_SSAD[i] = nErrorThreshold;
	
	const int window_offset = - (nWindowSize / 2) * (width + 1);
	const int start_c = MY_MAX(nWindowSize, d2) + nWindowSize;
	
	const unsigned char *pLeftImageData = pLeftImage->pixels;
	const unsigned char *pRightImageData = pRightImage->pixels;
	unsigned char *pDepthImageData = pDepthImage->pixels + window_offset;

	// run correlation algorithm
	for (d = d1; d != d2; d += d_step)
	{
		int c, r;
		
		// initialize
		for (r = 0; r < nWindowSize; r++)
		{
			const unsigned char *pLeftImageData_helper = pLeftImageData + r * width;
			const unsigned char *pRightImageData_helper = pRightImageData + r * width - d;
			
			H_SUM[r * width + start_c - 1] = 0;

			for (c = start_c - nWindowSize; c < start_c; c++)
			{
				SAD[c] = abs(pLeftImageData_helper[c] - pRightImageData_helper[c]);
				H_SUM[r * width + start_c - 1] += SAD[c];
			}
			
			for (c = start_c; c < width; c++)
			{
				SAD[c] = abs(pLeftImageData_helper[c] - pRightImageData_helper[c]);
				H_SUM[r * width + c] = H_SUM[r * width + c - 1] + SAD[c] - SAD[c - nWindowSize];
			}
		}

		int *pMinHelper = MIN_SSAD + (nWindowSize - 1) * width; // for topmost_row
		unsigned char *pDepthHelper = pDepthImageData + (nWindowSize - 1) * width; // for topmost_row
		for (c = start_c - 1; c < width; c++)
		{
			V_SUM[c] = 0;
			for (r = 0; r < nWindowSize; r++)
				V_SUM[c] += H_SUM[r * width + c];

			// required for writing topmost row
			// <topmost_row>
			if (V_SUM[c] < pMinHelper[c])
			{
				pMinHelper[c] = V_SUM[c];
				pDepthHelper[c] = abs(d);
			}
			// </topmost_row>
		}
		
		// go
		for (r = nWindowSize; r < height; r++)
		{
			const unsigned char *pLeftImageData_helper = pLeftImageData + r * width;
			const unsigned char *pRightImageData_helper = pRightImageData + r * width - d;
			
			int *hsum_helper = H_SUM + (r % nWindowSize) * width;
			int *pMinHelper = MIN_SSAD + r * width;
			unsigned char *pDepthHelper = pDepthImageData + r * width;
			
			const int save = hsum_helper[start_c - 1]; // for leftmost_column
			hsum_helper[start_c - 1] = 0;
			for (c = start_c - nWindowSize; c < start_c; c++)
			{
				SAD[c] = abs(pLeftImageData_helper[c] - pRightImageData_helper[c]);
				hsum_helper[start_c - 1] += SAD[c];
			}

			// required for writing leftmost column
			// <leftmost_column>
			const int result = V_SUM[start_c - 1] = V_SUM[start_c - 1] + hsum_helper[start_c - 1] - save;
			if (result < pMinHelper[start_c - 1])
			{
				pMinHelper[start_c - 1] = result;
				pDepthHelper[start_c - 1] = abs(d);
			}
			// </leftmost_column>

			// core loop
			for (c = start_c; c < width; c++)
			{
				SAD[c] = abs(pLeftImageData_helper[c] - pRightImageData_helper[c]);

				const int save = hsum_helper[c];
				hsum_helper[c] = hsum_helper[c - 1] + SAD[c] - SAD[c - nWindowSize];
				const int result = V_SUM[c] = V_SUM[c] + hsum_helper[c] - save;

				if (result < pMinHelper[c])
				{
					pMinHelper[c] = result;
					pDepthHelper[c] = abs(d);
				}
			}
		}
	}
	
	// free memory
	delete [] H_SUM;
	delete [] V_SUM;
	delete [] SAD;
	delete [] MIN_SSAD;
}
