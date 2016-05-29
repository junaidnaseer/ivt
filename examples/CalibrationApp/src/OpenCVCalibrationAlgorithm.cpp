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
// Filename:  OpenCVCalibrationAlgorithm.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "OpenCVCalibrationAlgorithm.h"

#include "Interfaces/VideoCaptureInterface.h"
#include "Interfaces/MainWindowInterface.h"
#include "Interfaces/ApplicationHandlerInterface.h"
#include "Image/ByteImage.h"
#include "Image/ImageProcessor.h"
#include "Image/IplImageAdaptor.h"

#include <cvaux.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

COpenCVCalibrationAlgorithm::COpenCVCalibrationAlgorithm(int nCameras, int nImages,
		int nRows, int nColumns, double dSquareSizeInMM,
		const char *pCameraParameterFileName, CVideoCaptureInterface *pVideoCapture)
{
	m_pVideoCapture = pVideoCapture;
	
	const int width = pVideoCapture->GetWidth();
	const int height = pVideoCapture->GetHeight();
	const CByteImage::ImageType type = pVideoCapture->GetType();
	
	if (type == CByteImage::eGrayScale)
	{
		m_ppCameraImages[0] = new CByteImage(width, height, type);
		m_ppCameraImages[1] = new CByteImage(width, height, type);
	}
	else
	{
		m_ppCameraImages[0] = 0;
		m_ppCameraImages[1] = 0;
	}
	
	m_ppImages[0] = new CByteImage(width, height, CByteImage::eRGB24);
	m_ppImages[1] = new CByteImage(width, height, CByteImage::eRGB24);
	
	m_sCameraParameterFileName = "";
	m_sCameraParameterFileName += pCameraParameterFileName;

	m_nCameras = nCameras;

	const int nCornersPerColumn = nRows - 1;
	const int nCornersPerRow = nColumns - 1;
	const int nCornersPerImage = nCornersPerColumn * nCornersPerRow;

	m_pCorners2DFloat = new CvPoint2D32f[nCornersPerImage];
	m_pCorners2D = new CvPoint2D64d[nCornersPerImage];
	m_pCorners3D = new CvPoint3D64d[nCornersPerImage];

	m_pCalibFilter = new CvCalibFilter();

	// initialize calibration filter
	double etalonParams[3] = { double(nColumns), double(nRows), dSquareSizeInMM };
	m_pCalibFilter->SetCameraCount(nCameras);
	m_pCalibFilter->SetFrames(nImages);
	m_pCalibFilter->SetEtalon(CV_CALIB_ETALON_CHESSBOARD, etalonParams);
}

COpenCVCalibrationAlgorithm::~COpenCVCalibrationAlgorithm()
{
	delete m_pCorners2DFloat;
	delete m_pCorners2D;
	delete m_pCorners3D;
		
	delete m_ppImages[0];
	delete m_ppImages[1];
	
	if (m_ppCameraImages[0])
	{
		delete m_ppCameraImages[0];
		delete m_ppCameraImages[1];
	}
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool COpenCVCalibrationAlgorithm::Calibrate()
{
	int nTotalCount = 0;

	if (m_nCameras < 1 || m_nCameras > 2)
		return false;

	while (true)
	{
		if (m_pVideoCapture->GetType() == CByteImage::eGrayScale)
		{
			if (!m_pVideoCapture->CaptureImage(m_ppCameraImages))
				break;
				
			ImageProcessor::ConvertImage(m_ppCameraImages[0], m_ppImages[0]);
			
			if (m_nCameras == 2)
				ImageProcessor::ConvertImage(m_ppCameraImages[1], m_ppImages[1]);
		}
		else
		{
			if (!m_pVideoCapture->CaptureImage(m_ppImages))
				break;
		}
		
		IplImage *ppIplImages[2] = { IplImageAdaptor::Adapt(m_ppImages[0]), IplImageAdaptor::Adapt(m_ppImages[1]) };

		// bug in CvCalibFilter::FindEtalon: if image is already 8bit grayscale, internal conversion routine fails...
		// therefore always feed RGB 24bit (3 channels, 8 bit)
		if (m_nCameras == 2)
		{
			if (m_pCalibFilter->FindEtalon(ppIplImages))
			{
		
				/*if (nTotalCount % 20 == 0)
				{
					CByteImage grayImage(m_ppImages[0]->width, m_ppImages[0]->height, CByteImage::eGrayScale);
					
					char szTemp[1024];
					sprintf(szTemp, "sequence/left%.4i.bmp", nTotalCount / 20);
					ImageProcessor::ConvertImage(m_ppImages[0], &grayImage, true);
					if (!grayImage.SaveToFile(szTemp))
					{
						printf("error: could not write bitmap to '%s'.\nplease create the directory 'sequence' first.\n", szTemp);
						break;
					}
					sprintf(szTemp, "sequence/right%.4i.bmp", nTotalCount / 20);
					ImageProcessor::ConvertImage(m_ppImages[1], &grayImage, true);
					if (!grayImage.SaveToFile(szTemp))
					{
						printf("error: could not write bitmap to '%s'.\nplease create the directory 'sequence' first.\n", szTemp);
						break;
					}
				}*/
				
				m_pCalibFilter->DrawPoints(ppIplImages);

				if ((nTotalCount++ % 20) == 0)
				{
					printf("corner picture taken\n");
					
					m_pCalibFilter->Push();
						
					if (m_pCalibFilter->IsCalibrated())
					{
						m_pCalibFilter->SaveCameraParams(m_sCameraParameterFileName.c_str());
						return true;
					}
				}
			}
			
			if (m_pLeftImageWidget != 0)
				m_pMainWindow->SetImage(m_pLeftImageWidget, m_ppImages[0]);
			if (m_pRightImageWidget != 0)
				m_pMainWindow->SetImage(m_pRightImageWidget, m_ppImages[1]);
		}
		else if (m_nCameras == 1)
		{
			if (m_pCalibFilter->FindEtalon(&ppIplImages[0]))
			{
				m_pCalibFilter->DrawPoints(&ppIplImages[0]);

				if ((nTotalCount++ % 20) == 0)
				{
					printf("corner picture taken\n");

					m_pCalibFilter->Push();
						
					if (m_pCalibFilter->IsCalibrated())
					{
						m_pCalibFilter->SaveCameraParams(m_sCameraParameterFileName.c_str());
						return true;
					}
				}
			}
			
			if (m_pLeftImageWidget != 0)
				m_pMainWindow->SetImage(m_pLeftImageWidget, m_ppImages[0]);
		}

		cvReleaseImageHeader(&ppIplImages[0]);
		cvReleaseImageHeader(&ppIplImages[1]);
		
		if (m_pApplicationHandlerInterface->ProcessEventsAndGetExit())
			break;
	}

	return false;
}
