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
// Filename:  ObjectFinderOrganizer.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "ObjectFinderOrganizer.h"

#include "ObjectFinder/ObjectFinderStereo.h"
#include "ObjectFinder/ObjectFinder.h"
#include "ObjectFinder/CompactRegionFilter.h"
#include "Structs/ObjectDefinitions.h"
#include "Color/ColorParameterSet.h"
#include "Image/ImageProcessor.h"
#include "Image/PrimitivesDrawer.h"
#include "Image/ByteImage.h"
#include "Calibration/Calibration.h"
#include "Calibration/StereoCalibration.h"
#include "Helpers/helpers.h"
#include "Interfaces/ApplicationHandlerInterface.h"
#include "gui/GUIFactory.h"
#include "VideoCapture/BitmapCapture.h"
#include "VideoCapture/BitmapSequenceCapture.h"
#include "VideoCapture/UncompressedAVICapture.h"

#ifdef USE_CAMERA
#include "VideoCapture/Linux1394Capture2.h"
#endif

#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_CAMERA_PARAMETER_RELATIVE_FILEPATH	"files/camera_armar_640x480.txt"

#define LEFT_CAMERA_UID     "00B09D0100501BDE"
#define RIGHT_CAMERA_UID    "00B09D0100501BE6"



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CObjectFinderOrganizer::CObjectFinderOrganizer()
{
	// object finder
	m_pObjectFinder = new CObjectFinder();
	m_pObjectFinderStereo = new CObjectFinderStereo();
	
	// set region filter
	m_pCompactRegionFilter = new CCompactRegionFilter();
	m_pObjectFinder->SetRegionFilter(m_pCompactRegionFilter);
	m_pObjectFinderStereo->SetRegionFilter(m_pCompactRegionFilter);
		
	// calibration
	m_pCalibrationLeft = new CCalibration();
	m_pCalibrationRight = new CCalibration();
	m_pStereoCalibration = 0;

	// color parameter set
	m_pColorParameterSet = new CColorParameterSet();
	
	// pointers to line edit fields
	m_pLineEditFileLeft = 0;
	m_pLineEditFileRight = 0;
	m_pLineEditColorParameterFile = 0;
	m_pLineEditCameraParameterFile = 0;
	
	// initialization
	m_bStereoAvailable = false;
	m_pVideoCapture = 0;
	
	m_ppImages[0] = 0;
	m_ppImages[1] = 0;
	m_ppResultImages[0] = 0;
	m_ppResultImages[1] = 0;
	
	m_pWindow = 0;
	m_color = eNone;

	m_bUpdateParameterSet = true;
	m_bSnapshot = false;
	
	#ifdef USE_CAMERA
	m_pVideoCapture = new CLinux1394Capture2(-1, CVideoCaptureInterface::e640x480, CVideoCaptureInterface::eRGB24)
	m_pVideoCapture->OpenCamera();
	const int width = m_pVideoCapture->GetWidth() == -1 ? 640 : m_pVideoCapture->GetWidth();
	const int height = m_pVideoCapture->GetHeight() == -1 ? 480 : m_pVideoCapture->GetHeight();
	const CByteImage::ImageType type = m_pVideoCapture->GetType() == -1 ? CByteImage::eRGB24 : m_pVideoCapture->GetType();
	m_ppImages[0] = new CByteImage(width, height, type);
	m_ppImages[1] = new CByteImage(width, height, type);
	m_ppResultImages[0] = new CByteImage(width, height, type);
	m_ppResultImages[1] = new CByteImage(width, height, type);
	#endif
}

