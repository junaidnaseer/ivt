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
// Filename:  Linux1394Capture2.cpp
// Author:    Kai Welke, Pedram Azad
// Date:      2007
// ****************************************************************************
// Requires:  * libdc1394v2
//            * libraw1394 >= 1.2.0
// ****************************************************************************
// Changes:   20.12.2007, Kai Welke
//            * Built CLinux1394Capture2 on base of CLinux1394Capture
//            * switched from libdc1394v1 to libdc1394v2 (solved problem 
//              with QT and allowed to use full firewire bandwidth)
//            * added check for maximum bandwidth
//            * generic camera feature access
//            * Format7_0 implemented with ROI and framerate settings
//              format7 support is camera dependant (dragonfly only
//              supports two discrete framerates, dragonfly2 can be
//              adjusted from 0.x ... 60.0 frames.
//            05.05.2011, Kai Welke
//            * Corrected bus reset. Bus for all cameras reset on 
//              creation of first instance. The previous bus reset broke
//              the support of multiple instances. 
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "VideoCapture/Linux1394Capture2.h"
#include "Image/ByteImage.h"
#include "Image/ImageProcessor.h"
#include "Threading/Threading.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <algorithm>


// ****************************************************************************
// Defines
// ****************************************************************************

#define DROP_FRAMES 1
#define NUM_BUFFERS 2


// *****************************************************************
// Static members
// *****************************************************************

int			CLinux1394Capture2::m_nOverallCameras = 0;
CLinux1394Capture2*	CLinux1394Capture2::m_pCameraOpener[MAX_CAMERAS];
int			CLinux1394Capture2::m_nInstances = 0;
dc1394camera_t*		CLinux1394Capture2::m_cameras[MAX_CAMERAS];
dc1394_t*		CLinux1394Capture2::m_pDC1394;
int			CLinux1394Capture2::m_nRemainingBandwidth = MAX_S400_BANDWIDTH;
int			CLinux1394Capture2::m_nCameraBandwidth[MAX_CAMERAS];



// *****************************************************************
// Constructors
// *****************************************************************

CLinux1394Capture2::CLinux1394Capture2(int nCameras, VideoMode mode, ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType, FrameRate frameRate) : m_mode(mode), m_colorMode(colorMode), m_bayerPatternType(bayerPatternType), m_frameRate(frameRate)
{
	// number of cameras
	m_nCameras = nCameras;
	
	// init images
	m_pTempImageHeader = new CByteImage();

	// init size
	width = -1;
	height = -1;

	// init features
	m_bUseUIDs = false;
	m_bFormat7Mode = false;

	// if we are the first instance
	if(m_nInstances == 0)
		InitFirstInstance();

	m_nInstances++;
}

CLinux1394Capture2::CLinux1394Capture2(VideoMode mode, ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType, FrameRate frameRate, int nNumberUIDs, ...) : m_mode(mode), m_colorMode(colorMode), m_bayerPatternType(bayerPatternType), m_frameRate(frameRate)
{
	// number cameras
	m_nCameras = nNumberUIDs;
	
	// fill camera UIDS
	for(int i = 0 ; i < MAX_CAMERAS ; i++)
		m_sCameraUID[i] = "";

	// read variable arguments
	va_list ap;
	va_start(ap, nNumberUIDs); 

	for(int u = 0 ; u < nNumberUIDs ; u++)
	{
		const char* pch = va_arg(ap, const char*);
		m_sCameraUID[u] += pch;
		transform(m_sCameraUID[u].begin(), m_sCameraUID[u].end(), m_sCameraUID[u].begin(), (int(*)(int)) toupper);
	} 

	va_end(ap);

	// init images
	m_pTempImageHeader = new CByteImage();

	// init size
	width = -1;
	height = -1;

	// set features
	m_bUseUIDs = true;
	m_bFormat7Mode = false;

	// if we are the first instance
	if(m_nInstances == 0)
		InitFirstInstance();

	m_nInstances++;
}


// ****************************************************************************
// Constructors for Format7 Modes
// ****************************************************************************

