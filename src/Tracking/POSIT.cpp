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
// Filename:  Posit.cpp
// Author:    Pedram Azad
// Date:      23.01.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "POSIT.h"

#include "Math/Math2d.h"
#include "Math/Math3d.h"
#include "Math/DoubleMatrix.h"
#include "Math/DoubleVector.h"
#include "Math/LinearAlgebra.h"
#include "Calibration/Calibration.h"

#include <stdio.h>


// ****************************************************************************
// This is an implementation of the algorithm described in:
// D. F. DeMenthon and L. S. Davis, "Model-based Object Pose in 25 Lines of Code",
// in Proc. of European Conference on Computer Vision (ECCV), pages 335-343, 1992.
// respectively
// in International Journal of Computer Vision (IJCV), vol. 15, 1995.
// ****************************************************************************



// ****************************************************************************
// Functions
// ****************************************************************************

bool POSIT::POSIT(const Vec3d *pPoints3D, const Vec2d *pPoints2D, int nPoints, Mat3d &R, Vec3d &t, const CCalibration *pCalibration, int nIterations)
{
	const int N = nPoints;
	const Vec3d *M = pPoints3D;
	const Vec2d *m = pPoints2D;
	
	if (N < 4)
	{
		printf("error: too few points for POSIT\n");
		return false;
	}
	
	const float f = 0.5f * (pCalibration->GetCameraParameters().focalLength.x + pCalibration->GetCameraParameters().focalLength.y);
	const float cx = pCalibration->GetCameraParameters().principalPoint.x;
	const float cy = pCalibration->GetCameraParameters().principalPoint.y;
	
	const int N_ = N - 1;
	int l;
		
	// build matrix A and B (pseudoinverse of A)
	CDoubleMatrix A(3, N_);
	for (l = 0; l < N_; l++)
	{
		A(0, l) = M[l + 1].x - M[0].x;
		A(1, l) = M[l + 1].y - M[0].y;
		A(2, l) = M[l + 1].z - M[0].z;
	}
	
	// calculate pseudoinverse
	CDoubleMatrix B(N_, 3);
	LinearAlgebra::CalculatePseudoInverseSVD(&A, &B);
	
	CDoubleVector x_(N_);
	CDoubleVector y_(N_);
	
	// Step 1: initialize epsilons
	float *e = new float[N_];
	for (l = 0; l < N_; l++)
		e[l] = 0.0f;
		
	Vec3d i, j, k;
	float s;
	
	// loop
	for (int n = 0; n < nIterations; n++)
	{		
		// Step 2
		for (l = 0; l < N_; l++)
		{
			x_.data[l] = (m[l + 1].x - cx) * (1.0f + e[l]) - (m[0].x - cx);
			y_.data[l] = (m[l + 1].y - cy) * (1.0f + e[l]) - (m[0].y - cy);
		}
		
		CDoubleVector I(3), J(3);
		LinearAlgebra::MulMatVec(&B, &x_, &I);
		LinearAlgebra::MulMatVec(&B, &y_, &J);
		
		Math3d::SetVec(i, float(I.data[0]), float(I.data[1]), float(I.data[2]));
		Math3d::SetVec(j, float(J.data[0]), float(J.data[1]), float(J.data[2]));
		
		const float s1 = Math3d::Length(i);
		const float s2 = Math3d::Length(j);
		s = 0.5f * (s1 + s2);
		
		Math3d::NormalizeVec(i);
		Math3d::NormalizeVec(j);
		
		// Step 3
		Math3d::CrossProduct(i, j, k);
		Math3d::NormalizeVec(k);
		
		const float Z0 = f / s;
		
		for (l = 0; l < N_; l++)
		{
			Vec3d M0Mi = { float(A(0, l)), float(A(1, l)), float(A(2, l)) };
			e[l] = Math3d::ScalarProduct(M0Mi, k) / Z0;
		}
	}
	
	delete [] e;
	
	// Step 4 (checking differences between epsilons as termination condition)
	// is skipped and a fixed number of iterations is used
	
	// Step 5
	Math3d::SetVec(t, m[0].x - cx, m[0].y - cy, f);
	Math3d::MulVecScalar(t, 1.0f / s, t);
		
	Vec3d j_;
	Math3d::CrossProduct(k, i, j_);
	Math3d::NormalizeVec(j_);
	
	Math3d::SetMat(R, i.x, i.y, i.z, j_.x, j_.y, j_.z, k.x, k.y, k.z);

	// handle cases in which M[0] is not the zero vector
	Vec3d temp;
	Math3d::MulMatVec(R, M[0], temp);
	Math3d::SubtractFromVec(t, temp);
	
	// take into account extrinsic calibration
	const Mat3d &Rc = pCalibration->m_rotation_inverse;
	const Vec3d &tc = pCalibration->m_translation_inverse;
	
	Math3d::MulMatMat(Rc, R, R);
	Math3d::MulMatVec(Rc, t, tc, t);
	
	return true;
}
