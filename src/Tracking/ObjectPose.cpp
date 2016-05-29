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
// Filename:  ObjectPose.h
// Author:    Moritz Ritter
// Date:      08.04.2008
// ****************************************************************************

// ****************************************************************************
// Implementation of the paper:
// "Fast and Globally Convergent Pose Estimation from Video Images"
// IEEE Transactions on Pattern Analysis and Machine Intelligence,
// vol. 22, no.6, June 2000
//
// Solves the 2D to 3D mapping problem for 3 or more non-colinear
// points (also for coplanar points).
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ObjectPose.h"

#include "Math/Math2d.h"
#include "Math/Math3d.h"
#include "Math/DoubleMatrix.h"
#include "Math/LinearAlgebra.h"
#include "Calibration/Calibration.h"

#include <math.h>
#include <stdio.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define TOL 1e-5
#define EPSILON 1e-8




// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CObjectPose::CObjectPose(const Vec3d *pObjectPoints, int nPoints)
{
	int i;

	m_nPoints = nPoints;

	// normalized object points 3xn matrix
	m_pP = new Vec3d[m_nPoints]; 
	
	m_tPbar = pObjectPoints[0];
	for (i = 1; i < m_nPoints; i++)
		Math3d::AddToVec(m_tPbar, pObjectPoints[i]);

	Math3d::MulVecScalar(m_tPbar, 1.0f / m_nPoints, m_tPbar);

	for (i = 0; i < m_nPoints; i++)
		Math3d::SubtractVecVec(pObjectPoints[i], m_tPbar, m_pP[i]);

	// homogeneous normalized image points 3xn matrix
	m_pQ = new Vec3d[m_nPoints];

	// 3x3xn matrix
	m_pF = new Mat3d[m_nPoints];
}

