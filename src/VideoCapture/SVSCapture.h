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
// Filename:  SVSCapture.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _SVS_CAMERA_H_
#define _SVS_CAMERA_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/VideoCaptureInterface.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;
class svsVideoImages;



// ****************************************************************************
// CSVSCapture
// ****************************************************************************

class CSVSCapture : public CVideoCaptureInterface
{
public:
	// consructor
	CSVSCapture(VideoMode, int nIndex = 0);
	
	// destructor
	~CSVSCapture();


	// public methods
	bool OpenCamera();
	void CloseCamera();
	bool CaptureImage(CByteImage **ppImages);
	
	int GetWidth() { return width; }
	int GetHeight() { return height; }
	CByteImage::ImageType GetType() { return (m_bColor)?CByteImage::eRGB24:CByteImage::eGrayScale; }
	int GetNumberOfCameras() { return 2; }

	// svs camera (use these methods after camera has been opened)
	void SetRed(int val);
	void SetBlue(int val);
	void SetExposure(int val);
	void SetGain(int val);
	void SetRectify(bool bRectify);
	void SetColor(bool bColor);
	
  
private:
	// private attributes
	svsVideoImages *svs_video;
	
	bool m_bColor;
	int m_nBytesPerPixel;
	int width, height;

	int m_nIndex;
	VideoMode m_mode;
};



#endif /* _SVS_CAMERA_H_ */
