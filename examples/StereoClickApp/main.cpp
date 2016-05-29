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

#include "Calibration/Calibration.h"
#include "Calibration/StereoCalibration.h"
#include "Image/ByteImage.h"
#include "Math/Math3d.h"

#include "Interfaces/ApplicationHandlerInterface.h"
#include "Interfaces/MainWindowEventInterface.h"
#include "gui/GUIFactory.h"

#include <stdio.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define CAMERA_PARAMETER_FILE "../../files/camera_armar_640x480.txt"



// ****************************************************************************
// CStereoClick
// ****************************************************************************

class CStereoClickApp : public CMainWindowEventInterface
{
public:
	// constructor
	CStereoClickApp() : m_bGotLeftPoint(false), m_bGotRightPoint(false)
	{
		last_point.x = 0.0;
		last_point.y = 0.0;
		last_point.z = 0.0;
	}

	// this is called when ever the user clicks into one of the image widgets
	void PointClicked(WIDGET_HANDLE widget, int x, int y)
	{
		if (widget == m_pImageWidgetLeft)
		{
			left_x = x;
			left_y = y;
			m_bGotLeftPoint = true;
		}
		else
		{
			right_x = x;
			right_y = y;
			m_bGotRightPoint = true;
		}
		
		if (m_bGotLeftPoint && m_bGotRightPoint)
		{
			Vec2d left_point = { (float) left_x, (float) left_y };
			Vec2d right_point = { (float) right_x, (float) right_y };
			Vec3d result_point;
			
			m_pStereoCalibration->Calculate3DPoint(left_point, right_point, result_point, false);
			
			printf("3d point = %f %f %f\n", result_point.x, result_point.y, result_point.z);
			printf("distance = %f\n", Math3d::Distance(last_point, result_point));
			
			Math3d::SetVec(last_point, result_point);
			
			m_bGotLeftPoint = false;
			m_bGotRightPoint = false;
		}
	}

	// init application and run
	int Run()
	{
		printf("click on one point in the left image and one point in the right image and the 3d coordinates will be printed on the screen, as well as the euclidian distance to the previously calculated 3d point.\n\n");
	
		CByteImage image_left, image_right;
		
		// load image
		if (!image_left.LoadFromFile("../../files/scene_left.bmp"))
		{
			printf("error: could not open left input file");
			return 1;
		}
		
		if (!image_right.LoadFromFile("../../files/scene_right.bmp"))
		{
			printf("error: could not open right input file");
			return 1;
		}
		
		m_pStereoCalibration = new CStereoCalibration();
		if (!m_pStereoCalibration->LoadCameraParameters(CAMERA_PARAMETER_FILE))
		{
			printf("error: could not open camera parameter file\n");
			return 1;
		}
		
		const int width = image_left.width;
		const int height = image_left.height;

		// create an application handler
		CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
		pApplicationHandler->Reset();
		
		// create a main window
		m_pMainWindow = CreateMainWindow(0, 0, 2*width, height, "Stereo Click Application");

		// events are sent to this class, hence this class needs to have the CMainWindowEventInterface
		m_pMainWindow->SetEventCallback(this);

		// create an image widget to display an image
		m_pImageWidgetLeft = m_pMainWindow->AddImage(0, 0, width, height);
		m_pImageWidgetRight = m_pMainWindow->AddImage(width, 0, width, height);
		
		// make the window visible
		m_pMainWindow->Show();
		
		// main loop
		while (!pApplicationHandler->ProcessEventsAndGetExit())
		{
			m_pMainWindow->SetImage(m_pImageWidgetLeft, &image_left);
			m_pMainWindow->SetImage(m_pImageWidgetRight, &image_right);
		}
		
		delete m_pMainWindow;
		delete pApplicationHandler;
		
		delete m_pStereoCalibration;
		
		return 0;
	}


private:
	// private attributes
	CMainWindowInterface *m_pMainWindow;
	WIDGET_HANDLE m_pImageWidgetLeft;
	WIDGET_HANDLE m_pImageWidgetRight;
	
	bool m_bGotLeftPoint;
	bool m_bGotRightPoint;
	
	int left_x, left_y;
	int right_x, right_y;
	
	Vec3d last_point;
	CStereoCalibration *m_pStereoCalibration;
};


int main()
{
	CStereoClickApp app;
	return app.Run();
}
