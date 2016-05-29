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
// Filename:  QTColorParameterWindow.cpp
// Author:    Pedram Azad
// Date:      2007
// ****************************************************************************
// Changes:   13.06.2008, Miguel Bernal Marin
//            * Updated code to be compatible with Qt4
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "QTColorParameterWindow.h"

#include "Color/ColorParameterSet.h"
#include "Image/ImageProcessor.h"
#include "Image/ByteImage.h"
#include "gui/QTWindow.h"
#include "gui/QTApplicationHandler.h"

#include <stdio.h>
#include <string>

#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qslider.h>
#include <qfiledialog.h>
#include <qlabel.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CQTColorParameterWindow::CQTColorParameterWindow(int width, int height) : CQTWindow(width + 450, height)
{
	this->width = width;
	this->height = height;
	
	// color parameter set
	m_pColorParameterSet = new CColorParameterSet();
	
	// pointers to line edit fields
	m_pLineEditColorParameterFile = 0;
	
	m_color = eNone;

	m_bUpdateParameterSet = true;
	
	// radio buttons
	QRadioButton *pCheckBoxColorSkin = new QRadioButton("Skin", this);
	pCheckBoxColorSkin->setFixedWidth(200);
	pCheckBoxColorSkin->move(width + 250, 20);
	
	QRadioButton *pCheckBoxColorYellow = new QRadioButton("Yellow", this);
	pCheckBoxColorYellow->setFixedWidth(200);
	pCheckBoxColorYellow->move(width + 250, 45);
	
	QRadioButton *pCheckBoxColorOrange = new QRadioButton("Orange", this);
	pCheckBoxColorOrange->setFixedWidth(200);
	pCheckBoxColorOrange->move(width + 250, 70);
	
	QRadioButton *pCheckBoxColorRed = new QRadioButton("Red", this);
	pCheckBoxColorRed->setFixedWidth(200);
	pCheckBoxColorRed->move(width + 250, 95);
	
	QRadioButton *pCheckBoxColorBlue = new QRadioButton("Blue", this);
	pCheckBoxColorBlue->setFixedWidth(200);
	pCheckBoxColorBlue->move(width + 350, 20);
	
	QRadioButton *pCheckBoxColorGreen = new QRadioButton("Green", this);
	pCheckBoxColorGreen->setFixedWidth(200);
	pCheckBoxColorGreen->move(width + 350, 45);
	
	QRadioButton *pCheckBoxColorCustom = new QRadioButton("Custom", this);
	pCheckBoxColorCustom->setFixedWidth(200);
	pCheckBoxColorCustom->move(width + 350, 70);

	// check boxes
	m_pCheckBoxShowSegmentedImage = new QCheckBox("Show Segmented Image", this);
	m_pCheckBoxShowSegmentedImage->setChecked(false);
	m_pCheckBoxShowSegmentedImage->setFixedWidth(200);
	m_pCheckBoxShowSegmentedImage->move(width + 20, 120);

	m_pCheckBoxShowRightImage = new QCheckBox("Show Right Image", this);
	m_pCheckBoxShowRightImage->setChecked(false);
	m_pCheckBoxShowRightImage->setFixedWidth(200);
	m_pCheckBoxShowRightImage->move(width + 20, 145);

	// radio button group
	m_pColorButtonGroup = new QButtonGroup();

	#if QT_VERSION >= 0x040000
	m_pColorButtonGroup->addButton(pCheckBoxColorSkin);
	m_pColorButtonGroup->addButton(pCheckBoxColorYellow);
	m_pColorButtonGroup->addButton(pCheckBoxColorOrange);
	m_pColorButtonGroup->addButton(pCheckBoxColorRed);
	m_pColorButtonGroup->addButton(pCheckBoxColorBlue);
	m_pColorButtonGroup->addButton(pCheckBoxColorGreen);
	m_pColorButtonGroup->addButton(pCheckBoxColorCustom);
	#else
	m_pColorButtonGroup->insert(pCheckBoxColorSkin);
	m_pColorButtonGroup->insert(pCheckBoxColorYellow);
	m_pColorButtonGroup->insert(pCheckBoxColorOrange);
	m_pColorButtonGroup->insert(pCheckBoxColorRed);
	m_pColorButtonGroup->insert(pCheckBoxColorBlue);
	m_pColorButtonGroup->insert(pCheckBoxColorGreen);
	m_pColorButtonGroup->insert(pCheckBoxColorCustom);
	#endif
	
	// slider
	m_pSliderColorH = new QSlider(Qt::Horizontal, this);
	m_pSliderColorH->setRange(0, 180);
	m_pSliderColorH->setPageStep(1);
	m_pSliderColorH->setValue(124);
	m_pSliderColorH->setFixedWidth(180);
	m_pSliderColorH->setFixedHeight(20);
	m_pSliderColorH->move(width + 255, 130);
	
	m_pSliderColorHT = new QSlider(Qt::Horizontal, this);
	m_pSliderColorHT->setRange(0, 180);
	m_pSliderColorHT->setPageStep(1);
	m_pSliderColorHT->setValue(26);
	m_pSliderColorHT->setFixedWidth(180);
	m_pSliderColorHT->setFixedHeight(20);
	m_pSliderColorHT->move(width + 255, 155);
	
	m_pSliderColorMinS = new QSlider(Qt::Horizontal, this);
	m_pSliderColorMinS->setRange(0, 255);
	m_pSliderColorMinS->setPageStep(1);
	m_pSliderColorMinS->setValue(109);
	m_pSliderColorMinS->setFixedWidth(180);
	m_pSliderColorMinS->setFixedHeight(20);
	m_pSliderColorMinS->move(width + 255, 180);
	
	m_pSliderColorMaxS = new QSlider(Qt::Horizontal, this);
	m_pSliderColorMaxS->setRange(0, 255);
	m_pSliderColorMaxS->setPageStep(1);
	m_pSliderColorMaxS->setValue(255);
	m_pSliderColorMaxS->setFixedWidth(180);
	m_pSliderColorMaxS->setFixedHeight(20);
	m_pSliderColorMaxS->move(width + 255, 205);
	
	m_pSliderColorMinV = new QSlider(Qt::Horizontal, this);
	m_pSliderColorMinV->setRange(0, 255);
	m_pSliderColorMinV->setPageStep(1);
	m_pSliderColorMinV->setValue(0);
	m_pSliderColorMinV->setFixedWidth(180);
	m_pSliderColorMinV->setFixedHeight(20);
	m_pSliderColorMinV->move(width + 255, 230);
	
	m_pSliderColorMaxV = new QSlider(Qt::Horizontal, this);
	m_pSliderColorMaxV->setRange(0, 255);
	m_pSliderColorMaxV->setPageStep(1);
	m_pSliderColorMaxV->setValue(255);
	m_pSliderColorMaxV->setFixedWidth(180);
	m_pSliderColorMaxV->setFixedHeight(20);
	m_pSliderColorMaxV->move(width + 255, 255);
	
	// text edit fields for file input
	m_pLineEditColorParameterFile = new QLineEdit(this);
	m_pLineEditColorParameterFile->setText("files/colors_kitchen.txt");
	m_pLineEditColorParameterFile->setReadOnly(true);
	m_pLineEditColorParameterFile->setFixedWidth(250);
	m_pLineEditColorParameterFile->setFixedHeight(20);
	m_pLineEditColorParameterFile->move(width + 20, 390);

	// buttons
	QPushButton *pPrintColorParametersButton = new QPushButton(this);
	pPrintColorParametersButton->setText("Print Color Parameters");
	pPrintColorParametersButton->setFixedHeight(20);
	pPrintColorParametersButton->setFixedWidth(180);
	pPrintColorParametersButton->move(width + 20, 190);

	QPushButton *pLoadColorParametersButton = new QPushButton(this);
	pLoadColorParametersButton->setText("Load Color Parameters");
	pLoadColorParametersButton->setFixedHeight(20);
	pLoadColorParametersButton->setFixedWidth(180);
	pLoadColorParametersButton->move(width + 20, 215);

	QPushButton *pSaveColorParametersButton = new QPushButton(this);
	pSaveColorParametersButton->setText("Save Color Parameters");
	pSaveColorParametersButton->setFixedHeight(20);
	pSaveColorParametersButton->setFixedWidth(180);
	pSaveColorParametersButton->move(width + 20, 240);

	QPushButton *pColorParameterFileBrowseButton = new QPushButton(this);
	pColorParameterFileBrowseButton->setText("Browse Color Parameter File");
	pColorParameterFileBrowseButton->setFixedHeight(20);
	pColorParameterFileBrowseButton->setFixedWidth(155);
	pColorParameterFileBrowseButton->move(width + 280, 390);
	
	QLabel *pLabelH = new QLabel(this);
	pLabelH->setText("H");
	pLabelH->setFixedWidth(40);
	pLabelH->setFixedHeight(20);
	pLabelH->move(width + 210, 130);
	
	QLabel *pLabelHTol = new QLabel(this);
	pLabelHTol->setText("H Tol");
	pLabelHTol->setFixedWidth(40);
	pLabelHTol->setFixedHeight(20);
	pLabelHTol->move(width + 210, 155);
	
	QLabel *pLabelMinS = new QLabel(this);
	pLabelMinS->setText("S Min");
	pLabelMinS->setFixedWidth(40);
	pLabelMinS->setFixedHeight(20);
	pLabelMinS->move(width + 210, 180);
	
	QLabel *pLabelMaxS = new QLabel(this);
	pLabelMaxS->setText("S Max");
	pLabelMaxS->setFixedWidth(40);
	pLabelMaxS->setFixedHeight(20);
	pLabelMaxS->move(width + 210, 205);
	
	QLabel *pLabelMinV = new QLabel(this);
	pLabelMinV->setText("V Min");
	pLabelMinV->setFixedWidth(40);
	pLabelMinV->setFixedHeight(20);
	pLabelMinV->move(width + 210, 230);
	
	QLabel *pLabelMaxV = new QLabel(this);
	pLabelMaxV->setText("V Max");
	pLabelMaxV->setFixedWidth(40);
	pLabelMaxV->setFixedHeight(20);
	pLabelMaxV->move(width + 210, 255);

	// initialize custom_par1 - custom_par6
	SliderValueChanged(0);
	
	// set color to red
	ColorRadioButtonClicked(3);
	#if QT_VERSION >= 0x040000
	m_pColorButtonGroup->button(3)->click();
        #else
	m_pColorButtonGroup->setButton(3);
        #endif
	
	// signal/slots
	connect(m_pColorButtonGroup, SIGNAL(clicked(int)), this, SLOT(ColorRadioButtonClicked(int)));
	connect(pPrintColorParametersButton, SIGNAL(clicked()), this, SLOT(PrintColorParametersButtonClicked()));
	connect(pLoadColorParametersButton, SIGNAL(clicked()), this, SLOT(LoadColorParametersButtonClicked()));
	connect(pSaveColorParametersButton, SIGNAL(clicked()), this, SLOT(SaveColorParametersButtonClicked()));
	connect(pColorParameterFileBrowseButton, SIGNAL(clicked()), this, SLOT(ColorParameterFileBrowseButtonClicked()));
	connect(m_pSliderColorH, SIGNAL(valueChanged(int)), this, SLOT(SliderValueChanged(int)));
	connect(m_pSliderColorHT, SIGNAL(valueChanged(int)), this, SLOT(SliderValueChanged(int)));
	connect(m_pSliderColorMinS, SIGNAL(valueChanged(int)), this, SLOT(SliderValueChanged(int)));
	connect(m_pSliderColorMaxS, SIGNAL(valueChanged(int)), this, SLOT(SliderValueChanged(int)));
	connect(m_pSliderColorMinV, SIGNAL(valueChanged(int)), this, SLOT(SliderValueChanged(int)));
	connect(m_pSliderColorMaxV, SIGNAL(valueChanged(int)), this, SLOT(SliderValueChanged(int)));
}

