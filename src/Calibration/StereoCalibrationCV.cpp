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
// Filename:  StereoCalibrationCV.cpp
// Author:    Pedram Azad
// Date:      2006
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "StereoCalibrationCV.h"
#include "StereoCalibration.h"
#include "Calibration.h"
#include "Math/Math3d.h"

#include <cvaux.h>
#include <stdio.h>



static void FillMatrix(float *pTargetMatrix, const Mat3d &sourceMatrix)
{
	pTargetMatrix[0] = sourceMatrix.r1;
	pTargetMatrix[1] = sourceMatrix.r2;
	pTargetMatrix[2] = sourceMatrix.r3;
	pTargetMatrix[3] = sourceMatrix.r4;
	pTargetMatrix[4] = sourceMatrix.r5;
	pTargetMatrix[5] = sourceMatrix.r6;
	pTargetMatrix[6] = sourceMatrix.r7;
	pTargetMatrix[7] = sourceMatrix.r8;
	pTargetMatrix[8] = sourceMatrix.r9;
}

static void FillMatrix(double *pTargetMatrix, const Mat3d &sourceMatrix)
{
	pTargetMatrix[0] = sourceMatrix.r1;
	pTargetMatrix[1] = sourceMatrix.r2;
	pTargetMatrix[2] = sourceMatrix.r3;
	pTargetMatrix[3] = sourceMatrix.r4;
	pTargetMatrix[4] = sourceMatrix.r5;
	pTargetMatrix[5] = sourceMatrix.r6;
	pTargetMatrix[6] = sourceMatrix.r7;
	pTargetMatrix[7] = sourceMatrix.r8;
	pTargetMatrix[8] = sourceMatrix.r9;
}

static void FillVector(float *pTargetVector, const Vec3d &sourceVector)
{
	pTargetVector[0] = (float) sourceVector.x;
	pTargetVector[1] = (float) sourceVector.y;
	pTargetVector[2] = (float) sourceVector.z;
}

static void FillCalibrationMatrix(float *pTargetMatrix, const CCalibration::CCameraParameters &cameraParameters)
{
	pTargetMatrix[0] = (float) cameraParameters.focalLength.x;
	pTargetMatrix[1] = 0;
	pTargetMatrix[2] = (float) cameraParameters.principalPoint.x;
	pTargetMatrix[3] = 0;
	pTargetMatrix[4] = (float) cameraParameters.focalLength.y;
	pTargetMatrix[5] = (float) cameraParameters.principalPoint.y;
	pTargetMatrix[6] = 0;
	pTargetMatrix[7] = 0;
	pTargetMatrix[8] = 1;
}


void StereoCalibrationCV::CalculateRectificationHomographies(CStereoCalibration *pStereoCalibration)
{
	CvStereoCamera stereoParams;
	CvCamera leftCamera, rightCamera;
	
	stereoParams.camera[0] = &leftCamera;
	stereoParams.camera[1] = &rightCamera;
	
	stereoParams.camera[0]->imgSize[0] = (float) pStereoCalibration->width;
    stereoParams.camera[0]->imgSize[1] = (float) pStereoCalibration->height;
    stereoParams.camera[1]->imgSize[0] = (float) pStereoCalibration->width;
    stereoParams.camera[1]->imgSize[1] = (float) pStereoCalibration->height;
	
	const CCalibration::CCameraParameters &leftCameraParameters = pStereoCalibration->GetLeftCalibration()->GetCameraParameters();
	const CCalibration::CCameraParameters &rightCameraParameters = pStereoCalibration->GetRightCalibration()->GetCameraParameters();
	
	FillMatrix(stereoParams.rotMatrix, pStereoCalibration->GetRightCalibration()->m_rotation_inverse);
	FillVector(stereoParams.transVector, pStereoCalibration->GetRightCalibration()->m_translation_inverse);
	
	int i;
	
	leftCamera.imgSize[0] = (float) leftCameraParameters.width;
	leftCamera.imgSize[1] = (float) leftCameraParameters.height;
	FillCalibrationMatrix(leftCamera.matrix, leftCameraParameters);
	for (i = 0; i < 4; i++) leftCamera.distortion[i] = (float) leftCameraParameters.distortion[i];
	FillMatrix(leftCamera.rotMatr, leftCameraParameters.rotation);
	FillVector(leftCamera.transVect, leftCameraParameters.translation);
	
	rightCamera.imgSize[0] = (float) rightCameraParameters.width;
	rightCamera.imgSize[1] = (float) rightCameraParameters.height;
	FillCalibrationMatrix(rightCamera.matrix, rightCameraParameters);
	for (i = 0; i < 4; i++) rightCamera.distortion[i] = (float) rightCameraParameters.distortion[i];
	FillMatrix(rightCamera.rotMatr, rightCameraParameters.rotation);
	FillVector(rightCamera.transVect, rightCameraParameters.translation);
	
	icvComputeRestStereoParams(&stereoParams);
	
	Math3d::SetMat(pStereoCalibration->rectificationHomographyLeft,
		float(stereoParams.coeffs[0][0][0]),
		float(stereoParams.coeffs[0][0][1]),
		float(stereoParams.coeffs[0][0][2]),
		float(stereoParams.coeffs[0][1][0]),
		float(stereoParams.coeffs[0][1][1]),
		float(stereoParams.coeffs[0][1][2]),
		float(stereoParams.coeffs[0][2][0]),
		float(stereoParams.coeffs[0][2][1]),
		float(stereoParams.coeffs[0][2][2])
	);
	
	Math3d::SetMat(pStereoCalibration->rectificationHomographyRight,
		float(stereoParams.coeffs[1][0][0]),
		float(stereoParams.coeffs[1][0][1]),
		float(stereoParams.coeffs[1][0][2]),
		float(stereoParams.coeffs[1][1][0]),
		float(stereoParams.coeffs[1][1][1]),
		float(stereoParams.coeffs[1][1][2]),
		float(stereoParams.coeffs[1][2][0]),
		float(stereoParams.coeffs[1][2][1]),
		float(stereoParams.coeffs[1][2][2])
	);
}

