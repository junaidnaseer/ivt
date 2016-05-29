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
#include "DataStructures/DynamicArray.h"
#include "DataProcessing/RANSAC.h"
#include "Classification/NearestNeighbor.h"
#include "Features/HarrisSIFTFeatures/HarrisSIFTFeatureCalculator.h"
#include "Features/FeatureEntry.h"
#include "Math/LinearAlgebra.h"
#include "Math/Vecd.h"

#include "Helpers/helpers.h"

#include <stdio.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define DEMO_IMAGE                        "../../files/scene_left.bmp"
#define TRAINING_IMAGE                    "../../files/training_image.bmp"



// ****************************************************************************
// CHarrisSIFTDemo
// ****************************************************************************

class CHarrisSIFTDemo : public CMainWindowEventInterface
{
public:
	// constructor
	CHarrisSIFTDemo() : m_fQualityThreshold(0.01f), m_fMatchingThreshold(0.5f), m_nKdLeaves(70)
	{
	}

	// this is called when the value of one of the sliders is changed
	void ValueChanged(WIDGET_HANDLE pWidget, int nValue)
	{
		if (pWidget == m_pSlider1)
			m_fQualityThreshold = nValue / 1000.0f;
		else if (pWidget == m_pSlider2)
			m_fMatchingThreshold = nValue / 100.0f;
		else if (pWidget == m_pSlider3)
			m_nKdLeaves = nValue;
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
		
		CByteImage trainingImage;
		if (!trainingImage.LoadFromFile(TRAINING_IMAGE))
		{
			printf("error: could not open training image\n");
			return 1;
		}
		
		const int width = capture.GetWidth();
		const int height = capture.GetHeight();

		// create temp image for the image processing
		CByteImage image(width, height, capture.GetType());
		CByteImage trainingImageGray(trainingImage.width, trainingImage.height, CByteImage::eGrayScale);
		CByteImage currentImageGray(width, height, CByteImage::eGrayScale);
		CByteImage resultImage1(width, height, CByteImage::eRGB24);
		CByteImage resultImage2(width, height, CByteImage::eRGB24);
		CByteImage *pImage = &image;


		// create an application handler
		CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
		pApplicationHandler->Reset();

		// create a main window
		CMainWindowInterface *pMainWindow = CreateMainWindow(0, 0, 2 * width, height + 100, "HarrisSIFT Demo");

		// events are sent to this class, hence this class needs to have the CMainWindowEventInterface
		pMainWindow->SetEventCallback(this);

		// create an image widget to display a window
		WIDGET_HANDLE pImageWidget1 = pMainWindow->AddImage(0, 100, width, height);
		WIDGET_HANDLE pImageWidget2 = pMainWindow->AddImage(width, 100, width, height);

		// create a checkbox for choosing to use RANSAC or not
		WIDGET_HANDLE pCheckBoxRANSAC = pMainWindow->AddCheckBox(950, 10, 200, 20, "RANSAC", true);

		// add a label and a slider for the quality threshold
		WIDGET_HANDLE pLabel1 = pMainWindow->AddLabel(10, 10, 200, 30, "Quality threshold");
		m_pSlider1 = pMainWindow->AddSlider(10, 35, 200, 40, 0, 100, 5, int(m_fQualityThreshold * 1000.0f + 0.5f));
		
		// add a label and a slider for the matching threshold
		WIDGET_HANDLE pLabel2 = pMainWindow->AddLabel(230, 10, 200, 30, "Matching threshold");
		m_pSlider2 = pMainWindow->AddSlider(230, 35, 200, 40, 0, 100, 5, int(m_fMatchingThreshold * 100.0f + 0.5f));
		
		// add a label and a slider for the number of leaves to visit in kd tree
		WIDGET_HANDLE pLabel3 = pMainWindow->AddLabel(460, 10, 200, 30, "kd-tree leaves");
		m_pSlider3 = pMainWindow->AddSlider(460, 35, 200, 40, 1, 200, 5, m_nKdLeaves);
		
		// add a labels to display processing stats
		WIDGET_HANDLE pLabel4 = pMainWindow->AddLabel(700, 10, 200, 20, "HarrisSIFT calculation: 0 ms");
		WIDGET_HANDLE pLabel5 = pMainWindow->AddLabel(700, 40, 200, 20, "Matching (kd-tree): 0 ms");
		WIDGET_HANDLE pLabel6 = pMainWindow->AddLabel(700, 70, 200, 20, "0 matches");

		// make the window visible
		pMainWindow->Show();
		
		CHarrisSIFTFeatureCalculator harrisSiftFeatureCalculator;
		CDynamicArray resultList1(1000), resultList2(1000);
		
		// calculate features for training view
		ImageProcessor::ConvertImage(&trainingImage, &trainingImageGray, true);
		harrisSiftFeatureCalculator.SetThreshold(m_fQualityThreshold);
		const int nTrainingFeatures = harrisSiftFeatureCalculator.CalculateFeatures(&trainingImageGray, &resultList1);
		int i;
		
		if (nTrainingFeatures < 1)
		{
			printf("error: training image does not contain any features\n");
			return 1;
		}
		
		const int nDimension = ((const CFeatureEntry *) resultList1.GetElement(0))->GetSize();
		
		// for recognition, only the first level is needed
		harrisSiftFeatureCalculator.SetNumberOfLevels(1);
		
		// create and train nearest neighbor classifier
		CNearestNeighbor nn(CNearestNeighbor::eKdTree);
		float *pData = new float[nTrainingFeatures * nDimension];
		for (i = 0; i < nTrainingFeatures; i++)
			memcpy(pData + i * nDimension, ((const CFeatureEntry *) resultList1.GetElement(i))->m_pFeature, nDimension * sizeof(float));
		if (!nn.Train(pData, nDimension, nTrainingFeatures))
		{
			printf("error: training failed\n");
			return 1;
		}
		delete [] pData;
		
		char buffer[1024];

		// needed for bundle request
		int nMaxFeatures = 2500;
		float *pQueryElements = new float[nMaxFeatures * nDimension];
		int *pResults = new int[nMaxFeatures];
		float *pResultErrors = new float[nMaxFeatures];

		// main loop
		while (!pApplicationHandler->ProcessEventsAndGetExit())
		{
			if (!capture.CaptureImage(&pImage))
				break;

			// prepare visualization
			ImageProcessor::ConvertImage(pImage, &resultImage1);
			ImageProcessor::ConvertImage(&trainingImageGray, &resultImage2);
			
			// convert to grayscale image
			ImageProcessor::ConvertImage(pImage, &currentImageGray, true);
			
			// apply current settings and reset list
			harrisSiftFeatureCalculator.SetThreshold(m_fQualityThreshold);
			resultList2.Clear();

			get_timer_value(true);
			
			const int nFeatures = harrisSiftFeatureCalculator.CalculateFeatures(&currentImageGray, &resultList2);
			
			const unsigned int t1 = get_timer_value();

			// set kd-tree parameter for heuristic search
			nn.SetKdTreeMaxLeaves(m_nKdLeaves);
			int i;

			// allocate greater amount of memory if necessary
			if (nFeatures > nMaxFeatures)
			{
				nMaxFeatures = nFeatures + 100;
				delete [] pQueryElements;
				pQueryElements = new float[nMaxFeatures * nDimension];
			}

			// copy features to one memory area for bundle request
			for (i = 0; i < nFeatures; i++)
			{
				const CFeatureEntry *pFeature = (const CFeatureEntry *) resultList2.GetElement(i);
				memcpy(pQueryElements + i * 128, pFeature->m_pFeature, 128 * sizeof(float));
			}

			// calculate nearest neighbor for all query features in one run
			if (!nn.Classify(pQueryElements, nDimension, nFeatures, pResults, pResultErrors))
			{
				printf("error: classifation failed\n");
				break;
			}

			const unsigned int t2 = get_timer_value();

			CDynamicArrayTemplate<PointPair2d> matchList;
			
			// visualize features for training view
			for (i = 0; i < nTrainingFeatures; ++i)
			{
				const CFeatureEntry *pFeature = (const CFeatureEntry *) resultList1.GetElement(i);
				PrimitivesDrawer::DrawCross(&resultImage2, pFeature->point, 5, 255, 0, 0);
			}

			// determine match candidates
			for (i = 0; i < nFeatures; i++)
			{
				const CFeatureEntry *pFeature = (const CFeatureEntry *) resultList2.GetElement(i);
				PrimitivesDrawer::DrawCross(&resultImage1, pFeature->point, 5, 255, 0, 0);
				
				// retrieve matching result
				float error = pResultErrors[i];
				
				if (error < m_fMatchingThreshold)
				{
					const int nIndex = pResults[i];
					const CFeatureEntry *pMatchedFeature = (const CFeatureEntry *) resultList1.GetElement(nIndex);
					
					// count and draw match
					PointPair2d match;
					Math2d::SetVec(match.p1, pFeature->point);
					Math2d::SetVec(match.p2, pMatchedFeature->point);
					matchList.AddElement(match);
				}
			}
			
			int nUseRANSAC;
			pMainWindow->GetValue(pCheckBoxRANSAC, nUseRANSAC);
			if (nUseRANSAC)
				RANSAC::RANSACAffineTransformation(matchList, matchList, 5.0f, 500);

			// visualize matches
			const int nFilteredMatches = matchList.GetSize();
			for (i = 0; i < nFilteredMatches; i++)
			{
				const Vec2d &p1 = matchList[i].p1;
				const Vec2d &p2 = matchList[i].p2;
				
				const Vec2d p1_ = { p1.x - width, p1.y };
				const Vec2d p2_ = { p2.x + width, p2.y };
					
				PrimitivesDrawer::DrawLine(&resultImage2, p1_, p2, 0, 255, 0);
				PrimitivesDrawer::DrawLine(&resultImage1, p2_, p1, 0, 255, 0);
			}
			
			// display some information
			sprintf(buffer, "Quality threshold = %.3f", m_fQualityThreshold);
			pMainWindow->SetText(pLabel1, buffer);
			sprintf(buffer, "Matching threshold = %.2f", m_fMatchingThreshold);
			pMainWindow->SetText(pLabel2, buffer);
			sprintf(buffer, "kd-tree leaves = %i", m_nKdLeaves);
			pMainWindow->SetText(pLabel3, buffer);

			// display the speed stats
			sprintf(buffer, "HarrisSIFT calculation: %.2f ms", t1 / 1000.0f);
			pMainWindow->SetText(pLabel4, buffer);
			sprintf(buffer, "Matching (NN only): %.2f ms", (t2 - t1) / 1000.0f);
			pMainWindow->SetText(pLabel5, buffer);
			sprintf(buffer, "%d matches", nFilteredMatches);
			pMainWindow->SetText(pLabel6, buffer);

			// display result images
			pMainWindow->SetImage(pImageWidget1, &resultImage1);
			pMainWindow->SetImage(pImageWidget2, &resultImage2);
		}
		
		delete [] pQueryElements;
		delete [] pResults;
		delete [] pResultErrors;
		delete pMainWindow;
		delete pApplicationHandler;
		
		return 0;
	}


private:
	// private attributes
	WIDGET_HANDLE m_pSlider1;
	WIDGET_HANDLE m_pSlider2;
	WIDGET_HANDLE m_pSlider3;
	
	CKdTree *m_pKdTree;

	float m_fQualityThreshold;
	float m_fMatchingThreshold;
	int m_nKdLeaves;
};


int main()
{
	CHarrisSIFTDemo demo;
	return demo.Run();
}
