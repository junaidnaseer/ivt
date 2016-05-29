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
// Author:    Manuel Mohr
// Date:      29.10.2008
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
#include "Image/PrimitivesDrawer.h"
#include "Math/Vecd.h"

#include "Helpers/helpers.h"

#include <stdio.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define DEMO_IMAGE "../../files/scene_left.bmp"



// ****************************************************************************
// CHarrisDemo
// ****************************************************************************

class CHarrisDemo : public CMainWindowEventInterface
{
public:
	// constructor
	CHarrisDemo() : m_fQualityThreshold(0.005f), m_nMaxPoints(700)
	{
	}

	// this is called when the value of one of the sliders is changed
	void ValueChanged(WIDGET_HANDLE pWidget, int nValue)
	{
		if (pWidget == m_pSlider1)
			m_fQualityThreshold = nValue / 1000.0f;
		else if (pWidget == m_pSlider2)
			m_nMaxPoints = nValue;
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

		// create temp image for the image processing
		CByteImage image(width, height, capture.GetType());
		CByteImage grayImage(width, height, CByteImage::eGrayScale);
		CByteImage *pImage = &image;


		// create an application handler
		CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
		pApplicationHandler->Reset();

		// create a main window
		CMainWindowInterface *pMainWindow = CreateMainWindow(0, 0, width, height + 100, "Harris Interest Point Detector Demo");

		// events are sent to this class, hence this class needs to have the CMainWindowEventInterface
		pMainWindow->SetEventCallback(this);

		// create an image widget to display a window
		WIDGET_HANDLE pImageWidget = pMainWindow->AddImage(0, 100, width, height);

		// add a label and a slider for the quality
		WIDGET_HANDLE pLabel1 = pMainWindow->AddLabel(10, 10, 150, 30, "Quality");
		m_pSlider1 = pMainWindow->AddSlider(10, 50, 150, 40, 0, 100, 10, int(m_fQualityThreshold * 1000.0f + 0.5f));

		// add a label and a slider for the number of interest points
		WIDGET_HANDLE pLabel2 = pMainWindow->AddLabel(200, 10, 200, 30, "Number of points");
		m_pSlider2 = pMainWindow->AddSlider(200, 50, 200, 40, 0, 1000, 50, m_nMaxPoints);
		
		// add a labels to display processing stats
		WIDGET_HANDLE pLabel3 = pMainWindow->AddLabel(520, 10, 120, 20, "666 ms");
		WIDGET_HANDLE pLabel4 = pMainWindow->AddLabel(520, 40, 120, 20, "666 fps");
		WIDGET_HANDLE pLabel5 = pMainWindow->AddLabel(520, 70, 120, 20, "666 points");

		// make the window visible
		pMainWindow->Show();
		
		
		Vec2d points[1000];
		char buffer[1024];

		// main loop
		while (!pApplicationHandler->ProcessEventsAndGetExit())
		{
			if (!capture.CaptureImage(&pImage))
				break;
			
			ImageProcessor::ConvertImage(pImage, &grayImage, true);

			get_timer_value(true);
			const int nPoints = ImageProcessor::CalculateHarrisInterestPoints(&grayImage, points, m_nMaxPoints, m_fQualityThreshold);
			const unsigned int t = get_timer_value();

			// display some information
			sprintf(buffer, "Quality = %.3f", m_fQualityThreshold);
			pMainWindow->SetText(pLabel1, buffer);
			sprintf(buffer, "Max number of points = %d", m_nMaxPoints);
			pMainWindow->SetText(pLabel2, buffer);

			// display the speed stats
			sprintf(buffer, "%.2f ms", t / 1000.0f);
			pMainWindow->SetText(pLabel3, buffer);
			sprintf(buffer, "%.2f fps", 1000000.0f / t);
			pMainWindow->SetText(pLabel4, buffer);
			sprintf(buffer, "%d points", nPoints);
			pMainWindow->SetText(pLabel5, buffer);

			// visualize interest points
			for (int i = 0; i < nPoints; ++i)
				PrimitivesDrawer::DrawCross(&grayImage, points[i], 5, 255, 255, 255);

			// display result image
			pMainWindow->SetImage(pImageWidget, &grayImage);
		}
		
		delete pMainWindow;
		delete pApplicationHandler;
		
		return 0;
	}


private:
	// private attributes
	WIDGET_HANDLE m_pSlider1;
	WIDGET_HANDLE m_pSlider2;

	float m_fQualityThreshold;
	int m_nMaxPoints;
};


int main()
{
	CHarrisDemo demo;
	return demo.Run();
}
