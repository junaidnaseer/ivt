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
// Filename:  Linux1394Capture.cpp
// Author:    Pedram Azad, Kai Welke
// Date:      2005
// ****************************************************************************
// Changes:   24.08.2006. Kai Welke
//            * Added YUV411 support
//            26.09.2006, Kai Welke
//            * added support for UIDs and different frame rates
//            * open multiple captures to allow different rates and formats
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "Linux1394Capture.h"
#include "Image/ByteImage.h"
#include "Image/ImageProcessor.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <algorithm>


// ****************************************************************************
// Defines
// ****************************************************************************

#define DROP_FRAMES 1
#define NUM_BUFFERS 8


// ****************************************************************************
// Static members
// ****************************************************************************

int			CLinux1394Capture::m_nOverallCameras = 0;
CLinux1394Capture::TCameraInfo		CLinux1394Capture::m_CameraInfo[MAX_CAMERAS];
CLinux1394Capture*	CLinux1394Capture::m_pCameraOpener[MAX_CAMERAS];
int			CLinux1394Capture::m_nInstances = 0;
raw1394handle_t 	CLinux1394Capture::m_handles[MAX_CAMERAS];
dc1394_cameracapture	CLinux1394Capture::m_cameras[MAX_CAMERAS];
nodeid_t*		CLinux1394Capture::m_pCameraNodes[MAX_PORTS];


using namespace std;



// ****************************************************************************
// Constructors
// ****************************************************************************

CLinux1394Capture::CLinux1394Capture(int nCameras, VideoMode mode, ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType, FrameRate frameRate) : m_mode(mode), m_colorMode(colorMode), m_bayerPatternType(bayerPatternType), m_frameRate(frameRate)
{
	m_nPorts = MAX_PORTS;
		
	m_pTempImageHeader = new CByteImage();
	
	m_pTempImage = 0;

	m_nCameras = nCameras;

	width = -1;
	height = -1;

	m_bUseUIDs = false;
	m_bFormat7Mode = false;

	if(m_nInstances == 0)
	{
		for(int i = 0 ; i < MAX_CAMERAS ; i++)
		{
			m_handles[i] = NULL;
			m_pCameraOpener[i] = NULL;
		}
	}

	m_nInstances++;
}

CLinux1394Capture::CLinux1394Capture(VideoMode mode, ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType, FrameRate frameRate, int nNumberUIDs, ...) : m_mode(mode), m_colorMode(colorMode), m_bayerPatternType(bayerPatternType), m_frameRate(frameRate)
{
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

	m_nPorts = MAX_PORTS;
		
	m_pTempImageHeader = new CByteImage();
	
	m_pTempImage = 0;

	m_nCameras = nNumberUIDs;

	width = -1;
	height = -1;

	m_bUseUIDs = true;
	m_bFormat7Mode = false;

	if(m_nInstances == 0)
	{
		for(int i = 0 ; i < MAX_CAMERAS ; i++)
		{
			m_handles[i] = NULL;
			m_pCameraOpener[i] = NULL;
		}
	}

	m_nInstances++;
}


// ****************************************************************************
// Constructors for Format7 Modes
// ****************************************************************************

CLinux1394Capture::CLinux1394Capture(int nCameras, VideoMode mode, int nFormat7PacketSize, int nFormat7MinX, int nFormat7MinY, int nFormat7Width, int nFormat7Height,
	ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType) :
	m_mode(mode), m_colorMode(colorMode), m_bayerPatternType(bayerPatternType)
{
	m_nPorts = MAX_PORTS;
		
	m_pTempImageHeader = new CByteImage();
	
	m_pTempImage = 0;

	m_nCameras = nCameras;

	width = -1;
	height = -1;
	
	m_bFormat7Mode = true;
	m_nFormat7PacketSize = nFormat7PacketSize == -1 ? USE_MAX_AVAIL : nFormat7PacketSize;
	m_nFormat7MinX = nFormat7MinX;
	m_nFormat7MinY = nFormat7MinY;
	m_nFormat7Width = nFormat7Width;
	m_nFormat7Height = nFormat7Height;
	
	m_bUseUIDs = false;

	if (m_nInstances == 0)
	{
		for (int i = 0 ; i < MAX_CAMERAS ; i++)
		{
			m_handles[i] = NULL;
			m_pCameraOpener[i] = NULL;
		}
	}

	m_nInstances++;
}

