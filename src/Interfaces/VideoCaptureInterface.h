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
// Filename:  VideoCaptureInterface.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************

/** \defgroup VideoCapture Video capture */

#ifndef _VIDEO_CAPTURE_INTERFACE_H_
#define _VIDEO_CAPTURE_INTERFACE_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Image/ByteImage.h"



// ****************************************************************************
// CVideoCaptureInterface
// ****************************************************************************

/*!
	\ingroup VideoCapture
	\brief Interface to video capture modules.
*/
class CVideoCaptureInterface
{
public:
	// enums
	enum VideoMode
	{
		e320x240,
		e640x480,
		e800x600,
		e768x576,
		e1024x768,
		e1280x960,
		e1600x1200,
		eNone
	};
	
	enum ColorMode
	{
		eRGB24,
		eBayerPatternToRGB24,
		eGrayScale,
		eYUV411ToRGB24
	};

	enum FrameRate
	{
		e60fps,
		e30fps,
		e15fps,
		e7_5fps,
		e3_75fps,
		e1_875fps
	};

	// destructor
	virtual ~CVideoCaptureInterface() { }

	// public methods
	virtual bool OpenCamera() = 0;
	virtual void CloseCamera() = 0;
	virtual bool CaptureImage(CByteImage **ppImages) = 0;

	virtual bool SetExposureTime(int nExposureInUS) { return false; }

	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual CByteImage::ImageType GetType() = 0;
	virtual int GetNumberOfCameras() = 0;
};



#endif /* _VIDEO_CAPTURE_INTERFACE_H_ */
