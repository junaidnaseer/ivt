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
// Filename:  PatchFeatureEntry.h
// Author:    Pedram Azad
// Date:      24.09.2008
// ****************************************************************************


#ifndef _PATCH_FEATURE_ENTRY_H_
#define _PATCH_FEATURE_ENTRY_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Features/FeatureEntry.h"
#include <math.h>



// ****************************************************************************
// CPatchFeatureEntry
// ****************************************************************************

/*!
	\ingroup FeatureRepresentations
	\brief Data structure for the representation of simple squared patch features.
*/
class CPatchFeatureEntry : public CFeatureEntry
{
public:
	// constructor
	CPatchFeatureEntry(const float *pFeature, int nSize, float x, float y, float angle, float scale) : CFeatureEntry(pFeature, nSize, x, y, angle, scale)
	{
	}

	CPatchFeatureEntry() : CFeatureEntry(0, 0.0f, 0.0f, 0.0f, 0.0f, Math3d::zero_vec)
	{
	}

	CPatchFeatureEntry(const CPatchFeatureEntry &featureEntry) : CFeatureEntry(featureEntry)
	{
	}


	// public methods
	bool ExtractFeature(const CByteImage *pImage, int nWindowSize, float x, float y);

	
	// pure virtual methods
	eFeatureType GetType() const
	{
		return tPatch;
	}

	CFeatureEntry* Clone() const
	{
		return new CPatchFeatureEntry(*this);
	}

	float Error(const CDynamicArrayElement *pElement) const
	{
		const CPatchFeatureEntry *pCastedElement = (const CPatchFeatureEntry *) pElement;
		const float *pFeature = pCastedElement->m_pFeature;

		float error = 0.0f;
		for (int i = 0; i < m_nSize; i++)
			error += (pFeature[i] - m_pFeature[i]) * (pFeature[i] - m_pFeature[i]);
			
		return error;
	}
	
	// other public methods
	void NormalizeIntensity()
	{
		int i;
		float mean = 0, normalize_factor = 0;
		
		for (i = 0; i < m_nSize; i++)
			mean += m_pFeature[i];
		mean /= m_nSize;
		
		for (i = 0; i < m_nSize; i++)
		{
			m_pFeature[i] -= mean;
			normalize_factor += m_pFeature[i] * m_pFeature[i];
		}
		
		normalize_factor = 1 / sqrtf(normalize_factor);
		
		for (i = 0; i < m_nSize; i++)
			m_pFeature[i] *= normalize_factor;
	}
};



#endif /* _PATCH_FEATURE_ENTRY_H_ */