CLinux1394Capture2::CLinux1394Capture2(int nCameras, VideoMode mode, float fFormat7FrameRate, int nFormat7MinX, int nFormat7MinY, int nFormat7Width, int nFormat7Height,	ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType) :
	m_mode(mode), m_colorMode(colorMode), m_bayerPatternType(bayerPatternType)
{
	// number cameras
	m_nCameras = nCameras;
	
	// init images
	m_pTempImageHeader = new CByteImage();

	// init size
	width = -1;
	height = -1;
	
	// set features
	m_bFormat7Mode = true;
	m_fFormat7FrameRate = fFormat7FrameRate;
	m_nFormat7MinX = nFormat7MinX;
	m_nFormat7MinY = nFormat7MinY;
	
	if (nFormat7Width == -1 || nFormat7Height == -1)
	{
		switch (mode)
		{
			case e640x480: m_nFormat7Width = 640; m_nFormat7Height = 480; break;
			case e800x600: m_nFormat7Width = 800; m_nFormat7Height = 600; break;
			case e1024x768: m_nFormat7Width = 1024; m_nFormat7Height = 768; break;
			case e1280x960: m_nFormat7Width = 1280; m_nFormat7Height = 960; break;
			case e1600x1200: m_nFormat7Width = 1600; m_nFormat7Height = 1200; break;
			default: m_nFormat7Width = 0; m_nFormat7Height = 0; break;
		}
	}
	else
	{
		m_nFormat7Width = nFormat7Width;
		m_nFormat7Height = nFormat7Height;
	}
	
	m_bUseUIDs = false;

	// if we are first instance
	if (m_nInstances == 0)
		InitFirstInstance();

	m_nInstances++;
}

CLinux1394Capture2::CLinux1394Capture2(VideoMode mode, float fFormat7FrameRate, int nFormat7MinX, int nFormat7MinY, int nFormat7Width, int nFormat7Height,
	ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType, int nNumberUIDs, ...) :
	m_mode(mode), m_colorMode(colorMode), m_bayerPatternType(bayerPatternType)
{
	// number cameras
	m_nCameras = nNumberUIDs;
	
	// fill camera UIDS
	for(int i = 0 ; i < MAX_CAMERAS ; i++)
		m_sCameraUID[i] = "";

	// read variable arguments
	va_list ap;
	va_start(ap, nNumberUIDs); 

	for(int u = 0 ; u < nNumberUIDs ; u++)
	{
		const char* pch = va_arg(ap, const char*);
		m_sCameraUID[u] += pch;
		transform(m_sCameraUID[u].begin(), m_sCameraUID[u].end(), m_sCameraUID[u].begin(), (int(*)(int)) toupper);
	} 

	va_end(ap);

	// init images
	m_pTempImageHeader = new CByteImage();

	// init size
	width = -1;
	height = -1;
	
	// init features
	m_bFormat7Mode = true;
	m_fFormat7FrameRate = fFormat7FrameRate;
	m_nFormat7MinX = nFormat7MinX;
	m_nFormat7MinY = nFormat7MinY;
	
	if (nFormat7Width == -1 || nFormat7Height == -1)
	{
		switch (mode)
		{
			case e640x480: m_nFormat7Width = 640; m_nFormat7Height = 480; break;
			case e800x600: m_nFormat7Width = 800; m_nFormat7Height = 600; break;
			case e1024x768: m_nFormat7Width = 1024; m_nFormat7Height = 768; break;
			case e1280x960: m_nFormat7Width = 1280; m_nFormat7Height = 960; break;
			case e1600x1200: m_nFormat7Width = 1600; m_nFormat7Height = 1200; break;
			default: m_nFormat7Width = 0; m_nFormat7Height = 0; break;
		}
	}
	else
	{
		m_nFormat7Width = nFormat7Width;
		m_nFormat7Height = nFormat7Height;
	}

	m_bUseUIDs = true;
	m_bFormat7Mode = true;

	// if we are first instance
	if (m_nInstances == 0)
		InitFirstInstance();
		
	m_nInstances++;
}


// *****************************************************************
// Destructor
// *****************************************************************

