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
// Filename:  Linux1394Capture2.h
// Author:    Kai Welke, Pedram Azad
// Date:      2007
// ****************************************************************************
// Requires:  * libdc1394v2
//            * libraw1394 >= 1.2.0
// ****************************************************************************
// Changes:   20.12.2007. Kai Welke
//            * Built CLinux1394Capture2 on base of CLinux1394Capture
//            * switched from libdc1394v1 to libdc1394v2 (solved problem 
//              with QT and allowed to use full firewire bandwidth)
//            * added check for maximum bandwidth
//            * generic camera feature access
//            * Format7_0 implemented with ROI and framerate settings
//              format7 support is camera dependant (dragonfly only
//              supports two discrete framerates, dragonfly2 can be
//              adjusted from 0.x ... 60.0 frames.
// ****************************************************************************


#ifndef _LINUX_1394_CAPTURE_2_H_
#define _LINUX_1394_CAPTURE_2_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/VideoCaptureInterface.h"
#include "Image/ImageProcessor.h"
#include <string>
#include <vector>

#include <libraw1394/raw1394.h>
#include <dc1394/dc1394.h>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;


// ****************************************************************************
// Defines
// ****************************************************************************

// don't set this smaller than 2
#define MAX_CAMERAS 4 
// maximum bandwidth of firewire (in quadlets / cycle)
#define MAX_S400_BANDWIDTH 4915



// ****************************************************************************
// CLinux1394Capture2
// ****************************************************************************

class CLinux1394Capture2 : public CVideoCaptureInterface
{
public:
	// constructor, variable arguments are camera uids as const char*
	CLinux1394Capture2(int nCameras, VideoMode mode, ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType = ImageProcessor::eBayerRG, FrameRate frameRate = e30fps); 
	CLinux1394Capture2(VideoMode mode, ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType = ImageProcessor::eBayerRG, FrameRate frameRate = e30fps, int nNumberUIDs = 0, ...);
	
	// format 7 modes
	CLinux1394Capture2(int nCameras, VideoMode mode, float fFormat7FrameRate = 30.0f, int nFormat7MinX = 0, int nFormat7MinY = 0, int nFormat7Width = -1, int nFormat7Height = -1, ColorMode colorMode = CVideoCaptureInterface::eBayerPatternToRGB24, ImageProcessor::BayerPatternType bayerPatternType = ImageProcessor::eBayerRG);
	
	CLinux1394Capture2(VideoMode mode, float fFormat7FrameRate = 30.0f, int nFormat7MinX = 0, int nFormat7MinY = 0, int nFormat7Width = -1, int nFormat7Height = -1, ColorMode colorMode = CVideoCaptureInterface::eBayerPatternToRGB24, ImageProcessor::BayerPatternType bayerPatternType = ImageProcessor::eBayerRG, int nNumberUIDs = 0, ...);
	
	
	// destructor
	~CLinux1394Capture2();


	// public methods
	bool OpenCamera();
	void CloseCamera();
	bool CaptureImage(CByteImage **ppImages);
	bool CaptureBayerPatternImage(CByteImage **ppImages);
        
    // SetCameraUids can be used if the UIDs are determined dynamically
	void SetCameraUids(std::vector<std::string> uids);   
	void SetGain(int nValue);
	void SetExposure(int nValue);
	void SetShutter(int nValue);
	void SetWhiteBalance(int nU, int nV, int nCamera = -1);
	void SetTemperature(int nTemperature);
	void SetFeature(dc1394feature_t feature, std::string sName, int nValue);

	void ListFeatures();
	
	int GetWidth() { return width; }
	int GetHeight() { return height; }
	CByteImage::ImageType GetType();
	int GetNumberOfCameras() { return m_nCameras; }
	
	dc1394camera_t* GetCameraHandle(int Index) { return m_cameras[Index]; }

	VideoMode GetVideoMode() const {return m_mode;}
	FrameRate GetFrameRate() const {return m_frameRate;}

protected:
	// private methods
	void InitFirstInstance();
	void ExitLastInstance();
	void ResetAllCameras();
	
	bool OpenCamera(int nCamera);
	bool InitCameraMode();
	dc1394framerate_t GetDCFrameRateMode(FrameRate frameRate);

	// conversion
	void ConvertYUV411(CByteImage* pInput, CByteImage* pOutput);
	void YUVToRGB(int y, int u, int v, unsigned char* output);

	// multiple camera handling
	bool ListCameras();
	std::string CamUIDToString(uint64_t uid);
	
	// private attributes
	// temporary images for capruting
	CByteImage *m_pTempImageHeader;
	
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
	dc1394video_mode_t m_video_mode;
	// format 7 mode
	bool m_bFormat7Mode;
	float m_fFormat7FrameRate;
	int m_nFormat7MinX;
	int m_nFormat7MinY;
	int m_nFormat7Width;
	int m_nFormat7Height;
	
	// static for all instances
	// lib dc + raw specific camera data
	static dc1394_t*       m_pDC1394;
	static dc1394camera_t* m_cameras[MAX_CAMERAS];

	// internal camera data
	static int m_nOverallCameras;
	static int m_nRemainingBandwidth;
	static CLinux1394Capture2 *m_pCameraOpener[MAX_CAMERAS];
	static int m_nCameraBandwidth[MAX_CAMERAS];

	// static instance counter for handle destruction
	static int m_nInstances;
};



#endif /* _LINUX_1394_CAPTURE_2_H_ */
