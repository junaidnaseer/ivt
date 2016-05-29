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
// Filename:  RANSAC.cpp
// Author:    Pedram Azad
// Date:      14.12.2009
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "RANSAC.h"

#include "Structs/Structs.h"
#include "Math/Math2d.h"
#include "Math/Math3d.h"
#include "Math/LinearAlgebra.h"

#include <stdlib.h>
#include <math.h>



// ****************************************************************************
// Functions
// ****************************************************************************

bool RANSAC::RANSACAffineTransformation(const CDynamicArrayTemplate<PointPair2d> &matchCandidates, CDynamicArrayTemplate<PointPair2d> &resultMatches, float fRANSACThreshold, int nIterations)
{
	const int nMatchCandidates = matchCandidates.GetSize();
	
	if (nMatchCandidates < 3)
	{
		printf("error: at least 3 match candidates must be provided for RANSAC::RANSACAffineTransformation (%i provided)\n", nMatchCandidates);
		return false;
	}
	
	Mat3d best_B;
	int i, max = 0;

	for (i = 0; i < nIterations; i++)
	{
		// identify 3 different points
		const int nFirstIndex = rand() % nMatchCandidates;
		
		int nTempIndex;
		
		do { nTempIndex = rand() % nMatchCandidates; } while (nTempIndex == nFirstIndex);
		const int nSecondIndex = nTempIndex;
		
		do { nTempIndex = rand() % nMatchCandidates; } while (nTempIndex == nFirstIndex || nTempIndex == nSecondIndex);
		
		Vec2d pFeaturesLeft[3];
		Vec2d pFeaturesRight[3];
		
		Math2d::SetVec(pFeaturesLeft[0], matchCandidates[nFirstIndex].p1);
		Math2d::SetVec(pFeaturesLeft[1], matchCandidates[nSecondIndex].p1);
		Math2d::SetVec(pFeaturesLeft[2], matchCandidates[nTempIndex].p1);
		
		Math2d::SetVec(pFeaturesRight[0], matchCandidates[nFirstIndex].p2);
		Math2d::SetVec(pFeaturesRight[1], matchCandidates[nSecondIndex].p2);
		Math2d::SetVec(pFeaturesRight[2], matchCandidates[nTempIndex].p2);
		
		// calculate affine transformation for these points
		Mat3d B;
		LinearAlgebra::DetermineAffineTransformation(pFeaturesRight, pFeaturesLeft, 3, B);
		
		// count support
		int nSupport = 0;
		for (int j = 0; j < nMatchCandidates; j++)
		{
			Vec2d p;
			Math2d::ApplyHomography(B, matchCandidates[j].p2, p);
			
			const float distance = Math2d::Distance(p, matchCandidates[j].p1);
			
			if (distance < fRANSACThreshold)
				nSupport++;
		}
		
		// store if it is the current maximum
		if (nSupport > max)
		{
			max = nSupport;
			Math3d::SetMat(best_B, B);
		}
	}

	// filter matches
	resultMatches.Clear();
	
	for (i = 0; i < nMatchCandidates; i++)
	{
		Vec2d p;
		Math2d::ApplyHomography(best_B, matchCandidates[i].p2, p);
		
		const float distance = Math2d::Distance(p, matchCandidates[i].p1);
		
		if (distance < fRANSACThreshold)
		{
			PointPair2d match;
			Math2d::SetVec(match.p1, matchCandidates[i].p1);
			Math2d::SetVec(match.p2, matchCandidates[i].p2);
			
			resultMatches.AddElement(match);
		}
	}

	return true;
}

