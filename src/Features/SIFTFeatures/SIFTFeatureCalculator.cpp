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
// Filename:  SIFTFeatureCalculator.cpp
// Author:    Pedram Azad, Lars Paetzold
// Date:      24.09.2008
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "SIFTFeatureCalculator.h"

#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"
#include "Math/FloatMatrix.h"
#include "Math/Constants.h"
#include "DataStructures/DynamicArray.h"
#include "SIFTFeatureEntry.h"

#include <math.h>



// ****************************************************************************
// Defines
// ****************************************************************************

#define S				3		// number of scales per octave
#define SIGMA0			1.0f	// initial sigma of each octave
#define PRESIGMA		0.5f	// sigma of the input image
#define PRESCALE		1.0f	// input will be prescaled, 2.0f doubles size, 0.5f half size
#define EDGE_THRESHOLD	10.0f


// ****************************************************************************
// Static variables
// ****************************************************************************

float CSIFTFeatureCalculator::edgethreshold_ = (EDGE_THRESHOLD + 1) * (EDGE_THRESHOLD + 1) / EDGE_THRESHOLD;
float CSIFTFeatureCalculator::k_ = powf(2.0f, 1.0f / S);
float CSIFTFeatureCalculator::diffk_ = sqrtf(k_ * k_ - 1);
float CSIFTFeatureCalculator::prescaledsigma_ = PRESIGMA * PRESCALE;
float CSIFTFeatureCalculator::diffsigma0_ = SIGMA0 * SIGMA0 - prescaledsigma_ * prescaledsigma_;

int CSIFTFeatureCalculator::m_bInitialized = false;
int CSIFTFeatureCalculator::SIFTPointers[256]; 
float CSIFTFeatureCalculator::SIFTWeights[256];
float CSIFTFeatureCalculator::SIFTOrientationWeights[256 * (MAX_SCALES + 1)];
float CSIFTFeatureCalculator::SIFTDescriptorWeights[256];
float CSIFTFeatureCalculator::SIFTDiffSigmas[MAX_SCALES];
float CSIFTFeatureCalculator::SIFTSigmas[MAX_SCALES];
int	  CSIFTFeatureCalculator::SIFTKernelRadius[MAX_SCALES];



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CSIFTFeatureCalculator::CSIFTFeatureCalculator(float fThreshold, int nOctaves)
{
	m_fThreshold = fThreshold;
	m_nOctaves = nOctaves;

	m_pResultList = 0;
	m_bManageMemory = true;
	
	InitializeVariables();
}