CLinux1394Capture::CLinux1394Capture(VideoMode mode, int nFormat7PacketSize, int nFormat7MinX, int nFormat7MinY, int nFormat7Width, int nFormat7Height,
	ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType, int nNumberUIDs, ...) :
	m_mode(mode), m_colorMode(colorMode), m_bayerPatternType(bayerPatternType)
{
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

	m_nPorts = MAX_PORTS;
		
	m_pTempImageHeader = new CByteImage();
	
	m_pTempImage = 0;

	m_nCameras = nNumberUIDs;

	width = -1;
	height = -1;
	
	m_bFormat7Mode = true;
	m_nFormat7PacketSize = nFormat7PacketSize == -1 ? USE_MAX_AVAIL : nFormat7PacketSize;
	m_nFormat7MinX = nFormat7MinX;
	m_nFormat7MinY = nFormat7MinY;
	m_nFormat7Width = nFormat7Width;
	m_nFormat7Height = nFormat7Height;

	m_bUseUIDs = true;
	m_bFormat7Mode = true;

	if (m_nInstances == 0)
	{
		for (int i = 0 ; i < MAX_CAMERAS ; i++)
		{
			m_handles[i] = NULL;
			m_pCameraOpener[i] = NULL;
		}
	}

	m_nInstances++;
}


// ****************************************************************************
// Destructor
// ****************************************************************************

CLinux1394Capture::~CLinux1394Capture()
{
	CloseCamera();
	
	delete m_pTempImageHeader;
	
	if (m_pTempImage)
		delete m_pTempImage;

	// clean up if last instance
	m_nInstances--;

	if(m_nInstances == 0)
	{
		// release camera nodes
		for(int p = 0 ; p < m_nPorts ; p++)
			dc1394_free_camera_nodes(m_pCameraNodes[p]);

		// destroy handles
		for(int h = 0 ; h < m_nOverallCameras ; h++)
			dc1394_destroy_handle(m_handles[h]);

		m_nOverallCameras = 0;
	}
}


bool CLinux1394Capture::OpenCamera()
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
				if(m_sCameraUID[c] == m_CameraInfo[a].sUID)
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

bool CLinux1394Capture::OpenCamera(int nCamera)
{
	int p = m_CameraInfo[nCamera].nPort;
	int i = m_CameraInfo[nCamera].nIndex;

	printf("Opening camera %d: Port %d, Index %d, UID 0x%s, Format7 = %i\n",nCamera,p,i,m_CameraInfo[nCamera].sUID.c_str(), m_bFormat7Mode);

	char *device_name = 0;
	
	if (m_bFormat7Mode)
	{
		if (dc1394_dma_setup_format7_capture(m_handles[nCamera], m_cameras[nCamera].node, i + 1, MODE_FORMAT7_0, SPEED_400, m_nFormat7PacketSize, m_nFormat7MinX, m_nFormat7MinY, m_nFormat7Width, m_nFormat7Height, NUM_BUFFERS, DROP_FRAMES, device_name, &m_cameras[nCamera]) != DC1394_SUCCESS)
		{
			printf("error: unable to setup camera (mode supported by camera?)\n");
			CloseCamera();
			return false;
		}
	}
	else
		{
		if (dc1394_dma_setup_capture(m_handles[nCamera], m_cameras[nCamera].node, i + 1, FORMAT_VGA_NONCOMPRESSED, m_video_mode, SPEED_400, GetDCFrameRateMode(m_frameRate), NUM_BUFFERS, DROP_FRAMES, device_name, &m_cameras[nCamera]) != DC1394_SUCCESS)
		{
			printf("error: unable to setup camera (mode supported by camera?)\n");
			CloseCamera();
			return false;
		}
	}
	
	// have the camera start sending us data
	if (dc1394_start_iso_transmission(m_handles[nCamera], m_cameras[nCamera].node) != DC1394_SUCCESS) 
	{
		printf("error: unable to start camera iso transmission\n");
		CloseCamera();
		return false;
	}

	return true;
}

void CLinux1394Capture::CloseCamera()
{
	for (int i = 0; i < m_nCameras; i++)
	{
		if (m_handles[i])
		{
			if(m_pCameraOpener[i] == this)
			{
				dc1394_dma_unlisten(m_handles[i], &m_cameras[i]);
				dc1394_dma_release_camera(m_handles[i], &m_cameras[i]);
				m_pCameraOpener[i] = NULL;
			}
		}
	}
	
	width = -1;
	height = -1;
	
	if (m_pTempImage)
	{
		delete m_pTempImage;
		m_pTempImage = 0;
	}
}