bool RANSAC::RANSACHomography(const CDynamicArrayTemplate<PointPair2d> &matchCandidates, CDynamicArrayTemplate<PointPair2d> &resultMatches, float fRANSACThreshold, int nIterations)
{
	const int nMatchCandidates = matchCandidates.GetSize();
	
	if (nMatchCandidates < 4)
	{
		printf("error: at least 4 match candidates must be provided for RANSAC::RANSACHomography (%i provided)\n", nMatchCandidates);
		return false;
	}
	
	Mat3d best_B;
	int i, max = 0;
	
	for (i = 0; i < nIterations; i++)
	{
		// identify 4 different points
		const int nFirstIndex = rand() % nMatchCandidates;
		
		int nTempIndex;
		
		do { nTempIndex = rand() % nMatchCandidates; } while (nTempIndex == nFirstIndex);
		const int nSecondIndex = nTempIndex;
		
		do { nTempIndex = rand() % nMatchCandidates; } while (nTempIndex == nFirstIndex || nTempIndex == nSecondIndex);
		const int nThirdIndex = nTempIndex;
		
		do { nTempIndex = rand() % nMatchCandidates; } while (nTempIndex == nFirstIndex || nTempIndex == nSecondIndex || nTempIndex == nThirdIndex);
		
		Vec2d pFeaturesLeft[4];
		Vec2d pFeaturesRight[4];
		
		Math2d::SetVec(pFeaturesLeft[0], matchCandidates[nFirstIndex].p1);
		Math2d::SetVec(pFeaturesLeft[1], matchCandidates[nSecondIndex].p1);
		Math2d::SetVec(pFeaturesLeft[2], matchCandidates[nThirdIndex].p1);
		Math2d::SetVec(pFeaturesLeft[3], matchCandidates[nTempIndex].p1);
		
		Math2d::SetVec(pFeaturesRight[0], matchCandidates[nFirstIndex].p2);
		Math2d::SetVec(pFeaturesRight[1], matchCandidates[nSecondIndex].p2);
		Math2d::SetVec(pFeaturesRight[2], matchCandidates[nThirdIndex].p2);
		Math2d::SetVec(pFeaturesRight[3], matchCandidates[nTempIndex].p1);
		
		// calculate affine transformation for these points
		Mat3d B;
		LinearAlgebra::DetermineHomography(pFeaturesRight, pFeaturesLeft, 4, B, false);
		
		// count support
		int nSupport = 0;
		for (int j = 0; j < nMatchCandidates; j++)
		{
			Vec2d p;
			Math2d::ApplyHomography(B, matchCandidates[j].p2, p);
			
			const float distance = Math2d::Distance(p, matchCandidates[j].p1);
			
			if (distance < fRANSACThreshold)
				nSupport++;
		}
		
		// store if it is the current maximum
		if (nSupport > max)
		{
			max = nSupport;
			Math3d::SetMat(best_B, B);
		}
	}
	
	// filter matches
	resultMatches.Clear();
	
	for (i = 0; i < nMatchCandidates; i++)
	{
		Vec2d p;
		Math2d::ApplyHomography(best_B, matchCandidates[i].p2, p);
		
		const float distance = Math2d::Distance(p, matchCandidates[i].p1);
		
		if (distance < fRANSACThreshold)
		{
			PointPair2d match;
			Math2d::SetVec(match.p1, matchCandidates[i].p1);
			Math2d::SetVec(match.p2, matchCandidates[i].p2);
			
			resultMatches.AddElement(match);
		}
	}
	
	return true;
}

bool RANSAC::RANSAC3DPlane(const CVec3dArray &pointCandidates, CVec3dArray &resultPoints, float fRANSACThreshold, int nIterations)
{
	const int nPointCandidates = pointCandidates.GetSize();
	
	if (nPointCandidates < 3)
	{
		printf("error: at least 3 point candidates must be provided for RANSAC::RANSAC3DPlane (%i provided)\n", nPointCandidates);
		return false;
	}
	
	int i, max = 0;
	float best_c = 0.0f;
	Vec3d best_n = { 0.0f, 0.0f };
	
	for (i = 0; i < nIterations; i++)
	{
		// identify 3 different points
		const int nFirstIndex = rand() % nPointCandidates;
		
		int nTempIndex;
		
		do { nTempIndex = rand() % nPointCandidates; } while (nTempIndex == nFirstIndex);
		const int nSecondIndex = nTempIndex;
		
		do { nTempIndex = rand() % nPointCandidates; } while (nTempIndex == nFirstIndex || nTempIndex == nSecondIndex);
		
		const Vec3d &p1 = pointCandidates[nFirstIndex];
		const Vec3d &p2 = pointCandidates[nSecondIndex];
		const Vec3d &p3 = pointCandidates[nTempIndex];
		
		Vec3d u1, u2, n;
		Math3d::SubtractVecVec(p2, p1, u1);
		Math3d::SubtractVecVec(p3, p1, u2);
		Math3d::CrossProduct(u1, u2, n);
		Math3d::NormalizeVec(n);
		const float c = Math3d::ScalarProduct(n, p1);
		
		// count support
		int nSupport = 0;
		for (int j = 0; j < nPointCandidates; j++)
			if (fabsf(Math3d::ScalarProduct(n, pointCandidates[j]) - c) <= fRANSACThreshold)
				nSupport++;
		
		// store if it is the current maximum
		if (nSupport > max)
		{
			max = nSupport;
			Math3d::SetVec(best_n, n);
			best_c = c;
		}
	}
	
	// filter points
	resultPoints.Clear();
	
	for (i = 0; i < nPointCandidates; i++)
	{
		if (fabsf(Math3d::ScalarProduct(pointCandidates[i], best_n) - best_c) <= fRANSACThreshold)
			resultPoints.AddElement(pointCandidates[i]);
	}
	
	return true;
}
