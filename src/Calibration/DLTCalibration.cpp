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
// Filename:  DLTCalibration.cpp
// Author:    Pedram Azad
// Date:      04.12.2009
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "Math/Math3d.h"
#include "Math/Math2d.h"
#include "Math/FloatMatrix.h"
#include "Math/FloatVector.h"
#include "Math/LinearAlgebra.h"
#include "Calibration/Calibration.h"

#include "DLTCalibration.h"

#include <stdio.h>
#include <math.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CDLTCalibration::CDLTCalibration()
{
	m_pPairElements = 0;
	m_nPairElements = 0;

	L1 = L6 = 1;
	L2 = L3 = L4 = L5 = L7 = L8 = L9 = L10 = L11 = 0;
}

CDLTCalibration::~CDLTCalibration()
{
}


// ****************************************************************************
// Methods
// ****************************************************************************

float CDLTCalibration::Calibrate(const CDLTCalibration::PairElement *pPairElements, int nPairElements, CCalibration &resultCalibration, DistortionType eCalculateDistortionParameters, int nIterations)
{
	m_pPairElements = pPairElements;
	m_nPairElements = nPairElements;
	
	resultCalibration.SetDistortion(0, 0, 0, 0);
	
	if (eCalculateDistortionParameters == eNoDistortion)
	{
		CalculateDLT(resultCalibration, true);
		ExtractFromDLT(resultCalibration);
	}
	else
	{
		for (int i = 0; i < nIterations; i++)
		{
			CalculateDLT(resultCalibration, i == 0);
			ExtractFromDLT(resultCalibration);
			
			if (eCalculateDistortionParameters == eRadialDistortion)
				CalculateRadialLensDistortion(resultCalibration);
			else if (eCalculateDistortionParameters == eRadialAndTangentialDistortion)
				CalculateRadialAndTangentialLensDistortion(resultCalibration);
		}
	}
	
	return CheckCalibration(resultCalibration);
}


void CDLTCalibration::GetImageCoordinatesDLT(const Vec3d &worldPoint, Vec2d &imagePoint)
{
	const float x = worldPoint.x;
	const float y = worldPoint.y;
	const float z = worldPoint.z;
		
	imagePoint.x = (L1 * x + L2 * y + L3 * z + L4) / (L9 * x + L10 * y + L11 * z + 1);
	imagePoint.y = (L5 * x + L6 * y + L7 * z + L8) / (L9 * x + L10 * y + L11 * z + 1);
}


float CDLTCalibration::CheckCalibration(const CCalibration &calibration)
{
	float error = 0.0f;
	float max = 0.0f;
	
	for (int i = 0; i < m_nPairElements; i++)
	{
		Vec2d imagePoint;
		calibration.WorldToImageCoordinates(m_pPairElements[i].worldPoint, imagePoint);
		
		const float distance = Math2d::Distance(m_pPairElements[i].imagePoint, imagePoint);
		
		if (distance > max)
			max = distance;
		
		error += distance;
	}
	
	return error / m_nPairElements;
}


float CDLTCalibration::CheckDLT()
{
	float error = 0.0f;
	
	for (int i = 0; i < m_nPairElements; i++)
	{
		Vec2d imagePoint;
		GetImageCoordinatesDLT(m_pPairElements[i].worldPoint, imagePoint);
		
		error += Math2d::Distance(m_pPairElements[i].imagePoint, imagePoint);
	}
	
	return error / m_nPairElements;
}


void CDLTCalibration::CalculateRadialLensDistortion(CCalibration &calibration)
{
	CFloatMatrix A(2, 2 * m_nPairElements);
	CFloatVector b(2 * m_nPairElements);
	
	const float cx = calibration.GetCameraParameters().principalPoint.x;
	const float cy = calibration.GetCameraParameters().principalPoint.y;
	const float fx = calibration.GetCameraParameters().focalLength.x;
	const float fy = calibration.GetCameraParameters().focalLength.y;
	
	for (int i = 0; i < m_nPairElements; i++)
	{
		const int ii = 2 * i;
		
		Vec2d undistorted_point;
		calibration.WorldToImageCoordinates(m_pPairElements[i].worldPoint, undistorted_point, false);
		
		const float u = undistorted_point.x;
		const float v = undistorted_point.y;
		const float ud = m_pPairElements[i].imagePoint.x;
		const float vd = m_pPairElements[i].imagePoint.y;
		
		const float x = (u - cx) / fx;
		const float y = (v - cy) / fy;
		
		const float rr = x * x + y * y;
		
		A(0, ii) = (u - cx) * rr;
		A(1, ii) = (u - cx) * rr * rr;
		A(0, ii + 1) = (v - cy) * rr;
		A(1, ii + 1) = (v - cy) * rr * rr;
		
		b[ii] = ud - u;
		b[ii + 1] = vd - v;
	}
	
	CFloatVector result(2);
	LinearAlgebra::SolveLinearLeastSquaresSimple(&A, &b, &result);
	
	calibration.SetDistortion((float) result[0], (float) result[1], 0, 0);
}