CObjectFinderOrganizer::~CObjectFinderOrganizer()
{
	delete m_pCompactRegionFilter;
	delete m_pObjectFinder;
	delete m_pObjectFinderStereo;
	delete m_pColorParameterSet;
		
	if (m_ppImages[0])
	{
		delete m_ppImages[0];
		delete m_ppImages[1];
		delete m_ppResultImages[0];
		delete m_ppResultImages[1];
	}
	
	if (m_pStereoCalibration)
		delete m_pStereoCalibration;
		
	delete m_pCalibrationLeft;
	delete m_pCalibrationRight;
	
	if (m_pVideoCapture)
		delete m_pVideoCapture;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CObjectFinderOrganizer::LoadColorParametersButtonClicked()
{
	char buf[1024];
	m_pWindow->GetText(m_pLineEditColorParameterFile, buf, 1024);
	m_pColorParameterSet->LoadFromFile(buf);
	
	int val;
	m_pWindow->GetValue(m_pComboBoxColor, val);
	std::string sColorName;
	CColorParameterSet::Translate((ObjectColor)val, sColorName);
	ColorChanged(sColorName.c_str());
}

void CObjectFinderOrganizer::SaveColorParametersButtonClicked()
{
	char buf[1024];
	m_pWindow->GetText(m_pLineEditColorParameterFile, buf, 1024);
	m_pColorParameterSet->SaveToFile(buf);
}

void CObjectFinderOrganizer::PrintColorParametersButtonClicked()
{
	int custom_par1, custom_par2, custom_par3, custom_par4, custom_par5, custom_par6;
		
	m_pWindow->GetValue(m_pSliderColorH, custom_par1);
	m_pWindow->GetValue(m_pSliderColorHT, custom_par2);
	m_pWindow->GetValue(m_pSliderColorMinS, custom_par3);
	m_pWindow->GetValue(m_pSliderColorMaxS, custom_par4);
	m_pWindow->GetValue(m_pSliderColorMinV, custom_par5);
	m_pWindow->GetValue(m_pSliderColorMaxV, custom_par6);
		
	printf("color parameters: %i %i %i %i %i %i\n", custom_par1, custom_par2, custom_par3, custom_par4, custom_par5, custom_par6);
}

void CObjectFinderOrganizer::LoadCameraParameterFileButtonClicked()
{
	const char *filter[2] = {"Text", "*.txt"};
	const int num_filter = 1;
	char buf[1024];
	buf[0] = 0;
	if (FileDialog(false, filter, num_filter, "Load Camera Parameter File", buf, 1024))
	{
		m_pWindow->SetText(m_pLineEditCameraParameterFile, buf);
		LoadCalibration(buf);
	}
}

void CObjectFinderOrganizer::ColorParameterFileBrowseButtonClicked()
{
	const char *filter[2] = {"Text", "*.txt"};
	const int num_filter = 1;
	char buf[1024];
	buf[0] = 0;
	if (FileDialog(false, filter, num_filter, "Load Color Parameter File", buf, 1024))
	{
		m_pWindow->SetText(m_pLineEditColorParameterFile, buf);
	}
}

void CObjectFinderOrganizer::LeftFileBrowseButtonClicked()
{
	const char *filter[4] = {"Input Image", "*.bmp", "Input Movie", "*.avi"};
	const int num_filter = 2;
	char buf[1024];
	buf[0] = 0;
	if (FileDialog(false, filter, num_filter, "Load Left Camera Input", buf, 1024))
	{
		m_pWindow->SetText(m_pLineEditFileLeft, buf);
	}
}

void CObjectFinderOrganizer::RightFileBrowseButtonClicked()
{
	const char *filter[4] = {"Input Image", "*.bmp", "Input Movie", "*.avi"};
	const int num_filter = 2;
	char buf[1024];
	buf[0] = 0;
	if (FileDialog(false, filter, num_filter, "Load Right Camera Input", buf, 1024))
	{
		m_pWindow->SetText(m_pLineEditFileRight, buf);
	}
}

void CObjectFinderOrganizer::LoadInputButtonClicked()
{
	if (!LoadInput())
		printf("error: could not open video source\n");
}

void CObjectFinderOrganizer::SnapshotButtonClicked()
{
	m_bSnapshot = true;
}

void CObjectFinderOrganizer::SliderValueChanged(int nValue)
{
	if (!m_bUpdateParameterSet)
		return;
		
	if (m_color == eNone)
	{
		m_pWindow->GetValue(m_pSliderColorH, custom_par1);
		m_pWindow->GetValue(m_pSliderColorHT, custom_par2);
		m_pWindow->GetValue(m_pSliderColorMinS, custom_par3);
		m_pWindow->GetValue(m_pSliderColorMaxS, custom_par4);
		m_pWindow->GetValue(m_pSliderColorMinV, custom_par5);
		m_pWindow->GetValue(m_pSliderColorMaxV, custom_par6);
	}
	else
	{
		int custom_par1, custom_par2, custom_par3, custom_par4, custom_par5, custom_par6;
		
		m_pWindow->GetValue(m_pSliderColorH, custom_par1);
		m_pWindow->GetValue(m_pSliderColorHT, custom_par2);
		m_pWindow->GetValue(m_pSliderColorMinS, custom_par3);
		m_pWindow->GetValue(m_pSliderColorMaxS, custom_par4);
		m_pWindow->GetValue(m_pSliderColorMinV, custom_par5);
		m_pWindow->GetValue(m_pSliderColorMaxV, custom_par6);
			
		m_pColorParameterSet->SetColorParameters(m_color,
							custom_par1,
							custom_par2,
							custom_par3,
							custom_par4,
							custom_par5,
							custom_par6);
	}
}

void CObjectFinderOrganizer::ColorChanged(const char *sColor)
{
	m_bUpdateParameterSet = false;

	m_color = CColorParameterSet::Translate(sColor);

	if (m_color != eNone)
	{
		m_pWindow->SetValue(m_pSliderColorH, m_pColorParameterSet->GetColorParameters(m_color)[0]);
		m_pWindow->SetValue(m_pSliderColorHT, m_pColorParameterSet->GetColorParameters(m_color)[1]);
		m_pWindow->SetValue(m_pSliderColorMinS, m_pColorParameterSet->GetColorParameters(m_color)[2]);
		m_pWindow->SetValue(m_pSliderColorMaxS, m_pColorParameterSet->GetColorParameters(m_color)[3]);
		m_pWindow->SetValue(m_pSliderColorMinV, m_pColorParameterSet->GetColorParameters(m_color)[4]);
		m_pWindow->SetValue(m_pSliderColorMaxV, m_pColorParameterSet->GetColorParameters(m_color)[5]);
	}
	else
	{
		m_pWindow->SetValue(m_pSliderColorH, custom_par1);
		m_pWindow->SetValue(m_pSliderColorHT, custom_par2);
		m_pWindow->SetValue(m_pSliderColorMinS, custom_par3);
		m_pWindow->SetValue(m_pSliderColorMaxS, custom_par4);
		m_pWindow->SetValue(m_pSliderColorMinV, custom_par5);
		m_pWindow->SetValue(m_pSliderColorMaxV, custom_par6);
	}
	
	m_bUpdateParameterSet = true;
}
	
void CObjectFinderOrganizer::ButtonPushed(WIDGET_HANDLE widget)
{
	if (widget == m_pPrintColorParametersButton) PrintColorParametersButtonClicked();
	else if (widget == m_pLoadColorParametersButton) LoadColorParametersButtonClicked();
	else if (widget == m_pSaveColorParametersButton) SaveColorParametersButtonClicked();
	else if (widget == m_pLoadInputButton) LoadInputButtonClicked();
	else if (widget == m_pSnapshotButton) SnapshotButtonClicked();
	else if (widget == m_pCameraParameterFileBrowseButton) LoadCameraParameterFileButtonClicked();
	else if (widget == m_pColorParameterFileBrowseButton) ColorParameterFileBrowseButtonClicked();
	else if (widget == m_pLeftFileBrowseButton) LeftFileBrowseButtonClicked();
	else if (widget == m_pRightFileBrowseButton) RightFileBrowseButtonClicked();
}
void CObjectFinderOrganizer::ValueChanged(WIDGET_HANDLE widget, int value)
{
	if (widget == m_pComboBoxColor)
	{
		int val;
		m_pWindow->GetValue(m_pComboBoxColor, val);
		std::string sColorName;
		CColorParameterSet::Translate((ObjectColor)val, sColorName);
		ColorChanged(sColorName.c_str());
	}
	else if (widget == m_pSliderColorH || 
		 widget == m_pSliderColorHT || 
		 widget == m_pSliderColorMinS || 
		 widget == m_pSliderColorMaxS || 
		 widget == m_pSliderColorMinV || 
		 widget == m_pSliderColorMaxV)
	{
		SliderValueChanged(0);
	}
}
	
void CObjectFinderOrganizer::LoadCalibration(const char *pCameraParameterFilename)
{
	if (!pCameraParameterFilename)
		return;

	if (!m_pObjectFinderStereo->Init(pCameraParameterFilename))
	{
		printf("camera parameter file not found\nstereo mode not available\n");
		m_bStereoAvailable = false;
	}
	else
		m_bStereoAvailable = true;

	if (m_pCalibrationLeft->LoadCameraParameters(pCameraParameterFilename, 0))
	{
		if (m_pCalibrationRight->LoadCameraParameters(pCameraParameterFilename, 1))
		{
			m_pStereoCalibration = new CStereoCalibration();
			
			if (!m_pStereoCalibration->LoadCameraParameters(pCameraParameterFilename))
				m_bStereoAvailable = false;
		}
		else
			m_bStereoAvailable = false;
	}
}


int CObjectFinderOrganizer::Run(int argc, char **argv)
{
	if (argc > 0 && argv[0])
	{
		const int nLength = strlen(argv[0]);
		
		for (int i = nLength - 1; i >= 0; i--)
		{
			if (argv[0][i] == '/' || argv[0][i] == '\\')
			{
				argv[0][i] = '\0';
				break;
			}
		}
	}
	
	std::string sIVTBaseDirectory = "";
	sIVTBaseDirectory += "../../";

	const int width = 640;
	const int height = 480;

	LoadCalibration((sIVTBaseDirectory + DEFAULT_CAMERA_PARAMETER_RELATIVE_FILEPATH).c_str());

	CApplicationHandlerInterface *application_handler = CreateApplicationHandler();
	application_handler->Reset();
	
	m_pWindow = CreateMainWindow(0, 0, width + 520, height, "HSV Color Segmentation Application");
	m_pWindow->SetEventCallback(this);
	
	// image widget
	WIDGET_HANDLE pImageWidget = m_pWindow->AddImage(0, 0, width, height);
	
	// check boxes
	WIDGET_HANDLE pCheckBoxShowSegmentedImage = m_pWindow->AddCheckBox(width + 20, 20, 250, 20, "Show Segmented Image", false);
	WIDGET_HANDLE pCheckBoxUseStereo = m_pWindow->AddCheckBox(width + 20, 45, 250, 20, "Use Stereo", true);
	WIDGET_HANDLE pCheckBoxDrawEpipolarLines = m_pWindow->AddCheckBox(width + 20, 70, 250, 20, "Draw Epipolar Lines", true);
	WIDGET_HANDLE pCheckBoxShowRightImage = m_pWindow->AddCheckBox(width + 20, 95, 250, 20, "Show Right Image", false);

	// slider
	m_pSliderColorH = m_pWindow->AddSlider(width + 335, 80, 180, 25, 0, 180, 10, 124);
	m_pSliderColorHT = m_pWindow->AddSlider(width + 335, 110, 180, 25, 0, 180, 10, 26);
	m_pSliderColorMinS = m_pWindow->AddSlider(width + 335, 140, 180, 25, 0, 255, 10, 109);
	m_pSliderColorMaxS = m_pWindow->AddSlider(width + 335, 170, 180, 25, 0, 255, 10, 255);
	m_pSliderColorMinV = m_pWindow->AddSlider(width + 335, 200, 180, 25, 0, 255, 10, 0);
	m_pSliderColorMaxV = m_pWindow->AddSlider(width + 335, 230, 180, 25, 0, 255, 10, 255);
	
	// combo boxes
	const char *entries1[3] = { "Bitmap, RGB 24 bit", "Bitmap Sequence, RGB 24 bit", "Uncompressed AVI, RGB 24 bit" };
	m_pComboBoxVideoSource = m_pWindow->AddComboBox(width + 20, 290, 250, 25, 3, entries1, 0);
	
	char *entries2[(int) eNumberOfColors];

	int i;
	for (i = 0; i < (int) eNumberOfColors; i++)
	{
		std::string sColorName;
		CColorParameterSet::Translate((ObjectColor) i, sColorName);
		
		char *ptr = new char[sColorName.length() + 1];
		strcpy(ptr, sColorName.c_str());
		entries2[i] = ptr;
	}

	m_pComboBoxColor = m_pWindow->AddComboBox(width + 335, 20, 180, 25, (int) eNumberOfColors, (const char **) entries2, 0);
	for (i = 0; i < (int) eNumberOfColors; i++)
		delete [] entries2[i];
	
	// text edit fields for file input
	m_pLineEditCameraParameterFile = m_pWindow->AddTextEdit(width + 20, 350, 250, 25, (sIVTBaseDirectory + DEFAULT_CAMERA_PARAMETER_RELATIVE_FILEPATH).c_str());
	m_pLineEditColorParameterFile = m_pWindow->AddTextEdit(width + 20, 380, 250, 25, (sIVTBaseDirectory + "files/colors.txt").c_str());
	m_pLineEditFileLeft = m_pWindow->AddTextEdit(width + 20, 410, 250, 25, (sIVTBaseDirectory + "files/scene_left.bmp").c_str());
	m_pLineEditFileRight = m_pWindow->AddTextEdit(width + 20, 440, 250, 25, (sIVTBaseDirectory + "files/scene_right.bmp").c_str());
	WIDGET_HANDLE pLineEditMinZDistance = m_pWindow->AddTextEdit(width + 380, 260, 135, 25, "500");
	WIDGET_HANDLE pLineEditMaxZDistance = m_pWindow->AddTextEdit(width + 380, 290, 135, 25, "3000");
	
	// buttons
	m_pPrintColorParametersButton = m_pWindow->AddButton(width + 20, 140, 250, 25, "Print Color Parameters");
	m_pLoadColorParametersButton = m_pWindow->AddButton(width + 20, 170, 250, 25, "Load Color Parameters");
	m_pSaveColorParametersButton = m_pWindow->AddButton(width + 20, 200, 250, 25, "Save Color Parameters");
	m_pLoadInputButton = m_pWindow->AddButton(width + 20, 230, 250, 25, "Load Input");
	m_pSnapshotButton = m_pWindow->AddButton(width + 20, 260, 250, 25, "Snapshot");
	
	m_pCameraParameterFileBrowseButton = m_pWindow->AddButton(width + 280, 350, 230, 25, "Load Camera Parameter File");
	m_pColorParameterFileBrowseButton = m_pWindow->AddButton(width + 280, 380, 230, 25, "Browse Color Parameter File");
	m_pLeftFileBrowseButton = m_pWindow->AddButton(width + 280, 410, 230, 25, "Browse Left Input");
	m_pRightFileBrowseButton = m_pWindow->AddButton(width + 280, 440, 230, 25, "Browse Right Input");
	
	// labels
	m_pWindow->AddLabel(width + 280, 80, 50, 25, "H");
	m_pWindow->AddLabel(width + 280, 110, 50, 25, "H Tol");
	m_pWindow->AddLabel(width + 280, 140, 50, 25, "S Min");
	m_pWindow->AddLabel(width + 280, 170, 50, 25, "S Max");
	m_pWindow->AddLabel(width + 280, 200, 50, 25, "V Min");
	m_pWindow->AddLabel(width + 280, 230, 50, 25, "V Max");
	m_pWindow->AddLabel(width + 280, 260, 100, 25, "Min Z (mm)");
	m_pWindow->AddLabel(width + 280, 290, 100, 25, "Max Z (mm)");
	m_pWindow->AddLabel(width + 280, 20, 50, 25, "Color");

	// initialize custom_par1 - custom_par6
	SliderValueChanged(0);
	
	// set color to blue
	std::string sBlue;
	CColorParameterSet::Translate(eBlue, sBlue);
	ColorChanged(sBlue.c_str());
	m_pWindow->SetValue(m_pComboBoxColor, (int)eBlue);
	
	m_pWindow->Show();
	
	// load input
	int val;
	m_pWindow->GetValue(pCheckBoxUseStereo, val);
	m_bUseStereo = (val != 0);
	#ifndef USE_CAMERA
	LoadInput();
	#endif
	
	LoadColorParametersButtonClicked();
	
	while (true)
	{
		// use stereo
		m_pWindow->GetValue(pCheckBoxUseStereo, val);
		m_bUseStereo = (val != 0);
		if (m_bUseStereo)
		{
			m_pWindow->Show(pCheckBoxShowRightImage);
			m_pWindow->Show(pCheckBoxDrawEpipolarLines);
		}
		else
		{
			m_pWindow->Hide(pCheckBoxShowRightImage);
			m_pWindow->Hide(pCheckBoxDrawEpipolarLines);
		}
		
		if (m_pVideoCapture && (m_bSnapshot || m_pVideoCapture->CaptureImage(m_ppImages)))
		{
			// query check boxes
			const int nMinPointsPerRegion = int(400 / ((640.0 * 480.0) / (m_ppImages[0]->width * m_ppImages[1]->height)));

			if (m_bStereoAvailable && m_bUseStereo)
			{			
				ImageProcessor::CopyImage(m_ppImages[0], m_ppResultImages[0]);
				ImageProcessor::CopyImage(m_ppImages[1], m_ppResultImages[1]);
				
				// update color parameter set (for sliders)
				m_pObjectFinderStereo->SetColorParameterSet(m_pColorParameterSet);
				m_pObjectFinderStereo->PrepareImages(m_ppImages);
				m_pObjectFinderStereo->ClearObjectList();
				
				m_pWindow->GetValue(pCheckBoxShowSegmentedImage, val);
				bool bShowSegmentedImage = (val != 0);
				if (m_color != eNone)
				{
					m_pObjectFinderStereo->FindObjects(m_ppImages, m_ppResultImages, m_color, nMinPointsPerRegion, bShowSegmentedImage);
				}
				else
				{
					int custom_par1, custom_par2, custom_par3, custom_par4, custom_par5, custom_par6;
		
					m_pWindow->GetValue(m_pSliderColorH, custom_par1);
					m_pWindow->GetValue(m_pSliderColorHT, custom_par2);
					m_pWindow->GetValue(m_pSliderColorMinS, custom_par3);
					m_pWindow->GetValue(m_pSliderColorMaxS, custom_par4);
					m_pWindow->GetValue(m_pSliderColorMinV, custom_par5);
					m_pWindow->GetValue(m_pSliderColorMaxV, custom_par6);
					
					m_pObjectFinderStereo->FindObjects(m_ppImages, m_ppResultImages, m_color, custom_par1, custom_par2, custom_par3, custom_par4, custom_par5, custom_par6, nMinPointsPerRegion, bShowSegmentedImage);
				}
				
				char buf[64];
				m_pWindow->GetText(pLineEditMinZDistance, buf, 64);
				const float fMinZ = (float) atof(buf);
				m_pWindow->GetText(pLineEditMaxZDistance, buf, 64);
				const float fMaxZ = (float) atof(buf);
				int nObjects = m_pObjectFinderStereo->Finalize(fMinZ, fMaxZ, false);

				Object3DList &objectList = m_pObjectFinderStereo->m_objectList;
				PointPair2d epipolarLine;
					
				m_pWindow->GetValue(pCheckBoxDrawEpipolarLines, val);
				bool bDrawEpipolarLines = (val != 0);
				m_pWindow->GetValue(pCheckBoxShowRightImage, val);
				bool bShowRightImage = (val != 0);
				
				// visualization
				for (int j = 0; j < nObjects; j++)
				{
					Object3DEntry &objectEntry = objectList.at(j);
			
					PrimitivesDrawer::DrawCircle(m_ppResultImages[0], objectEntry.region_left.centroid, 5, 255, 0, 255, -1);
					PrimitivesDrawer::DrawCircle(m_ppResultImages[1], objectEntry.region_right.centroid, 5, 255, 0, 255, -1);
					printf("object found at: %f %f %f\n", objectEntry.world_point.x, objectEntry.world_point.y, objectEntry.world_point.z);
									
					if (bDrawEpipolarLines)
					{
						m_pStereoCalibration->CalculateEpipolarLineInLeftImage(objectEntry.region_right.centroid, epipolarLine);
						PrimitivesDrawer::DrawLine(m_ppResultImages[0], epipolarLine);
						m_pStereoCalibration->CalculateEpipolarLineInRightImage(objectEntry.region_left.centroid, epipolarLine);
						PrimitivesDrawer::DrawLine(m_ppResultImages[1], epipolarLine);
					}
				}
		
				if (bShowRightImage)
					m_pWindow->SetImage(pImageWidget, m_ppResultImages[1]);
				else
					m_pWindow->SetImage(pImageWidget, m_ppResultImages[0]);
			}
			else
			{
				ImageProcessor::CopyImage(m_ppImages[0], m_ppResultImages[0]);

				// update color parameter set (for sliders)
				m_pObjectFinder->SetColorParameterSet(m_pColorParameterSet);

				m_pObjectFinder->PrepareImages(m_ppImages[0]);
				
				m_pObjectFinder->ClearObjectList();
				
				m_pWindow->GetValue(pCheckBoxShowSegmentedImage, val);
				bool bShowSegmentedImage = (val != 0);
				if (m_color != eNone)
				{
					m_pObjectFinder->FindObjects(m_ppImages[0], m_ppResultImages[0], m_color, nMinPointsPerRegion, bShowSegmentedImage);
				}
				else
				{
					int custom_par1, custom_par2, custom_par3, custom_par4, custom_par5, custom_par6;
		
					m_pWindow->GetValue(m_pSliderColorH, custom_par1);
					m_pWindow->GetValue(m_pSliderColorHT, custom_par2);
					m_pWindow->GetValue(m_pSliderColorMinS, custom_par3);
					m_pWindow->GetValue(m_pSliderColorMaxS, custom_par4);
					m_pWindow->GetValue(m_pSliderColorMinV, custom_par5);
					m_pWindow->GetValue(m_pSliderColorMaxV, custom_par6);
					
					m_pObjectFinder->FindObjects(m_ppImages[0], m_ppResultImages[0], m_color, custom_par1, custom_par2, custom_par3, custom_par4, custom_par5, custom_par6, nMinPointsPerRegion, bShowSegmentedImage);
				}
					
				int nObjects = m_pObjectFinder->Finalize();
				
				Object2DList &objectList = m_pObjectFinder->m_objectList;
				
				// visualization
				for (int j = 0; j < nObjects; j++)
				{
					Object2DEntry &objectEntry = objectList.at(j);
					PrimitivesDrawer::DrawCircle(m_ppResultImages[0], objectEntry.region.centroid, 5, 0, 0, 0, -1);
				}
				
				m_pWindow->SetImage(pImageWidget, m_ppResultImages[0]);
			}
		}
		else if (m_pVideoCapture)
		{
			ImageProcessor::Zero(m_ppResultImages[0]);
			m_pWindow->SetImage(pImageWidget, m_ppResultImages[0]);
		}
		
		if (application_handler->ProcessEventsAndGetExit())
			break;
	}

	delete m_pWindow;
	delete application_handler;
	
	m_pWindow = 0;
	
	return 0;
}


bool CObjectFinderOrganizer::LoadInput()
{
	char pFileNameLeft[1024];
	char pFileNameRight[1024];
	m_pWindow->GetText(m_pLineEditFileLeft, pFileNameLeft, 1024);
	m_pWindow->GetText(m_pLineEditFileRight, pFileNameRight, 1024);

	m_bSnapshot = false;

	if (m_pVideoCapture)
	{
		delete m_pVideoCapture;
		m_pVideoCapture = 0;
	}
	
	int currentItem;
	m_pWindow->GetValue(m_pComboBoxVideoSource, currentItem);
	switch (currentItem)
	{
		case eAVI:
			if (!pFileNameLeft || m_bUseStereo && !pFileNameRight)
				return false;
			m_pVideoCapture = new CUncompressedAVICapture(pFileNameLeft, m_bUseStereo ? pFileNameRight : 0);
			if (!m_pVideoCapture->OpenCamera())
			{
				printf("error: could not open camera\n");
				delete m_pVideoCapture;
				m_pVideoCapture = 0;
				return false;
			}
		break;
		
		case eBitmap:
			if (!pFileNameLeft || m_bUseStereo && !pFileNameRight)
				return false;
			m_pVideoCapture = new CBitmapCapture(pFileNameLeft, m_bUseStereo ? pFileNameRight : 0);
			if (!m_pVideoCapture->OpenCamera())
			{
				printf("error: could not open camera\n");
				delete m_pVideoCapture;
				m_pVideoCapture = 0;
				return false;
			}
		break;

		case eBitmapSequence:
			if (!pFileNameLeft || m_bUseStereo && !pFileNameRight)
				return false;
			m_pVideoCapture = new CBitmapSequenceCapture(pFileNameLeft, m_bUseStereo ? pFileNameRight : 0);
			if (!m_pVideoCapture->OpenCamera())
			{
				printf("error: could not open camera\n");
				delete m_pVideoCapture;
				m_pVideoCapture = 0;
				return false;
			}
		break;
	}
	
	if (m_ppImages[0])
	{
		delete m_ppImages[0];
		delete m_ppImages[1];
		delete m_ppResultImages[0];
		delete m_ppResultImages[1];
	}
	
	const int width = m_pVideoCapture->GetWidth();
	const int height = m_pVideoCapture->GetHeight();
	const CByteImage::ImageType type = m_pVideoCapture->GetType();
	
	m_ppImages[0] = new CByteImage(width, height, type);
	m_ppImages[1] = new CByteImage(width, height, type);
	m_ppResultImages[0] = new CByteImage(width, height, type);
	m_ppResultImages[1] = new CByteImage(width, height, type);

	return true;
}
