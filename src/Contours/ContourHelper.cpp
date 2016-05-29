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
// Filename:  ContourHelper.cpp
// Authors:   Derick Beng Yuh, Pedram Azad
// Date:      03.08.2010
// ****************************************************************************

#include "ContourHelper.h"
#include "Math/Math2d.h"



// ****************************************************************************
// Helper function used for testing if a middle point (p2) lies above, below or
// on the line between p1 and p3.
// Above : function returns negative value
// Below : function returns positive value
// On    : function reurns 0
// ****************************************************************************
static inline float det(const Vec2d &p1, const Vec2d &p2, const Vec2d &p3)
{
	return ((p1.x - p2.x) * (p3.y - p2.y)) - ((p3.x - p2.x) * (p1.y - p2.y));
}

static void BuildLowerHalfHull(const CVec2dArray &input, CVec2dArray &output)
{
	// Repeatedly add the leftmost point to the hull, then test to see if a
	// convexity violation has occurred. If it has, fix things up  by removing
	// the next-to-last point in the output sequence until convexity is restored.
	const int nPoints = input.GetSize();

	for (int i = 0; i < nPoints; i++)
	{
		output.AddElement(input[i]);
		
		while (output.GetSize() >= 3)
		{
			const int nLastIndex = output.GetSize() - 1;
			
			if (det(output[nLastIndex - 2], output[nLastIndex - 1], output[nLastIndex]) <= 0.0f) 
				output.DeleteElement(nLastIndex - 1);
			else
				break;
		}
	}
}

static void BuildUpperHalfHull(const CVec2dArray &input, CVec2dArray &output)
{
	// Repeatedly add the rightmost point to the hull, then test to see if a
	// convexity violation has occurred. If it has, fix things up  by removing
	// the next-to-last point in the output sequence until convexity is restored.
	const int nPoints = input.GetSize();

	for (int i = nPoints - 1; i >= 0; i--)
	{
		output.AddElement(input[i]);
		
		while (output.GetSize() >= 3)
		{
			const int nLastIndex = output.GetSize() - 1;

			if (det(output[nLastIndex - 2], output[nLastIndex - 1], output[nLastIndex]) <= 0.0f) 
				output.DeleteElement(nLastIndex - 1);
			else
				break;
		}
	}
}

static void QuicksortX(CVec2dArray &pPoints, int nLow, int nHigh)
{
	int i = nLow;
	int j = nHigh;
		
	const float fX = pPoints[(nLow + nHigh) / 2].x;

	while (i <= j)
	{    
		while (pPoints[i].x < fX) i++; 
		while (pPoints[j].x > fX) j--;
            
		if (i <= j)
		{
			Vec2d temp;
			Math2d::SetVec(temp, pPoints[i]);
			Math2d::SetVec(pPoints[i], pPoints[j]);
			Math2d::SetVec(pPoints[j], temp);
		
			i++;
			j--;
		}
	}

	if (nLow < j) QuicksortX(pPoints, nLow, j);
	if (i < nHigh) QuicksortX(pPoints, i, nHigh);
}


void ContourHelper::ComputeConvexHull(const Vec2d *pPoints, int nPoints, Vec2d *pResultPoints, int &nResultPoints)
{
	int i;

	CVec2dArray vertices(2 * nPoints);
    for (i = 0; i < nPoints; i++)       
		vertices.AddElement(pPoints[i]);

	QuicksortX(vertices, 0, nPoints - 1);
	
	const int nLastPoint = vertices.GetSize() - 1;
    const Vec2d &left = vertices[0];
	const Vec2d &right = vertices[nLastPoint];

	// partition points on plane according to their position with
	// with respect to the line between left and right
	CVec2dArray upper(2 * nPoints), lower(2 * nPoints);
	upper.AddElement(left); // must be added before loop
	for (i = 1; i < nLastPoint; i++)
	{
		const Vec2d &point = vertices[i];

		if (det(left, point, right) < 0.0f)
		{
			// point is above the line between left and right 
			upper.AddElement(point);
		}
		else
		{
			// point is below the line between left and right 	
			lower.AddElement(point);
		}
	}
	lower.AddElement(right); // must be added after loop

	CVec2dArray hull;
	hull.AddElement(left);
	
	// first build lower hull
	BuildLowerHalfHull(lower, hull);

	// then build upper hull
	BuildUpperHalfHull(upper, hull);

	// add points to result
	nResultPoints = 0;
	const int nHullSize = hull.GetSize() - 1;
	for (i = 0; i < nHullSize; i++)
		Math2d::SetVec(pResultPoints[nResultPoints++], hull[i]);
}
