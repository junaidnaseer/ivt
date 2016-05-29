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
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "Interfaces/ApplicationHandlerInterface.h"
#include "Interfaces/MainWindowInterface.h"
#include "Interfaces/MainWindowEventInterface.h"
#include "gui/GUIFactory.h"
#include "Image/ByteImage.h"
#include "Helpers/helpers.h"
#include "Image/ImageProcessor.h"



// ****************************************************************************
// CHSVApp
// ****************************************************************************

class CHSVApp : public CMainWindowEventInterface
{
public:
	CHSVApp()
	{
		h = 0;
		s = 0;
		v = 0;
		
		r = 0;
		g = 0;
		b = 0;
	}
	
	// this is called when ever the value of a slider changes
	void ValueChanged(WIDGET_HANDLE widget, int value)
	{
		bool recreate_image = false;
		
		if (widget == m_pSliderH)
		{
			h = value;
			hsv2rgb(h, s, v, r, g, b);
			recreate_image = true;
			
			m_pMainWindow->SetValue(m_pSliderR, r);
			m_pMainWindow->SetValue(m_pSliderG, g);
			m_pMainWindow->SetValue(m_pSliderB, b);
		}
		else if (widget == m_pSliderS)
		{
			s = value;
			hsv2rgb(h, s, v, r, g, b);
			recreate_image = true;
			
			m_pMainWindow->SetValue(m_pSliderR, r);
			m_pMainWindow->SetValue(m_pSliderG, g);
			m_pMainWindow->SetValue(m_pSliderB, b);
		}
		else if (widget == m_pSliderV)
		{
			v = value;
			hsv2rgb(h, s, v, r, g, b);
			recreate_image = true;
			
			m_pMainWindow->SetValue(m_pSliderR, r);
			m_pMainWindow->SetValue(m_pSliderG, g);
			m_pMainWindow->SetValue(m_pSliderB, b);
		}
		else if (widget == m_pSliderR)
		{
			r = value;
			rgb2hsv(r, g, b, h, s, v);
			recreate_image = true;
			
			m_pMainWindow->SetValue(m_pSliderH, h);
			m_pMainWindow->SetValue(m_pSliderS, s);
			m_pMainWindow->SetValue(m_pSliderV, v);
		}
		else if (widget == m_pSliderG)
		{
			g = value;
			rgb2hsv(r, g, b, h, s, v);
			recreate_image = true;
			
			m_pMainWindow->SetValue(m_pSliderH, h);
			m_pMainWindow->SetValue(m_pSliderS, s);
			m_pMainWindow->SetValue(m_pSliderV, v);
		}
		else if (widget == m_pSliderB)
		{
			b = value;
			rgb2hsv(r, g, b, h, s, v);
			recreate_image = true;
			
			m_pMainWindow->SetValue(m_pSliderH, h);
			m_pMainWindow->SetValue(m_pSliderS, s);
			m_pMainWindow->SetValue(m_pSliderV, v);
		}
		
		if (recreate_image)
		{
			int nPixels = m_pImage->width * m_pImage->height;
			for (int j = 0, offset = 0; j < nPixels; j++, offset += 3)
			{
				m_pImage->pixels[offset] = r;
				m_pImage->pixels[offset + 1] = g;
				m_pImage->pixels[offset + 2] = b;
			}
		
			m_pMainWindow->SetImage(m_pImageWidget, m_pImage);
			
			char buf[1024];
			sprintf(buf, "%d", h);
			m_pMainWindow->SetText(m_pLCD_H, buf);
			sprintf(buf, "%d", s);
			m_pMainWindow->SetText(m_pLCD_S, buf);
			sprintf(buf, "%d", v);
			m_pMainWindow->SetText(m_pLCD_V, buf);
			
			sprintf(buf, "%d", r);
			m_pMainWindow->SetText(m_pLCD_R, buf);
			sprintf(buf, "%d", g);
			m_pMainWindow->SetText(m_pLCD_G, buf);
			sprintf(buf, "%d", b);
			m_pMainWindow->SetText(m_pLCD_B, buf);
		}
	}
	
