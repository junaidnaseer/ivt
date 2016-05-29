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
// Filename:  QTColorParameterWindow.h
// Author:    Pedram Azad
// Date:      2007
// ****************************************************************************


#ifndef _QT_COLOR_PARAMETER_WINDOW_H_
#define _QT_COLOR_PARAMETER_WINDOW_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "QTWindow.h"
#include "Structs/ObjectDefinitions.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CColorParameterSet;
class CByteImage;
class QLineEdit;
class QSlider;
class QButtonGroup;
class QCheckBox;


// ****************************************************************************
// Enums
// ****************************************************************************

enum VideoSource
{
	eBitmap,
	eBitmapSequence,
	eAVI
};

enum ObjectClassifier
{
	eHandHeadClassifier
};



// ****************************************************************************
// CQTColorParameterWindow
// ****************************************************************************

class CQTColorParameterWindow : public CQTWindow
{
	Q_OBJECT
public:
	// constructor
	CQTColorParameterWindow(int width, int height);
	
	// destructor
	~CQTColorParameterWindow();
	
	
	// public methods
	void Update(const CByteImage * const *ppInputImages, int nImages);
	const CColorParameterSet* GetColorParameterSet() const;

private slots:
	void ColorRadioButtonClicked(int id);
	void PrintColorParametersButtonClicked();
	void LoadColorParametersButtonClicked();
	void SaveColorParametersButtonClicked();
	void ColorParameterFileBrowseButtonClicked();
	void SliderValueChanged(int nValue);
	
private:
	// private attributes
	QLineEdit *m_pLineEditColorParameterFile;
	QSlider *m_pSliderColorH;
	QSlider *m_pSliderColorHT;
	QSlider *m_pSliderColorMinS;
	QSlider *m_pSliderColorMaxS;
	QSlider *m_pSliderColorMinV;
	QSlider *m_pSliderColorMaxV;
	QButtonGroup *m_pColorButtonGroup;
	QCheckBox *m_pCheckBoxShowSegmentedImage;
	QCheckBox *m_pCheckBoxShowRightImage;

	ObjectColor m_color;
	CColorParameterSet *m_pColorParameterSet;
	
	bool m_bUpdateParameterSet;
	int custom_par1, custom_par2, custom_par3, custom_par4, custom_par5, custom_par6;
	
	int width, height;
};



#endif /* _QT_COLOR_PARAMETER_WINDOW_H_ */
