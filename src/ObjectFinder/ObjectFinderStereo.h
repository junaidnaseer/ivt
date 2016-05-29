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
// Filename:  ObjectFinderStereo.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _OBJECT_FINDER_STEREO_H_
#define _OBJECT_FINDER_STEREO_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Structs/ObjectDefinitions.h"
#include <string>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CRegionFilterInterface;
class CObjectEntryFilterInterface;
class CObjectClassifierInterface;
class CObjectFinder;
class CByteImage;
class CCalibration;
class CStereoCalibration;
class CColorParameterSet;



// ****************************************************************************
// CObjectFinderStereo
// ****************************************************************************

class CObjectFinderStereo
{
public:
	// constructor
	CObjectFinderStereo();
	
	// destructor
	~CObjectFinderStereo();
	
	
	// public methods
	bool Init(const char *pCameraParameterFileName);
	void Init(CStereoCalibration* pStereoCalibration);
	
	void SetColorParameterSet(const CColorParameterSet *pColorParameterSet);
		
	// first call this method for each new image
	void PrepareImages(const CByteImage * const *ppImages, float fROIFactor = -1, bool bCalculateHSVImage = true);
	
	// then for each color call one of the FindObjects-methods
	void FindObjects(const CByteImage * const *ppImages, CByteImage **ppResultImages, ObjectColor color, int nMinPointsPerRegion, bool bShowSegmentedImage);
	void FindObjects(const CByteImage * const *ppImages, CByteImage **ppResultImages, ObjectColor color, int nMinPointsPerRegion, CByteImage **ppResultSegmentedImages);
	void FindObjects(const CByteImage * const *ppImages, CByteImage **ppResultImages, ObjectColor colorName, int hue, int hue_tol, int min_sat, int max_sat, int min_v, int max_v, int nMinPointsPerRegion, bool bShowSegmentedImage);
	void FindObjectsInSegmentedImage(const CByteImage * const *ppImages, CByteImage **ppResultImages, ObjectColor color, int nMinPointsPerRegion, bool bShowSegmentedImage);
	
	// finally call this method
	int Finalize(float dMinZDistance, float fMaxZDistance, bool bInputImagesAreRectified, ObjectColor finalizeColor = eNone, float fMaxEpipolarDistance = 10, bool bUseDistortionParameters = true);
	
	// member access
	CByteImage* GetLeftSegmentationResult();
	CByteImage* GetRightSegmentationResult();
	const CStereoCalibration* GetStereoCalibration() { return m_pStereoCalibration; }
	
	// use these methods for setting a region/entry filter
	void SetRegionFilter(CRegionFilterInterface *pRegionFilter);
	void SetObjectEntryFilter(CObjectEntryFilterInterface *pObjectEntryFilter) { m_pObjectEntryFilter = pObjectEntryFilter; }
	
	// use these methods for manipulating the internal object list
	const Object3DList& GetObject3DList() { return m_objectList; }
	void ClearObjectList();
	void AddObject(const Object3DEntry &entry);
	
	// use these methods for adding classifiers (mostly not used)
	void AddObjectClassifier(CObjectClassifierInterface *pObjectClassifier);
	void RemoveObjectClassifier(CObjectClassifierInterface *pObjectClassifier);
	void ClearObjectClassifierList();

	// public attributes
	Object3DList m_objectList;


protected:
	// protected methods
	void UpdateObjectFinderLists(Object2DList &resultListLeft, Object2DList &resultListRight);
	int DetermineMatches(Object2DList &resultListLeft, Object2DList &resultListRight, float fMinZDistance, float fMaxZDistance, bool bInputImagesAreRectified, bool bUseDistortionParameters, ObjectColor finalizeColor, float fMaxYDiff);

	// protected attributes
	CObjectFinder *m_pObjectFinderLeft;
	CObjectFinder *m_pObjectFinderRight;
	CStereoCalibration *m_pStereoCalibration;
		
private:
	// private attributes
	std::vector<CObjectClassifierInterface*> m_objectClassifierList;
	CObjectEntryFilterInterface *m_pObjectEntryFilter;
	bool m_bOwnCalibration;
};



#endif /* _OBJECT_FINDER_STEREO_H_ */
