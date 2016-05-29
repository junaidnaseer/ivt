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
// Filename:  OpenGLCapture.h
// Author:    Pedram Azad
// Date:      27.07.2008
// ****************************************************************************

#ifndef _OPENGL_CAPTURE_H_
#define _OPENGL_CAPTURE_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/VideoCaptureInterface.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CStereoCalibration;
class CCalibration;
class COpenGLVisualizer;
class CFloatMatrix;
class CByteImage;



// ****************************************************************************
// COpenGLCapture
// ****************************************************************************

class COpenGLCapture : public CVideoCaptureInterface
{
public:
	// constructors
	COpenGLCapture();
	COpenGLCapture(const CCalibration &calibration);
	COpenGLCapture(const CStereoCalibration &stereoCalibration);

	// destructor
	~COpenGLCapture();


	// public methods
	bool OpenCamera();
	void CloseCamera();
	bool CaptureImage(CByteImage **ppImages);
	
	int GetWidth() { return width; }
	int GetHeight() { return height; }
	CByteImage::ImageType GetType() { return type; }
	int GetNumberOfCameras() { return m_nCameras; }

	// additional set methods (if using standard constructor)
	void Set(const CCalibration &calibration);
	void Set(const CStereoCalibration &stereoCalibration);


protected:
	// virtual draw method
	virtual void DrawScene() = 0;
	
	COpenGLVisualizer *m_pOpenGLVisualizer;


private:
	// private attributes
	CStereoCalibration *m_pStereoCalibration;
	
	CCalibration *m_pCalibration;
	int m_nCameras;
	int width, height;
	CByteImage::ImageType type;
};



#endif /* _OPENGL_CAPTURE_H_ */
