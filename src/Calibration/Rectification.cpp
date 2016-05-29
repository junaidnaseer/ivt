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
// Filename:  Rectification.cpp
// Author:    Pedram Azad
// Date:      04.10.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "Rectification.h"

#include "Image/ByteImage.h"
#include "Calibration/Calibration.h"
#include "Calibration/StereoCalibration.h"

#include <stdio.h>



// ****************************************************************************
// class CRectification::CRectificationMapper
// ****************************************************************************

void CRectification::CRectificationMapper::Init(const Mat3d &homography, const CCalibration *pCalibration)
{
	m_pCalibration = pCalibration;
	Math3d::SetMat(m_homography, homography);

	ComputeMap(m_pCalibration->GetCameraParameters().width, m_pCalibration->GetCameraParameters().height);
}

void CRectification::CRectificationMapper::ComputeOriginalCoordinates(const Vec2d &newCoordinates, Vec2d &originalCoordinates)
{
	Math2d::ApplyHomography(m_homography, newCoordinates, originalCoordinates);

	if (m_bUndistort)
	{
		// this is not a bug to distort; mapping needs inverse function!
		m_pCalibration->DistortImageCoordinates(originalCoordinates, originalCoordinates);
	}
}



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CRectification::CRectification(bool bInterpolate, bool bUndistort)
{
	m_pStereoCalibration = new CStereoCalibration();
	m_pUsedStereoCalibration = 0;

	m_pRectificationMapperLeft = new CRectificationMapper(bInterpolate, bUndistort);
	m_pRectificationMapperRight = new CRectificationMapper(bInterpolate, bUndistort);
}

CRectification::~CRectification()
{
	delete m_pStereoCalibration;

	delete m_pRectificationMapperLeft;
	delete m_pRectificationMapperRight;
}


// ****************************************************************************
// methods
// ****************************************************************************

bool CRectification::Init(const char *pCameraParameterFileName)
{
	if (m_pStereoCalibration->LoadCameraParameters(pCameraParameterFileName))
	{
		m_pUsedStereoCalibration = m_pStereoCalibration;

		UpdateMaps();

		return true;
	}
	
	return false;
}

void CRectification::Init(const CStereoCalibration *pStereoCalibration)
{
	m_pUsedStereoCalibration = pStereoCalibration;

	UpdateMaps();
}


void CRectification::UpdateMaps()
{
	if (!m_pUsedStereoCalibration)
	{
		printf("error: CRectification object has not been initialized for CRectification::UpdateMaps\n");
		return;
	}
	
	m_pRectificationMapperLeft->Init(m_pUsedStereoCalibration->rectificationHomographyLeft, m_pUsedStereoCalibration->GetLeftCalibration());
	m_pRectificationMapperRight->Init(m_pUsedStereoCalibration->rectificationHomographyRight, m_pUsedStereoCalibration->GetRightCalibration());
}

void CRectification::Rectify(const CByteImage * const *ppInputImages, CByteImage **ppOutputImages)
{
	m_pRectificationMapperLeft->PerformMapping(ppInputImages[0], ppOutputImages[0]);
	m_pRectificationMapperRight->PerformMapping(ppInputImages[1], ppOutputImages[1]);
}
