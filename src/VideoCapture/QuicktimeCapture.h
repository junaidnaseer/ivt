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
// Filename:  QuicktimeCapture.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _QUICKTIME_CAPTURE_H
#define _QUICKTIME_CAPTURE_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/VideoCaptureInterface.h"
#include <QuickTime/QuickTimeComponents.h>
#include <string>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;



// ****************************************************************************
// CQuicktimeCapture
// ****************************************************************************

class CQuicktimeCapture : public CVideoCaptureInterface
{
public:
  	// constructor
	CQuicktimeCapture(VideoMode mode, const char *pComponentName = 0);
	
	// destructor
	~CQuicktimeCapture();
	

	// virtual methods
	bool OpenCamera();
	void CloseCamera();
	bool CaptureImage(CByteImage **ppImages);
	
	int GetWidth() { return width; }
	int GetHeight() { return height; }
	CByteImage::ImageType GetType() { return CByteImage::eRGB24; }
	int GetNumberOfCameras() { return 1; }


private:
	// private methods
	static pascal OSErr sDataProc(SGChannel c, Ptr p, long len, long *offset, long chRefCon, TimeValue time, short writeType, long refcon);
	OSErr dataProc(SGChannel c, Ptr p, long length);
	
	// private attributes
	SeqGrabComponent grabber;
	SGChannel videoChannel;
	GWorldPtr gWorld;
	ImageDescriptionHandle imgDesc;
	
	long m_lImageLength;
	char *m_pBuffer;
	bool m_bImagePending;
	
	const VideoMode m_mode;
	int width, height;

	std::string m_sComponentName;
	Rect srcRect;
};



#endif /* QUICKTIME_CAPTURE_H_ */
