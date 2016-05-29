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
#include "Image/ShortImage.h"
#include "Image/ImageProcessor.h"

#include "Helpers/helpers.h"

#include <stdio.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define DEMO_IMAGE "../../files/scene_left.bmp"



// ****************************************************************************
// CFilterDemo
// ****************************************************************************

class CFilterDemo : public CMainWindowEventInterface
{
public:
	// constructor
	CFilterDemo() : m_nFilter(0), m_bShowOriginal(false)
	{
	}

	// this is called when the value (the selection) of the combo box changes
	void ValueChanged(WIDGET_HANDLE pWidget, int nValue)
	{
		if (pWidget == m_pComboBox)
		{
			m_nFilter = nValue;
		}
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

		// create temp image for the image processing
		CByteImage image(width, height, capture.GetType());
		CByteImage grayImage(width, height, CByteImage::eGrayScale);
		CByteImage tempImage(width, height, CByteImage::eGrayScale);
		CByteImage resultImage(width, height, CByteImage::eGrayScale);
		CShortImage shortImage(width, height);
		
		CByteImage *pImage = &image;


		// create an application handler
		CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
		pApplicationHandler->Reset();
		
		// create a main window
		m_pMainWindow = CreateMainWindow(0, 0, width, height + 90, "Filter Demo");

		// events are sent to this class, hence this class needs to have the CMainWindowEventInterface
		m_pMainWindow->SetEventCallback(this);

		// create an image widget to display an image
		WIDGET_HANDLE pImageWidget = m_pMainWindow->AddImage(0, 90, width, height);

		// add a lable and a combo box to select a filter
		m_pMainWindow->AddLabel(10, 10, 200, 30, "Filter Operation:");
		const char *filter_names[10] = {"Gaussian 3x3", "Gaussian 5x5", "Sobel X", "Sobel Y", "Sobel X&Y", "Prewitt X", "Prewitt Y", "Prewitt X&Y", "Gaussian + Laplacian1", "Gaussian + Laplacian2"};
		m_pComboBox = m_pMainWindow->AddComboBox(10, 40, 200, 40, 10, filter_names, m_nFilter);
		
		// add a button to toggle between the original image and the processed one
		m_pButton = m_pMainWindow->AddButton(220, 40, 120, 40, "Show Original");

		// add a labels to display processing stats
		WIDGET_HANDLE pLabel3 = m_pMainWindow->AddLabel(560, 40, 70, 20, "666 ms");
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
			
			const unsigned int t1 = get_timer_value(true);
			
			// select a filter operation
			switch (m_nFilter)
			{
				case 0: // Gaussian 3x3
					ImageProcessor::GaussianSmooth3x3(&grayImage, &resultImage);
				break;
				
				case 1: // Gaussian 5x5
					ImageProcessor::GaussianSmooth5x5(&grayImage, &resultImage);
				break;
				
				case 2: // Sobel X
					ImageProcessor::SobelX(&grayImage, &shortImage);
				break;
				
				case 3: // Sobel Y
					ImageProcessor::SobelY(&grayImage, &shortImage);
				break;
				
				case 4: // Sobel X&Y
					ImageProcessor::CalculateGradientImageSobel(&grayImage, &resultImage);
				break;
				
				case 5: // Prewitt X
					ImageProcessor::PrewittX(&grayImage, &shortImage);
				break;
				
				case 6: // Prewitt Y
					ImageProcessor::PrewittY(&grayImage, &shortImage);
				break;
				
				case 7: // Prewitt X&Y
					ImageProcessor::CalculateGradientImagePrewitt(&grayImage, &resultImage);
				break;
				
				case 8: // Laplacian1
					// we apply a Gaussian smooth to decrease nosie and get better results
					ImageProcessor::GaussianSmooth3x3(&grayImage, &tempImage);
					ImageProcessor::Laplace1(&tempImage, &shortImage);
				break;
				
				case 9: // Laplacian2
					// we apply a Gaussian smooth to decrease nosie and get better results
					ImageProcessor::GaussianSmooth3x3(&grayImage, &tempImage);
					ImageProcessor::Laplace2(&tempImage, &shortImage);
				break;
			}
			
			const unsigned int t2 = get_timer_value() - t1;
			
			if (m_nFilter == 2 || m_nFilter == 3 || m_nFilter == 5 || m_nFilter == 6 || m_nFilter == 8 || m_nFilter == 9)
			{
				// convert short image to a grayscale image
				ImageProcessor::ConvertImage(&shortImage, &resultImage);
			}
			
			// display the speed stats
			sprintf(buffer, "%2.2f ms", t2 / 1000.0f);
			m_pMainWindow->SetText(pLabel3, buffer);
			sprintf(buffer, "%3.2f fps", 1000000.0f / t2);
			m_pMainWindow->SetText(pLabel4, buffer);

			// display either the original image or the processed image
			if (m_bShowOriginal)
				m_pMainWindow->SetImage(pImageWidget, &grayImage);
			else
				m_pMainWindow->SetImage(pImageWidget, &resultImage);
		}
		
		delete m_pMainWindow;
		delete pApplicationHandler;
		
		return 0;
	}


private:
	// private attributes
	CMainWindowInterface *m_pMainWindow;
	WIDGET_HANDLE m_pComboBox;
	WIDGET_HANDLE m_pButton;

	bool m_bShowOriginal;
	int m_nFilter;
};


int main()
{
	CFilterDemo demo;
	return demo.Run();
}
