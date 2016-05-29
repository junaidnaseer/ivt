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
// Filename:  SIFTFeatureCalculator.h
// Author:    Pedram Azad, Lars Paetzold
// Date:      24.09.2008
// ****************************************************************************

#ifndef _SIFT_FEATURE_CALCULATOR__H_
#define _SIFT_FEATURE_CALCULATOR__H_


// ****************************************************************************
// Includes
// ****************************************************************************

#include "Interfaces/FeatureCalculatorInterface.h"
#include "DataStructures/DynamicArrayTemplatePointer.h"
#include "DataStructures/DynamicArrayTemplate.h"
#include <vector>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CFeatureEntry;
class CFloatMatrix;
class CByteImage;
struct Vec2d;


// ****************************************************************************
// Defines
// ****************************************************************************

#define MAX_SCALES	10



// ****************************************************************************
// CSIFTFeatureCalculator
// ****************************************************************************

/*!
	\ingroup FeatureComputation
	\brief Class for computing SIFT features in a CByteImage.
*/
class CSIFTFeatureCalculator : public CFeatureCalculatorInterface
{
public:
	// constructor
	CSIFTFeatureCalculator(float fThreshold = 0.05f, int nOctaves = 3);

	// destructor
	~CSIFTFeatureCalculator();
  
	
	// public methods
	int CalculateFeatures(const CByteImage *pImage, CDynamicArray *pResultList, bool bManageMemory = true);

	// member access
	void SetThreshold(float fThreshold) { m_fThreshold = fThreshold;}
	void SetNumberOfOctaves(int nOctaves) { m_nOctaves = nOctaves; }

	float GetThreshold() { return m_fThreshold; }
	int GetNumberOfOctaves() { return m_nOctaves; }

	// public static methods
	static void InitializeVariables();
	static void CreateSIFTDescriptors(const CFloatMatrix *pImage, CDynamicArray *pResultList, float x, float y, float scale, float sigma, const float *pOrientationWeights, bool bManageMemory = true, bool bPerform80PercentCheck = true);
	static void CreateSIFTDescriptors(const CByteImage *pImage, CDynamicArray *pResultList, float x, float y, float scale = 1.0f, bool bManageMemory = true, bool bPerform80PercentCheck = true);
	static void CreateSIFTDescriptors(const CByteImage *pImage, CDynamicArrayTemplatePointer<CFeatureEntry> &resultList, float x, float y, float scale = 1.0f, bool bPerform80PercentCheck = true);


private:
	// private methods
	static void DetermineDominatingOrientations(const CFloatMatrix *pAngleMatrix, const CFloatMatrix *pMagnitudeMatrix, CDynamicArrayTemplate<float> &orientations, bool bPerform80PercentCheck);
	void FindScaleSpaceExtrema(const CFloatMatrix *pImage, float scale, int nOctave);

	// private attributes
	float m_fThreshold;
	int m_nOctaves;

	CDynamicArray *m_pResultList;
	bool m_bManageMemory;

	// static constants for internal use
	static float edgethreshold_;
	static float k_;
	static float diffk_;
	static float prescaledsigma_;
	static float diffsigma0_;

	static int SIFTPointers[256]; 
	static float SIFTWeights[256];
	static float SIFTOrientationWeights[256 * (MAX_SCALES + 1)];
	static float SIFTDescriptorWeights[256];

	static float SIFTDiffSigmas[MAX_SCALES];
	static float SIFTSigmas[MAX_SCALES];
	static float* SIFTGaussianFilters[MAX_SCALES];
	static int SIFTKernelRadius[MAX_SCALES];

	static int m_bInitialized;
};



#endif /* _SIFT_FEATURE_CALCULATOR__H_ */
