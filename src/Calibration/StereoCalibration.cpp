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
// Filename:  StereoCalibration.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "StereoCalibration.h"

#include "Math/Math2d.h"
#include "Math/Math3d.h"
#include "Image/ByteImage.h"
#include "Image/ImageProcessor.h"
#include "Calibration/Calibration.h"
#include "Helpers/helpers.h"

#include <math.h>
#include <stdio.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CStereoCalibration::CStereoCalibration()
{
	m_pLeftCalibration = new CCalibration();
	m_pRightCalibration = new CCalibration();

	width = m_pLeftCalibration->GetCameraParameters().width;
	height = m_pLeftCalibration->GetCameraParameters().height;
	
	Vec3d t = { -100, 0, 0 };
	SetExtrinsicParameters(Math3d::unit_mat, Math3d::zero_vec, Math3d::unit_mat, t, false);
}

CStereoCalibration::~CStereoCalibration()
{
	delete m_pLeftCalibration;
	delete m_pRightCalibration;
}

CStereoCalibration::CStereoCalibration(const CStereoCalibration &stereoCalibration)
{
	m_pLeftCalibration = new CCalibration();
	m_pRightCalibration = new CCalibration();
	
	m_pLeftCalibration->Set(*stereoCalibration.GetLeftCalibration());
	m_pRightCalibration->Set(*stereoCalibration.GetRightCalibration());
	
	width = stereoCalibration.width;
	height = stereoCalibration.height;
	
	Math3d::SetMat(rectificationHomographyLeft, stereoCalibration.rectificationHomographyLeft);
	Math3d::SetMat(rectificationHomographyRight, stereoCalibration.rectificationHomographyRight);
	Math3d::SetMat(F, stereoCalibration.F);
	Math3d::SetMat(FT, stereoCalibration.FT);
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CStereoCalibration::Set(const CStereoCalibration &stereoCalibration)
{
	width = stereoCalibration.width;
	height = stereoCalibration.height;

	Math3d::SetMat(F, stereoCalibration.F);
	Math3d::SetMat(FT, stereoCalibration.FT);

	Math3d::SetMat(rectificationHomographyLeft, stereoCalibration.rectificationHomographyLeft);
	Math3d::SetMat(rectificationHomographyRight, stereoCalibration.rectificationHomographyRight);

	m_pLeftCalibration->Set(*stereoCalibration.GetLeftCalibration());
	m_pRightCalibration->Set(*stereoCalibration.GetRightCalibration());
}

void CStereoCalibration::SetDistortionParameters(float d1_left, float d2_left, float d3_left, float d4_left, float d1_right, float d2_right, float d3_right, float d4_right)
{
	m_pLeftCalibration->SetDistortion(d1_left, d2_left, d3_left, d4_left);
	m_pRightCalibration->SetDistortion(d1_right, d2_right, d3_right, d4_right);
}

void CStereoCalibration::SetSingleCalibrations(const CCalibration &leftCalibration, const CCalibration &rightCalibration, bool bTransformLeftCameraToIdentity)
{
	m_pLeftCalibration->Set(leftCalibration);
	m_pRightCalibration->Set(rightCalibration);

	width = leftCalibration.GetCameraParameters().width;
	height = leftCalibration.GetCameraParameters().height;
	
	if (bTransformLeftCameraToIdentity)
	{
		TransformLeftCameraToIdentity();
		CalculateFundamentalMatrix();
	}
	else
	{
		Mat3d left_rotation, right_rotation;
		Vec3d left_translation, right_translation;
		
		// store original transformations
		Math3d::SetMat(left_rotation, leftCalibration.GetCameraParameters().rotation);
		Math3d::SetMat(right_rotation, rightCalibration.GetCameraParameters().rotation);
		
		Math3d::SetVec(left_translation, leftCalibration.GetCameraParameters().translation);
		Math3d::SetVec(right_translation, rightCalibration.GetCameraParameters().translation);
		
		// calculate epipolar geometry
		TransformLeftCameraToIdentity();
		CalculateFundamentalMatrix();
	
		// reset transformations
		m_pLeftCalibration->SetRotation(left_rotation);
		m_pLeftCalibration->SetTranslation(left_translation);
	
		m_pRightCalibration->SetRotation(right_rotation);
		m_pRightCalibration->SetTranslation(right_translation);
	}
}

void CStereoCalibration::SetExtrinsicParameters(const Mat3d& left_rotation, const Vec3d& left_translation, const Mat3d& right_rotation, const Vec3d& right_translation, bool bTransformToIdentity)
{
	// set rotation of left and right calibration
	m_pLeftCalibration->SetRotation(left_rotation);
	m_pLeftCalibration->SetTranslation(left_translation);
	
	m_pRightCalibration->SetRotation(right_rotation);
	m_pRightCalibration->SetTranslation(right_translation);
	
	// transform to identy for fundemantal matrix calculation
	TransformLeftCameraToIdentity();
	
	// update fundametal matrix
	CalculateFundamentalMatrix();
	
	if(!bTransformToIdentity)
	{
		// reset transformation of left and right calibration
		m_pLeftCalibration->SetRotation(left_rotation);
		m_pLeftCalibration->SetTranslation(left_translation);
		
		m_pRightCalibration->SetRotation(right_rotation);
		m_pRightCalibration->SetTranslation(right_translation);	
	}
}


void CStereoCalibration::TransformLeftCameraToIdentity()
{
	Mat3d tempMat;
	Vec3d tempVec;
	
	const CCalibration::CCameraParameters &cameraParametersLeft = m_pLeftCalibration->GetCameraParameters();
	const CCalibration::CCameraParameters &cameraParametersRight = m_pRightCalibration->GetCameraParameters();
	
	Math3d::MulMatMat(cameraParametersRight.rotation, m_pLeftCalibration->m_rotation_inverse, tempMat);
	Math3d::MulMatVec(tempMat, cameraParametersLeft.translation, tempVec);
	Math3d::SubtractVecVec(cameraParametersRight.translation, tempVec, tempVec);
		
	m_pLeftCalibration->SetRotation(Math3d::unit_mat);
	m_pLeftCalibration->SetTranslation(Math3d::zero_vec);
	
	m_pRightCalibration->SetRotation(tempMat);
	m_pRightCalibration->SetTranslation(tempVec);
}

void CStereoCalibration::CalculateFundamentalMatrix()
{
	const CCalibration::CCameraParameters &left = m_pLeftCalibration->GetCameraParameters();
	const CCalibration::CCameraParameters &right = m_pRightCalibration->GetCameraParameters();
	const Mat3d &R = right.rotation;
	const Vec3d &t = right.translation;

	Mat3d E = { 0, -t.z, t.y, t.z, 0, -t.x, -t.y, t.x, 0 };
	Math3d::MulMatMat(E, R, E);
	
	Mat3d K1 = { left.focalLength.x, 0, left.principalPoint.x, 0, left.focalLength.y, left.principalPoint.y, 0, 0, 1 };
	Math3d::Invert(K1, K1);
	
	Mat3d K2 = { right.focalLength.x, 0, right.principalPoint.x, 0, right.focalLength.y, right.principalPoint.y, 0, 0, 1 };
	Math3d::Invert(K2, K2);
	Math3d::Transpose(K2, K2);
	
	Math3d::MulMatMat(K2, E, F);
	Math3d::MulMatMat(F, K1, F);
	
	Math3d::Transpose(F, FT);
}


void CStereoCalibration::Calculate3DPoint(const Vec2d &cameraPointLeft, const Vec2d &cameraPointRight, Vec3d &worldPoint, bool bInputImagesAreRectified, bool bUseDistortionParameters, PointPair3d *pConnectionLine)
{
	Vec2d pointLeft, pointRight;
	
	if (bInputImagesAreRectified)
	{
		Math2d::ApplyHomography(rectificationHomographyLeft, cameraPointLeft, pointLeft);
		Math2d::ApplyHomography(rectificationHomographyRight, cameraPointRight, pointRight);
	}
	else
	{
		Math2d::SetVec(pointLeft, cameraPointLeft);
		Math2d::SetVec(pointRight, cameraPointRight);
	}
	
	// calculate world coordinates of some reference point on the view line (not origin of the camera coordinate system)
	Vec3d u, v, a, b;
	m_pLeftCalibration->ImageToWorldCoordinates(pointLeft, u, 1, bUseDistortionParameters);
	m_pRightCalibration->ImageToWorldCoordinates(pointRight, v, 1, bUseDistortionParameters);
	
	// get "starting vector" of straight line
	Math3d::SetVec(a, m_pLeftCalibration->m_translation_inverse);
	Math3d::SetVec(b, m_pRightCalibration->m_translation_inverse);
	
	// calculate direction vector of straight line
	Math3d::SubtractFromVec(u, a);
	Math3d::SubtractFromVec(v, b);
	
	// left straight line:  x = r * u + a
	// right straight line: x = s * v + b
	// A x = b
	float A00 = -u.x; float A01 = v.x;
	float A10 = -u.y; float A11 = v.y;
	float A20 = -u.z; float A21 = v.z;
	float b0 = a.x - b.x;
	float b1 = a.y - b.y;
	float b2 = a.z - b.z;
	
	float ATA00 = A00 * A00 + A10 * A10 + A20 * A20;
	float ATA10 = A01 * A00 + A11 * A10 + A21 * A20;
	float ATA11 = A01 * A01 + A11 * A11 + A21 * A21;
	float ATb0 = A00 * b0 + A10 * b1 + A20 * b2;
	float ATb1 = A01 * b0 + A11 * b1 + A21 * b2;
	
	float L00 = sqrtf(ATA00);
	float L10 = ATA10 / L00;
	float L11 = sqrtf(ATA11 - L10 * L10);
	
	float yy0 = ATb0 / L00;
	float yy1 = (ATb1 - L10 * yy0) / L11;
	
	float s = yy1 / L11;
	float r = (yy0 - L10 * s) / L00;
	
	// p = (r * u + a + s * v + b) / 2
	Math3d::SetVec(worldPoint,
		(r * u.x + a.x + s * v.x + b.x) * 0.5f,
		(r * u.y + a.y + s * v.y + b.y) * 0.5f,
		(r * u.z + a.z + s * v.z + b.z) * 0.5f
	);
	
	if (pConnectionLine)
	{
		Math3d::SetVec(pConnectionLine->p1, r * u.x + a.x, r * u.y + a.y, r * u.z + a.z);
		Math3d::SetVec(pConnectionLine->p2, s * v.x + b.x, s * v.y + b.y, s * v.z + b.z);
	}
}


void CStereoCalibration::CalculateEpipolarLineInLeftImage(const Vec2d &pointInRightImage, Vec3d &l)
{
	Vec3d x = { pointInRightImage.x, pointInRightImage.y, 1 };
	Math3d::MulMatVec(FT, x, l);
}

void CStereoCalibration::CalculateEpipolarLineInLeftImage(const Vec2d &pointInRightImage, float &m, float &c)
{
	Vec3d l;
	CalculateEpipolarLineInLeftImage(pointInRightImage, l);
	
	m = -l.x / l.y;
	c = -l.z / l.y;
}

void CStereoCalibration::CalculateEpipolarLineInLeftImage(const Vec2d &pointInRightImage, PointPair2d &epipolarLine)
{
	Vec3d l;
	CalculateEpipolarLineInLeftImage(pointInRightImage, l);
	
	const float v1 = l.y != 0.0f ? -l.z / l.y : -1.0f;
	const float v2 = l.y != 0.0f ? (-l.z - l.x * float(width - 1)) / l.y : -1.0f;
	const float u1 = l.x != 0.0f ? -l.z / l.x : -1.0f;
	const float u2 = l.x != 0.0f ? (-l.z - l.y * float(height - 1)) / l.x : -1.0f;
	
	if (v1 >= 0.0f && v1 <= float(height - 1))
		Math2d::SetVec(epipolarLine.p1, 0.0f, v1);
	else if (v2 >= 0.0f && v2 <= float(height - 1))
		Math2d::SetVec(epipolarLine.p1, float(width - 1), v2);
	else if (u1 >= 0.0f && u1 <= float(width - 1))
		Math2d::SetVec(epipolarLine.p1, u1, 0.0f);
	else if (u2 >= 0.0f && u2 <= float(width - 1))
		Math2d::SetVec(epipolarLine.p1, u2, float(height - 1));
	
	if (v1 >= 0.0f && v1 <= float(height - 1) && (epipolarLine.p1.x != 0.0f || epipolarLine.p1.y != v1))
		Math2d::SetVec(epipolarLine.p2, 0.0f, v1);
	else if (v2 >= 0.0f && v2 <= float(height - 1) && (epipolarLine.p1.x != float(width - 1) || epipolarLine.p1.y != v2))
		Math2d::SetVec(epipolarLine.p2, float(width - 1), v2);
	else if (u1 >= 0.0f && u1 <= float(width - 1) && (epipolarLine.p1.x != u1 || epipolarLine.p1.y != 0.0f))
		Math2d::SetVec(epipolarLine.p2, u1, 0.0f);
	else if (u2 >= 0.0f && u2 <= float(width - 1) && (epipolarLine.p1.x != u2 || epipolarLine.p1.y != float(height - 1)))
		Math2d::SetVec(epipolarLine.p2, u2, float(height - 1));
}


void CStereoCalibration::CalculateEpipolarLineInRightImage(const Vec2d &pointInLeftImage, Vec3d &l)
{
	Vec3d x = { pointInLeftImage.x, pointInLeftImage.y, 1 };
	Math3d::MulMatVec(F, x, l);
}

void CStereoCalibration::CalculateEpipolarLineInRightImage(const Vec2d &pointInLeftImage, float &m, float &c)
{
	Vec3d l;
	CalculateEpipolarLineInRightImage(pointInLeftImage, l);
	
	m = -l.x / l.y;
	c = -l.z / l.y;
}

void CStereoCalibration::CalculateEpipolarLineInRightImage(const Vec2d &pointInLeftImage, PointPair2d &epipolarLine)
{
	Vec3d l;
	CalculateEpipolarLineInRightImage(pointInLeftImage, l);
	
	const float v1 = l.y != 0.0f ? -l.z / l.y : -1.0f;
	const float v2 = l.y != 0.0f ? (-l.z - l.x * float(width - 1)) / l.y : -1.0f;
	const float u1 = l.x != 0.0f ? -l.z / l.x : -1.0f;
	const float u2 = l.x != 0.0f ? (-l.z - l.y * float(height - 1)) / l.x : -1.0f;
	
	if (v1 >= 0.0f && v1 <= float(height - 1))
		Math2d::SetVec(epipolarLine.p1, 0.0f, v1);
	else if (v2 >= 0.0f && v2 <= float(height - 1))
		Math2d::SetVec(epipolarLine.p1, float(width - 1), v2);
	else if (u1 >= 0.0f && u1 <= float(width - 1))
		Math2d::SetVec(epipolarLine.p1, u1, 0.0f);
	else if (u2 >= 0.0f && u2 <= float(width - 1))
		Math2d::SetVec(epipolarLine.p1, u2, float(height - 1));
	
	if (v1 >= 0.0f && v1 <= float(height - 1) && (epipolarLine.p1.x != 0.0f || epipolarLine.p1.y != v1))
		Math2d::SetVec(epipolarLine.p2, 0.0f, v1);
	else if (v2 >= 0.0f && v2 <= float(height - 1) && (epipolarLine.p1.x != float(width - 1) || epipolarLine.p1.y != v2))
		Math2d::SetVec(epipolarLine.p2, float(width - 1), v2);
	else if (u1 >= 0.0f && u1 <= float(width - 1) && (epipolarLine.p1.x != u1 || epipolarLine.p1.y != 0.0f))
		Math2d::SetVec(epipolarLine.p2, u1, 0.0f);
	else if (u2 >= 0.0f && u2 <= float(width - 1) && (epipolarLine.p1.x != u2 || epipolarLine.p1.y != float(height - 1)))
		Math2d::SetVec(epipolarLine.p2, u2, float(height - 1));
}


float CStereoCalibration::CalculateEpipolarLineInLeftImageDistance(const Vec2d &pointInLeftImage, const Vec2d &pointInRightImage)
{
	Vec3d l;
	CalculateEpipolarLineInLeftImage(pointInRightImage, l);
	
	const float length = sqrtf(l.x * l.x + l.y * l.y);
	const float l1 = l.x / length;
	const float l2 = l.y / length;
	const float l3 = l.z / length;
	
	return l1 * pointInLeftImage.x + l2 * pointInLeftImage.y + l3;
}

float CStereoCalibration::CalculateEpipolarLineInRightImageDistance(const Vec2d &pointInLeftImage, const Vec2d &pointInRightImage)
{
	Vec3d l;
	CalculateEpipolarLineInRightImage(pointInLeftImage, l);
	
	const float length = sqrtf(l.x * l.x + l.y * l.y);
	const float l1 = l.x / length;
	const float l2 = l.y / length;
	const float l3 = l.z / length;
	
	return l1 * pointInRightImage.x + l2 * pointInRightImage.y + l3;
}


bool CStereoCalibration::LoadCameraParameters(const char *pCameraParameterFileName, bool bTransformLeftCameraToIdentity)
{
	if (!m_pLeftCalibration->LoadCameraParameters(pCameraParameterFileName, 0))
		return false;
	
	if (!m_pRightCalibration->LoadCameraParameters(pCameraParameterFileName, 1))
		return false;
	
	if (bTransformLeftCameraToIdentity)
	{
		TransformLeftCameraToIdentity();
		CalculateFundamentalMatrix();
	}
	else
	{
		Mat3d left_rotation, right_rotation;
		Vec3d left_translation, right_translation;
		
		// store original transformations
		Math3d::SetMat(left_rotation, m_pLeftCalibration->GetCameraParameters().rotation);
		Math3d::SetMat(right_rotation, m_pRightCalibration->GetCameraParameters().rotation);
		
		Math3d::SetVec(left_translation, m_pLeftCalibration->GetCameraParameters().translation);
		Math3d::SetVec(right_translation, m_pRightCalibration->GetCameraParameters().translation);
		
		// calculate epipolar geometry
		TransformLeftCameraToIdentity();
		CalculateFundamentalMatrix();
		
		// reset transformations
		m_pLeftCalibration->SetRotation(left_rotation);
		m_pLeftCalibration->SetTranslation(left_translation);
		
		m_pRightCalibration->SetRotation(right_rotation);
		m_pRightCalibration->SetTranslation(right_translation);
	}
	
	int nCameraCount = 0;
	
	FILE* f = fopen(pCameraParameterFileName, "r");
	if(!f)
		return false;
	
	if (fscanf(f, "%d", &nCameraCount) != 1 || nCameraCount != 2)
	{
		fclose(f);
        return false;
	}
	
	float skip_value;
	
	// skip values (27 for each camera, 2 * 8 for stereo quads)
	for (int i = 0; i < 2 * 27 + 16; i++)
		if (fscanf(f, "%f", &skip_value) != 1)
		{
			fclose(f);
			return false;
		}
	
	// read rectification parameters
	if (fscanf(f, "%f", &rectificationHomographyLeft.r1) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyLeft.r2) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyLeft.r3) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyLeft.r4) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyLeft.r5) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyLeft.r6) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyLeft.r7) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyLeft.r8) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyLeft.r9) != 1) { fclose(f); return false; }
	
	if (fscanf(f, "%f", &rectificationHomographyRight.r1) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyRight.r2) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyRight.r3) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyRight.r4) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyRight.r5) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyRight.r6) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyRight.r7) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyRight.r8) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &rectificationHomographyRight.r9) != 1) { fclose(f); return false; }
	
	fclose(f);
	
	width = m_pLeftCalibration->GetCameraParameters().width;
	height = m_pLeftCalibration->GetCameraParameters().height;
	
	return true;
}