CQTColorParameterWindow::~CQTColorParameterWindow()
{
	delete m_pColorParameterSet;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CQTColorParameterWindow::LoadColorParametersButtonClicked()
{
	#if QT_VERSION >= 0x040000
	m_pColorParameterSet->LoadFromFile(m_pLineEditColorParameterFile->text().toLatin1().constData());
        QAbstractButton *pActiveButton = m_pColorButtonGroup->checkedButton();	
        #else
	m_pColorParameterSet->LoadFromFile(m_pLineEditColorParameterFile->text().latin1());
	QButton *pActiveButton = m_pColorButtonGroup->selected();
	#endif
	
	if (pActiveButton)
	{
		ColorRadioButtonClicked(m_pColorButtonGroup->id(pActiveButton));
	}
}

void CQTColorParameterWindow::SaveColorParametersButtonClicked()
{
	
	#if QT_VERSION >= 0x040000
	m_pColorParameterSet->SaveToFile(m_pLineEditColorParameterFile->text().toLatin1().constData());
        #else
	m_pColorParameterSet->SaveToFile(m_pLineEditColorParameterFile->text().latin1());
	#endif
}

void CQTColorParameterWindow::PrintColorParametersButtonClicked()
{
	printf("color parameters: (%i, %i, %i, %i, %i, %i)\n", m_pSliderColorH->value(), m_pSliderColorHT->value(), m_pSliderColorMinS->value(), m_pSliderColorMaxS->value(), m_pSliderColorMinV->value(), m_pSliderColorMaxV->value());
}

void CQTColorParameterWindow::ColorParameterFileBrowseButtonClicked()
{
	#if QT_VERSION >= 0x040000
	QString s = QFileDialog::getOpenFileName(this,tr("Choose a file"),"",tr("Text (*.txt)") );
	#else
	QString s = QFileDialog::getOpenFileName("", "Text (*.txt)", this, "Open File Dialog", "Choose a file");
	#endif

	if (s.length() > 0)
		m_pLineEditColorParameterFile->setText(s);
}

void CQTColorParameterWindow::SliderValueChanged(int nValue)
{
	if (!m_bUpdateParameterSet)
		return;
	
	if (m_color == eNone)
	{
		custom_par1 = m_pSliderColorH->value();
		custom_par2 = m_pSliderColorHT->value();
		custom_par3 = m_pSliderColorMinS->value();
		custom_par4 = m_pSliderColorMaxS->value();
		custom_par5 = m_pSliderColorMinV->value();
		custom_par6 = m_pSliderColorMaxV->value();
	}
	else
	{
		m_pColorParameterSet->SetColorParameters(m_color,
			m_pSliderColorH->value(),
			m_pSliderColorHT->value(),
			m_pSliderColorMinS->value(),
			m_pSliderColorMaxS->value(),
			m_pSliderColorMinV->value(),
			m_pSliderColorMaxV->value()
		);
	}
}

void CQTColorParameterWindow::ColorRadioButtonClicked(int id)
{
	m_bUpdateParameterSet = false;
	
	if (id >= 0 && id <= 5)
	{
		switch (id)
		{
			case 0: m_color = eSkin; break;
			case 1: m_color = eYellow; break;
			case 2: m_color = eOrange; break;
			case 3: m_color = eRed; break;
			case 4: m_color = eBlue; break;
			case 5: m_color = eGreen; break;
		}
		
		m_pSliderColorH->setValue(m_pColorParameterSet->GetColorParameters(m_color)[0]);
		m_pSliderColorHT->setValue(m_pColorParameterSet->GetColorParameters(m_color)[1]);
		m_pSliderColorMinS->setValue(m_pColorParameterSet->GetColorParameters(m_color)[2]);
		m_pSliderColorMaxS->setValue(m_pColorParameterSet->GetColorParameters(m_color)[3]);
		m_pSliderColorMinV->setValue(m_pColorParameterSet->GetColorParameters(m_color)[4]);
		m_pSliderColorMaxV->setValue(m_pColorParameterSet->GetColorParameters(m_color)[5]);
	}
	else if (id == 6)
	{
		m_pSliderColorH->setValue(custom_par1);
		m_pSliderColorHT->setValue(custom_par2);
		m_pSliderColorMinS->setValue(custom_par3);
		m_pSliderColorMaxS->setValue(custom_par4);
		m_pSliderColorMinV->setValue(custom_par5);
		m_pSliderColorMaxV->setValue(custom_par6);
		m_color = eNone;
	}

	m_bUpdateParameterSet = true;
}

void CQTColorParameterWindow::Update(const CByteImage * const *ppInputImages, int nImages)
{
	const int nIndex = (nImages == 1 || !m_pCheckBoxShowRightImage->isChecked()) ? 0 : 1;
	
	CByteImage hsv_image(ppInputImages[nIndex]);
	CByteImage result_image(ppInputImages[nIndex]->width, ppInputImages[nIndex]->height, CByteImage::eGrayScale);
	
	ImageProcessor::CalculateHSVImage(ppInputImages[nIndex], &hsv_image);
	ImageProcessor::FilterHSV(&hsv_image, &result_image, m_pSliderColorH->value(), m_pSliderColorHT->value(), m_pSliderColorMinS->value(), m_pSliderColorMaxS->value(), m_pSliderColorMinV->value(), m_pSliderColorMaxV->value());
	ImageProcessor::Erode(&result_image, &result_image);
	ImageProcessor::Dilate(&result_image, &result_image);
	
	if (m_pCheckBoxShowSegmentedImage->isChecked())
		DrawImage(&result_image);
	else
		DrawImage(ppInputImages[nIndex]);
}

const CColorParameterSet* CQTColorParameterWindow::GetColorParameterSet() const
{
	return m_pColorParameterSet;
}
