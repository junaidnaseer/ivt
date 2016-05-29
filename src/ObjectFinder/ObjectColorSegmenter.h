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
// Filename:  ObjectColorSegmenter.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _OBJECT_COLOR_SEGMENTER_H_
#define _OBJECT_COLOR_SEGMENTER_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Structs/ObjectDefinitions.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;
class CColorParameterSet;



// ****************************************************************************
// CObjectColorSegmenter
// ****************************************************************************

class CObjectColorSegmenter
{
public:
	// constructor
	CObjectColorSegmenter();
	
	// destructor
	~CObjectColorSegmenter();
	
	
	// public methods
	
	// method for color initialization
	void SetColorParameterSet(const CColorParameterSet *pColorParameterSet);
		
	// call for each new input image
	void SetImage(const CByteImage *pImage, const Object2DList *pROIList = 0);
	
	// pure color segmentation method using labels for colors and CColorIntervalSet
	void CalculateSegmentedImage(CByteImage *pResultImage, ObjectColor color);
	
	// methods for doing segmentation and applying region growing algorithm afterwards
	void FindColoredRegions(CByteImage *pResultImage, RegionList &regionList, int nMinPointsPerRegion);
	void FindRegionsOfGivenColor(CByteImage *pResultImage, ObjectColor color, RegionList &regionList, int nMinPointsPerRegion);
	void FindRegionsOfGivenColor(CByteImage *pResultImage, ObjectColor color, int hue, int hue_tol, int min_sat, int max_sat, int min_v, int max_v, RegionList &regionList, int nMinPointsPerRegion);
		

private:
	// private attributes
	const CColorParameterSet *m_pColorParameterSet;
	
	const Object2DList *m_pROIList;
	
	CByteImage *m_pTempImage;
	const CByteImage *m_pRGBImage;
	CByteImage *m_pHSVImage;
};



#endif /* _OBJECT_COLOR_SEGMENTER_H_ */
