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
// Date:      06.04.2009
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
#include "Image/StereoVision.h"
#include "Calibration/Rectification.h"

#include "Helpers/helpers.h"

#include <stdio.h>
#include <stdlib.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define DEMO_IMAGE_LEFT "../../files/scene_left.bmp"
#define DEMO_IMAGE_RIGHT "../../files/scene_right.bmp"
#define DEMO_CAMERA_CALIBRATION "../../files/camera_armar_640x480.txt"


// ****************************************************************************
// Global functions
// ****************************************************************************

// difference of gaussians as an approximation to a laplacian of gaussian
void DifferenceOfGaussians(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for DifferenceOfGaussian\n");
		return;
	}
	
	CByteImage temp(pInputImage->width, pInputImage->height, CByteImage::eGrayScale);
	//ImageProcessor::GaussianSmooth3x3(pInputImage, &temp);
	ImageProcessor::CopyImage(pInputImage, &temp);
	
	ImageProcessor::GaussianSmooth(pInputImage, pOutputImage, 4.5f, 13);

	const int nPixels = pInputImage->width * pInputImage->height;
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int i = 0; i < nPixels; i++)
	{	
		output[i] = abs(temp.pixels[i] - output[i]);
	}
}

void GradientMeasure(const CByteImage *pInputImage, CByteImage *pOutputImage, int frame, int threshold)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for GradientMeasure\n");
		return;
	}
	
	ImageProcessor::Zero(pOutputImage);
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;
	
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int y = frame+1; y < height - frame; y++)
	{
		for (int x = frame+1; x < width - frame; x++)
		{
			int sum = 0;
			for (int v = -3; v <= 3; v++)
			{
				for (int u = -4; u < 4; u++)
				{
					sum +=input[(y + v)*width + (x + u)];
				}
			}
			
			output[y*width + x] = (sum > threshold ? 255 : 0);
		}
	}
}

void FilterDepthImage(const CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
	{
		printf("error: input and output image do not match for GradientMeasure\n");
		return;
	}
	
	const int width = pInputImage->width;
	const int height = pInputImage->height;
	const int nPixels = width * height;
	
	const unsigned char *input = pInputImage->pixels;
	unsigned char *output = pOutputImage->pixels;
	
	for (int i = 0; i < nPixels; i++)
	{
		output[i] = (input[i] ? output[i] : 0);
	}
}



// ****************************************************************************
// CStereoDepthMap
// ****************************************************************************

class CStereoDepthMap : public CMainWindowEventInterface
{
public:
	// constructor
	CStereoDepthMap() : m_bShowLoGImages(false), m_bShowGradientMeasure(false), m_nThreshold(128), m_nErrorThreshold(50), m_nWindowSize(13)
	{
	}