CSIFTFeatureCalculator::~CSIFTFeatureCalculator()
{
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CSIFTFeatureCalculator::InitializeVariables()
{
	if (!m_bInitialized)
	{
		int i, j, k, offset;

		for (j = 0, offset = 0; j < 16; j++)
		{
			for (i = 0; i < 16; i++, offset++)
			{
				// sigma = 8 according to paper (half of the window width, which is 16)
				// 2 * sigma * sigma = 128
				const int x = i / 4;
				const int y = j / 4;
				
				SIFTPointers[offset] = 8 * (4 * y + x);
				SIFTWeights[offset] = expf( -((j - 7.5f) * (j - 7.5f) + (i - 7.5f) * (i - 7.5f)) / 128.0f);
			}
		}

		// sigmas, diffsigmas and kernel sizes
		SIFTSigmas[0] = SIGMA0;
		// calculate Gaussians
		for (i = 1; i < S + 3; i++)
		{
			const float diffsigma = diffk_ * SIFTSigmas[i - 1];
			const int nKernelRadius = int(ceil(3 * diffsigma));
			const int nKernelSize = nKernelRadius * 2 + 1;
			
			SIFTSigmas[i] = SIFTSigmas[i - 1] * k_;
			SIFTDiffSigmas[i] = diffsigma;
			SIFTKernelRadius[i] = nKernelRadius;
		}

		// orientation weights
		float sigma = 1.5f * SIGMA0;
		for (k = 0, offset = 0; k <= MAX_SCALES; k++)
		{
			for (j = 0; j < 16; j++)
				for (i = 0; i < 16; i++)
					SIFTOrientationWeights[offset++] = expf(-((j - 7.5f) * (j - 7.5f) + (i - 7.5f) * (i - 7.5f)) / (2.0f * sigma * sigma));

			sigma *= k_;
		}
		
		// descriptor weights
		sigma = 8.0f;
		for (j = 0, offset = 0; j < 16; j++)
			for (i = 0; i < 16; i++)
				SIFTDescriptorWeights[offset++] = expf(-((j - 7.5f) * (j - 7.5f) + (i - 7.5f) * (i - 7.5f)) / (2.0f * sigma * sigma));
		
		m_bInitialized = true;
	}
}

int CSIFTFeatureCalculator::CalculateFeatures(const CByteImage *pImage, CDynamicArray *pResultList, bool bManageMemory)
{
	if (pImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image is not a grayscale image\n");
		return -1;
	}

	m_pResultList = pResultList;
	m_bManageMemory = bManageMemory;

	if (PRESCALE == 1.0f)
	{
		CFloatMatrix matrix(pImage->width, pImage->height);
		
		ImageProcessor::GaussianSmooth(pImage, &matrix, diffsigma0_ * diffsigma0_, 2 * int(ceil(3 * diffsigma0_)) + 1);
		
		FindScaleSpaceExtrema(&matrix, PRESCALE, m_nOctaves);
	}
	else
	{
		const int newWidth = int(pImage->width * PRESCALE);
		const int newHeight = int(pImage->height * PRESCALE);

		CFloatMatrix matrix(newWidth, newHeight);
		CByteImage image(newWidth, newHeight, CByteImage::eGrayScale);
		
		ImageProcessor::Resize(pImage, &image);
		ImageProcessor::GaussianSmooth(&image, &matrix, diffsigma0_ * diffsigma0_, 2 * int(ceil(3 * diffsigma0_)) + 1);
		
		FindScaleSpaceExtrema(&matrix, PRESCALE, m_nOctaves);
	}

	return pResultList->GetSize();
}


void CSIFTFeatureCalculator::DetermineDominatingOrientations(const CFloatMatrix *pAngleMatrix, const CFloatMatrix *pMagnitudeMatrix, CDynamicArrayTemplate<float> &orientations, bool bPerform80PercentCheck)
{
	const int nWindowSize = pAngleMatrix->columns;
	const int nPixels = nWindowSize * nWindowSize;
	int i;
	
	float histogram_space[36 + 2];
	float *histogram = histogram_space + 1;
  
	for (i = 0; i < 37; i++)
		histogram[i] = 0.0f;
	
	const float *angles = pAngleMatrix->data;
	const float *magnitudes = pMagnitudeMatrix->data;
	const float factor = 18 / FLOAT_PI;
	
	for (i = 0; i < nPixels; i++)
	{
		const int d = (int) (factor * angles[i] + 18);
		histogram[d] += magnitudes[i];
	}

	// add hits in bin 36 to bin 0
	histogram[0] += histogram[36];
	
	// find highest peak
	float max = 0.0f;
	int best_i = -1;
	for (i = 0; i < 36; i++)
		if (histogram[i] > max)
		{
			max = histogram[i];
			best_i = i;
		}

	// optimization trick
	histogram[-1] = histogram[35];
	histogram[36] = histogram[0];

	if (bPerform80PercentCheck)
	{
		// prepare 80% check
		max *= 0.8f;
		
		// store peaks
		for (i = 0; i < 36; i++)
		{
			if (histogram[i] >= max && histogram[i] > histogram[i + 1] && histogram[i] > histogram[i - 1])
			{
				// parabola fitting
				float angle = 10.0f * (i + 0.5f * (histogram[i + 1] - histogram[i - 1]) / (2.0f * histogram[i] - histogram[i + 1] - histogram[i - 1])) + 5.0f;
	      
				if (angle < 0.0f)
					angle += 360.0f;
	      
				orientations.AddElement(angle);
			}
		}
	}
	else
	{
		if (best_i != -1 && histogram[best_i] > histogram[best_i + 1] && histogram[best_i] > histogram[best_i - 1])
		{
			// parabola fitting
			float angle = 10.0f * (best_i + 0.5f * (histogram[best_i + 1] - histogram[best_i - 1]) / (2.0f * histogram[best_i] - histogram[best_i + 1] - histogram[best_i - 1])) + 5.0f;
      
			if (angle < 0.0f)
				angle += 360.0f;
      
			orientations.AddElement(angle);
		}
	}
}


void CSIFTFeatureCalculator::CreateSIFTDescriptors(const CFloatMatrix *pImage, CDynamicArray *pResultList, float xp, float yp, float scale, float sigma, const float *pOrientationWeights, bool bManageMemory, bool bPerform80PercentCheck)
{
	const int width = pImage->columns;
	const int height = pImage->rows;
	const float *input = pImage->data;

	// determine upper left corner of window and check boundaries
	int x0 = int(xp + 0.5f) - 9;
	int y0 = int(yp + 0.5f) - 9;

 	if (x0 < 0 || y0 < 0 || x0 + 17 >= width || y0 + 17 >= height) 
		return;
	
	// calculate gradients, angles and magnitude
	CFloatMatrix angleMatrix(16, 16);
	CFloatMatrix magnitudeMatrix(16, 16);
	float *angles = angleMatrix.data;
	float *magnitudes = magnitudeMatrix.data;
	const int diff = width - 16;
	
	for (int y = 0, input_offset = (y0 + 1) * width + x0 + 1, offset = 0; y < 16; y++, input_offset += diff)
	{
		for (int x = 0; x < 16; x++, input_offset++, offset++)
		{
			const float gx = input[input_offset + 1] - input[input_offset - 1];
			const float gy = input[input_offset + width] - input[input_offset - width];
			
			angles[offset] = atan2f(gy, gx);
			magnitudes[offset] = sqrtf(gx * gx + gy * gy) * pOrientationWeights[offset];
		}
	}
	
	// find dominating orientations
	CDynamicArrayTemplate<float> orientations(20);
	DetermineDominatingOrientations(&angleMatrix, &magnitudeMatrix, orientations, bPerform80PercentCheck);
	
	// apply descriptor weights (undo orientation weights)
	for (int i = 0; i < 256; i++)
		magnitudes[i] = magnitudes[i] / pOrientationWeights[i] * SIFTDescriptorWeights[i];

	// create SIFT descriptor for each dominating orientation
	const int nSize = orientations.GetSize();
	for (int k = 0; k < nSize; k++)
	{
		int i;
			
		CSIFTFeatureEntry *pEntry = new CSIFTFeatureEntry(xp / scale, yp / scale, orientations[k], sigma / scale);
		float *feature_vector = pEntry->m_pFeature;
		
		for (i = 0; i < 128; i++)
			feature_vector[i] = 0.0f;

		// create descriptor
		const float angle = -pEntry->angle * FLOAT_DEG2RAD;
		const float cosphi = cosf(angle);
		const float sinphi = sinf(angle);

		for (int y = 0, offset = 0; y < 16; y++)
		{
			for (int x = 0; x < 16; x++, offset++)
			{
				const int x_ = int(8.0f + cosphi * (x - 7.5f) - sinphi * (y - 7.5f));
				const int y_ = int(8.0f + sinphi * (x - 7.5f) + cosphi * (y - 7.5f));
				
				if (x_ >= 0 && x_ < 16 && y_ >= 0 && y_ < 16)
				{
					const int offset_ = y_ * 16 + x_;

					float phi = 1 + angles[offset] / FLOAT_PI; // [0, 2)
					phi += angle / FLOAT_PI; // [-2, 2)
					if (phi < 0.0f) phi += 2.0f; // [0, 2)
					int d = int(4.0f * phi);
					if (d > 7) d = 7;

					feature_vector[SIFTPointers[offset_] + d] += SIFTWeights[offset_] * magnitudes[offset];
				}
			}
		}
		
		// normalize and cut off entries > 0.2
		bool bChanged = false;
		float sum1 = 0.0f, sum2 = 0.0f;
		
		for (i = 0; i < 128; i++) 
			sum1 += feature_vector[i] * feature_vector[i];
		
		sum1 = sqrtf(sum1);
		
		for (i = 0; i < 128; i++)
		{
			feature_vector[i] /= sum1;
			
			if (feature_vector[i] > 0.2f)
			{ 
				feature_vector[i] = 0.2f;
				bChanged = true;
			}
			
			sum2 += feature_vector[i] * feature_vector[i];
		}
    
		if (bChanged)
		{
			sum2 = sqrtf(sum2);
			
			for (int i = 0; i < 128; i++) 
				feature_vector[i] /= sum2;
		}

		pResultList->AddElement(pEntry, false, bManageMemory);
	}
}

void CSIFTFeatureCalculator::CreateSIFTDescriptors(const CByteImage *pImage, CDynamicArray *pResultList, float xp, float yp, float scale, bool bManageMemory, bool bPerform80PercentCheck)
{
	const int width = pImage->width;
	const int height = pImage->height;
	const unsigned char *input = pImage->pixels;

	// determine upper left corner of window and check boundaries
	int x0 = int(xp + 0.5f) - 18 / 2;
	int y0 = int(yp + 0.5f) - 18 / 2;

 	if (x0 < 0 || y0 < 0 || x0 + 17 >= width || y0 + 17 >= height) 
		return;
	
	// calculate gradients, angles and magnitude
	CFloatMatrix angleMatrix(16, 16);
	CFloatMatrix magnitudeMatrix(16, 16);
	float *angles = angleMatrix.data;
	float *magnitudes = magnitudeMatrix.data;
	const int diff = width - 16;

	for (int y = 0, input_offset = (y0 + 1) * width + x0 + 1, offset = 0; y < 16; y++, input_offset += diff)
	{
		for (int x = 0; x < 16; x++, input_offset++, offset++)
		{
			const int gx = input[input_offset + 1] - input[input_offset - 1];
			const int gy = input[input_offset + width] - input[input_offset - width];
			
			angles[offset] = atan2f((float) gy, (float) gx);
			magnitudes[offset] = sqrtf(float(gx * gx + gy * gy)); // weighting with Gaussian is ommited in this version
		}
	}
	
	// find dominating orientations
	CDynamicArrayTemplate<float> orientations(20);
	DetermineDominatingOrientations(&angleMatrix, &magnitudeMatrix, orientations, bPerform80PercentCheck);

	// create SIFT descriptor for each dominating orientation
	const int nSize = orientations.GetSize();
	for (int k = 0; k < nSize; k++)
	{
		int i;
			
		CSIFTFeatureEntry *pEntry = new CSIFTFeatureEntry(xp / scale, yp / scale, orientations[k], scale);
		float *feature_vector = pEntry->m_pFeature;
		    
		for (i = 0; i < 128; i++)
			feature_vector[i] = 0.0f;

		// create descriptor
		const float angle = -pEntry->angle * FLOAT_DEG2RAD;
		const float cosphi = cosf(angle);
		const float sinphi = sinf(angle);

		for (int y = 0, offset = 0; y < 16; y++)
		{
			for (int x = 0; x < 16; x++, offset++)
			{
				const int x_ = int(8.0f + cosphi * (x - 7.5f) - sinphi * (y - 7.5f));
				const int y_ = int(8.0f + sinphi * (x - 7.5f) + cosphi * (y - 7.5f));
				
				if (x_ >= 0 && x_ < 16 && y_ >= 0 && y_ < 16)
				{
					const int offset_ = y_ * 16 + x_;

					float phi = 1 + angles[offset] / FLOAT_PI; // [0, 2)
					phi += angle / FLOAT_PI; // [-2, 2)
					if (phi < 0.0f) phi += 2.0f; // [0, 2)
					int d = int(4.0f * phi);
					if (d > 7) d = 7;

					feature_vector[SIFTPointers[offset_] + d] += SIFTWeights[offset_] * magnitudes[offset];
				}
			}
		}
		
		// normalize and cut off entries > 0.2f
		bool bChanged = false;
		float sum1 = 0.0f, sum2 = 0.0f;
		
		for (i = 0; i < 128; i++) 
			sum1 += feature_vector[i] * feature_vector[i];
		
		sum1 = 1.0f / sqrtf(sum1);
		
		for (i = 0; i < 128; i++)
		{
			feature_vector[i] *= sum1;
			
			if (feature_vector[i] > 0.2f)
			{ 
				feature_vector[i] = 0.2f;
				bChanged = true;
			}
			
			sum2 += feature_vector[i] * feature_vector[i];
		}
    
		if (bChanged)
		{
			sum2 = 1.0f / sqrtf(sum2);
			
			for (int i = 0; i < 128; i++) 
				feature_vector[i] *= sum2;
		}

		pResultList->AddElement(pEntry, false, bManageMemory);
	}
}

void CSIFTFeatureCalculator::CreateSIFTDescriptors(const CByteImage *pImage, CDynamicArrayTemplatePointer<CFeatureEntry> &resultList, float xp, float yp, float scale, bool bPerform80PercentCheck)
{
	const int width = pImage->width;
	const int height = pImage->height;
	const unsigned char *input = pImage->pixels;

	// determine upper left corner of window and check boundaries
	int x0 = int(xp + 0.5f) - 18 / 2;
	int y0 = int(yp + 0.5f) - 18 / 2;

 	if (x0 < 0 || y0 < 0 || x0 + 17 >= width || y0 + 17 >= height) 
		return;
	
	// calculate gradients, angles and magnitude
	CFloatMatrix angleMatrix(16, 16);
	CFloatMatrix magnitudeMatrix(16, 16);
	float *angles = angleMatrix.data;
	float *magnitudes = magnitudeMatrix.data;
	const int diff = width - 16;

	for (int y = 0, input_offset = (y0 + 1) * width + x0 + 1, offset = 0; y < 16; y++, input_offset += diff)
	{
		for (int x = 0; x < 16; x++, input_offset++, offset++)
		{
			const int gx = input[input_offset + 1] - input[input_offset - 1];
			const int gy = input[input_offset + width] - input[input_offset - width];
			
			angles[offset] = atan2f((float) gy, (float) gx);
			magnitudes[offset] = sqrtf(float(gx * gx + gy * gy)); // weighting with Gaussian is ommited in this version
		}
	}
	
	// find dominating orientations
	CDynamicArrayTemplate<float> orientations(20);
	DetermineDominatingOrientations(&angleMatrix, &magnitudeMatrix, orientations, bPerform80PercentCheck);

	// create SIFT descriptor for each dominating orientation
	const int nSize = orientations.GetSize();
	for (int k = 0; k < nSize; k++)
	{
		int i;
			
		CSIFTFeatureEntry *pEntry = new CSIFTFeatureEntry(xp / scale, yp / scale, orientations[k], scale);
		float *feature_vector = pEntry->m_pFeature;
		    
		for (i = 0; i < 128; i++)
			feature_vector[i] = 0.0f;

		// create descriptor
		const float angle = -pEntry->angle * FLOAT_DEG2RAD;
		const float cosphi = cosf(angle);
		const float sinphi = sinf(angle);

		for (int y = 0, offset = 0; y < 16; y++)
		{
			for (int x = 0; x < 16; x++, offset++)
			{
				const int x_ = int(8.0f + cosphi * (x - 7.5f) - sinphi * (y - 7.5f));
				const int y_ = int(8.0f + sinphi * (x - 7.5f) + cosphi * (y - 7.5f));
				
				if (x_ >= 0 && x_ < 16 && y_ >= 0 && y_ < 16)
				{
					const int offset_ = y_ * 16 + x_;

					float phi = 1 + angles[offset] / FLOAT_PI; // [0, 2)
					phi += angle / FLOAT_PI; // [-2, 2)
					if (phi < 0.0f) phi += 2.0f; // [0, 2)
					int d = int(4.0f * phi);
					if (d > 7) d = 7;

					feature_vector[SIFTPointers[offset_] + d] += SIFTWeights[offset_] * magnitudes[offset];
				}
			}
		}
		
		// normalize and cut off entries > 0.2f
		bool bChanged = false;
		float sum1 = 0.0f, sum2 = 0.0f;
		
		for (i = 0; i < 128; i++) 
			sum1 += feature_vector[i] * feature_vector[i];
		
		sum1 = 1.0f / sqrtf(sum1);
		
		for (i = 0; i < 128; i++)
		{
			feature_vector[i] *= sum1;
			
			if (feature_vector[i] > 0.2f)
			{ 
				feature_vector[i] = 0.2f;
				bChanged = true;
			}
			
			sum2 += feature_vector[i] * feature_vector[i];
		}
    
		if (bChanged)
		{
			sum2 = 1.0f / sqrtf(sum2);
			
			for (int i = 0; i < 128; i++) 
				feature_vector[i] *= sum2;
		}

		resultList.AddElement(pEntry);
	}
}


static void ScaleDown(const CFloatMatrix *pInputImage, CFloatMatrix *pOutputImage)
{
	const float *input = pInputImage->data;
	float *output = pOutputImage->data;
	
	const int output_width = pOutputImage->columns;
	const int output_height = pOutputImage->rows;
	const int width = pInputImage->columns;
	
	for (int v = 0, offset = 0, offset_ = 0; v < output_height; v++, offset_ += width)
	{
		for (int u = 0; u < output_width; u++, offset++, offset_ += 2)
			output[offset] = input[offset_];
	}
}

static void Diff(const CFloatMatrix *pInputImage1, const CFloatMatrix *pInputImage2, CFloatMatrix *pOutputImage)
{
	float *input1 = pInputImage1->data;
	float *input2 = pInputImage2->data;
	float *output = pOutputImage->data;
  
	const int nPixels = pInputImage1->rows * pInputImage1->columns;
  
	for (int i = 0; i < nPixels; i++)
		*output++ = *input1++ - *input2++;
}


void CSIFTFeatureCalculator::FindScaleSpaceExtrema(const CFloatMatrix *pImage, float scale, int nOctave)
{
	const int width = pImage->columns;
	const int height = pImage->rows;

	if (nOctave == 0 || width < 40 || height < 40)
		return;

	const int nBlurredImages = S + 3;
	const int nDOGImages = nBlurredImages - 1;
	
	int i;

	CFloatMatrix** ppBlurredImages = new CFloatMatrix*[nBlurredImages];
	CFloatMatrix** ppDOGImages = new CFloatMatrix*[nDOGImages];

	CFloatMatrix tempImage(height, width);

	// calculate Gaussians
	ppBlurredImages[0] = new CFloatMatrix(width, height);
	ImageProcessor::CopyMatrix(pImage, ppBlurredImages[0]);
	
	for (i = 1; i < nBlurredImages; i++)
	{
		ppBlurredImages[i] = new CFloatMatrix(width, height);		
		ImageProcessor::GaussianSmooth(ppBlurredImages[i - 1], ppBlurredImages[i], SIFTDiffSigmas[i] * SIFTDiffSigmas[i], 2 * SIFTKernelRadius[i] + 1);
	}

	// halfsize image with doubled sigma for later
	// ppBlurredImages[s] because ppBlurredImages[i] has (k^i * sigma)
	// therefore k^s * sigma = 2 * sigma as k^s = 2
	/*
	the octave is from sigma to 2 * sigma or k^0*sigma to k^s * sigma
	so we have s intervals from 0 to s having s+1 blurred images
	but for extrema detection we always need one image before and one after
	therefore if we want D(k^0*sigma) to D(k^s * sigma) we need
	D(k^-1*sigma) to D(k^s+1*sigma) having s+3 images.
	but we shift it to D(k^0*sigma) till D(k^s+2*digma)
	therefore our doubled sigma is like said before at k^s*sigma.
	but important is that we have to go from 0 to s+2.
	*/
	
	// calculate Difference of Gaussians (DoG)
	for (i = 0; i < nDOGImages; i++)
	{
		ppDOGImages[i] = new CFloatMatrix(width, height);
		Diff(ppBlurredImages[i + 1], ppBlurredImages[i], ppDOGImages[i]);
	}
		
	// find scale space extrema
	const float fThreshold = m_fThreshold * 255.0f;
	for (i = 1; i < nDOGImages - 1; i++)
	{
		const float *dm = ppDOGImages[i - 1]->data;
		const float *d	= ppDOGImages[i]->data;
		const float *dp = ppDOGImages[i + 1]->data;
		
		for (int y = 1, p = width + 1; y < height - 1; y++, p += 2)
		{
			for (int x = 1; x < width - 1; x++, p++)
			{
				const float dv = d[p];
				
				if (fabsf(dv) > fThreshold &&
					((
					dv > dp[p] && 
					dv > dp[p - width - 1] && 
					dv > dp[p - width] && 
					dv > dp[p - width + 1] && 
					dv > dp[p - 1] && 
					dv > dp[p + 1] && 
					dv > dp[p + width - 1] && 
					dv > dp[p + width] && 
					dv > dp[p + width + 1] &&

					dv > dm[p] &&
					dv > dm[p - width - 1] && 
					dv > dm[p - width] && 
					dv > dm[p - width + 1] && 
					dv > dm[p - 1] && 
					dv > dm[p + 1] && 
					dv > dm[p + width - 1] && 
					dv > dm[p + width] && 
					dv > dm[p + width + 1] &&

					dv > d[p - width - 1] && 
					dv > d[p - width] && 
					dv > d[p - width + 1] && 
					dv > d[p - 1] && 
					dv > d[p + 1] && 
					dv > d[p + width - 1] && 
					dv > d[p + width] && 
					dv > d[p + width + 1]
					)
					||
					(
					dv < dp[p] && 
					dv < dp[p - width - 1] && 
					dv < dp[p - width] && 
					dv < dp[p - width + 1] && 
					dv < dp[p - 1] && 
					dv < dp[p + 1] && 
					dv < dp[p + width - 1] && 
					dv < dp[p + width] && 
					dv < dp[p + width + 1] &&

					dv < dm[p] &&
					dv < dm[p - width - 1] && 
					dv < dm[p - width] && 
					dv < dm[p - width + 1] && 
					dv < dm[p - 1] && 
					dv < dm[p + 1] && 
					dv < dm[p + width - 1] && 
					dv < dm[p + width] && 
					dv < dm[p + width + 1] &&

					dv < d[p - width - 1] && 
					dv < d[p - width] && 
					dv < d[p - width + 1] && 
					dv < d[p - 1] && 
					dv < d[p + 1] && 
					dv < d[p + width - 1] && 
					dv < d[p + width] && 
					dv < d[p + width + 1]
					))
					)
				{
					 // eliminating edge responses
					const float Dxx = (d[p + 1] + d[p - 1] - 2.0f * d[p]);
					const float Dyy = (d[p + width] + d[p - width] - 2.0f * d[p]);
					const float Dxy = 0.25f * (d[p + 1 + width] + d[p - 1 - width] - d[p - 1 + width] - d[p + 1 - width]);
					const float score = (Dxx + Dyy) * (Dxx + Dyy) / (Dxx * Dyy - Dxy * Dxy); 

					if (fabsf(score) < edgethreshold_)	
						CreateSIFTDescriptors(ppBlurredImages[i], m_pResultList, float(x), float(y), scale, SIFTSigmas[i], SIFTOrientationWeights + i * 256, m_bManageMemory);
				}
			}
		}
	}

	CFloatMatrix scaled_image(width / 2, height / 2);
	ScaleDown(ppBlurredImages[S], &scaled_image);

	// free images
	for (i = 0; i < nBlurredImages; i++)
		delete ppBlurredImages[i];
	for (i = 0; i < nDOGImages; i++)
		delete ppDOGImages[i];
	
	FindScaleSpaceExtrema(&scaled_image, scale / 2, nOctave - 1);
}
