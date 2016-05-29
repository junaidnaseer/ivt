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
// Filename:  ObjectColorSegmenter.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ObjectColorSegmenter.h"

#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"
#include "Color/ColorParameterSet.h"



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CObjectColorSegmenter::CObjectColorSegmenter()
{
	m_pColorParameterSet = 0;
	
	m_pTempImage = 0;
	m_pHSVImage = 0;
	m_pRGBImage = 0;
	
	m_pROIList = 0;
}

CObjectColorSegmenter::~CObjectColorSegmenter()
{
	if (m_pHSVImage)
		delete m_pHSVImage;
	
	if (m_pTempImage)
		delete m_pTempImage;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CObjectColorSegmenter::SetImage(const CByteImage *pImage, const Object2DList *pROIList)
{
	m_pRGBImage = pImage;
	
	if (m_pTempImage && (m_pTempImage->width != pImage->width || m_pTempImage->height != pImage->height))
	{
		delete m_pTempImage;
		m_pTempImage = 0;
	}
	
	if (!m_pTempImage)
		m_pTempImage = new CByteImage(pImage->width, pImage->height, CByteImage::eGrayScale);
		
	if (m_pHSVImage && !m_pHSVImage->IsCompatible(pImage))
	{
		delete m_pHSVImage;
		m_pHSVImage = 0;
	}
	
	if (!m_pHSVImage)
		m_pHSVImage = new CByteImage(pImage);
	
	m_pROIList = pROIList;
	
	if (m_pROIList && m_pROIList->size() != 0)
	{
		const int nSize = (int) m_pROIList->size();
		for (int i = 0; i < nSize; i++)
			ImageProcessor::CalculateHSVImage(pImage, m_pHSVImage, &m_pROIList->at(i).region);
	}
	else
		ImageProcessor::CalculateHSVImage(pImage, m_pHSVImage);
}

void CObjectColorSegmenter::SetColorParameterSet(const CColorParameterSet *pColorParameterSet)
{
	m_pColorParameterSet = pColorParameterSet;
}

void CObjectColorSegmenter::FindColoredRegions(CByteImage *pResultImage, RegionList &regionList, int nMinPointsPerRegion)
{
	if (!m_pHSVImage)
	{
		printf("error: call CObjectColorSegmenter::SetImage first and do not use optimizer\n");
		return;
	}
	
	ImageProcessor::CalculateSaturationImage(m_pHSVImage, pResultImage);
	ImageProcessor::ThresholdBinarize(pResultImage, pResultImage, 120);
	
	ImageProcessor::Erode(pResultImage, m_pTempImage);
	ImageProcessor::Dilate(m_pTempImage, pResultImage);
	
	ImageProcessor::FindRegions(pResultImage, regionList, nMinPointsPerRegion, 0, true, true);
}

void CObjectColorSegmenter::CalculateSegmentedImage(CByteImage *pResultImage, ObjectColor color)
{
	if (!m_pColorParameterSet)
	{
		printf("error: color parameter set has not been set\n");
		return;
	}
		
	if (!m_pHSVImage)
	{
		printf("error: call CObjectColorSegmenter::SetImage first\n");
		return;
	}
	
	const int *pParameters = m_pColorParameterSet->GetColorParameters(color);
	
	if (!pParameters)
	{
		printf("error: could not load parameters for segmenting color\n");
		return;
	}
	
	if (m_pROIList && m_pROIList->size() != 0)
	{
		ImageProcessor::Zero(pResultImage);
		
		const int nSize = (int) m_pROIList->size();
		for (int i = 0; i < nSize; i++)
		{
			const Object2DEntry &entry = m_pROIList->at(i);
			
			if (entry.color == color)
				ImageProcessor::FilterHSV(m_pHSVImage, pResultImage, pParameters[0], pParameters[1], pParameters[2], pParameters[3], pParameters[4], pParameters[5], &entry.region);
		}
	}
	else
	{
		ImageProcessor::FilterHSV(m_pHSVImage, pResultImage, pParameters[0], pParameters[1], pParameters[2], pParameters[3], pParameters[4], pParameters[5]);
	}
}


void CObjectColorSegmenter::FindRegionsOfGivenColor(CByteImage *pResultImage, ObjectColor color, RegionList &regionList, int nMinPointsPerRegion)
{
	CalculateSegmentedImage(pResultImage, color);
	
	if (m_pROIList && m_pROIList->size() != 0)
	{
		const int nSize = (int) m_pROIList->size();
		for (int i = 0; i < nSize; i++)
		{
			const Object2DEntry &entry = m_pROIList->at(i);
			
			if (entry.color == color)
			{
				ImageProcessor::Erode(pResultImage, m_pTempImage, 3, &entry.region);
				ImageProcessor::Dilate(m_pTempImage, pResultImage, 3, &entry.region);
			}
		}
	}
	else
	{
		ImageProcessor::Erode(pResultImage, m_pTempImage);
		ImageProcessor::Dilate(m_pTempImage, pResultImage);
	}
	
	ImageProcessor::FindRegions(pResultImage, regionList, nMinPointsPerRegion, 0, true, true);
}

void CObjectColorSegmenter::FindRegionsOfGivenColor(CByteImage *pResultImage, ObjectColor color, int hue, int hue_tol, int min_sat, int max_sat, int min_v, int max_v, RegionList &regionList, int nMinPointsPerRegion)
{
	if (!m_pHSVImage)
	{
		printf("error: call CObjectColorSegmenter::SetImage first\n");
		return;
	}
	
	if (m_pROIList && m_pROIList->size() != 0)
	{
		ImageProcessor::Zero(pResultImage);
		
		const int nSize = (int) m_pROIList->size();
		for (int i = 0; i < nSize; i++)
		{
			const Object2DEntry &entry = m_pROIList->at(i);
			
			if (entry.color == color)
			{
				ImageProcessor::FilterHSV(m_pHSVImage, pResultImage, hue, hue_tol, min_sat, max_sat, min_v, max_v, &entry.region);
				ImageProcessor::Erode(pResultImage, m_pTempImage, 3, &entry.region);
				ImageProcessor::Dilate(m_pTempImage, pResultImage, 3, &entry.region);
			}
		}
	}
	else
	{
		ImageProcessor::FilterHSV(m_pHSVImage, pResultImage, hue, hue_tol, min_sat, max_sat, min_v, max_v);
		ImageProcessor::Erode(pResultImage, m_pTempImage);
		ImageProcessor::Dilate(m_pTempImage, pResultImage);
	}
	
	
	ImageProcessor::FindRegions(pResultImage, regionList, nMinPointsPerRegion, 0, true, true);
}