bool CLinux1394Capture::InitCameraMode()
{
	width = -1;
	height = -1;
	int video_mode = 0;

	if (m_mode == e640x480)
	{
		width = 640;
		height = 480;
			
		m_pTempImageHeader->width = 640;
		m_pTempImageHeader->height = 480;
		
		switch (m_colorMode)
		{
			case eGrayScale:
			case eBayerPatternToRGB24:
				m_video_mode = MODE_640x480_MONO;
			break;
			
			case eRGB24:
				m_video_mode = MODE_640x480_RGB;
			break;

			case eYUV411ToRGB24:
				m_video_mode = MODE_640x480_YUV411;
			break;
		}
	}
	else if (m_mode == e320x240)
	{
		width = 320;
		height = 240;
		
		switch (m_colorMode)
		{
			case eGrayScale:
				m_video_mode = MODE_640x480_MONO;
				m_pTempImageHeader->width = 640;
				m_pTempImageHeader->height = 480;
			break;
			
			case eBayerPatternToRGB24:
				m_video_mode = MODE_640x480_MONO;
				m_pTempImage = new CByteImage(640, 480, CByteImage::eRGB24);
				m_pTempImageHeader->width = 640;
				m_pTempImageHeader->height = 480;
			break;
			
			case eRGB24:
				m_video_mode = MODE_640x480_RGB;
				m_pTempImageHeader->width = 640;
				m_pTempImageHeader->height = 480;
			break;

			case eYUV411ToRGB24:
				m_video_mode = MODE_640x480_YUV411;
				m_pTempImage = new CByteImage(640, 480, CByteImage::eRGB24);
				m_pTempImageHeader->width = 640;
				m_pTempImageHeader->height = 480;
				break;
		}
	}
	else
	{
		// mode not supported
		return false;
	}
	
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

void CLinux1394Capture::ConvertYUV411(CByteImage* pInput, CByteImage* pOutput)
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

void CLinux1394Capture::YUVToRGB(int y, int u, int v, unsigned char* output)
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

bool CLinux1394Capture::CaptureImage(CByteImage **ppImages)
{
	if ( (width == -1) || (height == -1) )
		return false;

	int i;

	// first tell dc1394 that buffers can be reused
	for (i = 0; i < m_nCameras; i++)
	{
		if (!m_handles[m_nOpenedCameras[i]])
			return false;
		
		dc1394_dma_done_with_buffer(&m_cameras[m_nOpenedCameras[i]]);
	}

	for (i = 0; i < m_nCameras; i++)
	{
		dc1394_dma_single_capture(&m_cameras[m_nOpenedCameras[i]]);
	}

	for (i = 0; i < m_nCameras; i++)
	{
		m_pTempImageHeader->pixels = (unsigned char *) m_cameras[m_nOpenedCameras[i]].capture_buffer;
		
		if (m_mode == e640x480)
		{
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
		}
		else if (m_mode == e320x240)
		{
			switch (m_colorMode)
			{
				case eBayerPatternToRGB24:
					ImageProcessor::ConvertBayerPattern(m_pTempImageHeader, m_pTempImage, m_bayerPatternType);
					ImageProcessor::Resize(m_pTempImage, ppImages[i]);
				break;
				
				case eGrayScale:
				case eRGB24:
					ImageProcessor::Resize(m_pTempImageHeader, ppImages[i]);
				break;
				case eYUV411ToRGB24:
					ConvertYUV411(m_pTempImageHeader, m_pTempImage);
					ImageProcessor::Resize(m_pTempImage, ppImages[i]);
				break;
			}
		}
	}
	
	return true;
}

bool CLinux1394Capture::CaptureBayerPatternImage(CByteImage **ppImages)
{
	if ( (width == -1) || (height == -1) )
		return false;

	int i;

	// first tell dc1394 that buffers can be reused
	for (i = 0; i < m_nCameras; i++)
	{
		if (!m_handles[m_nOpenedCameras[i]])
			return false;
		
		dc1394_dma_done_with_buffer(&m_cameras[m_nOpenedCameras[i]]);
	}

	for (i = 0; i < m_nCameras; i++)
	{
		dc1394_dma_single_capture(&m_cameras[m_nOpenedCameras[i]]);
	}

	for (i = 0; i < m_nCameras; i++)
	{
		m_pTempImageHeader->pixels = (unsigned char *) m_cameras[m_nOpenedCameras[i]].capture_buffer;
		
		if (m_mode == e640x480)
		{
			ImageProcessor::CopyImage(m_pTempImageHeader, ppImages[i]);
		}
		else if (m_mode == e320x240)
		{
			ImageProcessor::Resize(m_pTempImageHeader, ppImages[i]);
		}
	}
	
	return true;
}

CByteImage::ImageType CLinux1394Capture::GetType()
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

void CLinux1394Capture::SetGain(unsigned int nValue)
{
	for (int i = 0; i < m_nCameras; i++)
	{
		if (nValue == -1)
		{
			dc1394_auto_on_off(m_handles[i], m_cameras[i].node, FEATURE_GAIN, 1);
		}
		else
		{
			dc1394_auto_on_off(m_handles[i], m_cameras[i].node, FEATURE_GAIN, 0);
			dc1394_set_gain(m_handles[i], m_cameras[i].node, nValue);
		}
	}
}

void CLinux1394Capture::SetExposure(unsigned int nValue)
{
	for (int i = 0; i < m_nCameras; i++)
	{
		if (nValue == -1)
		{
			dc1394_auto_on_off(m_handles[i], m_cameras[i].node, FEATURE_EXPOSURE, 1);
		}
		else
		{
			dc1394_auto_on_off(m_handles[i], m_cameras[i].node, FEATURE_EXPOSURE, 0);
			dc1394_set_exposure(m_handles[i], m_cameras[i].node, nValue);
		}
	}
}

void CLinux1394Capture::SetShutter(unsigned int nValue)
{
	for (int i = 0; i < m_nCameras; i++)
	{
		if (nValue == -1)
		{
			dc1394_auto_on_off(m_handles[i], m_cameras[i].node, FEATURE_SHUTTER, 1);
		}
		else
		{
			dc1394_auto_on_off(m_handles[i], m_cameras[i].node, FEATURE_SHUTTER, 0);
			dc1394_set_shutter(m_handles[i], m_cameras[i].node, nValue);
		}
	}
}

int CLinux1394Capture::GetDCFrameRateMode(FrameRate frameRate)
{
	switch(frameRate)
	{
		case e60fps: return FRAMERATE_60;
		case e30fps: return FRAMERATE_30;
		case e15fps: return FRAMERATE_15;
		case e7_5fps: return FRAMERATE_7_5;
		case e3_75fps: return FRAMERATE_3_75;
		case e1_875fps: return FRAMERATE_1_875;

		default: return FRAMERATE_30;
	};
}

bool CLinux1394Capture::ListCameras()
{
	// already listed
	if(m_nOverallCameras != 0)
		return true;

	// first: count cameras
	raw1394handle_t raw_handle = raw1394_new_handle();
	if (!raw_handle)
	{
		printf("error: unable to aquire a raw1394 handle\n");
		return false;
	}
	
	raw1394_portinfo ports[MAX_PORTS];
	m_nPorts = raw1394_get_port_info(raw_handle, ports, m_nPorts);
	raw1394_destroy_handle(raw_handle);
	printf("number of ports = %d\n", m_nPorts);

	// build info for all cameras
	for (int p = 0; p < m_nPorts; p++)
	{
		int camCount;
		
		// get the camera nodes and describe them as we find them
		raw_handle = raw1394_new_handle();
		raw1394_set_port(raw_handle, p);
		m_pCameraNodes[p] = dc1394_get_camera_nodes(raw_handle, &camCount, 1);
		raw1394_destroy_handle(raw_handle);
		
		// check cameras and retrieve info for capture
		for (int i = 0; i < camCount; i++)
		{
			m_handles[m_nOverallCameras] = dc1394_create_handle(p);
			if (!m_handles[m_nOverallCameras])
			{
				printf("error: unable to aquire a raw1394 handle\n");
				CloseCamera();
				return false;
			}

			m_cameras[m_nOverallCameras].node = m_pCameraNodes[p][i];
			dc1394_feature_set features;
	
			if (dc1394_get_camera_feature_set(m_handles[m_nOverallCameras], m_cameras[m_nOverallCameras].node, &features) != DC1394_SUCCESS) 
			{
				printf("info: unable to get feature set\n");
			}
			else
			{
				//dc1394_print_feature_set(&m_features);
			}
			
			unsigned int channel, speed;
		
			if (dc1394_get_iso_channel_and_speed(m_handles[m_nOverallCameras], m_cameras[m_nOverallCameras].node, &channel, &speed) != DC1394_SUCCESS) 
			{
				printf("error: unable to get the iso channel number\n");
				CloseCamera();
				return false;
			}

			// retrieve camera info
			dc1394_camerainfo info;

			if (dc1394_get_camera_info(m_handles[m_nOverallCameras], m_cameras[m_nOverallCameras].node, &info) != DC1394_SUCCESS) 
			{
				printf("error: unable to retrieve camera info\n");
				CloseCamera();	
				return false;		
			} else {
				m_CameraInfo[m_nOverallCameras].nPort = p;
				m_CameraInfo[m_nOverallCameras].nIndex = i;
				unsigned int low = info.euid_64;
				unsigned int high = (info.euid_64 >> 32);
				m_CameraInfo[m_nOverallCameras].sUID = CamUIDToString(low,high);
				m_nOverallCameras++;

				if(m_nOverallCameras > MAX_CAMERAS)
				{
					printf("error: found more cameras than MAX_CAMERAS\n");
					CloseCamera();
					return false;
				}
			}
		}
	}

	return true;
}

std::string CLinux1394Capture::CamUIDToString(unsigned int nLow, unsigned int nHigh)
{
	std::string UID = "";
	char szUID[17];
	sprintf(szUID,"%08X%08X\0",nHigh,nLow);
	UID += szUID;
	
	return UID;
}
