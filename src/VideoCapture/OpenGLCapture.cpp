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
// Filename:  OpenGLCapture.cpp
// Author:    Pedram Azad
// Date:      27.07.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "OpenGLCapture.h"
#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"
#include "Calibration/Calibration.h"
#include "Calibration/StereoCalibration.h"
#include "Visualizer/OpenGLVisualizer.h"



// *****************************************************************
// Constructors / Destructor
// *****************************************************************

COpenGLCapture::COpenGLCapture()
{
	m_nCameras = 0;

	width = height = 0;
	type = CByteImage::eRGB24;

	m_pOpenGLVisualizer = 0;
	m_pCalibration = 0;
	m_pStereoCalibration = 0;
}

COpenGLCapture::COpenGLCapture(const CCalibration &calibration)
{
	m_nCameras = 1;

	width = calibration.GetCameraParameters().width;
	height = calibration.GetCameraParameters().height;
	type = CByteImage::eRGB24;
	
	m_pOpenGLVisualizer = new COpenGLVisualizer();
	m_pCalibration = new CCalibration();
	m_pStereoCalibration = 0;
	
	m_pCalibration->Set(calibration);
}

COpenGLCapture::COpenGLCapture(const CStereoCalibration &stereoCalibration)
{
	m_nCameras = 2;

	width = stereoCalibration.width;
	height = stereoCalibration.height;
	type = CByteImage::eRGB24;
	
	m_pOpenGLVisualizer = new COpenGLVisualizer();
	m_pStereoCalibration = new CStereoCalibration();
	m_pCalibration = 0;

	m_pStereoCalibration->Set(stereoCalibration);
}

COpenGLCapture::~COpenGLCapture()
{
	if (m_pCalibration)
		delete m_pCalibration;

	if (m_pStereoCalibration)
		delete m_pStereoCalibration;

	if (m_pOpenGLVisualizer)
		delete m_pOpenGLVisualizer;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void COpenGLCapture::Set(const CCalibration &calibration)
{
	m_nCameras = 1;

	width = calibration.GetCameraParameters().width;
	height = calibration.GetCameraParameters().height;
	type = CByteImage::eRGB24;

	if (m_pStereoCalibration)
	{
		delete m_pStereoCalibration;
		m_pStereoCalibration = 0;
	}

	if (!m_pCalibration)
		m_pCalibration = new CCalibration();
	
	if (!m_pOpenGLVisualizer)
		m_pOpenGLVisualizer = new COpenGLVisualizer();

	m_pCalibration->Set(calibration);
}


void COpenGLCapture::Set(const CStereoCalibration &stereoCalibration)
{
	m_nCameras = 2;

	width = stereoCalibration.width;
	height = stereoCalibration.height;
	type = CByteImage::eRGB24;

	if (m_pCalibration)
	{
		delete m_pCalibration;
		m_pCalibration = 0;
	}

	if (!m_pStereoCalibration)
		m_pStereoCalibration = new CStereoCalibration();
	
	if (!m_pOpenGLVisualizer)
		m_pOpenGLVisualizer = new COpenGLVisualizer();

	m_pStereoCalibration->Set(stereoCalibration);
}

bool COpenGLCapture::OpenCamera()
{
	if (m_pStereoCalibration)
		return m_pOpenGLVisualizer->InitByCalibration(m_pStereoCalibration->GetLeftCalibration());
	else if (m_pCalibration)
		return m_pOpenGLVisualizer->InitByCalibration(m_pCalibration);
	else
		printf("error: no calibration object has been instantiated\n");

	return false;
}

void COpenGLCapture::CloseCamera()
{
}

bool COpenGLCapture::CaptureImage(CByteImage **ppImages)
{
	if (ppImages[0]->width != width || ppImages[0]->height != height || ppImages[0]->type != type)
		printf("error: image does not match COpenGLCapture::CaptureImage\n");
	
	if (m_nCameras == 1)
	{
		m_pOpenGLVisualizer->InitByCalibration(m_pCalibration);
		DrawScene(); // virtual method call
		m_pOpenGLVisualizer->GetImage(ppImages[0]);
		ImageProcessor::FlipY(ppImages[0], ppImages[0]);
	}
	else if (m_nCameras == 2)
	{
		if (ppImages[1]->width != width || ppImages[1]->height != height || ppImages[1]->type != type)
			printf("error: image does not match COpenGLCapture::CaptureImage\n");
	
		// left camera
		m_pOpenGLVisualizer->InitByCalibration(m_pStereoCalibration->GetLeftCalibration());
		DrawScene(); // virtual method call
		m_pOpenGLVisualizer->GetImage(ppImages[0]);
		ImageProcessor::FlipY(ppImages[0], ppImages[0]);
		
		// right camera
		m_pOpenGLVisualizer->InitByCalibration(m_pStereoCalibration->GetRightCalibration());
		DrawScene(); // virtual method call
		m_pOpenGLVisualizer->GetImage(ppImages[1]);
		ImageProcessor::FlipY(ppImages[1], ppImages[1]);
	}
	
	return true;
}