/*void StereoCalibrationCV::CalculateRectificationHomographiesExperimental(CStereoCalibration *pStereoCalibration)
{
	// algorithm by Fusiello et al. from 2000:
	// "A compact algorithm for rectification of stereo pairs"
	const CCalibration::CCameraParameters &left = pStereoCalibration->GetLeftCalibration()->GetCameraParameters();
	const CCalibration::CCameraParameters &right = pStereoCalibration->GetRightCalibration()->GetCameraParameters();
	
	const Mat3d &R1 = pStereoCalibration->GetLeftCalibration()->m_rotation_inverse;//left.rotation;
	const Mat3d &R2 = pStereoCalibration->GetRightCalibration()->m_rotation_inverse;//right.rotation;
	
	const Vec3d &t1 = pStereoCalibration->GetLeftCalibration()->m_translation_inverse;//left.translation;
	const Vec3d &t2 = pStereoCalibration->GetRightCalibration()->m_translation_inverse;//right.translation;
	
	const Mat3d A1 = { -left.focalLength.x, 0, left.principalPoint.x, 0, -left.focalLength.y, left.principalPoint.y, 0, 0, 1 };
	const Mat3d A2 = { -right.focalLength.x, 0, right.principalPoint.x, 0, -right.focalLength.y, right.principalPoint.y, 0, 0, 1 };
	
	printf("\nA1 = %f %f %f\n%f %f %f\n%f %f %f\n\n", A1.r1, A1.r2, A1.r3, A1.r4, A1.r5, A1.r6, A1.r7, A1.r8, A1.r9);
	printf("R1 = %f %f %f\n%f %f %f\n%f %f %f\n\n", R1.r1, R1.r2, R1.r3, R1.r4, R1.r5, R1.r6, R1.r7, R1.r8, R1.r9);
	printf("t1 = %f %f %f\n\n\n", t1.x, t1.y, t1.z);
	
	printf("A2 = %f %f %f\n%f %f %f\n%f %f %f\n\n", A2.r1, A2.r2, A2.r3, A2.r4, A2.r5, A2.r6, A2.r7, A2.r8, A2.r9);
	printf("R2 = %f %f %f\n%f %f %f\n%f %f %f\n\n", R2.r1, R2.r2, R2.r3, R2.r4, R2.r5, R2.r6, R2.r7, R2.r8, R2.r9);
	printf("t2 = %f %f %f\n\n\n", t2.x, t2.y, t2.z);

	const Vec3d &c1 = pStereoCalibration->GetLeftCalibration()->m_translation_inverse;
	const Vec3d &c2 = pStereoCalibration->GetRightCalibration()->m_translation_inverse;
	
	Vec3d v1, v2, v3, k = { R1.r7, R1.r8, R1.r9 };
	
	Math3d::SubtractVecVec(c1, c2, v1);
	Math3d::CrossProduct(k, v1, v2);
	Math3d::CrossProduct(v1, v2, v3);
	
	Math3d::NormalizeVec(v1);
	Math3d::NormalizeVec(v2);
	Math3d::NormalizeVec(v3);
	
	Mat3d R = { v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z };
	Mat3d A =
	{
		0.5f * (left.focalLength.x + right.focalLength.x), 0, 0.5f * (left.principalPoint.x + right.principalPoint.x),
		0, 0.5f * (left.focalLength.y + right.focalLength.y), 0.5f * (left.principalPoint.y + right.principalPoint.y),
		0, 0, 1
	};
	
	Mat3d T1, T2, M;
	
	Math3d::MulMatMat(A, R, T1);
	Math3d::MulMatMat(A1, R1, M);
	Math3d::Invert(M, M);
	Math3d::MulMatMat(T1, M, T1);
	
	Math3d::MulMatMat(A, R, T2);
	Math3d::MulMatMat(A2, R2, M);
	Math3d::Invert(M, M);
	Math3d::MulMatMat(T2, M, T2);
	
	Math3d::SetMat(pStereoCalibration->rectificationHomographyLeft, T1);
	Math3d::SetMat(pStereoCalibration->rectificationHomographyRight, T2);
}*/
