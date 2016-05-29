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
// Filename:  DragonFlyCapture.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


#ifndef _DRAGON_FLY_CAPTURE_H_
#define _DRAGON_FLY_CAPTURE_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/VideoCaptureInterface.h"
#include "Image/ImageProcessor.h"
#include <stdio.h>
#include <pgrflycapture.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define DRAGONFLY_MAX_CAMERAS		10


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;



// ****************************************************************************
// CDragonFlyCapture
// ****************************************************************************

class CDragonFlyCapture : public CVideoCaptureInterface
{
public:
	// constructor
	CDragonFlyCapture(int nCameras, VideoMode mode, ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType);

	//! Constructor that automatically detects the number of cameras
	/*! This constructor finds all the cameras on the machine.
	 * \param mode is the Video Mode
	 * \param colorMode is the color mode of the images to capture.
	 * \param bayerPatternType is the pattern type of the camera's CCD.
	 */
	CDragonFlyCapture(VideoMode mode, ColorMode colorMode, ImageProcessor::BayerPatternType bayerPatternType);

	// destructor
	~CDragonFlyCapture();


	// public methods
	bool OpenCamera();
	void CloseCamera();
	bool CaptureImage(CByteImage **ppImages);

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	CByteImage::ImageType GetType();
	int GetNumberOfCameras() { return m_nCameras; }

	void GetCurrentTimestamp(unsigned int &sec, unsigned int &usec);

	//! Get the serial number from the camera.
	/*! Get the serial number from the camera to identify it.
	 *  \param nCamera is the number of the camera to identify.
	 *  \return the serial number of the camera or 0 if
	 *   there is no camera.
	 */
	unsigned int GetSerialFromCamera(int nCamera);

	//! Swap the camera source.
	/*! Swap the camera source from two different sources.
	 *  Useful when one wants to use a stereo system.
	 *  \param nCamera1 camera 1 to swap
	 *  \param nCamera2 camera 2 to swap
	 */
	void SwapCameras(int nCamera1, int nCamera2);


private:
	// private attributes
	CByteImage *m_ppImages[DRAGONFLY_MAX_CAMERAS];
	CByteImage *m_ppOriginalImages[DRAGONFLY_MAX_CAMERAS];
	FlyCaptureImage *m_ppInternalImages[DRAGONFLY_MAX_CAMERAS];

	//!  Context pointer for the PGRFlyCapture library.
	FlyCaptureContext m_flyCaptureContext[DRAGONFLY_MAX_CAMERAS];
	//! Information from the camera.
	/*! This structure stores a variety of different pieces of information
	 *  associated with a particular camera. It is used with the
	 *  flycaptureBusEnumerateCamerasEx() method.
	 *  This structure has replaced FlyCaptureInfo.
	 */
	FlyCaptureInfoEx m_flyCaptureInfoEx;

	int m_nCameras;
	const VideoMode m_mode;
	const ColorMode m_colorMode;
	const ImageProcessor::BayerPatternType m_bayerPatternType;

	int width;
	int height;

	unsigned int m_sec;
	unsigned int m_usec;
};



#endif /* _DRAGON_FLY_CAPTURE_H_ */
