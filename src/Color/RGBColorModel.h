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
// Filename:  RGBColorModel.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************

/** \defgroup ColorProcessing Color Processing */

#ifndef __RGB_COLOR_MODEL_H__
#define __RGB_COLOR_MODEL_H__


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Math/Math3d.h"



// ****************************************************************************
// CRGBColorModel
// ****************************************************************************

/*!
	\ingroup ColorProcessing
	\brief Training and application of an RGB color model on the basis of the Mahalanobis distance.
*/
class CRGBColorModel
{
public:
	// constructor
	CRGBColorModel();

	// destructor
	~CRGBColorModel();


	// public methods
	void Reset(int nMaxNumberOfTriplets);
	bool AddRGBTriplet(int r, int g, int b);
	void CalculateColorModel();
	
	// saving/loading color models
	bool LoadFromFile(const char *pFileName);
	bool SaveToFile(const char *pFileName);

	// member access
	void SetFactor(float fFactor) { m_fFactor = fFactor; }
	void SetMean(const Vec3d &mean);
	void SetInverseCovariance(const Mat3d &inverse_covariance);

	// others
	// rgb is a vector containing an RGB triplet
	float CalculateColorProbability(const Vec3d &rgb);
	float CalculateColorDistanceSquared(int r, int g, int b);


private:
	// private attributes
	Vec3d rgb_mean;
	Mat3d inverse_rgb_covariance;
	
	Vec3d *m_pTriplets;
	int m_nMaxNumberOfTriplets;
	int m_nCurrentPosition;
	float m_fFactor;
};



#endif /* __RGB_COLOR_MODEL_H__ */
