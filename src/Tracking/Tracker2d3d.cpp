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
// Filename:  Tracker2d3d.cpp
// Author:    Pedram Azad
// Date:      25.01.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "Tracker2d3d.h"

#include "Math/FloatMatrix.h"
#include "Math/FloatVector.h"
#include "Math/LinearAlgebra.h"
#include "Math/Math2d.h"
#include "Math/Math3d.h"
#include "Calibration/Calibration.h"
#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"
#include "Tracking/ObjectPose.h"

#include <stdio.h>


// ****************************************************************************
// This is a simple (and only partly) implementation of the algorithm
// described in:
// E. Marchand, P. Bouthemy, F. Chaumette, and V. Moreau,
// "Robust Real-Time Visual Tracking using a 2D-3D Model-based Approach",
// in Proc. of International Conference on Computer Vision (ICCV),
// pages 262-268, 1999.
// ****************************************************************************



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CTracker2d3d::CTracker2d3d()
{
	m_nPixelsSearchDistance = 15;
	m_nPixelsDelta = 15;
}

CTracker2d3d::~CTracker2d3d()
{
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CTracker2d3d::Init(const CCalibration *pCalibration)
{
	m_pCalibration = pCalibration;
}

bool CTracker2d3d::Track(const CByteImage *pEdgeImage, Vec3d *pOutlinePoints, int nOutlinePoints,
		   Mat3d &rotation, Vec3d &translation)
{
	if (pEdgeImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image must be grayscale image for CTracker2d3d::Track\n");
		return false;
	}

	if (m_pCalibration->GetCameraParameters().width != pEdgeImage->width ||
		m_pCalibration->GetCameraParameters().height != pEdgeImage->height)
	{
		printf("error: calibration does not match image in CTracker2d3d::Track\n");
		return false;
	}

	const int nMaxPoints = 10000;

	Vec3d *pPointList3d = new Vec3d[nMaxPoints];
	Vec2d *pPointList2dProjection = new Vec2d[nMaxPoints];
	Vec2d *pPointList2d = new Vec2d[nMaxPoints];

	const unsigned char *pixels = pEdgeImage->pixels;
	const int width = pEdgeImage->width;
	const int height = pEdgeImage->height;

	Mat3d A;
	Math3d::SetMat(A, Math3d::unit_mat);

	int nPoints;

	for (int nRun = 0;; nRun++)
	{
		int i;
		nPoints = 0;

		for (i = 0; i < nOutlinePoints; i += 2)
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

				Math3d::SetVec(temp, p1_.x, p1_.y, 1);
				Math3d::MulMatVec(A, temp, temp);
				p1_.x = temp.x / temp.z;
				p1_.y = temp.y / temp.z;

				Math3d::SetVec(temp, p2_.x, p2_.y, 1);
				Math3d::MulMatVec(A, temp, temp);
				p2_.x = temp.x / temp.z;
				p2_.y = temp.y / temp.z;
			}

			const float length_ = Math2d::Distance(p1_, p2_);

			const int nControlPoints = int(length_ / m_nPixelsDelta) - 1;
			if (nControlPoints < 1 || nControlPoints > 10000)
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
					Vec2d temp2;

					Math3d::MulMatVec(rotation, p, translation, temp);
					m_pCalibration->WorldToImageCoordinates(temp, temp2, false);

					Vec3d temp3 = { temp2.x, temp2.y, 1 };

					Math3d::MulMatVec(A, temp3, temp3);		

					p_.x = temp3.x / temp3.z;
					p_.y = temp3.y / temp3.z;
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
					Math3d::SetVec(pPointList3d[nPoints], p);
					Math2d::SetVec(pPointList2dProjection[nPoints], p_);

					if (bPlus)
					{
						Math2d::SetVec(pPointList2d[nPoints], pn1);
					}	
					else
					{
						Math2d::SetVec(pPointList2d[nPoints], pn2);
					}

					if (++nPoints == nMaxPoints)
						break;
				}
			}
		}

		if (nPoints < 6)
			return false;

		if (nRun == 4)
		{
			// end after five runs
			break;
		}

		Mat3d A_;
		LinearAlgebra::DetermineAffineTransformation(pPointList2dProjection, pPointList2d, nPoints, A_);
		Math3d::MulMatMat(A_, A, A);
	}

	CObjectPose objectPose(pPointList3d, nPoints);
	objectPose.EstimatePose(pPointList2d, rotation, translation, m_pCalibration);
	
	delete [] pPointList3d;
	delete [] pPointList2d;
	delete [] pPointList2dProjection;
	
	return true;
}
