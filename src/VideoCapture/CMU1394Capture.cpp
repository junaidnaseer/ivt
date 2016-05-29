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
// Filename:  CMU1394Capture.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************
// Changes:   01.02.2006, Florian Hecht
//            * Added support for multiple cameras
//            14.06.2010, Kenn Sebesta
//            * Added methods SetGain, SetShutter, and SetFeature
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "Interfaces/VideoCaptureInterface.h"
#include "Image/ByteImage.h"
#include "CMU1394Capture.h"

#include <windows.h>
#include <1394Camera.h>

#include <stdio.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CCMU1394Capture::CCMU1394Capture(int nFormat, int nMode, int nRate, int nCameras)
{
	m_nFormat = nFormat;
	m_nMode = nMode;
	m_nRate = nRate;

	width = height = -1;

	m_nNumberOfCameras = nCameras;
	m_ppCameras = NULL;
		
	m_bOK = false;
}

CCMU1394Capture::~CCMU1394Capture()
{
	CloseCamera();
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CCMU1394Capture::OpenCamera()
{
	unsigned long w,h;
	int i, number_of_cameras;
	C1394Camera *cam;

	CloseCamera();


	// create a first camera to see how many cameras we have
	cam = new C1394Camera();

	if (!cam->CheckLink())
	{
		number_of_cameras = cam->GetNumberCameras();

		if (m_nNumberOfCameras == -1 || number_of_cameras < m_nNumberOfCameras)
			m_nNumberOfCameras = number_of_cameras;
	}
	else
	{
		m_nNumberOfCameras = 0;
		delete cam;

		return false;
	}

	// create the rest of the cameras
	if (m_nNumberOfCameras != 0)
	{
		int i;

		m_ppCameras = new C1394Camera*[m_nNumberOfCameras];

		m_ppCameras[0] = cam;
		for (i = 1; i < m_nNumberOfCameras; i++)
		{
			m_ppCameras[i] = new C1394Camera();
		}
	}


	// assume everything is going fine
	m_bOK = true;

	for (i = 0; i < m_nNumberOfCameras; i++)
	{
		cam = m_ppCameras[i];

		if (cam->CheckLink())
		{
			//printf("ERROR: camera %d CheckLink failed\n", i);
			m_bOK = false;
			continue;
		}

		// make this the i-th camera
		cam->SelectCamera(i);

#ifndef CMU1394_Version_645
		cam->m_cameraInitialized = false;    // is private in new version
#endif
		cam->InitCamera();
#ifndef CMU1394_Version_645
		cam->InquireControlRegisters();      // method doesn't exist anymore
#endif
		//cam->StatusControlRegisters();
		
		int format = -1, mode = -1, rate = -1;
		bool ok = false;
		
		if (m_nFormat == -1 || m_nMode == -1 || m_nRate == -1)
		{
			for (format = 0; format < 3; format++)
			{
				for (mode = 0; mode < 8; mode++)
				{
					for (rate = 5; rate >= 0; rate--)
					{
#ifdef CMU1394_Version_645
						if (cam->HasVideoFrameRate(format, mode, rate))
#else
						if (cam->m_videoFlags[format][mode][rate])
#endif
						{
							ok = true;
							break;
						}
					}

					if (ok)
						break;
				}

				if (ok)
					break;
			}
		}
		else
		{
#ifdef CMU1394_Version_645
			if (cam->HasVideoFrameRate(m_nFormat, m_nMode, m_nRate))
#else
			if (cam->m_videoFlags[m_nFormat][m_nMode][m_nRate])
#endif
			{
				format = m_nFormat;
				mode = m_nMode;
				rate = m_nRate;
				ok = true;
			}
		}
		
		if (ok)
		{
			cam->SetVideoFormat(format);
			cam->SetVideoMode(mode);
			cam->SetVideoFrameRate(rate);
			
			if (cam->StartImageAcquisition() != CAM_SUCCESS)
			{
				//printf("ERROR: couldn't start acquisition\n", format, mode, rate);
				m_bOK = false;
			}
			
#ifdef CMU1394_Version_645
			cam->GetVideoFrameDimensions(&w, &h);
			width=(int)w;
			height=(int)h;
#else
			width = cam->m_width;      // old version
			height = cam->m_height;    // old version
#endif
		}
		else
		{
			// could not find a valid mode
			m_bOK = false;
		}
	}

	return m_bOK;
}

int CCMU1394Capture::GetNumberOfCameras()
{
	return m_nNumberOfCameras;
}

void CCMU1394Capture::CloseCamera()
{
	int i;

	if (m_ppCameras != NULL)
	{
		for (i = 0; i < m_nNumberOfCameras; i++)
		{
			m_ppCameras[i]->StopImageAcquisition();
			delete m_ppCameras[i];
		}

		delete [] m_ppCameras;
	}

	m_bOK = false;
}

bool CCMU1394Capture::CaptureImage(CByteImage **ppImages)
{
	if (!m_bOK)
		return false;
	
	int i, num_dropped_frames = -1;
	bool all_ok = true;
	C1394Camera *cam;

	for (i = 0; i < m_nNumberOfCameras; i++)
	{
		cam = m_ppCameras[i];

		if (cam->AcquireImageEx(true, &num_dropped_frames) == CAM_SUCCESS)
		{
			cam->getRGB(ppImages[i]->pixels, height*width*3);
		}
		else
		{
			all_ok = false;
		}
	}
	
	return all_ok;
}

#ifndef CMU1394_Version_645
// Sets focus value in [units]. The [units] are not necessarily the same from one manufacturer to another.
void CCMU1394Capture::SetFocus(int nFocus, int nCamera)
{
	if (nCamera == -1)
	{
		for (int i = 0; i < m_nNumberOfCameras; i++)
			m_ppCameras[i]->SetFocus(nFocus);
	}
	else
	{
		if (nCamera < 0 || nCamera >= m_nNumberOfCameras)
			printf("error: parameter nCamera = %i is out of bounds (%i cameras available)\n", nCamera, m_nNumberOfCameras);
		else
			m_ppCameras[nCamera]->SetFocus(nFocus);
	}
}
#else
// Sets gain value in [units]. The [units] are not necessarily the same from one manufacturer to another.
void CCMU1394Capture::SetGain(int nGain, int nCamera)
{
	if (nCamera == -1)
	{
		for (int i = 0; i < m_nNumberOfCameras; i++)
		{
			C1394CameraControl *pFeature = m_ppCameras[i]->GetCameraControl(FEATURE_GAIN);
			
			if (pFeature)
				pFeature->SetValue(nGain);
			else
				printf("error: feature 'gain' not available for camera %i in CCMU1394Capture::SetGain\n", i);
		}
	}
	else
	{
		if (nCamera < 0 || nCamera >= m_nNumberOfCameras)
			printf("error: parameter nCamera = %i is out of bounds (%i cameras available) in CCMU1394Capture::SetGain\n", nCamera, m_nNumberOfCameras);
		else
		{
			C1394CameraControl *pFeature = m_ppCameras[nCamera]->GetCameraControl(FEATURE_GAIN);
			
			if (pFeature)
				pFeature->SetValue(nGain);
			else
				printf("error: feature 'gain' not available for camera %i in CCMU1394Capture::SetGain\n", nCamera);
			
		}
	}
}

// Sets shutter value in [units]. The [units] are not necessarily the same from one manufacturer to another.
void CCMU1394Capture::SetShutter(int nShutter, int nCamera)
{
	if (nCamera == -1)
	{
		for (int i = 0; i < m_nNumberOfCameras; i++)
		{
			C1394CameraControl *pFeature = m_ppCameras[i]->GetCameraControl(FEATURE_GAIN);
			
			if (pFeature)
				pFeature->SetValue(nShutter);
			else
				printf("error: feature 'shutter' not available for camera %i in CCMU1394Capture::SetShutter\n", i);
		}
	}
	else
	{
		if (nCamera < 0 || nCamera >= m_nNumberOfCameras)
			printf("error: parameter nCamera = %i is out of bounds (%i cameras available) in CCMU1394Capture::SetShutter\n", nCamera, m_nNumberOfCameras);
		else
		{
			C1394CameraControl *pFeature = m_ppCameras[nCamera]->GetCameraControl(FEATURE_SHUTTER);
			
			if (pFeature)
				pFeature->SetValue(nShutter);
			else
				printf("error: feature 'shutter' not available for camera %i in CCMU1394Capture::SetShutter\n", nCamera);
			
		}
	}
}

// Sets arbitrary feature value in [units]. The [units] are not necessarily the same from one manufacturer to another.
// This function is dangerous, as there is no error checking. ONLY TO BE USED IF YOU KNOW WHAT YOU ARE DOING.
void CCMU1394Capture::SetFeature(int nFeature, int nValue, int nCamera)
{
	if (nCamera == -1)
	{
		for (int i = 0; i < m_nNumberOfCameras; i++)
		{
			C1394CameraControl *pFeature = m_ppCameras[i]->GetCameraControl((CAMERA_FEATURE) nFeature);
			
			if (pFeature)
				pFeature->SetValue(nValue);
			else
				printf("error: feature %i not available for camera %i in CCMU1394Capture::SetFeature\n", nFeature, i);
		}
	}
	else
	{
		if (nCamera < 0 || nCamera >= m_nNumberOfCameras)
			printf("error: parameter nCamera = %i is out of bounds (%i cameras available) in CCMU1394Capture::SetFeature\n", nCamera, m_nNumberOfCameras);
		else
		{
			C1394CameraControl *pFeature = m_ppCameras[nCamera]->GetCameraControl((CAMERA_FEATURE) nFeature);
			
			if (pFeature)
				pFeature->SetValue(nValue);
			else
				printf("error: feature %i not available for camera %i in CCMU1394Capture::SetFeature\n", nFeature, nCamera);
			
		}
	}
}
#endif
