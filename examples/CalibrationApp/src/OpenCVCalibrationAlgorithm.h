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
// Filename:  OpenCVCalibrationAlgorithm.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


#ifndef _OPEN_CV_CALIBRATION_ALGORITHM_H_
#define _OPEN_CV_CALIBRATION_ALGORITHM_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Math/Math3d.h"
#include <cv.h>
#include <string>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CvCalibFilter;
class CVideoCaptureInterface;
class CApplicationHandlerInterface;
class CGUIInterface;
class CByteImage;
class CMainWindowInterface;


// ****************************************************************************
// Typedefs
// ****************************************************************************

typedef void* WIDGET_HANDLE;



// ****************************************************************************
// COpenCVCalibrationAlgorithm
// ****************************************************************************

class COpenCVCalibrationAlgorithm
{
public:
	// constructor
	COpenCVCalibrationAlgorithm(int nCameras, int nImages, int nRows, int nColumns, double dSquareSizeInMM,
		const char *pCameraParameterFileName, CVideoCaptureInterface *pVideoCapture);

	// destructor
	~COpenCVCalibrationAlgorithm();

    
    void SetGUI(CApplicationHandlerInterface *pApplicationHandlerInterface, CMainWindowInterface *pMainWindow, WIDGET_HANDLE pLeftImageWidget, WIDGET_HANDLE pRightImageWidget = 0)
	{
		m_pApplicationHandlerInterface = pApplicationHandlerInterface;
		m_pMainWindow = pMainWindow;
		m_pLeftImageWidget = pLeftImageWidget;
		m_pRightImageWidget = pRightImageWidget;
	}

	// calibration methods
	bool Calibrate();


private:
	// private attributes
	CvCalibFilter *m_pCalibFilter;
	CVideoCaptureInterface *m_pVideoCapture;
	CApplicationHandlerInterface *m_pApplicationHandlerInterface;
	CMainWindowInterface *m_pMainWindow;
	WIDGET_HANDLE m_pLeftImageWidget;
	WIDGET_HANDLE m_pRightImageWidget;

	// number of cameras: 1 (monocular) or 2 (stereo)
	int m_nCameras;

	CvPoint2D32f *m_pCorners2DFloat;
	CvPoint2D64d *m_pCorners2D;
	CvPoint3D64d *m_pCorners3D;
	
	CByteImage *m_ppImages[2];
	CByteImage *m_ppCameraImages[2];

	std::string m_sCameraParameterFileName;
};



#endif /* _OPEN_CV_CALIBRATION_ALGORITHM_H_ */