void CDLTCalibration::CalculateRadialAndTangentialLensDistortion(CCalibration &calibration)
{
	CFloatMatrix A(4, 2 * m_nPairElements);
	CFloatVector b(2 * m_nPairElements);
	
	const float cx = calibration.GetCameraParameters().principalPoint.x;
	const float cy = calibration.GetCameraParameters().principalPoint.y;
	const float fx = calibration.GetCameraParameters().focalLength.x;
	const float fy = calibration.GetCameraParameters().focalLength.y;
	
	for (int i = 0; i < m_nPairElements; i++)
	{
		const int ii = 2 * i;
		
		Vec2d undistorted_point;
		calibration.WorldToImageCoordinates(m_pPairElements[i].worldPoint, undistorted_point, false);
		
		const float u = undistorted_point.x;
		const float v = undistorted_point.y;
		const float ud = m_pPairElements[i].imagePoint.x;
		const float vd = m_pPairElements[i].imagePoint.y;
		
		const float x = (u - cx) / fx;
		const float y = (v - cy) / fy;
		
		const float rr = x * x + y * y;
		
		A(0, ii) = (u - cx) * rr;
		A(1, ii) = (u - cx) * rr * rr;
		A(2, ii) = 2 * x * y * fx;
		A(3, ii) = (rr + 2 * x * x) * fx;
		A(0, ii + 1) = (v - cy) * rr;
		A(1, ii + 1) = (v - cy) * rr * rr;
		A(2, ii + 1) = (rr + 2 * y * y) * fy;
		A(3, ii + 1) = 2 * x * y * fy;
		
		b[ii] = ud - u;
		b[ii + 1] = vd - v;
	}
	
	CFloatVector result(4);
	LinearAlgebra::SolveLinearLeastSquaresSimple(&A, &b, &result);
	
	calibration.SetDistortion((float) result[0], (float) result[1], (float) result[2], (float) result[3]);
}


void CDLTCalibration::ExtractFromDLT(CCalibration &calibration)
{
	const float LL = L9 * L9 + L10 * L10 + L11 * L11;
	const float L = sqrt(LL);
	const float atc = L1 * L9 + L2 * L10 + L3 * L11;
	const float btc = L5 * L9 + L6 * L10 + L7 * L11;
	const float atb = L1 * L5 * L2 * L6 + L3 * L7;
	const float ata = L1 * L1 + L2 * L2 + L3 * L3;
	const float btb = L5 * L5 + L6 * L6 + L7 * L7;
	
	const float cx = atc / LL;
	const float cy = btc / LL;
	const float fx = sqrt(ata / LL - cx * cx);
	const float fy = sqrt(btb / LL - cy * cy);
	
	const float r31 = L9 / L;
	const float r32 = L10 / L;
	const float r33 = L11 / L;
	const float r11 = (L1 / L - cx * r31) / fx;
	const float r12 = (L2 / L - cx * r32) / fx;
	const float r13 = (L3 / L - cx * r33) / fx;
	const float r21 = (L5 / L - cy * r31) / fy;
	const float r22 = (L6 / L - cy * r32) / fy;
	const float r23 = (L7 / L - cy * r33) / fy;
	
	const Mat3d rotation = { r11, r12, r13, r21, r22, r23, r31, r32, r33 };
	Vec3d translation;
	
	Mat3d abc = { L1, L2, L3, L5, L6, L7, L9, L10, L11 };
	Math3d::Invert(abc, abc);
	
	const Vec3d x = { L4, L8, 1 };
	Math3d::MulMatVec(abc, x, translation);
	Math3d::MulMatVec(rotation, translation, translation);
	
	calibration.SetIntrinsicBase(cx, cy, fx, fy);
	calibration.SetRotation(rotation);
	calibration.SetTranslation(translation);
}


void CDLTCalibration::CalculateDLT(const CCalibration &calibration, bool bFirstCall)
{
	CFloatMatrix A(11, 2 * m_nPairElements);
	CFloatVector b(2 * m_nPairElements);
	
	for (int i = 0; i < m_nPairElements; i++)
	{
		const float x = m_pPairElements[i].worldPoint.x;
		const float y = m_pPairElements[i].worldPoint.y;
		const float z = m_pPairElements[i].worldPoint.z;
		
		float u = m_pPairElements[i].imagePoint.x;
		float v = m_pPairElements[i].imagePoint.y;
		
		if (!bFirstCall)
		{
			const Vec2d distortedPoint = { u, v };
			Vec2d undistortedPoint;

			calibration.UndistortImageCoordinates(distortedPoint, undistortedPoint);

			u = undistortedPoint.x;
			v = undistortedPoint.y;
		}
		
		const int ii = 2 * i;
		
		A(0, ii) = x;
		A(1, ii) = y;
		A(2, ii) = z;
		A(3, ii) = 1;
		A(4, ii) = 0;
		A(5, ii) = 0;
		A(6, ii) = 0;
		A(7, ii) = 0;
		A(8, ii) = -u * x;
		A(9, ii) = -u * y;
		A(10, ii) = -u * z;
		
		A(0, ii + 1) = 0;
		A(1, ii + 1) = 0;
		A(2, ii + 1) = 0;
		A(3, ii + 1) = 0;
		A(4, ii + 1) = x;
		A(5, ii + 1) = y;
		A(6, ii + 1) = z;
		A(7, ii + 1) = 1;
		A(8, ii + 1) = -v * x;
		A(9, ii + 1) = -v * y;
		A(10, ii + 1) = -v * z;
		
		b[ii] = u;
		b[ii + 1] = v;
	}
	
	CFloatVector result(11);
	LinearAlgebra::SolveLinearLeastSquaresSimple(&A, &b, &result);
	
	L1 = (float) result[0];
	L2 = (float) result[1];
	L3 = (float) result[2];
	L4 = (float) result[3];
	L5 = (float) result[4];
	L6 = (float) result[5];
	L7 = (float) result[6];
	L8 = (float) result[7];
	L9 = (float) result[8];
	L10 = (float) result[9];
	L11 = (float) result[10];
}
