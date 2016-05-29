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
// Author:    Florian Hecht
// Date:      03.11.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "Interfaces/ApplicationHandlerInterface.h"
#include "gui/GUIFactory.h"
#include "Interfaces/MainWindowInterface.h"
#include "Interfaces/MainWindowEventInterface.h"
#include "VideoCapture/BitmapCapture.h"

#include "Image/ByteImage.h"
#include "Image/ImageProcessor.h"
#include "Calibration/Undistortion.h"
#include "Calibration/Rectification.h"

#include "Helpers/helpers.h"

#include <stdio.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define DEMO_IMAGE_LEFT "../../files/distorted_left.bmp"
#define DEMO_IMAGE_RIGHT "../../files/distorted_right.bmp"
#define DEMO_CAMERA_CALIBRATION "../../files/camera_armar_640x480_4mm.txt"



// ****************************************************************************
// CUnidistortionRectificationDemo
// ****************************************************************************

class CUnidistortionRectificationDemo : public CMainWindowEventInterface
{
public:
	// constructor
	CUnidistortionRectificationDemo() : m_bUndistort(true), m_bRectify(false)
	{
	}

	// this is called when the value of one of the checkboxes is changed
	void ValueChanged(WIDGET_HANDLE pWidget, int nValue)
	{
		if (pWidget == m_pCheckBox1)
			m_bUndistort = nValue != 0;
		else if (pWidget == m_pCheckBox2)
			m_bRectify = nValue != 0;
	}

	// init application and run
	int Run()
	{
		CBitmapCapture capture(DEMO_IMAGE_LEFT, DEMO_IMAGE_RIGHT);
		if (!capture.OpenCamera())
		{
			printf("error: could not open camera\n");
			return 1;
		}
		
		const int width = capture.GetWidth();
		const int height = capture.GetHeight();

		// create images for the image processing
		CByteImage *ppImages[2] = { new CByteImage(width, height, capture.GetType()), new CByteImage(width, height, capture.GetType()) };
		CByteImage *ppResultImages[2] = { new CByteImage(width, height, capture.GetType()), new CByteImage(width, height, capture.GetType()) };

		// create and initialize the undistortion
		CUndistortion undistortion;
		if (!undistortion.Init(DEMO_CAMERA_CALIBRATION))
		{
			printf("error: could not load camera parameters\n");
			return 1;
		}
		
		// create and initialize the rectification
		CRectification rectification;
		if (!rectification.Init(DEMO_CAMERA_CALIBRATION))
		{
			printf("error: could not load camera parameters\n");
			return 1;
		}
		
		// create and initialize the rectification without undistortion
		CRectification rectification2(true, false);
		if (!rectification2.Init(DEMO_CAMERA_CALIBRATION))
		{
			printf("error: could not load camera parameters\n");
			return 1;
		}
		
		
		// create an application handler
		CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
		pApplicationHandler->Reset();

		// create a main window
		m_pMainWindow = CreateMainWindow(0, 0, 2*width, height + 60, "UndistortionRectificationDemo");

		// events are sent to this class, hence this class needs to have the CMainWindowEventInterface
		m_pMainWindow->SetEventCallback(this);

		// create two image widgets to display the two images
		WIDGET_HANDLE pImageWidgetLeft = m_pMainWindow->AddImage(0, 60, width, height);
		WIDGET_HANDLE pImageWidgetRight = m_pMainWindow->AddImage(width, 60, width, height);

		// add a checkbox to use undistortion
		m_pCheckBox1 = m_pMainWindow->AddCheckBox(10, 10, 100, 30, "Undistort", m_bUndistort);
		
		// add a checkbox to use rectification
		m_pCheckBox2 = m_pMainWindow->AddCheckBox(120, 10, 100, 30, "Rectify", m_bRectify);
		
		// add labels to display processing stats
		WIDGET_HANDLE pLabel1 = m_pMainWindow->AddLabel(1200, 10, 70, 20, "666 ms");
		WIDGET_HANDLE pLabel2 = m_pMainWindow->AddLabel(1200, 30, 70, 20, "666 fps");

		// make the window visible
		m_pMainWindow->Show();
		
		
		char buffer[1024];

		// main loop
		while (!pApplicationHandler->ProcessEventsAndGetExit())
		{
			if (!capture.CaptureImage(ppImages))
				break;
			
			const unsigned int t1 = get_timer_value(true);
			
			if (m_bUndistort)
			{
				if (m_bRectify)
				{
					// rectification (with undistortion)
					rectification.Rectify(ppImages, ppResultImages);
				}
				else
				{
					// undistortion
					undistortion.Undistort(ppImages, ppResultImages);
				}
			}
			else
			{
				if (m_bRectify)
				{
					// rectification without undistortion
					rectification2.Rectify(ppImages, ppResultImages);
				}
				else
				{
					// simply copy the images
					ImageProcessor::CopyImage(ppImages[0], ppResultImages[0]);
					ImageProcessor::CopyImage(ppImages[1], ppResultImages[1]);
				}
			}
			
			const unsigned int t2 = get_timer_value() - t1;

			// display the speed stats
			sprintf(buffer, "%.2f ms", t2 / 1000.0f);
			m_pMainWindow->SetText(pLabel1, buffer);
			sprintf(buffer, "%.2f fps", 1000000.0f / t2);
			m_pMainWindow->SetText(pLabel2, buffer);

			// display the images
			m_pMainWindow->SetImage(pImageWidgetLeft, ppResultImages[0]);
			m_pMainWindow->SetImage(pImageWidgetRight, ppResultImages[1]);
		}
		
		delete ppImages[0];
		delete ppImages[1];
		delete ppResultImages[0];
		delete ppResultImages[1];
		
		delete m_pMainWindow;
		delete pApplicationHandler;
		
		return 0;
	}


private:
	// private attributes
	CMainWindowInterface *m_pMainWindow;
	WIDGET_HANDLE m_pCheckBox1;
	WIDGET_HANDLE m_pCheckBox2;
	
	bool m_bUndistort;
	bool m_bRectify;
};


int main()
{
	CUnidistortionRectificationDemo demo;
	return demo.Run();
}
