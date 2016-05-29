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
// Filename:  Undistortion.cpp
// Author:    Pedram Azad
// Date:      04.10.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "Undistortion.h"

#include "Image/ByteImage.h"
#include "Calibration/Calibration.h"
#include "Calibration/StereoCalibration.h"

#include <stdio.h>



// ****************************************************************************
// class CUndistortion::CUndistortionMapper
// ****************************************************************************

void CUndistortion::CUndistortionMapper::Init(const CCalibration *pCalibration)
{
	m_pCalibration = pCalibration;
	ComputeMap(m_pCalibration->GetCameraParameters().width, m_pCalibration->GetCameraParameters().height);
}

void CUndistortion::CUndistortionMapper::ComputeOriginalCoordinates(const Vec2d &newCoordinates, Vec2d &originalCoordinates)
{
	m_pCalibration->DistortImageCoordinates(newCoordinates, originalCoordinates);
}



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CUndistortion::CUndistortion(bool bInterpolate)
{
	m_pStereoCalibration = new CStereoCalibration();
	m_pCalibration = new CCalibration();

	m_pUndistortionMapperLeft = new CUndistortionMapper(bInterpolate);
	m_pUndistortionMapperRight = new CUndistortionMapper(bInterpolate);
}

CUndistortion::~CUndistortion()
{
	delete m_pStereoCalibration;
	delete m_pCalibration;

	delete m_pUndistortionMapperLeft;
	delete m_pUndistortionMapperRight;
}


// ****************************************************************************
// Methods
// ****************************************************************************

int CUndistortion::Init(const char *pCameraParameterFileName)
{
	if (m_pStereoCalibration->LoadCameraParameters(pCameraParameterFileName))
	{
		// stereo calibration
		m_pCalibrationLeft = m_pStereoCalibration->GetLeftCalibration();
		m_pCalibrationRight = m_pStereoCalibration->GetRightCalibration();

		UpdateMaps();

		return 2;
	}
	else
	{
		// not a stereo calibration
		if (m_pCalibration->LoadCameraParameters(pCameraParameterFileName))
		{
			m_pCalibrationLeft = m_pCalibration;
			m_pCalibrationRight = 0;

			UpdateMaps();
			
			return 1;
		}
	}

	return 0;
}

void CUndistortion::Init(const CCalibration *pCalibration)
{
	m_pCalibrationLeft = pCalibration;
	m_pCalibrationRight = 0;

	UpdateMaps();
}

void CUndistortion::Init(const CStereoCalibration *pStereoCalibration)
{
	m_pCalibrationLeft = pStereoCalibration->GetLeftCalibration();
	m_pCalibrationRight = pStereoCalibration->GetRightCalibration();

	UpdateMaps();
}

void CUndistortion::UpdateMaps()
{
	if (!m_pCalibrationLeft && !m_pCalibrationRight)
	{
		printf("error: CUndistortion object has not been initialized for CUndistortion::UpdateMaps\n");
		return;
	}
	
	if (m_pCalibrationLeft)
		m_pUndistortionMapperLeft->Init(m_pCalibrationLeft);
	
	if (m_pCalibrationRight)
		m_pUndistortionMapperRight->Init(m_pCalibrationRight);
}


void CUndistortion::Undistort(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	m_pUndistortionMapperLeft->PerformMapping(pInputImage, pOutputImage);
}


void CUndistortion::Undistort(const CByteImage * const *ppInputImages, CByteImage **ppOutputImages)
{
	m_pUndistortionMapperLeft->PerformMapping(ppInputImages[0], ppOutputImages[0]);
	m_pUndistortionMapperRight->PerformMapping(ppInputImages[1], ppOutputImages[1]);
}
