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
// Date:      24.11.2009
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "VideoCapture/BitmapCapture.h"
#include "Interfaces/ApplicationHandlerInterface.h"
#include "Interfaces/MainWindowInterface.h"
#include "Interfaces/MainWindowEventInterface.h"
#include "gui/GUIFactory.h"

#include "Image/ByteImage.h"
#include "Image/ImageProcessor.h"
#include "Helpers/helpers.h"

#include <stdio.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define DEMO_IMAGE "../../files/geometric_scene.bmp"



// ****************************************************************************
// CHoughLinesDemo
// ****************************************************************************

class CHoughLinesDemo : public CMainWindowEventInterface
{
public:
	// constructor
	CHoughLinesDemo() : m_nCannyLowThreshold(50), m_nCannyHighThreshold(60), m_nLinesToExtract(4), m_bShowEdges(false)
	{
	}

	// this is called when the value of one of the sliders is changed
	void ValueChanged(WIDGET_HANDLE pWidget, int nValue) 
	{
		if (pWidget == m_pSliderCannyLow)
			m_nCannyLowThreshold = nValue;
		else if (pWidget == m_pSliderCannyHigh)
			m_nCannyHighThreshold = nValue;
		else if (pWidget == m_pSliderLinesToExtract)
			m_nLinesToExtract = nValue;
	}

	// this is called when a button is pushed
	void ButtonPushed(WIDGET_HANDLE pWidget)
	{
		if (pWidget == m_pButton)
		{
			m_bShowEdges = !m_bShowEdges;

			if (m_bShowEdges)
				m_pMainWindow->SetText(m_pButton, "Show Result");
			else
				m_pMainWindow->SetText(m_pButton, "Show Edges");
		}
	}

	// init application and run
	int Run()
	{
		CBitmapCapture capture(DEMO_IMAGE);

		// open camera
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
		CByteImage tempImage(width, height, CByteImage::eGrayScale);
		CByteImage visualizationImage(width, height, CByteImage::eRGB24);
		CByteImage *pImage = &image;


		// create an application handler
		CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
		pApplicationHandler->Reset();
		
		// create a main window
		m_pMainWindow = CreateMainWindow(0, 0, width, height + 135, "Hough Line Detection Demo");

		// events are sent to this class, hence this class needs to have the CMainWindowEventInterface
		m_pMainWindow->SetEventCallback(this);

		// create an image widget to display a window
		WIDGET_HANDLE pImageWidget = m_pMainWindow->AddImage(0, 135, width, height);

		// add a label and a slider for the low threshold
		WIDGET_HANDLE pLabelCannyLow = m_pMainWindow->AddLabel(15, 15, 200, 30, "Canny low threshold: 0");
		m_pSliderCannyLow = m_pMainWindow->AddSlider(15, 30, 200, 40, 0, 1020, 102, m_nCannyLowThreshold);

		// add a label and a slider for the high threshold
		WIDGET_HANDLE pLabelCannyHigh = m_pMainWindow->AddLabel(15, 70, 200, 30, "Canny high threshold: 0");
		m_pSliderCannyHigh = m_pMainWindow->AddSlider(15, 85, 200, 40, 0, 1020, 102, m_nCannyHighThreshold);
		
		// add a label and a slider for the number of lines to extract
		WIDGET_HANDLE pLabelLines = m_pMainWindow->AddLabel(260, 15, 200, 30, "Lines to extract: 0 lines");
		m_pSliderLinesToExtract = m_pMainWindow->AddSlider(260, 30, 200, 40, 0, 30, 5, m_nLinesToExtract);
		
		// add a button to toggle between the original image and the processed one
		m_pButton = m_pMainWindow->AddButton(510, 80, 110, 35, "Show Edges");

		// add a labels to display processing stats
		WIDGET_HANDLE pLabelMS = m_pMainWindow->AddLabel(560, 15, 70, 20, "0 ms");
		WIDGET_HANDLE pLabelFPS = m_pMainWindow->AddLabel(560, 45, 70, 20, "0 fps");

		// make the window visible
		m_pMainWindow->Show();
		

		char buffer[1024];
		
		CVec2dArray resultListLines;
		CDynamicArrayTemplate<int> resultHits;
		CVec2dArray edgePoints(10000), edgeDirections(10000);
		
		// main loop
		while (!pApplicationHandler->ProcessEventsAndGetExit())
		{
			if (!capture.CaptureImage(&pImage))
				break;
			
			// this is for visualization purposes only
			ImageProcessor::ConvertImage(pImage, &visualizationImage);
			
			get_timer_value(true);
			
			// convert input image to grayscale image
			ImageProcessor::ConvertImage(&image, &tempImage, true);
			
			// smooth image
			ImageProcessor::GaussianSmooth3x3(&tempImage, &grayImage);

			// detect edges with Canny edge detector
			ImageProcessor::Canny(&grayImage, edgePoints, edgeDirections, m_nCannyLowThreshold, m_nCannyHighThreshold);
			
			// detect lines with Hough transform
			ImageProcessor::HoughTransformLines(edgePoints, edgeDirections, width, height, m_nLinesToExtract, 1, resultListLines, resultHits, &visualizationImage);

			const unsigned int t = get_timer_value();
			
			// display the speed stats
			sprintf(buffer, "%2.2f ms", t / 1000.0f);
			m_pMainWindow->SetText(pLabelMS, buffer);
			sprintf(buffer, "%3.2f fps", 1000000.0f / t);
			m_pMainWindow->SetText(pLabelFPS, buffer);
			sprintf(buffer, "Canny low threshold: %i", m_nCannyLowThreshold);
			m_pMainWindow->SetText(pLabelCannyLow, buffer);
			sprintf(buffer, "Canny high threshold: %i", m_nCannyHighThreshold);
			m_pMainWindow->SetText(pLabelCannyHigh, buffer);
			sprintf(buffer, "Lines to extract: %i", m_nLinesToExtract);
			m_pMainWindow->SetText(pLabelLines, buffer);

			// display either the original image or the processed image
			if (m_bShowEdges)
			{
				ImageProcessor::Canny(&grayImage, &grayImage, m_nCannyLowThreshold, m_nCannyHighThreshold);
				m_pMainWindow->SetImage(pImageWidget, &grayImage);
			}
			else
				m_pMainWindow->SetImage(pImageWidget, &visualizationImage);
		}
		
		delete m_pMainWindow;
		delete pApplicationHandler;
		
		return 0;
	}


private:
	// private attributes
	CMainWindowInterface *m_pMainWindow;
	WIDGET_HANDLE m_pSliderCannyLow, m_pSliderCannyHigh;
	WIDGET_HANDLE m_pSliderLinesToExtract;
	WIDGET_HANDLE m_pButton;

	bool m_bShowEdges;
	int m_nCannyLowThreshold;
	int m_nCannyHighThreshold;
	int m_nLinesToExtract;
};


int main()
{
	CHoughLinesDemo demo;
	return demo.Run();
}
