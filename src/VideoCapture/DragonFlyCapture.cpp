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
// Filename:  DragonFlyCapture.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************
// Changes:   13.06.2008, Miguel Bernal Marin
//            * Added methods GetSerialFromCamera and SwapCameras
//            * Added constructor detecting the number of cameras
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "DragonFlyCapture.h"

#include "Image/ByteImage.h"
#include "Image/ImageProcessor.h"



// ****************************************************************************
// Constructors / Destructor
// ****************************************************************************

CDragonFlyCapture::CDragonFlyCapture(int nCameras, VideoMode mode, ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType) :
	m_nCameras(nCameras), m_mode(mode), m_colorMode(colorMode), m_bayerPatternType(bayerPatternType)
{
	width = -1;
	height = -1;

	for (int i = 0; i < DRAGONFLY_MAX_CAMERAS; i++)
	{
		m_ppImages[i] = 0;
		m_ppOriginalImages[i] = 0;
		m_ppInternalImages[i] = 0;

		m_flyCaptureContext[i] = 0;
	}
}

// Constructor that detects the numer of cameras
CDragonFlyCapture::CDragonFlyCapture(VideoMode mode, ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType)
	: m_mode(mode), m_colorMode(colorMode), m_bayerPatternType(bayerPatternType)
{
	width = -1;
	height = -1;

	unsigned int nCameras = DRAGONFLY_MAX_CAMERAS;

	FlyCaptureError ret = ::flycaptureBusCameraCount(&nCameras);
	if (ret != FLYCAPTURE_OK)
		m_nCameras = 0;
		
	m_nCameras = (int) nCameras;

	for (int i = 0; i < DRAGONFLY_MAX_CAMERAS; i++)
	{
		m_ppImages[i] = 0;
		m_ppOriginalImages[i] = 0;
		m_ppInternalImages[i] = 0;
		m_flyCaptureContext[i] = 0;
	}
}

CDragonFlyCapture::~CDragonFlyCapture()
{
	CloseCamera();
}


// ****************************************************************************
// Methods
// ****************************************************************************

CByteImage::ImageType CDragonFlyCapture::GetType()
{
	return m_colorMode == eGrayScale ? CByteImage::eGrayScale : CByteImage::eRGB24;
}

bool CDragonFlyCapture::OpenCamera()
{
	if (m_nCameras <= 0)
		return false;

	CloseCamera();

	switch (m_mode)
	{
		case e320x240:
			width = 320;
			height = 240;
		break;

		case e640x480:
			width = 640;
			height = 480;
		break;

		default:
			return false;
	}


	FlyCaptureError ret;

	/*if (m_nCameras == 1)
	{
		if (m_ulSerialNumber == -1 && !ChooseCamera())
			return false;

		ret = ::flycaptureInitializeFromSerialNumber(m_flyCaptureContext[0], m_ulSerialNumber);
		if (ret != FLYCAPTURE_OK)
			return false;
	}*/

	// create a context for and initialize every camera on the bus.
	for (int i = 0; i < m_nCameras; i++)
	{
		ret = ::flycaptureCreateContext(&m_flyCaptureContext[i]);
		if (ret != FLYCAPTURE_OK)
			return false;

		ret = ::flycaptureInitialize(m_flyCaptureContext[i], i);
		if (ret != FLYCAPTURE_OK)
			return false;

		//ret = ::flycaptureStart(m_flyCaptureContext[i], FLYCAPTURE_VIDEOMODE_640x480Y8, FLYCAPTURE_FRAMERATE_30);
		if (m_colorMode == eGrayScale)
			ret = ::flycaptureStartCustomImage(m_flyCaptureContext[i], 0, 4, 4, 640, 480, 100.0, FLYCAPTURE_MONO8);
		else
			ret = ::flycaptureStartCustomImage(m_flyCaptureContext[i], 0, 4, 4, 640, 480, 100.0, FLYCAPTURE_RAW8);

		if (ret != FLYCAPTURE_OK)
			return false;

		m_ppInternalImages[i] = new FlyCaptureImage();

		if (m_colorMode == eRGB24)
			m_ppImages[i] = new CByteImage(width, height, CByteImage::eRGB24);
		else
			m_ppImages[i] = new CByteImage(width, height, CByteImage::eGrayScale);

		if (m_mode == e320x240)
			m_ppOriginalImages[i] = new CByteImage(640, 480, CByteImage::eRGB24);
	}

	return true;
}

