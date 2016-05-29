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
// Filename:  RGBColorModel.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "RGBColorModel.h"

#include "Math/Math3d.h"

#include <math.h>
#include <stdio.h>



// *****************************************************************
// Constructor / Destructor
// *****************************************************************

CRGBColorModel::CRGBColorModel()
{
	m_pTriplets = 0;
	m_nMaxNumberOfTriplets = 0;
	m_nCurrentPosition = 0;
	m_fFactor = 0.5f;
}

CRGBColorModel::~CRGBColorModel()
{
	if (m_pTriplets)
		delete [] m_pTriplets;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CRGBColorModel::Reset(int nMaxNumberOfTriplets)
{
	if (m_pTriplets)
		delete [] m_pTriplets;
		
	m_nMaxNumberOfTriplets = nMaxNumberOfTriplets;
	m_nCurrentPosition = 0;
		
	m_pTriplets = new Vec3d[nMaxNumberOfTriplets];
}

bool CRGBColorModel::AddRGBTriplet(int r, int g, int b)
{
	if (m_nCurrentPosition >= m_nMaxNumberOfTriplets)
		return false;

	Math3d::SetVec(m_pTriplets[m_nCurrentPosition++], float(r), float(g), float(b));
	
	return true;
}

void CRGBColorModel::CalculateColorModel()
{
	Mat3d rgb_covariance;
	int i;
	
	Math3d::SetVec(rgb_mean, Math3d::zero_vec);
	
	for (i = 0; i < m_nCurrentPosition; i++)
		Math3d::AddToVec(rgb_mean, m_pTriplets[i]);
		
	Math3d::MulVecScalar(rgb_mean, 1.0f / m_nCurrentPosition, rgb_mean);
	
	Math3d::SetMat(rgb_covariance, Math3d::zero_mat);
	
	for (i = 0; i < m_nCurrentPosition; i++)
	{
		const float r = m_pTriplets[i].x - rgb_mean.x;
		const float g = m_pTriplets[i].y - rgb_mean.y;
		const float b = m_pTriplets[i].z - rgb_mean.z;
		rgb_covariance.r1 += r * r;
		rgb_covariance.r2 += r * g;
		rgb_covariance.r3 += r * b;
		rgb_covariance.r4 += r * g;
		rgb_covariance.r5 += g * g;
		rgb_covariance.r6 += g * b;
		rgb_covariance.r7 += r * b;
		rgb_covariance.r8 += g * b;
		rgb_covariance.r9 += b * b;
	}
	
	Math3d::MulMatScalar(rgb_covariance, 1.0f / (m_nCurrentPosition - 1), rgb_covariance);
	Math3d::Invert(rgb_covariance, inverse_rgb_covariance);
	
	printf("mean = %f %f %f\n", rgb_mean.x, rgb_mean.y, rgb_mean.z);
}

float CRGBColorModel::CalculateColorProbability(const Vec3d &rgb)
{
	Vec3d diff;
	
	Math3d::SubtractVecVec(rgb, rgb_mean, diff);

	return exp(-m_fFactor * Math3d::EvaluateForm(diff, inverse_rgb_covariance));
}

float CRGBColorModel::CalculateColorDistanceSquared(int r, int g, int b)
{
	const float rd = (r - rgb_mean.x);
	const float gd = (g - rgb_mean.y);
	const float bd = (b - rgb_mean.z);
	
	return rd * rd + gd * gd + bd * bd;
}
	
bool CRGBColorModel::LoadFromFile(const char *pFileName)
{
	FILE *f = fopen(pFileName, "r");
	if (!f)
		return false;
	
	if (fscanf(f, "%f %f %f\n", &rgb_mean.x, &rgb_mean.y, &rgb_mean.z) == EOF)
	{
		fclose(f);
		return false;
	}
		
	if (fscanf(f, "%f %f %f %f %f %f %f %f %f\n",
		&inverse_rgb_covariance.r1, &inverse_rgb_covariance.r2, &inverse_rgb_covariance.r3,
		&inverse_rgb_covariance.r4, &inverse_rgb_covariance.r5, &inverse_rgb_covariance.r6,
		&inverse_rgb_covariance.r7, &inverse_rgb_covariance.r8, &inverse_rgb_covariance.r9) == EOF)
	{
		fclose(f);
		return false;
	}
		
	fclose(f);
		
	return true;
}

bool CRGBColorModel::SaveToFile(const char *pFileName)
{
	FILE *f = fopen(pFileName, "w");
	if (!f)
		return false;
	
	if (fprintf(f, "%.10f %.10f %.10f\n", rgb_mean.x, rgb_mean.y, rgb_mean.z) < 0)
	{
		fclose(f);
		return false;
	}
		
	if (fprintf(f, "%.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f\n",
		inverse_rgb_covariance.r1, inverse_rgb_covariance.r2, inverse_rgb_covariance.r3,
		inverse_rgb_covariance.r4, inverse_rgb_covariance.r5, inverse_rgb_covariance.r6,
		inverse_rgb_covariance.r7, inverse_rgb_covariance.r8, inverse_rgb_covariance.r9) < 0)
	{
		fclose(f);
		return false;
	}
		
	fclose(f);
	
	return true;
}

void CRGBColorModel::SetMean(const Vec3d &mean)
{
	Math3d::SetVec(rgb_mean, mean);
}

void CRGBColorModel::SetInverseCovariance(const Mat3d &inverse_covariance)
{
	Math3d::SetMat(inverse_rgb_covariance, inverse_covariance);
}
