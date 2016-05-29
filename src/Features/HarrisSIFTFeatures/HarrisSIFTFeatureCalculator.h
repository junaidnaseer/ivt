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
// Filename:  HarrisSIFTFeatureCalculator.h
// Author:    Pedram Azad
// Date:      20.11.2007
// ****************************************************************************

/** \defgroup FeatureComputation Computation of local features */


#ifndef _HARRIS_SIFT_FEATURE_CALCULATOR__H_
#define _HARRIS_SIFT_FEATURE_CALCULATOR__H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/FeatureCalculatorInterface.h"
#include "DataStructures/DynamicArrayTemplatePointer.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CFeatureEntry;
class CByteImage;
struct Vec2d;



// ****************************************************************************
// CHarrisSIFTFeatureCalculator
// ****************************************************************************

/*!
	\ingroup FeatureComputation
	\brief Class for computing Harris-SIFT features in a CByteImage.
*/
class CHarrisSIFTFeatureCalculator : public CFeatureCalculatorInterface
{
public:
	// constructor
	CHarrisSIFTFeatureCalculator(float fThreshold = 0.01f, int nLayers = 3, int nMaxInterestPoints = 500);

	// destructor
	~CHarrisSIFTFeatureCalculator();
  
	
	// public methods
	int CalculateFeatures(const CByteImage *pImage, CDynamicArray *pResultList, bool bManageMemory = true);
	int CalculateFeatures(const CByteImage *pImage, CDynamicArrayTemplatePointer<CFeatureEntry> &resultList);
	CFeatureEntry* CreateCopy(const CFeatureEntry *pFeatureEntry);

	// member access
	void SetMaxInterestPoints(int nMaxInterestPoints) { m_nMaxInterestPoints = nMaxInterestPoints; }
	void SetThreshold(float fThreshold) { m_fThreshold = fThreshold;}
	void SetMinDistance(float fMinDistance) { m_fMinDistance = fMinDistance; }
	void SetNumberOfLevels(int nLevels) { m_nLevels = nLevels; }
	void SetPerform80PercentCheck(bool bPerform80PercentCheck) { m_bPerform80PercentCheck = bPerform80PercentCheck; }

	int GetMaxInterestPoints() { return m_nMaxInterestPoints; }
	float GetThreshold() { return m_fThreshold; }
	float GetMinDistance() { return m_fMinDistance; }
	int GetNumberOfLevels() { return m_nLevels; }

	const Vec2d* GetInterestPoints() const { return m_pInterestPoints; }
	int GetNumberOfInterestPoints() { return m_nInterestPoints; }


private:
	// private methods
	void FindInterestPoints(const CByteImage *pImage, float scale, int nLevel);

	// private attributes
	int m_nMaxInterestPoints;
	float m_fThreshold;
	float m_fMinDistance;
	int m_nLevels;
	bool m_bPerform80PercentCheck;

	CDynamicArray *m_pResultList;
	CDynamicArrayTemplatePointer<CFeatureEntry> *m_pResultListTemplate;
	bool m_bTemplateList;
	bool m_bManageMemory; // only needed if CDynamicArray is used

	const CByteImage *m_pImage;
	int m_nInterestPoints;
	Vec2d *m_pInterestPoints;
};



#endif /* _HARRIS_SIFT_FEATURE_CALCULATOR__H_ */
