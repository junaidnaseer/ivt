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
// Filename:  HarrisSIFTFeatureCalculator.cpp
// Author:    Pedram Azad
// Date:      20.11.2007
// ****************************************************************************

// ******************************************************************************************************
// Implementation of the paper:
// P. Azad, T. Asfour, R. Dillmann,
// "Combining Harris Interest Points and the SIFT Descriptor for Fast Scale-Invariant Object Recognition"
// IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS),
// St. Louis, USA, pp. 4275-4280, 2009.
// ******************************************************************************************************

#include "HarrisSIFTFeatureCalculator.h"

#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"
#include "Math/FloatMatrix.h"

#include "DataStructures/DynamicArray.h"
#include "Features/SIFTFeatures/SIFTFeatureCalculator.h"
#include "Features/SIFTFeatures/SIFTFeatureEntry.h"

#include <math.h>



static const float scale_factor = 0.75f;



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CHarrisSIFTFeatureCalculator::CHarrisSIFTFeatureCalculator(float fThreshold, int nLevels, int nMaxInterestPoints)
{
	CSIFTFeatureCalculator::InitializeVariables();

	m_nMaxInterestPoints = nMaxInterestPoints;
	m_nLevels = nLevels;
	m_fThreshold = fThreshold;
	m_fMinDistance = 5.0f;

	m_bPerform80PercentCheck = true;

	m_pInterestPoints = new Vec2d[m_nMaxInterestPoints];
	m_nInterestPoints = 0;

	m_pResultList = 0;
	m_pResultListTemplate = 0;
	m_bTemplateList = true;
	m_bManageMemory = true;

	m_pImage = 0;
}

CHarrisSIFTFeatureCalculator::~CHarrisSIFTFeatureCalculator()
{
	delete [] m_pInterestPoints;
}


// ****************************************************************************
// Methods
// ****************************************************************************

CFeatureEntry* CHarrisSIFTFeatureCalculator::CreateCopy(const CFeatureEntry *pFeatureEntry)
{
	return new CSIFTFeatureEntry(*((CSIFTFeatureEntry *) pFeatureEntry));
}

int CHarrisSIFTFeatureCalculator::CalculateFeatures(const CByteImage *pImage, CDynamicArray *pResultList, bool bManageMemory)
{
	if (pImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image is not a grayscale image\n");
		return -1;
	}

	m_bTemplateList = false;
	m_bManageMemory = bManageMemory;

	m_pResultList = pResultList;
	m_pImage = pImage;
	
	FindInterestPoints(pImage, 1, m_nLevels);

	return pResultList->GetSize();
}

int CHarrisSIFTFeatureCalculator::CalculateFeatures(const CByteImage *pImage, CDynamicArrayTemplatePointer<CFeatureEntry> &resultList)
{
	if (pImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image is not a grayscale image\n");
		return -1;
	}

	m_bTemplateList = true;

	m_pResultListTemplate = &resultList;
	m_pImage = pImage;
	
	FindInterestPoints(pImage, 1, m_nLevels);

	return resultList.GetSize();
}

void CHarrisSIFTFeatureCalculator::FindInterestPoints(const CByteImage *pImage, float scale, int nLevel)
{
	// calculate feature points
	m_nInterestPoints = ImageProcessor::CalculateHarrisInterestPoints(pImage, m_pInterestPoints, m_nMaxInterestPoints, m_fThreshold, m_fMinDistance);

	if (m_bTemplateList)
	{
		for (int i = 0; i < m_nInterestPoints; i++)
			CSIFTFeatureCalculator::CreateSIFTDescriptors(pImage, *m_pResultListTemplate, m_pInterestPoints[i].x, m_pInterestPoints[i].y, scale, m_bPerform80PercentCheck);
	}
	else
	{
		for (int i = 0; i < m_nInterestPoints; i++)
			CSIFTFeatureCalculator::CreateSIFTDescriptors(pImage, m_pResultList, m_pInterestPoints[i].x, m_pInterestPoints[i].y, scale, m_bManageMemory, m_bPerform80PercentCheck);
	}

	if (nLevel > 1)
	{
		// recursive call
		CByteImage scaled_image(int(m_pImage->width * powf(scale_factor, float(m_nLevels - nLevel + 1)) + 0.5f), int(m_pImage->height * powf(scale_factor, float(m_nLevels - nLevel + 1.0f)) + 0.5f), CByteImage::eGrayScale);
		ImageProcessor::Resize(m_pImage, &scaled_image);
		FindInterestPoints(&scaled_image, scale * scale_factor, nLevel - 1);
	}
}
