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
// Filename:  Linux1394Capture.h
// Author:    Pedram Azad, Kai Welke
// Date:      2005
// ****************************************************************************
// Changes:   24.08.2006, Kai Welke
//            * Added YUV411 support
//            25.09.2006, Kai Welke
//            * Added support for UIDs and different frame rates
// ****************************************************************************


#ifndef _LINUX_1394_CAPTURE_H_
#define _LINUX_1394_CAPTURE_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/VideoCaptureInterface.h"
#include "Image/ImageProcessor.h"
#include <string>

#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>



// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;


// ****************************************************************************
// Defines
// ****************************************************************************

#define MAX_CAMERAS 4 // don't set this smaller than 2
#define MAX_PORTS 4




// ****************************************************************************
// CLinux1394Capture
// ****************************************************************************

class CLinux1394Capture : public CVideoCaptureInterface
{
public:
	// constructor, variable arguments are camera uids as const char*
	CLinux1394Capture(int nCameras, VideoMode mode, ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType = ImageProcessor::eBayerRG, FrameRate frameRate = e30fps); 
	CLinux1394Capture(VideoMode mode, ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType = ImageProcessor::eBayerRG, FrameRate frameRate = e30fps, int nNumberUIDs = 0, ...);
	
	// format 7 modes
	CLinux1394Capture(int nCameras, VideoMode mode, int nFormat7PacketSize = -1, int nFormat7MinX = 0, int nFormat7MinY = 0, int nFormat7Width = 640, int nFormat7Height = 480,
	ColorMode colorMode = CVideoCaptureInterface::eBayerPatternToRGB24, ImageProcessor::BayerPatternType bayerPatternType = ImageProcessor::eBayerRG);
	
	CLinux1394Capture(VideoMode mode, int nFormat7PacketSize = -1, int nFormat7MinX = 0, int nFormat7MinY = 0, int nFormat7Width = 640, int nFormat7Height = 480,
	ColorMode colorMode = CVideoCaptureInterface::eBayerPatternToRGB24, ImageProcessor::BayerPatternType bayerPatternType = ImageProcessor::eBayerRG, int nNumberUIDs = 0, ...);
	
	
	// destructor
	~CLinux1394Capture();


	// public methods
	bool OpenCamera();
	void CloseCamera();
	bool CaptureImage(CByteImage **ppImages);
	bool CaptureBayerPatternImage(CByteImage **ppImages);
        
	void SetGain(unsigned int nValue);
	void SetExposure(unsigned int nValue);
	void SetShutter(unsigned int nValue);

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	CByteImage::ImageType GetType();
	int GetNumberOfCameras() { return m_nCameras; }

	
private:
	struct TCameraInfo
	{
		int	nPort;
		int	nIndex;
		std::string sUID;
	};
	
	// private methods
	bool OpenCamera(int nCamera);
	bool InitCameraMode();
	int GetDCFrameRateMode(FrameRate frameRate);

	// conversion
	void ConvertYUV411(CByteImage* pInput, CByteImage* pOutput);
	void YUVToRGB(int y, int u, int v, unsigned char* output);

	// multiple camera handling
	bool ListCameras();
	std::string CamUIDToString(unsigned int nLow, unsigned int nHigh);
	
	// private attributes
	// temporary images for capruting
	CByteImage *m_pTempImageHeader;
	CByteImage *m_pTempImage;
	
	int m_nPorts;
	int m_nMode;

	int width;
	int height;
	
	// number of cameras requested by user
	int m_nCameras;
	// requested video mode (see VideoCaptureInterface.h)
	const VideoMode m_mode;
	// requested color mode (see VideoCaptureInterface.h)
	const ColorMode m_colorMode;
	// requested frame rate (see VideoCaptureInterface.h)
	FrameRate m_frameRate;
	// requested bayer pattern type (see ImageProcessor.h)
	const ImageProcessor::BayerPatternType m_bayerPatternType;
	// unique camera ids as requested by user
	std::string m_sCameraUID[MAX_CAMERAS];
	bool m_bUseUIDs;
	// opened camera ids
	int m_nOpenedCameras[MAX_CAMERAS];
	// video mode
	int m_video_mode;
	// format 7 mode
	bool m_bFormat7Mode;
	int m_nFormat7PacketSize;
	int m_nFormat7MinX;
	int m_nFormat7MinY;
	int m_nFormat7Width;
	int m_nFormat7Height;
	
	// static for all instances
	// lib dc + raw specific camera data
	static raw1394handle_t m_handles[MAX_CAMERAS];
	static dc1394_cameracapture m_cameras[MAX_CAMERAS];
	static nodeid_t *m_pCameraNodes[MAX_PORTS];

	// internal camera data
	static int m_nOverallCameras;
	static TCameraInfo m_CameraInfo[MAX_CAMERAS];
	static CLinux1394Capture *m_pCameraOpener[MAX_CAMERAS];

	// static instance counter for handle destruction
	static int m_nInstances;
};



#endif /* _LINUX_1394_CAPTURE_H_ */