	// this is called when the value of one of the checkboxes is changed
	void ValueChanged(WIDGET_HANDLE pWidget, int nValue)
	{
		if (pWidget == m_pCheckBox1)
			m_bShowLoGImages = nValue > 0;
		else if (pWidget == m_pCheckBox2)
			m_bShowGradientMeasure = nValue > 0;
		else if (pWidget == m_pSlider1)
			m_nThreshold = nValue;
		else if (pWidget == m_pSlider2)
			m_nErrorThreshold = nValue;
		else if (pWidget == m_pSlider3)
			m_nWindowSize = 2*nValue + 1;
			
		// display the thresholds
		char buffer[1024];
		sprintf(buffer, "Gradient Threshold (%d)", m_nThreshold);
		m_pMainWindow->SetText(m_pSliderLabel1, buffer);
		sprintf(buffer, "Error Threshold (%d)", m_nErrorThreshold);
		m_pMainWindow->SetText(m_pSliderLabel2, buffer);
		sprintf(buffer, "Window Size (%d)", m_nWindowSize);
		m_pMainWindow->SetText(m_pSliderLabel3, buffer);
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
		
		const int width0 = capture.GetWidth();
		const int height0 = capture.GetHeight();
		const CByteImage::ImageType type = capture.GetType();

		const int width = width0 / 2;
		const int height = height0/ 2;

		// create images for the image processing
		CByteImage *ppImages[2] = { new CByteImage(width0, height0, capture.GetType()), new CByteImage(width0, height0, capture.GetType()) };
		CByteImage *ppGrayImages[] = { new CByteImage(width0, height0, CByteImage::eGrayScale), new CByteImage(width0, height0, CByteImage::eGrayScale) };
		CByteImage *ppGrayImages2[] = { new CByteImage(width0, height0, CByteImage::eGrayScale), new CByteImage(width0, height0, CByteImage::eGrayScale) };
		CByteImage *ppGrayImages3[] = { new CByteImage(width, height, CByteImage::eGrayScale), new CByteImage(width, height, CByteImage::eGrayScale) };
		CByteImage *ppLoGImages[] = { new CByteImage(width, height, CByteImage::eGrayScale), new CByteImage(width, height, CByteImage::eGrayScale) };
		CByteImage depth_image(width, height, CByteImage::eGrayScale);
		CByteImage gradient_measure(width, height, CByteImage::eGrayScale);
		
		// create and initialize the rectification
		CRectification rectification;
		rectification.Init(DEMO_CAMERA_CALIBRATION);
		
		// create and initialize the stereo vision module
		CStereoVision stereo_vision;
		
		// create an application handler
		CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
		pApplicationHandler->Reset();

		// create a main window
		m_pMainWindow = CreateMainWindow(0, 0, 3*width, height + 90, "Stereo Depth Map Demo");

		// events are sent to this class, hence this class needs to have the CMainWindowEventInterface
		m_pMainWindow->SetEventCallback(this);

		// create two image widgets to display the two images
		WIDGET_HANDLE pImageWidgetLeft = m_pMainWindow->AddImage(0, 0, width, height);
		WIDGET_HANDLE pImageWidgetRight = m_pMainWindow->AddImage(width, 0, width, height);
		WIDGET_HANDLE pImageWidgetDepth = m_pMainWindow->AddImage(2*width, 0, width, height);
		
		// add checkboxes
		m_pCheckBox1 = m_pMainWindow->AddCheckBox(10, height + 10, 200, 30, "Show LoG Images", false);
		m_pCheckBox2 = m_pMainWindow->AddCheckBox(10, height + 50, 200, 30, "Show Grad Measure", false);
		
		// add sliders
		m_pSliderLabel1 = m_pMainWindow->AddLabel(220, height + 10, 200, 30, "Gradient Threshold (128)");
		m_pSlider1 = m_pMainWindow->AddSlider(220, height + 50, 200, 30, 0, 255, 25, m_nThreshold);
		m_pSliderLabel2 = m_pMainWindow->AddLabel(430, height + 10, 200, 30, "Error Threshold (50)");
		m_pSlider2 = m_pMainWindow->AddSlider(430, height + 50, 200, 30, 0, 255, 25, m_nErrorThreshold);
		m_pSliderLabel3 = m_pMainWindow->AddLabel(640, height + 10, 200, 30, "Window Size (13)");
		m_pSlider3 = m_pMainWindow->AddSlider(640, height + 50, 200, 30, 1, 13, 1, m_nWindowSize / 2);
		
		// add labels to display processing stats
		WIDGET_HANDLE pLabel1 = m_pMainWindow->AddLabel(850, height + 10, 100, 20, "666 ms");
		WIDGET_HANDLE pLabel2 = m_pMainWindow->AddLabel(850, height + 30, 100, 20, "666 fps");

		// make the window visible
		m_pMainWindow->Show();
		
		
		char buffer[1024];

		// main loop
		while (!pApplicationHandler->ProcessEventsAndGetExit())
		{
			if (!capture.CaptureImage(ppImages))
				break;
				
			unsigned int t1 = get_timer_value(true);
			
			// convert images to gray scale
			ImageProcessor::ConvertImage(ppImages[0], ppGrayImages[0], true);
			ImageProcessor::ConvertImage(ppImages[1], ppGrayImages[1], true);
			
			// rectify images
			rectification.Rectify(ppGrayImages, ppGrayImages2);

			// reduce the images to 1/2 x 1/2 of the original size
			ImageProcessor::Resize(ppGrayImages2[0], ppGrayImages3[0]);
			ImageProcessor::Resize(ppGrayImages2[1], ppGrayImages3[1]);
			
			// calculate two difference of gaussians as an approximation to
			// two laplacian of gaussians
			DifferenceOfGaussians(ppGrayImages3[0], ppLoGImages[0]);
			DifferenceOfGaussians(ppGrayImages3[1], ppLoGImages[1]);
			
			// calculate the stereo match
			stereo_vision.ProcessFast(ppLoGImages[0], ppLoGImages[1], &depth_image, m_nWindowSize, 0, 80, 1, m_nErrorThreshold);
			
			// calculate a gradient measure to discard regions in the image without enough texture
			// the regions have unreliable matches (the result is already thresholded)
			GradientMeasure(ppLoGImages[0], &gradient_measure, m_nWindowSize, m_nThreshold);
			
			// remove those regions
			ImageProcessor::And(&gradient_measure, &depth_image, &depth_image);
			
			// multiply the depth values by 2 to make them more visible
			ImageProcessor::Amplify(&depth_image, &depth_image, 2);
			
			unsigned int t2 = get_timer_value();
			
			// display the speed stats
			sprintf(buffer, "%.2f ms", t2 / 1000.0f);
			m_pMainWindow->SetText(pLabel1, buffer);
			sprintf(buffer, "%.2f fps", 1000000.0f / t2);
			m_pMainWindow->SetText(pLabel2, buffer);

			// display the images
			if (m_bShowLoGImages)
			{
				m_pMainWindow->SetImage(pImageWidgetLeft, ppLoGImages[0]);
				m_pMainWindow->SetImage(pImageWidgetRight, ppLoGImages[1]);
			}
			else
			{
				m_pMainWindow->SetImage(pImageWidgetLeft, ppGrayImages3[0]);
				m_pMainWindow->SetImage(pImageWidgetRight, ppGrayImages3[1]);
			}
			
			if (m_bShowGradientMeasure)
			{
				m_pMainWindow->SetImage(pImageWidgetDepth, &gradient_measure);
			}
			else
			{
				m_pMainWindow->SetImage(pImageWidgetDepth, &depth_image);
			}
		}
		
		delete ppImages[0];
		delete ppImages[1];
		delete ppGrayImages[0];
		delete ppGrayImages[1];
		delete ppGrayImages2[0];
		delete ppGrayImages2[1];
		delete ppGrayImages3[0];
		delete ppGrayImages3[1];
		delete ppLoGImages[0];
		delete ppLoGImages[1];
		
		delete m_pMainWindow;
		delete pApplicationHandler;
		
		return 0;
	}


private:
	// private attributes
	CMainWindowInterface *m_pMainWindow;
	WIDGET_HANDLE m_pCheckBox1;
	WIDGET_HANDLE m_pCheckBox2;
	WIDGET_HANDLE m_pSlider1;
	WIDGET_HANDLE m_pSlider2;
	WIDGET_HANDLE m_pSlider3;
	WIDGET_HANDLE m_pSliderLabel1;
	WIDGET_HANDLE m_pSliderLabel2;
	WIDGET_HANDLE m_pSliderLabel3;
	
	bool m_bShowLoGImages;
	bool m_bShowGradientMeasure;
	int m_nThreshold;
	int m_nErrorThreshold;
	int m_nWindowSize;
};


int main()
{
	CStereoDepthMap demo;
	return demo.Run();
}