CObjectPose::~CObjectPose()
{
	delete [] m_pP;
	delete [] m_pQ;
	delete [] m_pF;
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CObjectPose::EstimatePose(const Vec2d *pImagePoints,
	Mat3d &rotationMatrix, Vec3d &translationVector,
	const CCalibration *pCalibration, int nMaxIterations)
{
	if (m_nPoints < 3)
	{
		printf("error: too few points for object pose estimation.\n");
		return false;
	}

	int i;

	// normalize image points
	const Vec2d &focalLength = pCalibration->GetCameraParameters().focalLength;
	const Vec2d &principalPoint = pCalibration->GetCameraParameters().principalPoint;

	for (i = 0; i < m_nPoints; i++)
	{
		m_pQ[i].x = (pImagePoints[i].x - principalPoint.x) / focalLength.x;
		m_pQ[i].y = (pImagePoints[i].y - principalPoint.y) / focalLength.y;
		m_pQ[i].z = 1.0;
	}

	// compute projection matrices
	for (i = 0; i < m_nPoints; i++)
	{
		const float scalar = 1.0f / Math3d::SquaredLength(m_pQ[i]);
		Math3d::MulVecTransposedVec(m_pQ[i], m_pQ[i], m_pF[i]);
		Math3d::MulMatScalar(m_pF[i], scalar, m_pF[i]);
	}

	// compute the matrix factor required for computing t ( inv( eye(3) - sum(F,3) / n ) / n )
	Mat3d sum;
	Mat3d temp;
	// sum(F, 3)
	sum = m_pF[0];
	for (i = 1; i < m_nPoints; i++)
		Math3d::AddToMat(sum, m_pF[i]);
	// sum / n
	Math3d::MulMatScalar(sum, 1.0f / m_nPoints, sum);
	// eye(3) - sum
	Math3d::SubtractMatMat(Math3d::unit_mat, sum, temp);
	// inv(temp)
	Math3d::Invert(temp, temp);
	// m_tFactor = temp / n
	Math3d::MulMatScalar(temp, 1.0f / m_nPoints, m_tFactor);

	float old_error, new_error;
	int it = 0;

	AbsKernel(rotationMatrix, translationVector, old_error);
	it++;
	AbsKernel(rotationMatrix, translationVector, new_error);
	it++;

	while (fabsf((old_error - new_error) / old_error) > TOL && new_error > EPSILON && it < nMaxIterations)
	{
		old_error = new_error;
		AbsKernel(rotationMatrix, translationVector, new_error);
		it++;
	}

	// get back to original reference frame
	Vec3d vec;
	Math3d::MulMatVec(rotationMatrix, m_tPbar, vec);
	Math3d::SubtractVecVec(translationVector, vec, translationVector);

	// take into account extrinsic calibration
	const Mat3d &Rc = pCalibration->m_rotation_inverse;
	const Vec3d &tc = pCalibration->m_translation_inverse;
	Math3d::MulMatMat(Rc, rotationMatrix, rotationMatrix);
	Math3d::MulMatVec(Rc, translationVector, tc, translationVector);

	return true;
}

void CObjectPose::AbsKernel(Mat3d &R, Vec3d &t, float &error)
{
	int i;

	// compute Q
	for (i = 0; i < m_nPoints; i++)
		Math3d::MulMatVec(m_pF[i], m_pQ[i], m_pQ[i]);
	
	// compute Q relative to qbar
	Vec3d qbar;
	qbar = m_pQ[0];
	for (i = 1; i < m_nPoints; i++)
		Math3d::AddToVec(qbar, m_pQ[i]);
	Math3d::MulVecScalar(qbar, 1.0f / m_nPoints, qbar);
	for (i = 0; i < m_nPoints; i++)
		Math3d::SubtractVecVec(m_pQ[i], qbar, m_pQ[i]);

	// compute M matrix ( M = M + P(:, i) * Q(:, i)' )
	CDoubleMatrix fM(3, 3);
	Mat3d temp;
	for (i = 0; i < 9; i++)
		fM.data[i] = 0;	
	for (i = 0; i < m_nPoints; i++)
	{
		Math3d::MulVecTransposedVec(m_pP[i], m_pQ[i], temp);
		fM.data[0] = fM.data[0] + temp.r1; fM.data[1] = fM.data[1] + temp.r2; fM.data[2] = fM.data[2] + temp.r3;
		fM.data[3] = fM.data[3] + temp.r4; fM.data[4] = fM.data[4] + temp.r5; fM.data[5] = fM.data[5] + temp.r6;
		fM.data[6] = fM.data[6] + temp.r7; fM.data[7] = fM.data[7] + temp.r8; fM.data[8] = fM.data[8] + temp.r9;
	}
	
	// calculate SVD of M
	CDoubleMatrix fU(3, 3);
	CDoubleMatrix fS(3, 3);
	CDoubleMatrix fV(3, 3);
	LinearAlgebra::SVD(&fM, &fS, &fU, &fV);
	
	// compute transposed U
	Mat3d transU;
	LinearAlgebra::Transpose(&fU, &fU);
	transU.r1 = float(fU.data[0]); transU.r2 = float(fU.data[1]); transU.r3 = float(fU.data[2]);
	transU.r4 = float(fU.data[3]); transU.r5 = float(fU.data[4]); transU.r6 = float(fU.data[5]);
	transU.r7 = float(fU.data[6]); transU.r8 = float(fU.data[7]); transU.r9 = float(fU.data[8]);
	
	// R = V * U'
	temp.r1 = float(fV.data[0]); temp.r2 = float(fV.data[1]); temp.r3 = float(fV.data[2]);
	temp.r4 = float(fV.data[3]); temp.r5 = float(fV.data[4]); temp.r6 = float(fV.data[5]);
	temp.r7 = float(fV.data[6]); temp.r8 = float(fV.data[7]); temp.r9 = float(fV.data[8]);
	Math3d::MulMatMat(temp, transU, R);

	if (Math3d::Det(R) > 0)
	{
		// no need to change R
		// estimate current translation
		t = tEstimate(R);

		if (t.z < 0) // we need to invert t (object should be in front of the camera)
		{
			// R = -[V(:, 1:2) -V(:, 3)] * U'
			temp.r1 = float(-fV.data[0]); temp.r2 = float(-fV.data[1]); temp.r3 = float(fV.data[2]);
			temp.r4 = float(-fV.data[3]); temp.r5 = float(-fV.data[4]); temp.r6 = float(fV.data[5]);
			temp.r7 = float(-fV.data[6]); temp.r8 = float(-fV.data[7]); temp.r9 = float(fV.data[8]);
			Math3d::MulMatMat(temp, transU, R);
			// estimate current translation
			t = tEstimate(R);
		}
	}
	else
	{
		// R = [V(:, 1:2) -V(:, 3)] * U'
		temp.r1 = float(fV.data[0]); temp.r2 = float(fV.data[1]); temp.r3 = float(-fV.data[2]);
		temp.r4 = float(fV.data[3]); temp.r5 = float(fV.data[4]); temp.r6 = float(-fV.data[5]);
		temp.r7 = float(fV.data[6]); temp.r8 = float(fV.data[7]); temp.r9 = float(-fV.data[8]);
		Math3d::MulMatMat(temp, transU, R);
		// estimate current translation
		t = tEstimate(R);

		if (t.z < 0) // we need to invert t (object should be in front of the camera)
		{
			// R = -V * U'
			temp.r1 = float(-fV.data[0]); temp.r2 = float(-fV.data[1]); temp.r3 = float(-fV.data[2]);
			temp.r4 = float(-fV.data[3]); temp.r5 = float(-fV.data[4]); temp.r6 = float(-fV.data[5]);
			temp.r7 = float(-fV.data[6]); temp.r8 = float(-fV.data[7]); temp.r9 = float(-fV.data[8]);
			Math3d::MulMatMat(temp, transU, R);
			// estimate current translation
			t = tEstimate(R);
		}
	}

	// calculate current Q
	for (i = 0; i < m_nPoints; i++)
	{
		// Q(:, i) = R * P(: , i) + t
		Math3d::MulMatVec(R, m_pP[i], m_pQ[i]);
		Math3d::AddVecVec(m_pQ[i], t, m_pQ[i]);
	}

	// calculate error
	error = 0.0f;
	Vec3d vec;

	for (i = 0; i < m_nPoints; i++)
	{
		// (eye(3) - F(:, :, i)) * Q(:, i)
		Math3d::SubtractMatMat(Math3d::unit_mat, m_pF[i], temp);
		Math3d::MulMatVec(temp, m_pQ[i], vec);

		error += Math3d::SquaredLength(vec);
	}
}

Vec3d CObjectPose::tEstimate(Mat3d &R)
{
	Vec3d sum = Math3d::zero_vec;
	Vec3d temp;

	// sum = sum + F(:, :, i) * R * P(:, i)
	for (int i = 0; i < m_nPoints; i++)
	{
		Math3d::MulMatVec(R, m_pP[i], temp);
		Math3d::MulMatVec(m_pF[i], temp, temp);
		Math3d::AddToVec(sum, temp);
	}

	Math3d::MulMatVec(m_tFactor, sum, sum);

	return sum;
}
