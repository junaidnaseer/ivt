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
// Filename:  RAPiD.cpp
// Author:    Pedram Azad
// Date:      25.01.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "RAPiD.h"

#include "Math/FloatMatrix.h"
#include "Math/FloatVector.h"
#include "Math/LinearAlgebra.h"
#include "Calibration/Calibration.h"
#include "DataStructures/DynamicArray.h"
#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"
#include "Tracking/ICP.h"

#include <stdio.h>


// ****************************************************************************
// This is an implementation of the algorithm described in:
// C. Harris, "Tracking with rigid models", in "Active Vision",
// edited by A. Blake, pages 59-73, MIT Press, 1992.
// The Kalman filtering step is not implemented.
// ****************************************************************************



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CRAPiD::CRAPiD()
{
	m_nPixelsSearchDistance = 15;
	m_nPixelsDelta = 15;
}

CRAPiD::~CRAPiD()
{
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CRAPiD::Init(const CCalibration *pCalibration)
{
	m_pCalibration = pCalibration;
}

bool CRAPiD::Track(const CByteImage *pEdgeImage, Vec3d *pOutlinePoints, int nOutlinePoints,
		   Mat3d &rotation, Vec3d &translation)
{
	if (pEdgeImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image must be grayscale image for CRAPiD::Track\n");
		return false;
	}

	if (m_pCalibration->GetCameraParameters().width != pEdgeImage->width ||
		m_pCalibration->GetCameraParameters().height != pEdgeImage->height)
	{
		printf("error: calibration does not match image in CRAPiD::Track\n");
		return false;
	}

	CDynamicArray rapidList(1000);

	const unsigned char *pixels = pEdgeImage->pixels;
	const int width = pEdgeImage->width;
	const int height = pEdgeImage->height;

	for (int i = 0; i < nOutlinePoints; i += 2)
	{
		Vec3d p1, p2;
		Math3d::SetVec(p1, pOutlinePoints[i]);
		Math3d::SetVec(p2, pOutlinePoints[i + 1]);

		Vec2d p1_, p2_;
		{
			Vec3d temp;
			
			Math3d::MulMatVec(rotation, p1, translation, temp);
			m_pCalibration->WorldToImageCoordinates(temp, p1_, false);

			Math3d::MulMatVec(rotation, p2, translation, temp);
			m_pCalibration->WorldToImageCoordinates(temp, p2_, false);
		}

		const float length_ = Math2d::Distance(p1_, p2_);

		const int nControlPoints = int(length_ / m_nPixelsDelta) - 1;
		if (nControlPoints < 1)
			continue;

		Vec3d u;
		Math3d::SubtractVecVec(p2, p1, u);
		const float delta_length = Math3d::Length(u) / nControlPoints;
		Math3d::NormalizeVec(u);
		Math3d::MulVecScalar(u, delta_length, u);

		Vec2d u_ = { p1_.x - p2_.x, p1_.y - p2_.y };
		Math2d::NormalizeVec(u_);

		// calculate normal vector
		Vec2d n = { p1_.y - p2_.y, p2_.x - p1_.x };
		Math2d::NormalizeVec(n);
		
		Vec3d p;
		Math3d::SetVec(p, p1);

		for (int j = 0; j < nControlPoints; j++)
		{
			Math3d::AddToVec(p, u);

			Vec2d p_;
			{
				Vec3d temp;
				Math3d::MulMatVec(rotation, p, translation, temp);
				m_pCalibration->WorldToImageCoordinates(temp, p_, false);
			}

			// search in perpendicular direction
			Vec2d pn1, pn2;
			int k, l = -1;
			bool bPlus = true;

			Math2d::SetVec(pn1, p_);
			Math2d::SetVec(pn2, p_);
			for (k = 0; k < m_nPixelsSearchDistance; k++)
			{
				int x, y;

				// can be optimized: checking boundaries within loop is slow but convenient
				x = int(pn1.x + 0.5f);
				y = int(pn1.y + 0.5f);
				if (x > 0 && x < width && y > 0 && y < height && pixels[y * width + x])
				{
					l = k;
					bPlus = true;
					break;
				}

				x = int(pn2.x + 0.5f);
				y = int(pn2.y + 0.5f);
				if (x > 0 && x < width && y > 0 && y < height && pixels[y * width + x])
				{
					l = k;
					bPlus = false;
					break;
				}

				Math2d::AddToVec(pn1, n);
				Math2d::SubtractFromVec(pn2, n);
			}

			if (l != -1)
			{
				CRAPiDElement *pRAPiDElement = new CRAPiDElement();

				pRAPiDElement->l = float(l);
				Math2d::SetVec(pRAPiDElement->n, n);
				Math2d::NormalizeVec(pRAPiDElement->n);
				Math3d::SetVec(pRAPiDElement->p, p);

				if (!bPlus)
					Math2d::MulVecScalar(pRAPiDElement->n, -1, pRAPiDElement->n);

				rapidList.AddElement(pRAPiDElement);
			}
		}
	}

	return RAPiD(rapidList, m_pCalibration, rotation, translation);
}

bool CRAPiD::RAPiD(CDynamicArray &elementList, const CCalibration *pCalibration, Mat3d &rotation, Vec3d &translation)
{
	if (elementList.GetSize() < 6)
	{
		printf("error: not enough input points for RAPiD::RAPiD\n");
		return false;
	}

	const CCalibration::CCameraParameters &cameraParameters = pCalibration->GetCameraParameters();
	const float fx = cameraParameters.focalLength.x;
	const float fy = cameraParameters.focalLength.y;

	Vec3d t;
	Math3d::MulMatVec(cameraParameters.rotation, translation, cameraParameters.translation, t);

	Mat3d R;
	Math3d::MulMatMat(cameraParameters.rotation, rotation, R);

	const int nPoints = elementList.GetSize();

	int i;

	CFloatMatrix A(6, 6);
	ImageProcessor::Zero(&A);

	CFloatVector b(6);
	b.data[0] = b.data[1] = b.data[2] = b.data[3] = b.data[4] = b.data[5] = 0;

	CFloatMatrix C(1, 6), CT(6, 1);
	CFloatMatrix CTC(6, 6);

	for (i = 0; i < nPoints; i++)
	{
		CRAPiDElement *pElement = (CRAPiDElement *) elementList.GetElement(i);

		Vec3d p;
		Math3d::MulMatVec(R, pElement->p, p);

		const float x = p.x;
		const float y = p.y;
		const float z = p.z;

		const float xc = x + t.x;
		const float yc = y + t.y;
		const float zc = z + t.z;

		const float u0 = fx * xc / zc;
		const float v0 = fy * yc / zc;

		const float nx = pElement->n.x;
		const float ny = pElement->n.y;

		const float c1 = (-nx * u0 * y - ny * (fy * z + v0 * y)) / zc;
		const float c2 = (nx * (fx * z + u0 * x) + ny * v0 * x) / zc;
		const float c3 = (-nx * fx * y + ny * fy * x) / zc;
		const float c4 = (nx * fx) / zc;
		const float c5 = (ny * fy) / zc;
		const float c6 = (-nx * u0 - ny * v0) / zc;

		const float l = pElement->l;

		b.data[0] += l * c1;
		b.data[1] += l * c2;
		b.data[2] += l * c3;
		b.data[3] += l * c4;
		b.data[4] += l * c5;
		b.data[5] += l * c6;

		C(0, 0) = c1;
		C(0, 1) = c2;
		C(0, 2) = c3;
		C(0, 3) = c4;
		C(0, 4) = c5;
		C(0, 5) = c6;

		LinearAlgebra::Transpose(&C, &CT);
		LinearAlgebra::MulMatMat(&C, &CT, &CTC);
		LinearAlgebra::AddToMat(&A, &CTC);
	}

	CFloatVector q(6);
	LinearAlgebra::SolveLinearLeastSquaresSVD(&A, &b, &q);

	Vec3d delta_theta = { q.data[0], q.data[1], q.data[2] };
	Vec3d delta_t = { q.data[3], q.data[4], q.data[5] };

	// calculate new transformation consisting of rotation matrix R and translation vector t
	Vec3d *pPoints = new Vec3d[nPoints];
	Vec3d *pNewPoints = new Vec3d[nPoints];

	for (i = 0; i < nPoints; i++)
	{
		CRAPiDElement *pElement = (CRAPiDElement *) elementList.GetElement(i);

		Math3d::SetVec(pPoints[i], pElement->p);

		Vec3d p;
		Math3d::MulMatVec(R, pElement->p, p);

		Vec3d temp;
		Math3d::CrossProduct(delta_theta, p, temp);
		Math3d::AddVecVec(p, temp, pNewPoints[i]);
		Math3d::AddToVec(pNewPoints[i], delta_t);
		Math3d::AddToVec(pNewPoints[i], t);

		// transform back from camera coordinate system into world coordinate system
		Math3d::MulMatVec(pCalibration->m_rotation_inverse, pNewPoints[i], pCalibration->m_translation_inverse, pNewPoints[i]);
	}

	// apply algorithm by Horn for calculating the optimal transformation
	CICP::CalculateOptimalTransformation(pPoints, pNewPoints, nPoints, rotation, translation);

	delete [] pPoints;
	delete [] pNewPoints;

	return true;
}
