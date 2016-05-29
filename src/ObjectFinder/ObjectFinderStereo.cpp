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
// Filename:  ObjectFinderStereo.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ObjectFinderStereo.h"

#include "ObjectFinder.h"
#include "Calibration/StereoCalibration.h"
#include "Interfaces/ObjectClassifierInterface.h"
#include "Interfaces/ObjectEntryFilterInterface.h"
#include "Image/ByteImage.h"

#include <stdio.h>
#include <math.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CObjectFinderStereo::CObjectFinderStereo()
{
	m_pObjectFinderLeft = new CObjectFinder();
	m_pObjectFinderRight = new CObjectFinder();

	m_pStereoCalibration = new CStereoCalibration();
	m_bOwnCalibration = true;
	
	m_pObjectEntryFilter = 0;
}

CObjectFinderStereo::~CObjectFinderStereo()
{
	if(m_bOwnCalibration)
		delete m_pStereoCalibration;

	delete m_pObjectFinderLeft;
	delete m_pObjectFinderRight;
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CObjectFinderStereo::Init(const char *pCameraParameterFileName)
{
	if(!m_bOwnCalibration)
		m_pStereoCalibration = new CStereoCalibration();
		
	m_bOwnCalibration = true;
	
	return m_pStereoCalibration->LoadCameraParameters(pCameraParameterFileName);
}

void CObjectFinderStereo::Init(CStereoCalibration* pStereoCalibration)
{
	if (m_bOwnCalibration)
		delete m_pStereoCalibration;
		
	m_bOwnCalibration = false;
	
	m_pStereoCalibration = pStereoCalibration;
}
	
void CObjectFinderStereo::SetColorParameterSet(const CColorParameterSet *pColorParameterSet)
{
	m_pObjectFinderLeft->SetColorParameterSet(pColorParameterSet);
	m_pObjectFinderRight->SetColorParameterSet(pColorParameterSet);
}

void CObjectFinderStereo::SetRegionFilter(CRegionFilterInterface *pRegionFilter)
{
	m_pObjectFinderLeft->SetRegionFilter(pRegionFilter);
	m_pObjectFinderRight->SetRegionFilter(pRegionFilter);
}

CByteImage* CObjectFinderStereo::GetLeftSegmentationResult()
{
	return m_pObjectFinderLeft->GetSegmentationResult();
}

CByteImage* CObjectFinderStereo::GetRightSegmentationResult()
{
	return m_pObjectFinderRight->GetSegmentationResult();
}


void CObjectFinderStereo::PrepareImages(const CByteImage * const *ppImages, float fROIFactor, bool bCalculateHSVImage)
{
	m_pObjectFinderLeft->PrepareImages(ppImages[0], fROIFactor, bCalculateHSVImage);
	m_pObjectFinderRight->PrepareImages(ppImages[1], fROIFactor, bCalculateHSVImage);
}

int CObjectFinderStereo::Finalize(float fMinZDistance, float fMaxZDistance, bool bInputImagesAreRectified, ObjectColor finalizeColor, float fMaxYDiff, bool bUseDistortionParameters)
{
	m_pObjectFinderLeft->Finalize();
	m_pObjectFinderRight->Finalize();
	
	const int nRet = DetermineMatches(m_pObjectFinderLeft->m_objectList, m_pObjectFinderRight->m_objectList, fMinZDistance, fMaxZDistance, bInputImagesAreRectified, bUseDistortionParameters, finalizeColor, fMaxYDiff);
	
	// update object lists of (2D) CObjectFinder objects
	UpdateObjectFinderLists(m_pObjectFinderLeft->m_objectList, m_pObjectFinderRight->m_objectList);
	
	return nRet;
}


void CObjectFinderStereo::FindObjects(const CByteImage * const *ppImages, CByteImage **ppResultImages, ObjectColor color, int nMinPointsPerRegion, bool bShowSegmentedImage)
{
	CByteImage *pResultImageLeft = ppResultImages ? ppResultImages[0] : 0;
	CByteImage *pResultImageRight = ppResultImages ? ppResultImages[1] : 0;
	
	m_pObjectFinderLeft->FindObjects(ppImages[0], pResultImageLeft, color, nMinPointsPerRegion, bShowSegmentedImage);
	m_pObjectFinderRight->FindObjects(ppImages[1], pResultImageRight, color, nMinPointsPerRegion, bShowSegmentedImage);
}

void CObjectFinderStereo::FindObjects(const CByteImage * const *ppImages, CByteImage **ppResultImages, ObjectColor color, int nMinPointsPerRegion, CByteImage **ppSegmentedResultImages)
{
	CByteImage *pResultImageLeft = ppResultImages ? ppResultImages[0] : 0;
	CByteImage *pResultImageRight = ppResultImages ? ppResultImages[1] : 0;
	
	m_pObjectFinderLeft->FindObjects(ppImages[0], pResultImageLeft, color, nMinPointsPerRegion, ppSegmentedResultImages[0]);
	m_pObjectFinderRight->FindObjects(ppImages[1], pResultImageRight, color, nMinPointsPerRegion, ppSegmentedResultImages[1]);
}

void CObjectFinderStereo::FindObjects(const CByteImage * const *ppImages, CByteImage **ppResultImages, ObjectColor colorName, int hue, int hue_tol, int min_sat, int max_sat, int min_v, int max_v, int nMinPointsPerRegion, bool bShowSegmentedImage)
{
	CByteImage *pResultImageLeft = ppResultImages ? ppResultImages[0] : 0;
	CByteImage *pResultImageRight = ppResultImages ? ppResultImages[1] : 0;
	
	m_pObjectFinderLeft->FindObjects(ppImages[0], pResultImageLeft, colorName, hue, hue_tol, min_sat, max_sat, min_v, max_v, nMinPointsPerRegion, bShowSegmentedImage);
	m_pObjectFinderRight->FindObjects(ppImages[1], pResultImageRight, colorName, hue, hue_tol, min_sat, max_sat, min_v, max_v, nMinPointsPerRegion, bShowSegmentedImage);
}

void CObjectFinderStereo::FindObjectsInSegmentedImage(const CByteImage * const *ppImages, CByteImage **ppResultImages, ObjectColor color, int nMinPointsPerRegion, bool bShowSegmentedImage)
{
	CByteImage *pResultImageLeft = ppResultImages ? ppResultImages[0] : 0;
	CByteImage *pResultImageRight = ppResultImages ? ppResultImages[1] : 0;
	
	m_pObjectFinderLeft->FindObjectsInSegmentedImage(ppImages[0], pResultImageLeft, color, nMinPointsPerRegion, bShowSegmentedImage);
	m_pObjectFinderRight->FindObjectsInSegmentedImage(ppImages[1], pResultImageRight, color, nMinPointsPerRegion, bShowSegmentedImage);
}


int CObjectFinderStereo::DetermineMatches(Object2DList &resultListLeft, Object2DList &resultListRight, float fMinZDistance, float fMaxZDistance, bool bInputImagesAreRectified, bool bUseDistortionParameters, ObjectColor finalizeColor, float fMaxEpipolarDistance)
{
	// create copy of current object list
	Object3DList oldObjectList = m_objectList;

	if (finalizeColor == eNone)
	{
		m_objectList.clear();
	}
	else
	{
		for (int i = 0; i < (int) m_objectList.size(); i++)
		{
			if (m_objectList.at(i).color == finalizeColor)
			{
				m_objectList.erase(m_objectList.begin() + i);
				i--;
			}
		}
	}

	int i, j;
	
	for (i = 0; i < (int) resultListLeft.size(); i++)
		resultListLeft.at(i).reserved = 0;
		
	for (i = 0; i < (int) resultListRight.size(); i++)
		resultListRight.at(i).reserved = 0;
	
	// add already existing region pairs
	for (i = 0; i < (int) oldObjectList.size(); i++)
	{
		Object3DEntry &objectEntry = oldObjectList.at(i);
		
		if (finalizeColor != eNone && finalizeColor != objectEntry.color)
			continue;
		
		const int region_left_id = objectEntry.region_id_left;
		const int region_right_id = objectEntry.region_id_right;
		int nMatchLeft = -1, nMatchRight = -1;
		
		for (j = 0; j < (int) resultListLeft.size(); j++)
		{
			if (resultListLeft.at(j).id == region_left_id)
			{
				nMatchLeft = j;
				break;
			}
		}
		
		for (j = 0; j < (int) resultListRight.size(); j++)
			if (resultListRight.at(j).id == region_right_id)
			{
				nMatchRight = j;
				break;
			}
		
		if (nMatchLeft != -1 && nMatchRight != -1)
		{
			// calculate 3D point
			objectEntry.region_left = resultListLeft.at(nMatchLeft).region;
			objectEntry.region_right = resultListRight.at(nMatchRight).region;
			m_pStereoCalibration->Calculate3DPoint(objectEntry.region_left.centroid, objectEntry.region_right.centroid, objectEntry.pose.translation, bInputImagesAreRectified, bUseDistortionParameters);

			Math3d::SetVec(objectEntry.world_point, objectEntry.pose.translation);
			
			if (objectEntry.pose.translation.z >= fMinZDistance && objectEntry.pose.translation.z <= fMaxZDistance &&
				(!m_pObjectEntryFilter || m_pObjectEntryFilter->CheckEntry(objectEntry)))
			{
				resultListLeft.at(nMatchLeft).reserved = 1;
				resultListRight.at(nMatchRight).reserved = 1;
				m_objectList.push_back(objectEntry);
			}
		}
	}
	
	// add new region pairs
	for (i = 0; i < (int) resultListLeft.size(); i++)
	{
		Object2DEntry &entryLeft = resultListLeft.at(i);
		
		if (!entryLeft.reserved && (finalizeColor == eNone || entryLeft.color == finalizeColor))
		{
			// find best match in list for right image
			float best_diff = fMaxEpipolarDistance;
			int best_j = -1;
			
			for (j = 0; j < (int) resultListRight.size(); j++)
			{
				Object2DEntry &entryRight = resultListRight.at(j);
				
				if (!entryRight.reserved)
				{
					const float ratio = entryLeft.region.nPixels < entryRight.region.nPixels ? (float) entryLeft.region.nPixels / entryRight.region.nPixels : (float) entryRight.region.nPixels / entryLeft.region.nPixels;
					const float ratio2 = entryLeft.region.ratio < entryRight.region.ratio ? (float) entryLeft.region.ratio / entryRight.region.ratio : (float) entryRight.region.ratio / entryLeft.region.ratio;
					const float y_diff = bInputImagesAreRectified ? fabsf(entryLeft.region.centroid.y - entryRight.region.centroid.y) : fabsf(m_pStereoCalibration->CalculateEpipolarLineInLeftImageDistance(entryLeft.region.centroid, entryRight.region.centroid));

					Vec3d position;
					m_pStereoCalibration->Calculate3DPoint(entryLeft.region.centroid, entryRight.region.centroid, position, bInputImagesAreRectified, bUseDistortionParameters);
					
					//printf("%.2f %.2f -- %.2f %.2f -- %.2f %.2f %.2f\n", entryLeft.region.centroid.x, entryLeft.region.centroid.y, entryRight.region.centroid.x, entryRight.region.centroid.y, ratio, y_diff, position.z);
			
					if (ratio > 0.5f && ratio2 > 0.5f && y_diff < fMaxEpipolarDistance &&
						(entryLeft.type == entryRight.type || entryLeft.type == eCompactObject || entryRight.type == eCompactObject) && entryLeft.color == entryRight.color && (finalizeColor == eNone || entryLeft.color == finalizeColor) &&
						position.z >= fMinZDistance && position.z <= fMaxZDistance && y_diff < best_diff)
					{
						best_diff = y_diff;
						best_j = j;
					}
				}
			}
			
			if (best_j != -1)
			{
				Object2DEntry &entryRight = resultListRight.at(best_j);
				Object3DEntry entry;
				
				entry.region_left = entryLeft.region;
				entry.region_right = entryRight.region;
				entry.region_id_left = entryLeft.id;
				entry.region_id_right = entryRight.id;
				entry.color = entryLeft.color;
				entry.type = entryLeft.type == eCompactObject ? entryRight.type : entryLeft.type;
				entry.sName = "CompactObject";
				entry.sOivFilePath = "";
				
				m_pStereoCalibration->Calculate3DPoint(entry.region_left.centroid, entry.region_right.centroid, entry.pose.translation, bInputImagesAreRectified, bUseDistortionParameters);

				Math3d::SetVec(entry.world_point, entry.pose.translation);
				
				if (!m_pObjectEntryFilter || m_pObjectEntryFilter->CheckEntry(entry))
				{
					entryLeft.reserved = 1;
					entryRight.reserved = 1;
					m_objectList.push_back(entry);
				}
			}
		}
	}
	
	// run object classifiers
	for (i = 0; i < (int) m_objectClassifierList.size(); i++)
		m_objectClassifierList.at(i)->Classify(m_objectList);
	
	return (int) m_objectList.size();
}

void CObjectFinderStereo::UpdateObjectFinderLists(Object2DList &resultListLeft, Object2DList &resultListRight)
{
	int i;

	// delete unmatched entries in left 2D list
	for (i = 0; i < (int) resultListLeft.size(); i++)
		if (!resultListLeft.at(i).reserved)
		{
			resultListLeft.erase(resultListLeft.begin() + i);
			i--;
		}
		
	// delete unmatched entries in right 2D list
	for (i = 0; i < (int) resultListRight.size(); i++)
		if (!resultListRight.at(i).reserved)
		{
			resultListRight.erase(resultListRight.begin() + i);
			i--;
		}

	// assign types to 2D lists
	for (i = 0; i < (int) m_objectList.size(); i++)
	{
		int j;
		
		const Object3DEntry &object = m_objectList.at(i);
		
		for (j = 0; j < (int) resultListLeft.size(); j++)
		{
			Object2DEntry &entry = resultListLeft.at(j);
		
			if (entry.color == object.color && entry.region.centroid.x == object.region_left.centroid.x && entry.region.centroid.y == object.region_left.centroid.y && entry.region.nPixels == object.region_left.nPixels)
				entry.type = object.type;
		}
		
		for (j = 0; j < (int) resultListRight.size(); j++)
		{
			Object2DEntry &entry = resultListRight.at(j);
		
			if (entry.color == object.color && entry.region.centroid.x == object.region_right.centroid.x && entry.region.centroid.y == object.region_right.centroid.y && entry.region.nPixels == object.region_right.nPixels)
				entry.type = object.type;
		}
	}
}

void CObjectFinderStereo::ClearObjectList()
{
	m_objectList.clear();
	m_pObjectFinderLeft->ClearObjectList();
	m_pObjectFinderRight->ClearObjectList();
}

void CObjectFinderStereo::AddObject(const Object3DEntry &entry)
{
	m_objectList.push_back(entry);
	
	Object2DEntry entry2D;
	entry2D.type = entry.type;
	entry2D.color = entry.color;
	
	entry2D.id = entry.region_id_left;
	entry2D.region = entry.region_left;
	m_pObjectFinderLeft->AddObject(entry2D);
	
	entry2D.id = entry.region_id_right;
	entry2D.region = entry.region_right;
	m_pObjectFinderRight->AddObject(entry2D);
}


void CObjectFinderStereo::AddObjectClassifier(CObjectClassifierInterface *pObjectClassifier)
{
	m_objectClassifierList.push_back(pObjectClassifier);
}

void CObjectFinderStereo::RemoveObjectClassifier(CObjectClassifierInterface *pObjectClassifier)
{
	for (std::vector<CObjectClassifierInterface*>::iterator it = m_objectClassifierList.begin(); it != m_objectClassifierList.end(); it++)
	{
		if (*it == pObjectClassifier)
		{
			m_objectClassifierList.erase(it);
			break;
		}
	}
}

void CObjectFinderStereo::ClearObjectClassifierList()
{
	m_objectClassifierList.clear();
}