bool CStereoCalibration::SaveCameraParameters(const char *pFileName) const
{
	FILE *f = fopen(pFileName, "w");
	if (!f)
		return false;
	
	fprintf(f, "2\n\n");
	
	const CCalibration::CCameraParameters &cameraParametersLeft = m_pLeftCalibration->GetCameraParameters();
	const CCalibration::CCameraParameters &cameraParametersRight = m_pRightCalibration->GetCameraParameters();
	
	// write camera parameters of left camera
	float cx = cameraParametersLeft.principalPoint.x;
	float cy = cameraParametersLeft.principalPoint.y;
	float fx = cameraParametersLeft.focalLength.x;
	float fy = cameraParametersLeft.focalLength.y;
	float d1 = cameraParametersLeft.distortion[0];
	float d2 = cameraParametersLeft.distortion[1];
	float d3 = cameraParametersLeft.distortion[2];
	float d4 = cameraParametersLeft.distortion[3];
	
	fprintf(f, "%.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f ", float(cameraParametersLeft.width), float(cameraParametersLeft.height), fx, 0.0f, cx, 0.0f, fy, cy, 0.0f, 0.0f, 1.0f);
	fprintf(f, "%.10f %.10f %.10f %.10f ", d1, d2, d3, d4);
	fprintf(f, "%.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f ", cameraParametersLeft.rotation.r1, cameraParametersLeft.rotation.r2, cameraParametersLeft.rotation.r3, cameraParametersLeft.rotation.r4, cameraParametersLeft.rotation.r5, cameraParametersLeft.rotation.r6, cameraParametersLeft.rotation.r7, cameraParametersLeft.rotation.r8, cameraParametersLeft.rotation.r9);
	fprintf(f, "%.10f %.10f %.10f\n\n", cameraParametersLeft.translation.x, cameraParametersLeft.translation.y, cameraParametersLeft.translation.z);
	
	// write camera parameters of right camera
	cx = cameraParametersRight.principalPoint.x;
	cy = cameraParametersRight.principalPoint.y;
	fx = cameraParametersRight.focalLength.x;
	fy = cameraParametersRight.focalLength.y;
	d1 = cameraParametersRight.distortion[0];
	d2 = cameraParametersRight.distortion[1];
	d3 = cameraParametersRight.distortion[2];
	d4 = cameraParametersRight.distortion[3];
	
	fprintf(f, "%.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f ", float(cameraParametersLeft.width), float(cameraParametersLeft.height), fx, 0.0f, cx, 0.0f, fy, cy, 0.0f, 0.0f, 1.0f);
	fprintf(f, "%.10f %.10f %.10f %.10f ", d1, d2, d3, d4);
	fprintf(f, "%.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f ", cameraParametersRight.rotation.r1, cameraParametersRight.rotation.r2, cameraParametersRight.rotation.r3, cameraParametersRight.rotation.r4, cameraParametersRight.rotation.r5, cameraParametersRight.rotation.r6, cameraParametersRight.rotation.r7, cameraParametersRight.rotation.r8, cameraParametersRight.rotation.r9);
	fprintf(f, "%.10f %.10f %.10f\n\n", cameraParametersRight.translation.x, cameraParametersRight.translation.y, cameraParametersRight.translation.z);
	
	// skipped values of OpenCV format
	fprintf(f, "%.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f\n", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	fprintf(f, "%.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f\n", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	
	// write rectification homography of left camera
	fprintf(f, "%.10f ", rectificationHomographyLeft.r1);
	fprintf(f, "%.10f ", rectificationHomographyLeft.r2);
	fprintf(f, "%.10f ", rectificationHomographyLeft.r3);
	fprintf(f, "%.10f ", rectificationHomographyLeft.r4);
	fprintf(f, "%.10f ", rectificationHomographyLeft.r5);
	fprintf(f, "%.10f ", rectificationHomographyLeft.r6);
	fprintf(f, "%.10f ", rectificationHomographyLeft.r7);
	fprintf(f, "%.10f ", rectificationHomographyLeft.r8);
	fprintf(f, "%.10f ", rectificationHomographyLeft.r9);
	fprintf(f, "\n");
	
	// write rectification homography of right camera
	fprintf(f, "%.10f ", rectificationHomographyRight.r1);
	fprintf(f, "%.10f ", rectificationHomographyRight.r2);
	fprintf(f, "%.10f ", rectificationHomographyRight.r3);
	fprintf(f, "%.10f ", rectificationHomographyRight.r4);
	fprintf(f, "%.10f ", rectificationHomographyRight.r5);
	fprintf(f, "%.10f ", rectificationHomographyRight.r6);
	fprintf(f, "%.10f ", rectificationHomographyRight.r7);
	fprintf(f, "%.10f ", rectificationHomographyRight.r8);
	fprintf(f, "%.10f ", rectificationHomographyRight.r9);
	fprintf(f, "\n");
	
	fclose(f);
	
	return true;
}

void CStereoCalibration::GetProjectionMatricesForRectifiedImages(Mat3d &P1Left, Vec3d &p2Left, Mat3d &P1Right, Vec3d &p2Right) const
{
	Mat3d K, H;
	
	// left camera
	Math3d::Invert(rectificationHomographyLeft, H);
	m_pLeftCalibration->GetCalibrationMatrix(K);
	Math3d::MulMatMat(H, K, K);
	Math3d::MulMatMat(K, m_pLeftCalibration->GetCameraParameters().rotation, P1Left);
	Math3d::MulMatVec(K, m_pLeftCalibration->GetCameraParameters().translation, p2Left);
	
	// right camera
	Math3d::Invert(rectificationHomographyRight, H);
	m_pRightCalibration->GetCalibrationMatrix(K);
	Math3d::MulMatMat(H, K, K);
	Math3d::MulMatMat(K, m_pRightCalibration->GetCameraParameters().rotation, P1Right);
	Math3d::MulMatVec(K, m_pRightCalibration->GetCameraParameters().translation, p2Right);
}