CLinux1394Capture2::~CLinux1394Capture2()
{
	// close all related cameras
	CloseCamera();
	
	// delete images
	delete m_pTempImageHeader;

	// remove this instance
	m_nInstances--;

	// if we are last instance	
	if(m_nInstances == 0)
		ExitLastInstance();
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CLinux1394Capture2::InitFirstInstance()
{
	// set required fields to initial values
	for (int i = 0 ; i < MAX_CAMERAS ; i++)
	{
		m_pCameraOpener[i] = NULL;
		m_cameras[i] = NULL;
	}
	
	m_nOverallCameras = 0;
	
	// create new dc handle
	m_pDC1394 = dc1394_new();
	
	// reset all busses (disabled, since some firewire cards take obver 5 seconds to perform the reset)
	//ResetAllCameras();
}

void CLinux1394Capture2::ExitLastInstance()
{
	// check if there were listed cameras with no opener and close them
	for(int c = 0 ; c < m_nOverallCameras ; c++)
	{
		if(m_cameras[c] != NULL)
		{
			dc1394_camera_free(m_cameras[c]);
		}
			
		m_cameras[c] = NULL;
	}

	// free dc handle	
	dc1394_free(m_pDC1394);
}

bool CLinux1394Capture2::OpenCamera()
{
	CloseCamera();
	
	// create temporary images and init modes
	if(!InitCameraMode())
	{
		printf("error: selected camera mode not supported\n");
		return false;
	}

	// retrieve info for all cameras
	if(!ListCameras())
	{
		CloseCamera();
		return false;
	}
		
	// open cameras
	if(m_bUseUIDs)
	{
		for(int c = 0 ; c < m_nCameras ; c++)
		{
			bool bOpened = false;
			for(int a = 0 ; a < m_nOverallCameras ; a++)
			{
				if(m_sCameraUID[c] == CamUIDToString(m_cameras[a]->guid))
				{
					if(m_pCameraOpener[a])
					{
						printf("error: camera with UID 0x%s already opened\n", m_sCameraUID[c].c_str());
						return false;
					}
					if(!OpenCamera(a))
						return false;

					m_pCameraOpener[a] = this;
					m_nOpenedCameras[c] = a;
					bOpened = true;
				}
			}

			if(!bOpened)
			{
				printf("error: could not find camera with UID 0x%s\n",m_sCameraUID[c].c_str());
				CloseCamera();
				return false;
			}
		}
	} else {
		int nNumberCameras = 0;
		for(int c = 0 ; c < m_nOverallCameras ; c++)
		{
			if(m_pCameraOpener[c] == NULL)
			{
				if(!OpenCamera(c))
					return false;
	
				m_pCameraOpener[c] = this;
				m_nOpenedCameras[nNumberCameras] = c;
				nNumberCameras++;
			}

			if( (m_nCameras != -1) && (nNumberCameras == m_nCameras))
				break;
		}

		if(m_nCameras == -1) 
		{
			if(nNumberCameras == 0)
			{
				printf("error: no camera found\n");
				CloseCamera();
				return false;
			}
			m_nCameras = nNumberCameras;
		}
		
		if(nNumberCameras < m_nCameras)
		{
			printf("error: could not open requested number of cameras\n");
			CloseCamera();
			return false;
		}
	}

	return true;
}

bool CLinux1394Capture2::OpenCamera(int nCamera)
{
	printf("Opening camera %d: Vendor %s, Model %s, UID 0x%s, Format7 = %i\n",nCamera, m_cameras[nCamera]->vendor, m_cameras[nCamera]->model,  CamUIDToString(m_cameras[nCamera]->guid).c_str(), m_bFormat7Mode);
	
	if (dc1394_video_set_iso_speed(m_cameras[nCamera], DC1394_ISO_SPEED_400) != DC1394_SUCCESS)
        {
        	printf("error: unable to set ISO speed to 400\n");
		CloseCamera();
		return false;
        };
	
	if (dc1394_video_set_mode(m_cameras[nCamera],m_video_mode) != DC1394_SUCCESS)
	{
		printf("error: unable to set video mode\n");
		CloseCamera();
		return false;	
	}
	
	if (m_bFormat7Mode)
	{
		if (dc1394_format7_set_image_size(m_cameras[nCamera],m_video_mode, m_nFormat7Width, m_nFormat7Height) != DC1394_SUCCESS)
		{
			printf("error: unable to set format7 image size (%d, %d)\n", m_nFormat7Width, m_nFormat7Height);
			CloseCamera();
			return false;
		}
		
		if (dc1394_format7_set_image_position(m_cameras[nCamera],m_video_mode, m_nFormat7MinX, m_nFormat7MinY) != DC1394_SUCCESS)
		{
			printf("error: unable to set format7 image position (%d, %d)\n", m_nFormat7MinX, m_nFormat7MinY);
			CloseCamera();
			return false;
		}
		
		// calculate packet size from, framerate
		unsigned int nPacketSize = 0;
		if (m_fFormat7FrameRate < 0)
		{
			dc1394_format7_get_recommended_packet_size(m_cameras[nCamera], m_video_mode, &nPacketSize);
		}
		else
		{
			uint64_t frame_size;
			unsigned int nMinBytes, nMaxBytes;
			dc1394_format7_get_total_bytes(m_cameras[nCamera], m_video_mode, &frame_size);
			dc1394_format7_get_packet_parameters(m_cameras[nCamera], m_video_mode, &nMinBytes, &nMaxBytes);
			
			int num_packets = (int) (1.0f / (0.0125f * m_fFormat7FrameRate) + 0.5f);	// number of packets per frame period
			int denominator = num_packets * 8; 
			nPacketSize = (unsigned int) ( ( frame_size + denominator - 1) / float(denominator) / 10.0);
			nPacketSize = nMinBytes * ( nPacketSize / nMinBytes);
			
			if (nPacketSize < nMinBytes)
				nPacketSize = nMinBytes;
				
			if (nPacketSize > nMaxBytes)
				nPacketSize = nMaxBytes;
		}

		if (dc1394_format7_set_packet_size(m_cameras[nCamera],m_video_mode, nPacketSize) != DC1394_SUCCESS)
		{
			printf("error: unable to set format7 packet size %d (framerate %f)\n", nPacketSize, m_fFormat7FrameRate);
			CloseCamera();
			return false;
		}
	}
	else
	{
		if (dc1394_video_set_framerate(m_cameras[nCamera], GetDCFrameRateMode(m_frameRate)) != DC1394_SUCCESS)
		{
			printf("error: could not set framerate\n");
			CloseCamera();
			return false;
		}
	}
	
	// calculate bandwidth usage
	unsigned int nBandwidth;
	if(dc1394_video_get_bandwidth_usage(m_cameras[nCamera],&nBandwidth) != DC1394_SUCCESS)
	{
		printf("error: unable to calculate bandwidth usage\n");
		CloseCamera();
		return false;
	}
	
	m_nCameraBandwidth[nCamera] = nBandwidth;
	m_nRemainingBandwidth -= nBandwidth;
	printf("Camera %d bandwidth usage: %d quadlets/cycle (%d quadlets/cycle left)\n",nCamera, nBandwidth, m_nRemainingBandwidth);

	if(m_nRemainingBandwidth < 0)
	{
		printf("warning: Opening camera %d would exceed maximum firewire bandwidth (%d quadlets/cycle).\n", nCamera, MAX_S400_BANDWIDTH);
		m_nRemainingBandwidth += nBandwidth;
        
        // deprecated: using two firewire ports the calculation is wrong. Further several cameras do not provide proper bandwidth information
		//CloseCamera();
		//return false;
	}

	// hack: port seems to be busy for some time. so give time and try multiple calls to setup	
	bool bSuccess = false;
	int nTries = 5;

	while(!bSuccess && (nTries > 0))
	{
		if( dc1394_capture_setup(m_cameras[nCamera], NUM_BUFFERS, DC1394_CAPTURE_FLAGS_DEFAULT) == DC1394_SUCCESS)
			bSuccess = true;

		Threading::SleepThread(10);
		nTries--;
	}

	if(!bSuccess)
	{
		printf("error: unable to setup camera (mode supported by camera?)\n");
		CloseCamera();
		return false;
	}
	
	
	if(dc1394_video_set_transmission(m_cameras[nCamera], DC1394_ON) != DC1394_SUCCESS) 
	{
		printf("error: unable to start camera iso transmission\n");
		CloseCamera();
		return false;
	}

	return true;
}

void CLinux1394Capture2::CloseCamera()
{
	for (int i = 0; i < m_nOverallCameras; i++)
	{
		if(m_pCameraOpener[i] == this)
		{
			// stop video transmission
			if(dc1394_video_set_transmission(m_cameras[i], DC1394_OFF) != DC1394_SUCCESS)
				printf("Error: could not stop iso transmission\n");
			
			// stop capturer
			if(dc1394_capture_stop(m_cameras[i]) != DC1394_SUCCESS)
				printf("Error: could not stop capturing\n");
			
			// no opener for this camera
			m_pCameraOpener[i] = NULL;
			
			// adjust remaining bandwidth
			m_nRemainingBandwidth += m_nCameraBandwidth[i];
		}
	}
	
	width = -1;
	height = -1;
}

bool CLinux1394Capture2::InitCameraMode()
{
	width = -1;
	height = -1;
	int video_mode = 0;

	// for format 7 modes (only use mode 0)
	if (m_bFormat7Mode)
	{
		// set video mode to format 7
		m_video_mode = DC1394_VIDEO_MODE_FORMAT7_0;
		
		// set image size
		width = m_nFormat7Width;
		height = m_nFormat7Height;
		
		// set capture image header correctly
		m_pTempImageHeader->width = width;
		m_pTempImageHeader->height = height;
		m_pTempImageHeader->bytesPerPixel = 1;
		m_pTempImageHeader->type = CByteImage::eGrayScale;
		
		return true;
	}
	
	// standard modes
	if (m_mode == e640x480)
	{
		width = 640;
		height = 480;
		
		switch (m_colorMode)
		{
			case eGrayScale:
			case eBayerPatternToRGB24:
				m_video_mode = DC1394_VIDEO_MODE_640x480_MONO8;
			break;
			
			case eRGB24:
				m_video_mode = DC1394_VIDEO_MODE_640x480_RGB8;
			break;

			case eYUV411ToRGB24:
				m_video_mode = DC1394_VIDEO_MODE_640x480_YUV411;
			break;
		}
	}
	else if (m_mode == e800x600)
	{
		width = 800;
		height = 600;
		
		switch (m_colorMode)
		{
			case eGrayScale:
			case eBayerPatternToRGB24:
				m_video_mode = DC1394_VIDEO_MODE_800x600_MONO8;
			break;
			
			case eRGB24:
				m_video_mode = DC1394_VIDEO_MODE_800x600_RGB8;
			break;

			case eYUV411ToRGB24: return false;
		}
	}
	else if (m_mode == e1024x768)
	{
		width = 1024;
		height = 768;
		
		switch (m_colorMode)
		{
			case eGrayScale:
			case eBayerPatternToRGB24:
				m_video_mode = DC1394_VIDEO_MODE_1024x768_MONO8;
			break;
			
			case eRGB24:
				m_video_mode = DC1394_VIDEO_MODE_1024x768_RGB8;
			break;

			case eYUV411ToRGB24: return false;
		}
	}
	else if (m_mode == e1280x960)
	{
		width = 1280;
		height = 960;
		
		switch (m_colorMode)
		{
			case eGrayScale:
			case eBayerPatternToRGB24:
				m_video_mode = DC1394_VIDEO_MODE_1280x960_MONO8;
			break;
			
			case eRGB24:
				m_video_mode = DC1394_VIDEO_MODE_1280x960_RGB8;
			break;

			case eYUV411ToRGB24: return false;
		}
	}
	else if (m_mode == e1600x1200)
	{
		width = 1600;
		height = 1200;
		
		switch (m_colorMode)
		{
			case eGrayScale:
			case eBayerPatternToRGB24:
				m_video_mode = DC1394_VIDEO_MODE_1600x1200_MONO8;
			break;
			
			case eRGB24:
				m_video_mode = DC1394_VIDEO_MODE_1600x1200_RGB8;
			break;

			case eYUV411ToRGB24: return false;
		}
	}
	else
	{
		// mode not supported
		return false;
	}
	
	m_pTempImageHeader->width = width;
	m_pTempImageHeader->height = height;
	
	if (m_colorMode == eBayerPatternToRGB24 || m_colorMode == eGrayScale)
	{
		m_pTempImageHeader->bytesPerPixel = 1;
		m_pTempImageHeader->type = CByteImage::eGrayScale;
	}
	else if (m_colorMode == eRGB24 || m_colorMode == eYUV411ToRGB24)
	{
		m_pTempImageHeader->bytesPerPixel = 3;
		m_pTempImageHeader->type = CByteImage::eRGB24;
	}

	return true;
}


void CLinux1394Capture2::ConvertYUV411(CByteImage* pInput, CByteImage* pOutput)
{
	int width = pInput->width;
	int height = pInput->height;

	int y1,y2,y3,y4,u,v;
	int r,g,b;
	
	unsigned char* output = pOutput->pixels;
	unsigned char* input = pInput->pixels;
	// read chunks
	for(int i = 0 ; i < height ; i++)
	{
		for(int j = 0 ; j < width / 4 ; j++)
		{
			u  = input[0];
			y1 = input[1];
			y2 = input[2];

			v  = input[3];
			y3 = input[4];
			y4 = input[5];
			
			YUVToRGB(y1,u,v,output);
			YUVToRGB(y2,u,v,output + 3);
			YUVToRGB(y3,u,v,output + 6);
			YUVToRGB(y4,u,v,output + 9);
			
			input += 6;
			output += 12;
		}
	}
}

void CLinux1394Capture2::YUVToRGB(int y, int u, int v, unsigned char* output)
{
	int r,g,b;
	
	// u and v are +-0.5
	u -= 128;
	v -= 128;
	
	// conversion
	r = int(y + 1.370705 * v);
	g = int(y - 0.698001 * v - 0.337633 * u);
	b = int(y + 1.732446 * u);
	
	// clamp to 0..1
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	
	output[0] = (unsigned char) r;
	output[1] = (unsigned char) g;
	output[2] = (unsigned char) b;
}


// *****************************************************************
// CaptureImage
// *****************************************************************

bool CLinux1394Capture2::CaptureImage(CByteImage **ppImages)
{
	if ( (width == -1) || (height == -1) )
		return false;

	int i;

	for (i = 0; i < m_nCameras; i++)
	{
		int nCurrentIndex = m_nOpenedCameras[i];
		dc1394video_frame_t* pCurrentFrame = NULL;

		while(pCurrentFrame == NULL)
		{
			if(dc1394_capture_dequeue(m_cameras[nCurrentIndex], DC1394_CAPTURE_POLICY_POLL, &pCurrentFrame) != DC1394_SUCCESS)
			{
				printf("Error: could not capture current frame on camera %d\n",nCurrentIndex);
				return false;
			}
		}

		m_pTempImageHeader->pixels = (unsigned char *) pCurrentFrame->image;

		switch (m_colorMode)
		{
			case eBayerPatternToRGB24:
				ImageProcessor::ConvertBayerPattern(m_pTempImageHeader, ppImages[i], m_bayerPatternType);
			break;

			case eGrayScale:
			case eRGB24:
				ImageProcessor::CopyImage(m_pTempImageHeader, ppImages[i]);
			break;

			case eYUV411ToRGB24:
				ConvertYUV411(m_pTempImageHeader, ppImages[i]);
			break;
		}

		if(dc1394_capture_enqueue(m_cameras[nCurrentIndex], pCurrentFrame) != DC1394_SUCCESS)
		{
			printf("Error: could not release current frame of camera %d\n", nCurrentIndex);
			return false;
		}
	}
	
	return true;
}

bool CLinux1394Capture2::CaptureBayerPatternImage(CByteImage **ppImages)
{
	if ( (width == -1) || (height == -1) )
		return false;

	int i;

	// free last frame
	for (i = 0; i < m_nCameras; i++)
	{
		int nCurrentIndex = m_nOpenedCameras[i];

		dc1394video_frame_t* pCurrentFrame = NULL;
		// retrieve frame (used poll also WAIT is possible)
		while(pCurrentFrame == NULL)
		{
			if(dc1394_capture_dequeue(m_cameras[nCurrentIndex], DC1394_CAPTURE_POLICY_POLL, &pCurrentFrame) != DC1394_SUCCESS)
			{
				printf("Error: could not capture current frame on camera %d\n",nCurrentIndex);
				return false;
			}
		}

		m_pTempImageHeader->pixels = (unsigned char *) pCurrentFrame->image;
		
		if(m_bFormat7Mode)
		{
			ImageProcessor::CopyImage(m_pTempImageHeader, ppImages[i]);
		}
			
		if (m_mode == e640x480)
		{
			ImageProcessor::CopyImage(m_pTempImageHeader, ppImages[i]);
		}
		else if (m_mode == e320x240)
		{
			ImageProcessor::Resize(m_pTempImageHeader, ppImages[i]);
		}

		if(dc1394_capture_enqueue(m_cameras[nCurrentIndex], pCurrentFrame) != DC1394_SUCCESS)
		{
			printf("Error: could not release current frame of camera %d\n",nCurrentIndex);
			return false;
		}
	}
	
	return true;
}

CByteImage::ImageType CLinux1394Capture2::GetType()
{
	switch (m_colorMode)
	{
		case eYUV411ToRGB24:
		case eBayerPatternToRGB24:
		case eRGB24:
			return CByteImage::eRGB24;
		break;
		
		case eGrayScale:
			return CByteImage::eGrayScale;
		break;
	}
	
	return (CByteImage::ImageType) -1;
}

void CLinux1394Capture2::SetCameraUids(std::vector<std::string> uids)
{
	m_nCameras = uids.size();
	
	for(int u = 0 ; u < m_nCameras ; u++)
	{
		m_sCameraUID[u] += uids[u];
		transform(m_sCameraUID[u].begin(), m_sCameraUID[u].end(), m_sCameraUID[u].begin(), (int(*)(int)) toupper);
	} 
}

void CLinux1394Capture2::SetGain(int nValue)
{
	SetFeature(DC1394_FEATURE_GAIN, "gain", nValue);
}

void CLinux1394Capture2::SetExposure(int nValue)
{
	SetFeature(DC1394_FEATURE_EXPOSURE, "exposure", nValue);
}

void CLinux1394Capture2::SetShutter(int nValue)
{
	SetFeature(DC1394_FEATURE_SHUTTER, "shutter", nValue);
}

void CLinux1394Capture2::SetWhiteBalance(int nU, int nV, int nCamera)
{
	if(nCamera == -1)
	{
		for (int i = 0; i < m_nCameras; i++)
		{
			int nCurrentCamera = m_nOpenedCameras[i];
			dc1394_feature_whitebalance_set_value(m_cameras[nCurrentCamera], nU, nV);
		}
	} else {
		int nCurrentCamera = m_nOpenedCameras[nCamera];
		dc1394_feature_whitebalance_set_value(m_cameras[nCurrentCamera], nU, nV);
	}
}

void CLinux1394Capture2::SetTemperature(int nTemperature)
{
	for (int i = 0; i < m_nCameras; i++)
	{
		int nCurrentCamera = m_nOpenedCameras[i];
		dc1394_feature_temperature_set_value(m_cameras[nCurrentCamera], nTemperature);
	}
}

void CLinux1394Capture2::ListFeatures()
{
	for (int i = 0; i < m_nCameras; i++)
	{
		printf("== Camera %d =========================================================================\n", i);
		int nCurrentCamera = m_nOpenedCameras[i];
		dc1394featureset_t 	feature_set;

		dc1394_feature_get_all(m_cameras[nCurrentCamera], &feature_set);
		dc1394_feature_print_all(&feature_set,stdout);
	}	
}

void CLinux1394Capture2::SetFeature(dc1394feature_t feature, std::string sName, int nValue)
{
	dc1394bool_t bPresent;
	
	for (int i = 0; i < m_nCameras; i++)
	{
		int nCurrentCamera = m_nOpenedCameras[i];
		dc1394_feature_is_present(m_cameras[nCurrentCamera],feature, &bPresent);
		
		if(!bPresent)
		{
			printf("warning: camera %d does not support %s feature\n", nCurrentCamera, sName.c_str());
			return;
		}  
		
		// check boundaries
		uint32_t nMin, nMax;
		dc1394_feature_get_boundaries(m_cameras[nCurrentCamera],feature, &nMin, &nMax);
		
		if( (nValue != -1) && ((nValue < nMin) || (nValue > nMax)) )
		{
			printf("warning: requested %s %d for camera %d is not in the valid range [%d;%d]\n", sName.c_str(), nValue, nCurrentCamera, nMin, nMax);
			return;
		}
		
		dc1394feature_modes_t availableModes;
		dc1394_feature_get_modes(m_cameras[nCurrentCamera],feature, &availableModes);
		
		bool bHasAuto = false;
		bool bHasManual = false;
		
		for(int i = 0 ; i < availableModes.num ; i++)
		{
			if(availableModes.modes[i] == DC1394_FEATURE_MODE_AUTO)
				bHasAuto = true;
			if(availableModes.modes[i] == DC1394_FEATURE_MODE_MANUAL)
				bHasManual = true;
		}
		
		if (nValue == -1)
		{
			if(!bHasAuto)
			{
				printf("warning: camera %d has no auto %s feature\n", nCurrentCamera, sName.c_str());
				return;
			}
			dc1394_feature_set_mode(m_cameras[nCurrentCamera], feature, DC1394_FEATURE_MODE_AUTO);
		}
		else
		{
			if(!bHasManual)
			{
				printf("warning: camera %d has no manual %s feature\n", nCurrentCamera, sName.c_str());
				return;
			}
			dc1394_feature_set_mode(m_cameras[nCurrentCamera], feature, DC1394_FEATURE_MODE_MANUAL);
			dc1394_feature_set_value(m_cameras[nCurrentCamera], feature, nValue);
		}
	}
}

dc1394framerate_t CLinux1394Capture2::GetDCFrameRateMode(FrameRate frameRate)
{
	switch(frameRate)
	{
		case e60fps: return DC1394_FRAMERATE_60;
		case e30fps: return DC1394_FRAMERATE_30;
		case e15fps: return DC1394_FRAMERATE_15;
		case e7_5fps: return DC1394_FRAMERATE_7_5;
		case e3_75fps: return DC1394_FRAMERATE_3_75;
		case e1_875fps: return DC1394_FRAMERATE_1_875;

		default: return DC1394_FRAMERATE_30;
	};
}

void CLinux1394Capture2::ResetAllCameras()
{
	dc1394camera_list_t * list;
	dc1394camera_t *camera;
	dc1394error_t err;

    bool bFinished = false;
    int nIndex = 0;
    
    while(!bFinished)
    {
		err=dc1394_camera_enumerate (m_pDC1394, &list);
		
		// check if last camera
		if (nIndex >= list->num) 
		{
			break;			
		}

		camera = dc1394_camera_new (m_pDC1394, list->ids[nIndex].guid);
		
		if (!camera) 
		{
			break;
		}

		dc1394_camera_free_list (list);

		dc1394_reset_bus (camera);
		
		dc1394_camera_free (camera);
		nIndex++;

		// give time to bus for recovery after bus reset. Needed for some firewire cards.		
		usleep(50000);
	}
}

bool CLinux1394Capture2::ListCameras()
{
	// already listed
	if(m_nOverallCameras != 0)
		return true;

	// enumarate cameras
	dc1394camera_list_t* camera_list;
	
	if(dc1394_camera_enumerate(m_pDC1394, &camera_list) != DC1394_SUCCESS)
	{
		printf("error: cannot enumerate cameras\n");
		return false;
	}
   
	int nNumberCams = camera_list->num;
	
	for(int i = 0 ; i < nNumberCams ; i++)
	{
		printf("= camera %d ========================================= :\n",i);
		
		m_cameras[i] = dc1394_camera_new(m_pDC1394, camera_list->ids[i].guid);
		m_pCameraOpener[i] = NULL;
		
		if(dc1394_camera_print_info(m_cameras[i], stdout) != DC1394_SUCCESS)
		{
			printf("error: could not retrieve info for camera %d\n",i);
			
			// free camera list
			dc1394_camera_free_list(camera_list);
	
			return false;
		}
	}
	
	// free camera list
	dc1394_camera_free_list(camera_list);
	
	m_nOverallCameras = nNumberCams;

	// check some stuff	
	if(m_nOverallCameras > MAX_CAMERAS)
	{
		printf("error: found more cameras than MAX_CAMERAS\n");
		CloseCamera();
		return false;
	}	
	
	// output info and create camera structures
	printf("= Found %d cameras ==================================\n",nNumberCams);
	return true;
}

std::string CLinux1394Capture2::CamUIDToString(uint64_t uid)
{
	std::string UID = "";
	int nLow = (int) uid;
	int nHigh = (int) (uid >> 32);
	char szUID[17];
	sprintf(szUID,"%08X%08X\0",nHigh,nLow);
	UID += szUID;
	
	return UID;
}
