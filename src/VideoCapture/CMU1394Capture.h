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
// Filename:  CMU1394Capture.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************
// Changes:   01.02.2006, Florian Hecht
//            * Added support for multiple cameras
//            14.06.2010, Kenn Sebesta
//            * Added methods SetGain, SetShutter, and SetFeature
// ****************************************************************************

#ifndef _CMU_1394_CAPTURE_H_
#define _CMU_1394_CAPTURE_H_


// ****************************************************************************
// Defines
// ****************************************************************************

// comment out this define for older versions of CMU1394
#define CMU1394_Version_645


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/VideoCaptureInterface.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class C1394Camera;
class CByteImage;



// ****************************************************************************
// CCMU1394Capture
// ****************************************************************************

class CCMU1394Capture : public CVideoCaptureInterface
{
public:
	// constructor
	CCMU1394Capture(int nFormat = -1, int nMode = -1, int nRate = -1, int nCameras = 1);

	// destructor
	~CCMU1394Capture();
	

	// public methods
	bool OpenCamera();
	void CloseCamera();
	bool CaptureImage(CByteImage **ppImages);
	
	int GetWidth() { return width; }
	int GetHeight() { return height; }
	CByteImage::ImageType GetType() { return CByteImage::eRGB24; }
	int GetNumberOfCameras();
	
	// non-interface methods
	// nCamera = -1 sets all cameras, nCamera = 0 first camera, nCamera = 1 second camera, and so on.
#ifndef CMU1394_Version_645
	void SetFocus(int nFocus, int nCamera = -1);
#else
	void SetShutter(int nShutter, int nCamera = -1);
	void SetGain(int nGain, int nCamera = -1);
	void SetFeature(int nFeature, int nValue, int nCamera = -1);
#endif

private:
	// private attributes
	int m_nNumberOfCameras;
	C1394Camera **m_ppCameras;

	// private methods
	int m_nFormat;
	int m_nMode;
	int m_nRate;

	int width;
	int height;

	bool m_bOK;
};



#endif /* _CMU_1394_CAPTURE_H_ */
