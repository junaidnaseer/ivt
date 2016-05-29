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
#include "gui/GUIFactory.h"

#include "Visualizer/OpenGLVisualizer.h"
#include "Visualizer/OpenGLVisualizerControl.h"
#include "Calibration/Calibration.h"
#include "Math/Math3d.h"
#include "Math/Constants.h"
#include "Helpers/helpers.h"

#include <math.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define FPS						30



// ****************************************************************************
// main
// ****************************************************************************

int main()
{
	CCalibration calibration;
	const int width = calibration.GetCameraParameters().width;
	const int height = calibration.GetCameraParameters().height;

	// create the application handler
	CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
	pApplicationHandler->Reset();
	
	// create the main window
	CMainWindowInterface *pMainWindow = CreateMainWindow(0, 0, width, height, "OpenGL Demo");
	WIDGET_HANDLE pOpenGLWidget = pMainWindow->AddGLWidget(0, 0, width, height);
	
	pMainWindow->Show();
	
	pMainWindow->MakeCurrentGLWidget(pOpenGLWidget);
	
	// create the visualizer
	COpenGLVisualizer visualizer;
	visualizer.InitByCalibration(&calibration);
	
	// create the 3D view controls
	COpenGLVisualizerControl control(pOpenGLWidget, width, height, &visualizer, pMainWindow, NULL);
	
	// move the origin to (0, 0, 1500)
	Transformation3d ViewMatrix;
	ViewMatrix.rotation = Math3d::unit_mat;
	ViewMatrix.translation = Math3d::zero_vec;
	ViewMatrix.translation.z = 1500;
	
	control.SetViewMatrix(ViewMatrix);
	visualizer.SetViewMatrix(ViewMatrix);
	
	// send events to the control object
	pMainWindow->SetEventCallback(&control);
	
	
	const float r = 250;
	float angle = 0;
	
	get_timer_value(true);

	while (!pApplicationHandler->ProcessEventsAndGetExit())
	{
		const Vec3d point1 = { r * cosf(angle), 0, r * sinf(angle)};
		const Vec3d point2 = { r * cosf(angle + FLOAT_PI), 0, r * sinf(angle + FLOAT_PI)};
	
		visualizer.Clear();
		
		visualizer.DrawSphere(point1, 100, COpenGLVisualizer::red);
		visualizer.DrawSphere(point2, 100, COpenGLVisualizer::red);
		visualizer.DrawCylinder(point1, point2, 50, 50, COpenGLVisualizer::blue);
		
		//angle += 0.05f;
		
		pMainWindow->SwapBuffersGLWidget(pOpenGLWidget);
		
		// timing
		const unsigned int T = (unsigned int) (1000000.0f / FPS);
		while (get_timer_value() < T);
		get_timer_value(true);
		
		control.Update();
	}
	
	delete pMainWindow;
	delete pApplicationHandler;
	
	return 0;
}