void CDragonFlyCapture::CloseCamera()
{
	for (int i = 0; i < m_nCameras; i++)
	{
		if (m_flyCaptureContext[i])
		{
			// stop all cameras from grabbing and destroy their contexts.
			::flycaptureStop(m_flyCaptureContext[i]);
			::flycaptureDestroyContext(m_flyCaptureContext[i]);
			m_flyCaptureContext[i] = 0;
		}

		if (m_ppImages[i])
		{
			delete m_ppImages[i];
			m_ppImages[i] = 0;
		}

		if (m_ppInternalImages[i])
		{
			delete m_ppInternalImages[i];
			m_ppInternalImages[i] = 0;
		}

		if (m_ppOriginalImages[i])
		{
			delete m_ppOriginalImages[i];
			m_ppOriginalImages[i] = 0;
		}
	}
}

bool CDragonFlyCapture::CaptureImage(CByteImage **ppImages)
{
	int i;

	FlyCaptureError ret;

	if (!m_flyCaptureContext)
		return false;

	// first grab
	for (i = 0; i < m_nCameras; i++)
	{
		ret = ::flycaptureGrabImage2(m_flyCaptureContext[i], m_ppInternalImages[i]);
		if (ret != FLYCAPTURE_OK)
			return false;
	}

	CByteImage image_header(640, 480, CByteImage::eGrayScale, true);

	// then get images
	for (i = 0; i < m_nCameras; i++)
	{
		image_header.pixels = m_ppInternalImages[i]->pData;

		if (m_mode == e320x240)
		{
			//ret = ::flycaptureStippledToBGR24(m_flyCaptureContext[i], m_ppInternalImages[i], m_ppOriginalImages[i]->pixels);
			//if (ret != FLYCAPTURE_OK) return 0;
			
			if (m_colorMode == eGrayScale)
			{
				ImageProcessor::Resize(&image_header, ppImages[i]);
			}
			else
			{
				ImageProcessor::ConvertBayerPattern(&image_header, m_ppOriginalImages[i], m_bayerPatternType);
				ImageProcessor::Resize(m_ppOriginalImages[i], ppImages[i]);
			}
		}
		else if (m_mode == e640x480)
		{
			//ret = ::flycaptureStippledToBGR24(m_flyCaptureContext[i], m_ppInternalImages[i], ppImages[i]->pixels);
			//if (ret != FLYCAPTURE_OK) return false;

			if (m_colorMode == eGrayScale)
			{
				ImageProcessor::CopyImage(&image_header, ppImages[i]);
			}
			else
			{
				ImageProcessor::ConvertBayerPattern(&image_header, ppImages[i], m_bayerPatternType);
			}
		}
	}

	m_sec = m_ppInternalImages[0]->timeStamp.ulSeconds;
	m_usec = m_ppInternalImages[0]->timeStamp.ulMicroSeconds;

	return true;
}


void CDragonFlyCapture::GetCurrentTimestamp(unsigned int &sec, unsigned int &usec)
{
	sec = m_sec;
	usec = m_usec;
}

unsigned int CDragonFlyCapture::GetSerialFromCamera(int nCamera)
{
	if (m_flyCaptureContext[nCamera])
	{
		if (::flycaptureGetCameraInfo(m_flyCaptureContext[nCamera], &m_flyCaptureInfoEx) == FLYCAPTURE_OK)
			return m_flyCaptureInfoEx.SerialNumber;
	}
  
	return 0;
}

void CDragonFlyCapture::SwapCameras(int nCamera1, int nCamera2)
{
	if (m_flyCaptureContext[nCamera1] && m_flyCaptureContext[nCamera2])
	{
		FlyCaptureContext fCC_tmp = m_flyCaptureContext[nCamera1];
		m_flyCaptureContext[nCamera1] = m_flyCaptureContext[nCamera2];
		m_flyCaptureContext[nCamera2] = fCC_tmp;
	}
}


/*bool CDragonFlyCapture::ChooseCamera()
{
	CameraGUIError guiError;
	CameraGUIContext m_guicontext;
	int nDialogStatus;


	guiError = ::pgrcamguiCreateContext(&m_guicontext);
	if (guiError != PGRCAMGUI_OK)
		return false;

	guiError = ::pgrcamguiShowCameraSelectionModal(m_guicontext, m_flyCaptureContext, &m_ulSerialNumber, &nDialogStatus);
	if(guiError != PGRCAMGUI_OK)
		return false;

	WriteCameraID();

	return true;
}*/
