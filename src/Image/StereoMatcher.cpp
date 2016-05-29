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
// Filename:  StereoMatcher.cpp
// Author:    Pedram Azad
// Date:      12.03.2007
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "StereoMatcher.h"

#include "Image/ByteImage.h"
#include "Calibration/StereoCalibration.h"
#include "Calibration/Calibration.h"
#include "Math/Math2d.h"
#include "Math/Math3d.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <float.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CStereoMatcher::CStereoMatcher()
{
	m_pStereoCalibration = new CStereoCalibration();
	m_bOwnStereoCalibrationObject = true;
}

CStereoMatcher::~CStereoMatcher()
{
	if (m_bOwnStereoCalibrationObject)
		delete m_pStereoCalibration;
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CStereoMatcher::LoadCameraParameters(const char *pFileName)
{
	if (!m_bOwnStereoCalibrationObject)
		m_pStereoCalibration = new CStereoCalibration();
	
	return m_pStereoCalibration->LoadCameraParameters(pFileName);
}

void CStereoMatcher::InitCameraParameters(CStereoCalibration *pStereoCalibration, bool bCloneCalibration)
{
	if (bCloneCalibration)
	{
		if (!m_bOwnStereoCalibrationObject)
			m_pStereoCalibration = new CStereoCalibration();
		
		m_pStereoCalibration->Set(*pStereoCalibration);
		m_bOwnStereoCalibrationObject = true;
	}
	else
	{
		if (m_bOwnStereoCalibrationObject)
			delete m_pStereoCalibration;
		
		m_pStereoCalibration = pStereoCalibration;
		m_bOwnStereoCalibrationObject = false;
	}
}

int CStereoMatcher::GetDisparityEstimate(const float z)
{
	const Vec3d z_vector = { 0, 0, z };
	Vec3d p;
	m_pStereoCalibration->GetLeftCalibration()->CameraToWorldCoordinates(z_vector, p);

	Vec2d left, right;
	m_pStereoCalibration->GetLeftCalibration()->WorldToImageCoordinates(p, left, false);
	m_pStereoCalibration->GetRightCalibration()->WorldToImageCoordinates(p, right, false);
	
	return int(fabsf(left.x - right.x));
}

int CStereoMatcher::Match(const CByteImage *pLeftImage, const CByteImage *pRightImage, int x, int y, int nWindowSize,
	int d1, int d2, Vec2d &result, Vec3d &result_3d, float fThreshold, bool bInputImagesAreUndistorted)
{
	if (pLeftImage->width != pRightImage->width || pLeftImage->height != pRightImage->height ||
		pLeftImage->type != CByteImage::eGrayScale || pRightImage->type != CByteImage::eGrayScale)
	{
		printf("error: input images do not match for CStereoMatcher::Match\n");
		return -100000;
	}

	float *values_ = new float[4 * pLeftImage->width];
	float *values = values_ + 2 * pLeftImage->width;
	
	int best_d = SingleZNCC(pLeftImage, pRightImage, x, y, nWindowSize, d1 - 1, d2 + 1, values);
	
	if (best_d > -100000 && best_d >= d1 && best_d <= d2 && values[best_d] > fThreshold)
	{
		const float y0 = values[best_d - 1];
		const float y1 = values[best_d];
		const float y2 = values[best_d + 1];
		const float xmin = (y0 - y2) / (2.0f * (y0 - 2.0f * y1 + y2));
		const float disparity = fabsf(xmin) < 0.5f ? best_d + xmin : best_d;
		
		float m, c;
		const Vec2d point_left = { float(x), float(y) };
		m_pStereoCalibration->CalculateEpipolarLineInRightImage(point_left, m, c);
		
		result.x = x - disparity;
		result.y = m * (x - disparity) + c;
		
		m_pStereoCalibration->Calculate3DPoint(point_left, result, result_3d, false, !bInputImagesAreUndistorted);
	}
	else
	{
		// adjust return value if threshold constraint was not met
		if (best_d > -100000 && best_d >= d1 && best_d <= d2)
			best_d = -100003;
	}
	
	delete [] values_;
	
	return best_d;
}

int CStereoMatcher::MatchZSAD(const CByteImage *pLeftImage, const CByteImage *pRightImage, int x, int y, int nWindowSize,
	int d1, int d2, Vec2d &result, Vec3d &result_3d, float fThreshold, bool bInputImagesAreUndistorted)
{
	if (pLeftImage->width != pRightImage->width || pLeftImage->height != pRightImage->height ||
		pLeftImage->type != CByteImage::eGrayScale || pRightImage->type != CByteImage::eGrayScale)
	{
		printf("error: input images do not match for CStereoMatcher::MatchSAD\n");
		return -100000;
	}

	float *values_ = new float[4 * pLeftImage->width];
	float *values = values_ + 2 * pLeftImage->width;
	
	int best_d = SingleZSAD(pLeftImage, pRightImage, x, y, nWindowSize, d1 - 1, d2 + 1, values);
	
	if (best_d > -100000 && best_d >= d1 && best_d <= d2 && 255.0f * values[best_d] < fThreshold)
	{
		const float y0 = values[best_d - 1];
		const float y1 = values[best_d];
		const float y2 = values[best_d + 1];
		const float xmin = (y0 - y2) / (2.0f * (y0 - 2.0f * y1 + y2));
		const float disparity = fabsf(xmin) < 0.5f ? best_d + xmin : best_d;
		
		float m, c;
		const Vec2d point_left = { float(x), float(y) };
		m_pStereoCalibration->CalculateEpipolarLineInRightImage(point_left, m, c);
		
		result.x = x - disparity;
		result.y = m * (x - disparity) + c;
		
		m_pStereoCalibration->Calculate3DPoint(point_left, result, result_3d, false, !bInputImagesAreUndistorted);
	}
	else
	{
		// adjust return value if threshold constraint was not met
		if (best_d > -100000 && best_d >= d1 && best_d <= d2)
			best_d = -100003;
	}
	
	delete [] values_;
	
	return best_d;
}


int CStereoMatcher::SingleZNCC(const CByteImage *pInputImageLeft, const CByteImage *pInputImageRight, int x, int y, int nWindowSize, int d1, int d2, float *values)
{
	const int width = pInputImageLeft->width;
	const int height = pInputImageLeft->height;
	const int w2 = nWindowSize / 2;
	
	if (x < w2 || x >= width - w2 || y < w2 || y >= height - w2)
		return -100001;
	
	const unsigned char *input_left = pInputImageLeft->pixels;
	const unsigned char *input_right = pInputImageRight->pixels;
	const int nVectorLength = nWindowSize * nWindowSize;
	
	float *vector1 = new float[nVectorLength];
	float *vector2 = new float[nVectorLength];
	
	const int offset = (y - w2) * width + (x - w2);
	const int diff = width - nWindowSize;
	const Vec2d camera_point = { float(x), float(y) };
	
	// calculate mean value
	float mean = 0.0f;
	for (int yy = 0, offset2 = offset, offset3 = 0; yy < nWindowSize; yy++, offset2 += diff)
		for (int x = 0; x < nWindowSize; x++, offset2++, offset3++)
		{
			vector1[offset3] = input_left[offset2];
			mean += vector1[offset3];
		}
	mean /= nVectorLength;
	
	// apply additive and muliplicative normalization
	int k;
	float factor = 0.0f;
	for (k = 0; k < nVectorLength; k++)
	{
		vector1[k] -= mean;
		factor += vector1[k] * vector1[k];
	}
	
	if (factor < 60 * nVectorLength)
	{
		delete [] vector1;
		delete [] vector2;
		return -100002;
	}
	
	factor = 1.0f / sqrtf(factor);
	for (k = 0; k < nVectorLength; k++)
		vector1[k] *= factor;
	
	float best_value = -FLT_MAX;
	int d, best_d = -100004;

	const int max_d = d2 < x ? d2 : x;
	
	float m, c;
	m_pStereoCalibration->CalculateEpipolarLineInRightImage(camera_point, m, c);
	
	// determine correspondence
	for (d = d1; d <= max_d; d++)
	{
		const int xx = x - d; // x-coordinate of center of window in right image
		const int yy = int(m * xx + c + 0.5f); // y-coordinate of center of window in right image
		
		if (xx < w2 || xx >= width - w2 || yy < w2 || yy >= height - w2)
			continue;
		
		const int offset_right = (yy - w2) * width + (xx - w2); // offset of top left corner of window in right image
		
		// determine mean value
		float mean = 0.0f;
		for (int y = 0, offset2 = offset_right, offset3 = 0; y < nWindowSize; y++, offset2 += diff)
			for (int x = 0; x < nWindowSize; x++, offset2++, offset3++)
			{
				vector2[offset3] = input_right[offset2];
				mean += vector2[offset3];
				
			}
		mean /= nVectorLength;
		
		// apply additive and multiplicative normalization
		float factor = 0.0f;
		for (k = 0; k < nVectorLength; k++)
		{
			vector2[k] -= mean;
			factor += vector2[k] * vector2[k];
		}

		if (factor < 60 * nVectorLength)
			continue;

		factor = 1.0f / sqrtf(factor);
		for (k = 0; k < nVectorLength; k++)
			vector2[k] *= factor;
		
		float value = 0.0f;
		for (k = 0; k < nVectorLength; k++)
			value += vector1[k] * vector2[k];

		// save correlation results for subpixel calculations
		values[d] = value;
		
		// determine maximum correlation value
		if (value > best_value)
		{
			best_value = value;
			best_d = d;
		}
	}
	
	delete [] vector1;
	delete [] vector2;
	
	return best_d;
}

int CStereoMatcher::SingleZSAD(const CByteImage *pInputImageLeft, const CByteImage *pInputImageRight, int x, int y, int nWindowSize, int d1, int d2, float *values)
{
	const int width = pInputImageLeft->width;
	const int height = pInputImageLeft->height;
	const int w2 = nWindowSize / 2;
	
	if (x < w2 || x >= width - w2 || y < w2 || y >= height - w2)
		return -100001;
	
	const unsigned char *input_left = pInputImageLeft->pixels;
	const unsigned char *input_right = pInputImageRight->pixels;
	const int nVectorLength = nWindowSize * nWindowSize;
	
	int *vector1 = new int[nVectorLength];
	int *vector2 = new int[nVectorLength];
	
	const int offset = (y - w2) * width + (x - w2);
	const int diff = width - nWindowSize;
	const Vec2d camera_point = { float(x), float(y) };
	
	// calculate mean value
	int mean1 = 0;
	for (int yy = 0, offset2 = offset, offset3 = 0; yy < nWindowSize; yy++, offset2 += diff)
		for (int x = 0; x < nWindowSize; x++, offset2++, offset3++)
		{
			vector1[offset3] = input_left[offset2];
			mean1 += vector1[offset3];
		}
	mean1 /= nVectorLength;
	
	// apply additive and muliplicative normalization
	float best_value = FLT_MAX;
	int d, best_d = -100004;

	const int max_d = d2 < x ? d2 : x;
	
	float m, c;
	m_pStereoCalibration->CalculateEpipolarLineInRightImage(camera_point, m, c);
	
	// determine correspondence
	for (d = d1; d <= max_d; d++)
	{
		const int xx = x - d; // x-coordinate of center of window in right image
		const int yy = int(m * xx + c + 0.5f); // y-coordinate of center of window in right image
		
		if (xx < w2 || xx >= width - w2 || yy < w2 || yy >= height - w2)
			continue;
		
		const int offset_right = (yy - w2) * width + (xx - w2); // offset of top left corner of window in right image
		
		// determine mean value
		int mean2 = 0;
		for (int y = 0, offset2 = offset_right, offset3 = 0; y < nWindowSize; y++, offset2 += diff)
			for (int x = 0; x < nWindowSize; x++, offset2++, offset3++)
			{
				vector2[offset3] = input_right[offset2];
				mean2 += vector2[offset3];
				
			}
		mean2 /= nVectorLength;
		
		int value_int = 0;
		for (int k = 0; k < nVectorLength; k++)
			value_int += abs((vector1[k] - mean1) - (vector2[k] - mean2));
		const float value = float(value_int) / (nVectorLength * 255.0f);

		// save correlation results for subpixel calculations
		values[d] = value;
		
		// determine maximum correlation value
		if (value < best_value)
		{
			best_value = value;
			best_d = d;
		}
	}
	
	delete [] vector1;
	delete [] vector2;
	
	return best_d;
}