	int Run()
	{
		const int width = 640;
		const int height = 480;
		const int nPixels = width * height;
		
		m_pImage = new CByteImage(width, height, CByteImage::eRGB24);
		ImageProcessor::Zero(m_pImage);
		
		// create an application handler
		CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
		pApplicationHandler->Reset();
		
		// create a window with many widgets
		m_pMainWindow = CreateMainWindow(0, 0, width + 440, height, "HSV to RGB Converter");
		m_pMainWindow->SetEventCallback(this);
		
		m_pImageWidget = m_pMainWindow->AddImage(0, 0, width, height);
		
		m_pLCD_H = m_pMainWindow->AddTextEdit(width + 50 , 230, 100, 50, "0");
		m_pLCD_S = m_pMainWindow->AddTextEdit(width + 160, 230, 100, 50, "0");
		m_pLCD_V = m_pMainWindow->AddTextEdit(width + 270, 230, 100, 50, "0");
		
		m_pLCD_R = m_pMainWindow->AddTextEdit(width + 50 , 290, 100, 50, "0");
		m_pLCD_G = m_pMainWindow->AddTextEdit(width + 160, 290, 100, 50, "0");
		m_pLCD_B = m_pMainWindow->AddTextEdit(width + 270, 290, 100, 50, "0");
		
		m_pSliderH = m_pMainWindow->AddSlider(width + 40, 20, 380, 20, 0, 359, 36, 0);
		m_pSliderS = m_pMainWindow->AddSlider(width + 40, 50, 380, 20, 0, 255, 25, 0);
		m_pSliderV = m_pMainWindow->AddSlider(width + 40, 80, 380, 20, 0, 255, 25, 0);
		
		m_pSliderR = m_pMainWindow->AddSlider(width + 40, 120, 380, 20, 0, 255, 25, 0);
		m_pSliderG = m_pMainWindow->AddSlider(width + 40, 150, 380, 20, 0, 255, 25, 0);
		m_pSliderB = m_pMainWindow->AddSlider(width + 40, 180, 380, 20, 0, 255, 25, 0);
		
		WIDGET_HANDLE pLabelH = m_pMainWindow->AddLabel(width + 20, 20, 10, 20, "H");
		WIDGET_HANDLE pLabelS = m_pMainWindow->AddLabel(width + 20, 50, 10, 20, "S");
		WIDGET_HANDLE pLabelV = m_pMainWindow->AddLabel(width + 20, 80, 10, 20, "V");
		
		WIDGET_HANDLE pLabelR = m_pMainWindow->AddLabel(width + 20, 120, 10, 20, "R");
		WIDGET_HANDLE pLabelG = m_pMainWindow->AddLabel(width + 20, 150, 10, 20, "G");
		WIDGET_HANDLE pLabelB = m_pMainWindow->AddLabel(width + 20, 180, 10, 20, "B");
		
		WIDGET_HANDLE pLabelHSV = m_pMainWindow->AddLabel(width + 20, 245, 30, 20, "HSV");
		WIDGET_HANDLE pLabelRGB = m_pMainWindow->AddLabel(width + 20, 305, 30, 20, "RGB");
		
		m_pMainWindow->Show();
		
		m_pMainWindow->SetImage(m_pImageWidget, m_pImage);
	
		// main loop
		for (int i = 0;; i++)
		{
			if (pApplicationHandler->ProcessEventsAndGetExit())
				break;
		}
		
		delete m_pMainWindow;
		delete pApplicationHandler;
		
		delete m_pImage;
		
		return 0;
	}
	
public:
	int h, s, v;
	int r, g, b;
	
	CByteImage *m_pImage;
	
	CMainWindowInterface *m_pMainWindow;
	
	WIDGET_HANDLE m_pImageWidget;
	
	WIDGET_HANDLE m_pSliderH;
	WIDGET_HANDLE m_pSliderS;
	WIDGET_HANDLE m_pSliderV;
	
	WIDGET_HANDLE m_pSliderR;
	WIDGET_HANDLE m_pSliderG;
	WIDGET_HANDLE m_pSliderB;
	
	WIDGET_HANDLE m_pLCD_H;
	WIDGET_HANDLE m_pLCD_S;
	WIDGET_HANDLE m_pLCD_V;
	
	WIDGET_HANDLE m_pLCD_R;
	WIDGET_HANDLE m_pLCD_G;
	WIDGET_HANDLE m_pLCD_B;
};


int main()
{
	CHSVApp app;
	return app.Run();
}
