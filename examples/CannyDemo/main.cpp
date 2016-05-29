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
// Date:      29.10.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "Interfaces/ApplicationHandlerInterface.h"
#include "Interfaces/MainWindowInterface.h"
#include "Interfaces/MainWindowEventInterface.h"
#include "VideoCapture/BitmapCapture.h"
#include "gui/GUIFactory.h"

#include "Image/ByteImage.h"
#include "Image/ImageProcessor.h"

#include "Helpers/helpers.h"

#include <stdio.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define DEMO_IMAGE "../../files/scene_left.bmp"



// ****************************************************************************
// CCannyDemo
// ****************************************************************************

class CCannyDemo : public CMainWindowEventInterface
{
public:
	// constructor
	CCannyDemo() : m_nCannyLowThreshold(80), m_nCannyHighThreshold(190), m_bShowOriginal(false)
	{
	}

	// this is called when the value of one of the sliders is changed
	void ValueChanged(WIDGET_HANDLE pWidget, int nValue) 
	{
		if (pWidget == m_pSlider1)
			m_nCannyLowThreshold = nValue;
		else if (pWidget == m_pSlider2)
			m_nCannyHighThreshold = nValue;
	}

	// this is called when a button is pushed
	void ButtonPushed(WIDGET_HANDLE pWidget)
	{
		if (pWidget == m_pButton)
		{
			m_bShowOriginal = ! m_bShowOriginal;

			if (m_bShowOriginal)
				m_pMainWindow->SetText(m_pButton, "Show Result");
			else
				m_pMainWindow->SetText(m_pButton, "Show Original");
		}
	}

	// init application and run
	int Run()
	{
		CBitmapCapture capture(DEMO_IMAGE);
		if (!capture.OpenCamera())
		{
			printf("error: could not open camera\n");
			return 1;
		}
		
		const int width = capture.GetWidth();
		const int height = capture.GetHeight();

		// create images
		CByteImage image(width, height, capture.GetType());
		CByteImage grayImage(width, height, CByteImage::eGrayScale);
		CByteImage *pImage = &image;

		// create an application handler
		CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
		pApplicationHandler->Reset();
		
		// create a main window
		m_pMainWindow = CreateMainWindow(0, 0, width, height + 100, "Canny Demo");

		// events are sent to this class, hence this class needs to have the CMainWindowEventInterface
		m_pMainWindow->SetEventCallback(this);

		// create an image widget to display a window
		WIDGET_HANDLE pImageWidget = m_pMainWindow->AddImage(0, 100, width, height);

		// add a label and a slider for the low threshold
		WIDGET_HANDLE pLabelCannyLow = m_pMainWindow->AddLabel(10, 10, 200, 30, "Canny low threshold: 0");
		m_pSlider1 = m_pMainWindow->AddSlider(10, 35, 200, 40, 0, 1020, 102, m_nCannyLowThreshold);

		// add a label and a slider for the high threshold
		WIDGET_HANDLE pLabelCannyHigh = m_pMainWindow->AddLabel(230, 10, 200, 30, "Canny high threshold: 0");
		m_pSlider2 = m_pMainWindow->AddSlider(230, 35, 200, 40, 0, 1020, 102, m_nCannyHighThreshold);
		
		// add a button to toggle between the original image and the processed one
		m_pButton = m_pMainWindow->AddButton(440, 30, 100, 40, "Show Original");

		// add a labels to display processing stats
		WIDGET_HANDLE pLabel3 = m_pMainWindow->AddLabel(560, 30, 70, 20, "666 ms");
		WIDGET_HANDLE pLabel4 = m_pMainWindow->AddLabel(560, 60, 70, 20, "666 fps");

		// make the window visible
		m_pMainWindow->Show();
		

		char buffer[1024];

		// main loop
		while (!pApplicationHandler->ProcessEventsAndGetExit())
		{
			if (!capture.CaptureImage(&pImage))
				break;
			
			// convert input image to grayscale image
			ImageProcessor::ConvertImage(&image, &grayImage, true);

			get_timer_value(true);

			// create a binary image from the gray scale image through m_nBinarizationThreshold binarization
			ImageProcessor::Canny(&grayImage, &grayImage, m_nCannyLowThreshold, m_nCannyHighThreshold);

			const unsigned int t = get_timer_value();
			
			// display the speed stats
			sprintf(buffer, "%2.2f ms", t / 1000.0f);
			m_pMainWindow->SetText(pLabel3, buffer);
			sprintf(buffer, "%3.2f fps", 1000000.0f / t);
			m_pMainWindow->SetText(pLabel4, buffer);
			sprintf(buffer, "Canny low threshold: %i", m_nCannyLowThreshold);
			m_pMainWindow->SetText(pLabelCannyLow, buffer);
			sprintf(buffer, "Canny high threshold: %i", m_nCannyHighThreshold);
			m_pMainWindow->SetText(pLabelCannyHigh, buffer);

			// display either the original image or the processed image
			if (m_bShowOriginal)
				m_pMainWindow->SetImage(pImageWidget, &image);
			else
				m_pMainWindow->SetImage(pImageWidget, &grayImage);
		}
		
		delete pApplicationHandler;
		
		return 0;
	}


private:
	// private attributes
	CMainWindowInterface *m_pMainWindow;
	WIDGET_HANDLE m_pSlider1;
	WIDGET_HANDLE m_pSlider2;
	WIDGET_HANDLE m_pButton;

	bool m_bShowOriginal;
	int m_nCannyLowThreshold;
	int m_nCannyHighThreshold;
};


int main()
{
	CCannyDemo demo;
	return demo.Run();
}
