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
// Filename:  ObjectFinder.cpp
// Author:    Pedram Azad
// Date:      02.12.2007
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ObjectFinder.h"

#include "ObjectColorSegmenter.h"
#include "Image/ImageProcessor.h"
#include "Image/PrimitivesDrawer.h"
#include "Image/ByteImage.h"
#include "Interfaces/RegionFilterInterface.h"
#include "Interfaces/ObjectClassifierInterface.h"
#include "Helpers/helpers.h"

#include <stdio.h>
#include <math.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CObjectFinder::CObjectFinder()
{
	m_pObjectColorSegmenter = new CObjectColorSegmenter();
	
	m_pSegmentedImage = 0;
	m_pRegionFilter = 0;
	m_nIDCounter = 0;
	m_bUseROI = false;
}

CObjectFinder::~CObjectFinder()
{
	delete m_pObjectColorSegmenter;
		
	if (m_pSegmentedImage)
		delete m_pSegmentedImage;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CObjectFinder::SetColorParameterSet(const CColorParameterSet *pColorParameterSet)
{
	m_pObjectColorSegmenter->SetColorParameterSet(pColorParameterSet);
}

void CObjectFinder::PrepareImages(const CByteImage *pImage, float fROIFactor, bool bCalculateHSVImage)
{
	if (m_pSegmentedImage && (m_pSegmentedImage->width != pImage->width || m_pSegmentedImage->height != pImage->height))
	{
		delete m_pSegmentedImage;
		m_pSegmentedImage = 0;
	}
	
	if (!m_pSegmentedImage)
		m_pSegmentedImage = new CByteImage(pImage->width, pImage->height, CByteImage::eGrayScale);
		
	m_bUseROI = fROIFactor != -1;
	m_ROIList.clear();

	if (m_bUseROI)
	{
		for (int i = 0; i < (int) m_objectList.size(); i++)
		{
			// create copy
			Object2DEntry entry = m_objectList.at(i);

			const float w = floor((entry.region.max_x - entry.region.min_x + 1) * fROIFactor * 0.5f);
			const float h = floor((entry.region.max_y - entry.region.min_y + 1) * fROIFactor * 0.5f);

			entry.region.min_x = int(entry.region.centroid.x - w - 0.5f);
			entry.region.min_y = int(entry.region.centroid.y - h - 0.5f);
			entry.region.max_x = int(entry.region.centroid.x + w + 0.5f);
			entry.region.max_y = int(entry.region.centroid.y + h + 0.5f);

			m_ROIList.push_back(entry);
		}
	}
	
	if (bCalculateHSVImage)
		m_pObjectColorSegmenter->SetImage(pImage, &m_ROIList);
	
	m_objectList.clear();
}

void CObjectFinder::FindObjects(const CByteImage *pImage, CByteImage *pResultImage, ObjectColor color, int nMinPointsPerRegion, bool bShowSegmentedImage)
{
	RegionList regionList;
	
	// do color segmentation and perform region growing
	switch (color)
	{
		case eColored:
			m_pObjectColorSegmenter->FindColoredRegions(m_pSegmentedImage, regionList, nMinPointsPerRegion);
		
		default:
			m_pObjectColorSegmenter->FindRegionsOfGivenColor(m_pSegmentedImage, color, regionList, nMinPointsPerRegion);
	}
	
	if (bShowSegmentedImage && pResultImage)
		ImageProcessor::ConvertImage(m_pSegmentedImage, pResultImage);
	
	// run object detectors
	CheckRegionsForObjects(pImage, m_pSegmentedImage, pResultImage, regionList, color);
}

void CObjectFinder::FindObjects(const CByteImage *pImage, CByteImage *pResultImage, ObjectColor color, int nMinPointsPerRegion, CByteImage *pResultSegmentedImage)
{
	RegionList regionList;
	
	// do color segmentation and perform region growing
	switch (color)
	{
		case eColored:
			m_pObjectColorSegmenter->FindColoredRegions(pResultSegmentedImage, regionList, nMinPointsPerRegion);
		
		default:
			m_pObjectColorSegmenter->FindRegionsOfGivenColor(pResultSegmentedImage, color, regionList, nMinPointsPerRegion);
	}
	
	// run object detectors
	CheckRegionsForObjects(pImage, pResultSegmentedImage, pResultImage, regionList, color);
}

void CObjectFinder::FindObjects(const CByteImage *pImage, CByteImage *pResultImage, ObjectColor color, int hue, int hue_tol, int min_sat, int max_sat, int min_v, int max_v, int nMinPointsPerRegion, bool bShowSegmentedImage)
{
	RegionList regionList;

	// do color segmentation and perform region growing
	m_pObjectColorSegmenter->FindRegionsOfGivenColor(m_pSegmentedImage, color, hue, hue_tol, min_sat, max_sat, min_v, max_v, regionList, nMinPointsPerRegion);

	if (bShowSegmentedImage)
		ImageProcessor::ConvertImage(m_pSegmentedImage, pResultImage);

	// run object detectors
	CheckRegionsForObjects(pImage, m_pSegmentedImage, pResultImage, regionList, color);
}

void CObjectFinder::FindObjectsInSegmentedImage(const CByteImage *pSegmentedImage, CByteImage *pResultImage, ObjectColor color, int nMinPointsPerRegion, bool bShowSegmentedImage)
{
	RegionList regionList;
	
	// perform region growing
	ImageProcessor::FindRegions(pSegmentedImage, regionList, nMinPointsPerRegion, 0, true, true);

	if (bShowSegmentedImage)
		ImageProcessor::ConvertImage(pSegmentedImage, pResultImage);
	
	// run object detectors
	CheckRegionsForObjects(0, pSegmentedImage, pResultImage, regionList, color);
}

void CObjectFinder::CheckRegionsForObjects(const CByteImage *pColorImage, const CByteImage *pSegmentedImage, CByteImage *pResultImage, RegionList &regionList, ObjectColor color)
{
	const int nRegions = (int) regionList.size();

	for (int i = 0; i < nRegions; i++)
	{
		const MyRegion &region = regionList.at(i);
		
		if (!m_pRegionFilter || m_pRegionFilter->CheckRegion(pColorImage, pSegmentedImage, region))
		{
			Object2DEntry objectEntry;
			objectEntry.color = color;
			objectEntry.region = region;
			objectEntry.type = eCompactObject;
			objectEntry.sName = "CompactObject";
			
			float min = 100.0f;
			int best_id = -1;
			
			for (int j = 0; j < (int) m_ROIList.size(); j++)
			{
				const Object2DEntry &entry = m_ROIList.at(j);
				const float fRatio = entry.region.nPixels < region.nPixels ? float(entry.region.nPixels) / region.nPixels : float(region.nPixels) / entry.region.nPixels;
				const float fDistance = Math2d::Distance(entry.region.centroid, region.centroid);
				
				if (entry.color == color && fRatio > 0.75f && fDistance < min)
				{
					min = fDistance;
					best_id = entry.id;
				}
			}
			
			if (best_id == -1)
				objectEntry.id = m_nIDCounter++;
			else
				objectEntry.id = best_id;

			m_objectList.push_back(objectEntry);
			
			if (pResultImage)
				PrimitivesDrawer::DrawRegion(pResultImage, region, 255, 0, 0, 2);
		}
	}
}

int CObjectFinder::Finalize()
{
	return (int) m_objectList.size();
}

void CObjectFinder::ClearObjectList()
{
	m_objectList.clear();
}

void CObjectFinder::AddObject(const Object2DEntry &entry)
{
	m_objectList.push_back(entry);
}
