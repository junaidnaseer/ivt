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
// Filename:  ObjectFinderOrganizer.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _OBJECT_FINDER_ORGANIZER_H_
#define _OBJECT_FINDER_ORGANIZER_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Structs/ObjectDefinitions.h"

#include "Interfaces/MainWindowEventInterface.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CObjectFinder;
class CObjectFinderStereo;
class CRegionFilterInterface;
class CVideoCaptureInterface;
class CColorParameterSet;
class CByteImage;
class CCalibration;
class CStereoCalibration;


// ****************************************************************************
// Enums
// ****************************************************************************

enum VideoSource
{
	eBitmap,
	eBitmapSequence,
	eAVI
};



// ****************************************************************************
// CObjectFinderOrganizer
// ****************************************************************************

class CObjectFinderOrganizer : public CMainWindowEventInterface
{
public:
	// constructor
	CObjectFinderOrganizer();
	
	// destructor
	~CObjectFinderOrganizer();
	
	
	// public methods
	int Run(int argc, char **argv);


private:
	// private methods
	bool LoadInput();

	void ColorChanged(const char *sColor);
	void PrintColorParametersButtonClicked();
	void LoadColorParametersButtonClicked();
	void SaveColorParametersButtonClicked();
	void LoadInputButtonClicked();
	void SnapshotButtonClicked();
	void LoadCameraParameterFileButtonClicked();
	void ColorParameterFileBrowseButtonClicked();
	void RightFileBrowseButtonClicked();
	void LeftFileBrowseButtonClicked();
	void SliderValueChanged(int nValue);
	
	void ButtonPushed(WIDGET_HANDLE widget);
	void ValueChanged(WIDGET_HANDLE widget, int value);
	
private:
	// private methods
	void LoadCalibration(const char *pCameraParameterFilename);

	// private attributes
	CObjectFinder *m_pObjectFinder;
	CObjectFinderStereo *m_pObjectFinderStereo;
	CRegionFilterInterface *m_pCompactRegionFilter;

	CVideoCaptureInterface *m_pVideoCapture;
	
	WIDGET_HANDLE m_pLineEditFileLeft;
	WIDGET_HANDLE m_pLineEditFileRight;
	WIDGET_HANDLE m_pLineEditCameraParameterFile;
	WIDGET_HANDLE m_pLineEditColorParameterFile;
	WIDGET_HANDLE m_pSliderColorH;
	WIDGET_HANDLE m_pSliderColorHT;
	WIDGET_HANDLE m_pSliderColorMinS;
	WIDGET_HANDLE m_pSliderColorMaxS;
	WIDGET_HANDLE m_pSliderColorMinV;
	WIDGET_HANDLE m_pSliderColorMaxV;
	WIDGET_HANDLE m_pComboBoxVideoSource;
	WIDGET_HANDLE m_pComboBoxColor;
	
	WIDGET_HANDLE m_pPrintColorParametersButton;
	WIDGET_HANDLE m_pLoadColorParametersButton;
	WIDGET_HANDLE m_pSaveColorParametersButton;
	WIDGET_HANDLE m_pLoadInputButton;
	WIDGET_HANDLE m_pSnapshotButton;
	WIDGET_HANDLE m_pCameraParameterFileBrowseButton;
	WIDGET_HANDLE m_pColorParameterFileBrowseButton;
	WIDGET_HANDLE m_pLeftFileBrowseButton;
	WIDGET_HANDLE m_pRightFileBrowseButton;
	
	bool m_bStereoAvailable;
	bool m_bUseStereo;
	int m_nNewVideoSource;
	
	CCalibration *m_pCalibrationLeft;
	CCalibration *m_pCalibrationRight;
	CStereoCalibration *m_pStereoCalibration;
	
	CByteImage *m_ppImages[2];
	CByteImage *m_ppResultImages[2];
	
	//CQTWindow *m_pWindow;
	CMainWindowInterface *m_pWindow;

	ObjectColor m_color;
	CColorParameterSet *m_pColorParameterSet;

	bool m_bSnapshot;
	bool m_bUpdateParameterSet;
	int custom_par1, custom_par2, custom_par3, custom_par4, custom_par5, custom_par6;
};



#endif /* _OBJECT_FINDER_ORGANIZER_H_ */
