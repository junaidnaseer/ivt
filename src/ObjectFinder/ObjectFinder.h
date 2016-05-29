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
// Filename:  ObjectFinder.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _OBJECT_FINDER_H_
#define _OBJECT_FINDER_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Structs/ObjectDefinitions.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CRegionFilterInterface;
class CObjectClassifierInterface;
class CObjectColorSegmenter;
class CColorParameterSet;
class CByteImage;



// ****************************************************************************
// CObjectFinder
// ****************************************************************************

class CObjectFinder
{
public:
	// constructor
	CObjectFinder();
	
	// destructor
	~CObjectFinder();
	
	
	// public methods
	void SetColorParameterSet(const CColorParameterSet *pColorParameterSet);
	
	// first call this method for each new image
	void PrepareImages(const CByteImage *pImage, float fROIFactor = -1, bool bCalculateHSVImage = true);
	
	// then for each color call one of the FindObjects-methods
	void FindObjects(const CByteImage *pImage, CByteImage *pResultImage, ObjectColor color, int nMinPointsPerRegion, bool bShowSegmentedImage);
	void FindObjects(const CByteImage *pImage, CByteImage *pResultImage, ObjectColor color, int nMinPointsPerRegion, CByteImage *pResultSegmentedImage);
	void FindObjects(const CByteImage *pImage, CByteImage *pResultImage, ObjectColor colorName, int hue, int hue_tol, int min_sat, int max_sat, int min_v, int max_v, int nMinPointsPerRegion, bool bShowSegmentedImage);
	
	// instead of calling PrepareImages and one of the methods above one can directly feed the segmented image
	void FindObjectsInSegmentedImage(const CByteImage *pSegmentedImage, CByteImage *pResultImage, ObjectColor color, int nMinPointsPerRegion, bool bShowSegmentedImage);
	
	// finally call this method
	int Finalize();
	
	
	// for having access to segmentation result
	CByteImage* GetSegmentationResult() { return m_pSegmentedImage; }
	
	// use these methods for setting a region filter and classifiers
	void SetRegionFilter(CRegionFilterInterface *pRegionFilter) { m_pRegionFilter = pRegionFilter; }
	
	// use these moethods for manipulating the object list
	void AddObject(const Object2DEntry &entry);
	void ClearObjectList();


private:
	// private methods
	void CheckRegionsForObjects(const CByteImage *pColorImage, const CByteImage *pSegmentedImage, CByteImage *pResultImage, RegionList &regionList, ObjectColor color);
	
	// private attributes
	CObjectColorSegmenter *m_pObjectColorSegmenter;
	CByteImage *m_pSegmentedImage;
	
	CRegionFilterInterface *m_pRegionFilter;
	
	int m_nIDCounter;
	bool m_bUseROI;
	Object2DList m_ROIList;
	
public:
	Object2DList m_objectList;
};



#endif /* _OBJECT_FINDER_H_ */
