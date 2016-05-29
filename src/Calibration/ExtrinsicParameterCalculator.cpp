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
// Filename:  ExtrinsicParameterCalculator.cpp
// Author:    Pedram Azad
// Date:      22.12.2009
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ExtrinsicParameterCalculator.h"
#include "Image/PrimitivesDrawer.h"
#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"
#include "Calibration/Calibration.h"
#include "Math/Math3d.h"
#include "Math/Math2d.h"
#include "Helpers/helpers.h"

#include <stdio.h>



// ****************************************************************************
// Functions
// ****************************************************************************

bool ExtrinsicParameterCalculator::GetPointsAndTranslationAndRotation(const CCalibration *pCalibration,
		const CByteImage *pImage, int nColumns, int nRows, float fSquareSize, // input
		Vec2d *pPoints, Mat3d &rotation, Vec3d &translation) // output
{
	printf("not yet implemented\n");
	return false;
}


void ExtrinsicParameterCalculator::DrawExtrinsic(CByteImage *pResultImage, const CCalibration *pCalibration, const Vec2d *pPoints, int nPoints, float fSquareSize)
{
	const Vec3d worldPoint0 = Math3d::zero_vec;
	const Vec3d worldPointX = { 2.0f * fSquareSize, 0.0f, 0.0f };
	const Vec3d worldPointY = { 0.0f, 2.0f * fSquareSize, 0.0f };
	
	Vec2d imagePoint0, imagePointX, imagePointY;
	pCalibration->WorldToImageCoordinates(worldPoint0, imagePoint0);
	pCalibration->WorldToImageCoordinates(worldPointX, imagePointX);
	pCalibration->WorldToImageCoordinates(worldPointY, imagePointY);
	
	PrimitivesDrawer::DrawLine(pResultImage, imagePoint0, imagePointX, 255, 255, 255, 3);
	PrimitivesDrawer::DrawLine(pResultImage, imagePoint0, imagePointY, 255, 255, 255, 3);
	PrimitivesDrawer::DrawCircle(pResultImage, imagePoint0, 5, 255, 255, 255, -1);
	
	if (pPoints && nPoints > 0)
	{
		Vec2d last_point;
		Math2d::SetVec(last_point, pPoints[0]);

		for (int i = 0; i < nPoints; i++)
		{
			int r, g, b;
			hsv2rgb(int(float(i) / (nPoints - 1) * 240), 255, 255, r, g, b);
			PrimitivesDrawer::DrawCircle(pResultImage, pPoints[i], 3, r, g, b, -1);
			PrimitivesDrawer::DrawLine(pResultImage, last_point, pPoints[i], r, g, b, 1);
			Math2d::SetVec(last_point, pPoints[i]);
		}
	}
}
