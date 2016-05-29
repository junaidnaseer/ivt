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
// Filename:  Calibration.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************
// Changes:   17.11.2006, Stefanie Speidel
//            * improved calculations for radial lens distortion
//            * added calculations for tangential lens distortion
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "Calibration.h"
#include "Math/Matd.h"
#include "Math/Vecd.h"
#include "Image/ByteImage.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>




// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CCalibration::CCalibration()
{
	m_cameraParameters.width = 640;
	m_cameraParameters.height = 480;
	
	Math2d::SetVec(m_cameraParameters.principalPoint, 320, 240);
	Math2d::SetVec(m_cameraParameters.focalLength, 580, 580);
	SetRotation(Math3d::unit_mat);
	SetTranslation(Math3d::zero_vec);
	
	for (int i = 0; i < 4; i++)
		m_cameraParameters.distortion[i] = 0;
}

CCalibration::~CCalibration()
{
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CCalibration::GetCalibrationMatrix(Mat3d &K) const
{
	Math3d::SetMat(K,
		m_cameraParameters.focalLength.x, 0, m_cameraParameters.principalPoint.x,
		0, m_cameraParameters.focalLength.y, m_cameraParameters.principalPoint.y,
		0, 0, 1);
}

void CCalibration::GetProjectionMatrix(Mat3d &P, Vec3d &p) const
{
	Mat3d K;
	GetCalibrationMatrix(K);

	Math3d::MulMatMat(K, m_cameraParameters.rotation, P);
	Math3d::MulMatVec(K, m_cameraParameters.translation, p);
}


void CCalibration::CalculateInverseTransformation()
{
	Math3d::Transpose(m_cameraParameters.rotation, m_rotation_inverse);
	Math3d::MulMatVec(m_rotation_inverse, m_cameraParameters.translation, m_translation_inverse);
	Math3d::MulVecScalar(m_translation_inverse, -1, m_translation_inverse);
}


void CCalibration::SetCameraParameters(float fx, float fy, float cx, float cy,
		float d1, float d2, float d3, float d4,
		const Mat3d &R, const Vec3d &t, int width, int height)
{
	m_cameraParameters.width = width;
	m_cameraParameters.height = height;
	
	Math3d::SetMat(m_cameraParameters.rotation, R);
	Math3d::SetVec(m_cameraParameters.translation, t);

	Math2d::SetVec(m_cameraParameters.focalLength, fx, fy);	
	Math2d::SetVec(m_cameraParameters.principalPoint, cx, cy);
	
	m_cameraParameters.distortion[0] = d1;
	m_cameraParameters.distortion[1] = d2;
	m_cameraParameters.distortion[2] = d3;
	m_cameraParameters.distortion[3] = d4;
	
	CalculateInverseTransformation();
}

void CCalibration::Set(const CCalibration &calibration)
{
	const CCameraParameters &cameraParameters = calibration.GetCameraParameters();
	
	m_cameraParameters.width = cameraParameters.width;
	m_cameraParameters.height = cameraParameters.height;
	
	Math3d::SetMat(m_cameraParameters.rotation, cameraParameters.rotation);
	Math3d::SetVec(m_cameraParameters.translation, cameraParameters.translation);

	Math2d::SetVec(m_cameraParameters.focalLength, cameraParameters.focalLength);	
	Math2d::SetVec(m_cameraParameters.principalPoint, cameraParameters.principalPoint);
	
	m_cameraParameters.distortion[0] = cameraParameters.distortion[0];
	m_cameraParameters.distortion[1] = cameraParameters.distortion[1];
	m_cameraParameters.distortion[2] = cameraParameters.distortion[2];
	m_cameraParameters.distortion[3] = cameraParameters.distortion[3];
	
	CalculateInverseTransformation();
}

void CCalibration::SetRotation(const Mat3d &rotation)
{
	Math3d::SetMat(m_cameraParameters.rotation, rotation);
	CalculateInverseTransformation();
}

void CCalibration::SetTranslation(const Vec3d &translation)
{
	Math3d::SetVec(m_cameraParameters.translation, translation);
	CalculateInverseTransformation();
}

void CCalibration::SetIntrinsicBase(float cx, float cy, float fx, float fy)
{
	Math2d::SetVec(m_cameraParameters.principalPoint, cx, cy);
	Math2d::SetVec(m_cameraParameters.focalLength, fx, fy);
}

void CCalibration::SetDistortion(float d1, float d2, float d3, float d4)
{
	m_cameraParameters.distortion[0] = d1;
	m_cameraParameters.distortion[1] = d2;
	m_cameraParameters.distortion[2] = d3;
	m_cameraParameters.distortion[3] = d4;
}


void CCalibration::WorldToImageCoordinates(const Vec3d &worldPoint, Vec2d &imagePoint, bool bUseDistortionParameters) const
{
	Vec3d cameraPoint;
	WorldToCameraCoordinates(worldPoint, cameraPoint);
	CameraToImageCoordinates(cameraPoint, imagePoint, bUseDistortionParameters);
}

void CCalibration::ImageToWorldCoordinates(const Vec2d &imagePoint, Vec3d &worldPoint, float zc, bool bUseDistortionParameters) const
{
	Vec3d cameraPoint;
	ImageToCameraCoordinates(imagePoint, cameraPoint, zc, bUseDistortionParameters);
	CameraToWorldCoordinates(cameraPoint, worldPoint);
}

void CCalibration::WorldToCameraCoordinates(const Vec3d &worldPoint, Vec3d &cameraPoint) const
{
	Math3d::MulMatVec(m_cameraParameters.rotation, worldPoint, m_cameraParameters.translation, cameraPoint);
}

void CCalibration::CameraToWorldCoordinates(const Vec3d &cameraPoint, Vec3d &worldPoint) const
{
	Math3d::MulMatVec(m_rotation_inverse, cameraPoint, m_translation_inverse, worldPoint);
}

void CCalibration::CameraToImageCoordinates(const Vec3d &cameraPoint, Vec2d &imagePoint, bool bUseDistortionParameters) const
{
	imagePoint.x = m_cameraParameters.principalPoint.x + (cameraPoint.x * m_cameraParameters.focalLength.x) / cameraPoint.z;
	imagePoint.y = m_cameraParameters.principalPoint.y + (cameraPoint.y * m_cameraParameters.focalLength.y) / cameraPoint.z;
	
	if (bUseDistortionParameters)
		DistortImageCoordinates(imagePoint, imagePoint);
}

void CCalibration::ImageToCameraCoordinates(const Vec2d &imagePoint_, Vec3d &cameraPoint, float zc, bool bUseDistortionParameters) const
{
	Vec2d imagePoint;
	
	if (bUseDistortionParameters)
		UndistortImageCoordinates(imagePoint_, imagePoint);
	else
		Math2d::SetVec(imagePoint, imagePoint_);
	
	cameraPoint.x = (imagePoint.x - m_cameraParameters.principalPoint.x) / m_cameraParameters.focalLength.x * zc;
	cameraPoint.y = (imagePoint.y - m_cameraParameters.principalPoint.y) / m_cameraParameters.focalLength.y * zc;
	cameraPoint.z = zc;
}

void CCalibration::UndistortImageCoordinates(const Vec2d &distortedImagePoint, Vec2d &undistortedImagePoint) const
{
	// distortion parameters
	const float d1 = m_cameraParameters.distortion[0];
	const float d2 = m_cameraParameters.distortion[1];
	const float d3 = m_cameraParameters.distortion[2];
	const float d4 = m_cameraParameters.distortion[3];
	
	// undistort
	const float x_const = (distortedImagePoint.x - m_cameraParameters.principalPoint.x) / m_cameraParameters.focalLength.x;
	const float y_const = (distortedImagePoint.y - m_cameraParameters.principalPoint.y) / m_cameraParameters.focalLength.y;
	
	float x = x_const;
	float y = y_const;
	
	for (int i = 0; i < 10; i++)
	{
		// radial distortion
		const float rr = x * x + y * y;
		const float factor = 1 + d1 * rr + d2 * rr * rr;
		
		// tangential distortion
		const float dx = d3 * (2 * x * y) + d4 * (rr + 2 * x * x);
		const float dy = d3 * (rr + 2 * y * y) + d4 * (2 * x * y);
		
		x = (x_const - dx) / factor;
		y = (y_const - dy) / factor;
	}
	
	undistortedImagePoint.x = x * m_cameraParameters.focalLength.x + m_cameraParameters.principalPoint.x;
	undistortedImagePoint.y = y * m_cameraParameters.focalLength.y + m_cameraParameters.principalPoint.y;
}

void CCalibration::DistortImageCoordinates(const Vec2d &undistortedImagePoint, Vec2d &distortedImagePoint) const
{
	const float x = (undistortedImagePoint.x - m_cameraParameters.principalPoint.x) / m_cameraParameters.focalLength.x;
	const float y = (undistortedImagePoint.y - m_cameraParameters.principalPoint.y) / m_cameraParameters.focalLength.y;
	
	// distortion parameters
	const float d1 = m_cameraParameters.distortion[0];
	const float d2 = m_cameraParameters.distortion[1];
	const float d3 = m_cameraParameters.distortion[2];
	const float d4 = m_cameraParameters.distortion[3];

	// radial distortion
	const float rr = x * x + y * y;
	const float factor = 1 + d1 * rr + d2 * rr * rr;
	
	// tangential distortion
	const float dx = d3 * (2 * x * y) + d4 * (rr + 2 * x * x);
	const float dy = d3 * (rr + 2 * y * y) + d4 * (2 * x * y);
	
	distortedImagePoint.x = m_cameraParameters.focalLength.x * (factor * x + dx) + m_cameraParameters.principalPoint.x;
	distortedImagePoint.y = m_cameraParameters.focalLength.y * (factor * y + dy) + m_cameraParameters.principalPoint.y;
}

// <obsolete names>
void CCalibration::GetCameraCoordinates(const Vec3d &worldPoint, Vec2d &imagePoint, bool bUseDistortionParameters) const
{
	WorldToImageCoordinates(worldPoint, imagePoint, bUseDistortionParameters);
}

void CCalibration::GetWorldCoordinates(const Vec2d &imagePoint, Vec3d &worldPoint, float zc, bool bUseDistortionParameters) const
{
	ImageToWorldCoordinates(imagePoint, worldPoint, zc, bUseDistortionParameters);
}

void CCalibration::UndistortCameraCoordinates(const Vec2d &distortedImagePoint, Vec2d &undistortedImagePoint) const
{
	UndistortImageCoordinates(distortedImagePoint, undistortedImagePoint);
}

void CCalibration::DistortCameraCoordinates(const Vec2d &undistortedImagePoint, Vec2d &distortedImagePoint) const
{
	DistortImageCoordinates(undistortedImagePoint, distortedImagePoint);
}
// </obsolete names>


bool CCalibration::LoadCameraParameters(const char *pCameraParameterFileName, int nCamera, bool bSetExtrinsicToIdentity)
{
	int i, nCameraCount = 0;
	
	FILE* f = fopen(pCameraParameterFileName, "r");
	if(!f)
		return false;
	
	if (fscanf(f, "%d", &nCameraCount) != 1 || nCameraCount < nCamera + 1)
	{
		fclose(f);
		return false;
	}
	
	float skip_value, w, h;
	
	for (i = 0; i < nCamera; i++)
		for (int j = 0; j < 27; j++)
			if (fscanf(f, "%f", &skip_value) != 1)
			{
				fclose(f);
				return false;
			}
	
	if (fscanf(f, "%f", &w) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &h) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.focalLength.x) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &skip_value) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.principalPoint.x) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &skip_value) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.focalLength.y) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.principalPoint.y) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &skip_value) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &skip_value) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &skip_value) != 1) { fclose(f); return false; }
	for (i = 0; i < 4; i++)
		if (fscanf(f, "%f", &m_cameraParameters.distortion[i]) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.rotation.r1) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.rotation.r2) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.rotation.r3) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.rotation.r4) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.rotation.r5) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.rotation.r6) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.rotation.r7) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.rotation.r8) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.rotation.r9) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.translation.x) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.translation.y) != 1) { fclose(f); return false; }
	if (fscanf(f, "%f", &m_cameraParameters.translation.z) != 1) { fclose(f); return false; }
	
	m_cameraParameters.width = int(w + 0.5f);
	m_cameraParameters.height = int(h + 0.5f);
	
	fclose(f);
	
	if (bSetExtrinsicToIdentity)
	{
		SetRotation(Math3d::unit_mat);
		SetTranslation(Math3d::zero_vec);
	}
	else
	{
		CalculateInverseTransformation();
	}
	
	return true;
}

