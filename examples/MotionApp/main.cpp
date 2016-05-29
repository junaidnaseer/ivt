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
// Filename:  main.cpp
// Author:    Pedram Azad
// Date:      29.09.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#if defined WIN32
#include "VideoCapture/VFWCapture.h"
#elif defined __APPLE__
#include "VideoCapture/QuicktimeCapture.h"
#else
#include "VideoCapture/Linux1394Capture2.h"
#endif

#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"

#include "Interfaces/ApplicationHandlerInterface.h"
#include "Interfaces/MainWindowInterface.h"
#include "gui/GUIFactory.h"

#include <stdio.h>



// ****************************************************************************
// main
// ****************************************************************************

int main()
{
	// create capture object
	#if defined WIN32
	CVFWCapture capture(0);
	#elif defined __APPLE__
	CQuicktimeCapture capture(CVideoCaptureInterface::e640x480);
	#else
	CLinux1394Capture2 capture(-1, CVideoCaptureInterface::e640x480, CVideoCaptureInterface::eRGB24)
	#endif

	// open camera
	if (!capture.OpenCamera())
	{
		printf("error: could not open camera\n");
		return 1;
	}
	
	const int width = capture.GetWidth();
	const int height = capture.GetHeight();
	const int nPixels = width * height;

	CByteImage image(width, height, CByteImage::eRGB24);
	CByteImage grayImage(width, height, CByteImage::eGrayScale);
	CByteImage lastImage(&grayImage);
	
	CByteImage *pImage = &image;
	
	// create the application handler
	CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
	pApplicationHandler->Reset();
	
	// create a window
	CMainWindowInterface *pMainWindow = CreateMainWindow(0, 0, 2 * width, height, "Motion Segmentation");
	WIDGET_HANDLE pImageWidget1 = pMainWindow->AddImage(0, 0, width, height);
	WIDGET_HANDLE pImageWidget2 = pMainWindow->AddImage(width, 0, width, height);
	
	pMainWindow->Show();
	
	while (!pApplicationHandler->ProcessEventsAndGetExit())
	{
		capture.CaptureImage(&pImage);
		
		ImageProcessor::ConvertImage(&image, &grayImage, true);
		
		// simple motion detection
		ImageProcessor::AbsoluteDifference(&grayImage, &lastImage, &lastImage);
		
		ImageProcessor::ThresholdBinarize(&lastImage, &lastImage, 20);
		
		pMainWindow->SetImage(pImageWidget1, &image);
		pMainWindow->SetImage(pImageWidget2, &lastImage);
		
		ImageProcessor::CopyImage(&grayImage, &lastImage);
	}
	
	delete pMainWindow;
	delete pApplicationHandler;
	
	return 0;
}
