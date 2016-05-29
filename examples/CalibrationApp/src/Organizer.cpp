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
// Filename:  Organizer.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************

#if defined WIN32
#include "VideoCapture/VFWCapture.h"
#elif defined __APPLE__
#include "VideoCapture/QuickTimeCapture.h"
#else
#include "VideoCapture/Linux1394Capture2.h"
#endif

#include "Organizer.h"
#include "OpenCVCalibrationAlgorithm.h"
#include "Math/Math3d.h"

#include "Interfaces/ApplicationHandlerInterface.h"
#include "Interfaces/MainWindowInterface.h"
#include "gui/GUIFactory.h"

#include <string>
#include <stdio.h>



#define CAMERA_CALIBRATION_PARAMETER_FILENAME	"cameras.txt"

// for calibration
// note: NUMBER_OF_ROWS and NUMBER_OF_COLUMNS refer to the number of squares per row
//       repsectively column in the chessboard pattern. The effective number of
//       corners is then (NUMBER_OF_ROWS - 1) * (NUMBER_OF_COLUMNS - 1)
#define NUMBER_OF_IMAGES				20
#define NUMBER_OF_ROWS					7
#define NUMBER_OF_COLUMNS				9
#define SQUARE_SIZE_IN_MM				36.3



int COrganizer::Run(CVideoCaptureInterface::VideoMode mode)
{
	#if defined WIN32
	CVFWCapture capture(0);
	#elif defined __APPLE__
	CQuicktimeCapture capture(CVideoCaptureInterface::e640x480);
	#else
	CLinux1394Capture2 capture(-1, CVideoCaptureInterface::e640x480, CVideoCaptureInterface::eRGB24);
	#endif
	
	if (!capture.OpenCamera())
	{
		printf("error: could not open camera\n");
		return 1;
	}

	const int width = capture.GetWidth();
	const int height = capture.GetHeight();
	const int nCameras = capture.GetNumberOfCameras();

	printf("resolution is %ix%i\n", width, height);
	printf("%i camera(s) found\n\n", nCameras);

	if (width == -1 || height == -1)
	{
		printf("error: invalid resolution\nexiting...\n");
		return 1;
	}
	
	COpenCVCalibrationAlgorithm calibration_algorithm(nCameras, NUMBER_OF_IMAGES,
		NUMBER_OF_ROWS, NUMBER_OF_COLUMNS, SQUARE_SIZE_IN_MM,
		CAMERA_CALIBRATION_PARAMETER_FILENAME, &capture);
	
	CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
	pApplicationHandler->Reset();
	
	CMainWindowInterface *pMainWindow = CreateMainWindow(0, 0, (nCameras == 2 ? 2*width : width), height, "Calibration Application");
	WIDGET_HANDLE pLeftImageWidget = pMainWindow->AddImage(0, 0, width, height);
	
	WIDGET_HANDLE pRightImageWidget = 0;
	if (nCameras == 2)
	{
		pRightImageWidget = pMainWindow->AddImage(width, 0, width, height);
	}
	
	pMainWindow->Show();
	
	calibration_algorithm.SetGUI(pApplicationHandler, pMainWindow, pLeftImageWidget, pRightImageWidget);

	bool bRet = calibration_algorithm.Calibrate();
	
	delete pMainWindow;
	delete pApplicationHandler;
		
	return bRet ? 0 : 1;
}