bool CCalibration::SaveCameraParameters(const char *pFileName) const
{
	const Mat3d &rotation = m_cameraParameters.rotation;
	const Vec3d &translation = m_cameraParameters.translation;
	
	const float cx = m_cameraParameters.principalPoint.x;
	const float cy = m_cameraParameters.principalPoint.y;
	const float fx = m_cameraParameters.focalLength.x;
	const float fy = m_cameraParameters.focalLength.y;
	
	const float d1 = m_cameraParameters.distortion[0];
	const float d2 = m_cameraParameters.distortion[1];
	const float d3 = m_cameraParameters.distortion[2];
	const float d4 = m_cameraParameters.distortion[3];
	
	FILE *f = fopen(pFileName, "w");
	if (!f)
		return false;
	
	fprintf(f, "1\n\n");
	fprintf(f, "%i %i %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f ", m_cameraParameters.width, m_cameraParameters.height, fx, 0.0, cx, 0.0, fy, cy, 0.0, 0.0, 1.0);
	fprintf(f, "%.10f %.10f %.10f %.10f ", d1, d2, d3, d4);
	fprintf(f, "%.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f ", rotation.r1, rotation.r2, rotation.r3, rotation.r4, rotation.r5, rotation.r6, rotation.r7, rotation.r8, rotation.r9);
	fprintf(f, "%.10f %.10f %.10f\n", translation.x, translation.y, translation.z);

	fclose(f);

	return true;
}

void CCalibration::PrintCameraParameters() const
{
	printf("cx/cy = %.10f / %.10f\nfx/fy = %.10f / %.10f\nt = %.10f %.10f %.10f\nR = %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f %.10f\nd1/d2/d3/d4 = %.10f %.10f %.10f %.10f\n\n",
		   m_cameraParameters.principalPoint.x,
		   m_cameraParameters.principalPoint.y,
		   m_cameraParameters.focalLength.x,
		   m_cameraParameters.focalLength.y,
		   m_cameraParameters.translation.x,
		   m_cameraParameters.translation.y,
		   m_cameraParameters.translation.z,
		   m_cameraParameters.rotation.r1,
		   m_cameraParameters.rotation.r2,
		   m_cameraParameters.rotation.r3,
		   m_cameraParameters.rotation.r4,
		   m_cameraParameters.rotation.r5,
		   m_cameraParameters.rotation.r6,
		   m_cameraParameters.rotation.r7,
		   m_cameraParameters.rotation.r8,
		   m_cameraParameters.rotation.r9,
		   m_cameraParameters.distortion[0],
		   m_cameraParameters.distortion[1],
		   m_cameraParameters.distortion[2],
		   m_cameraParameters.distortion[3]
	);
}
