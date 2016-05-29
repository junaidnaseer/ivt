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
// Date:      29.09.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#if defined WIN32
#include "VideoCapture/VFWCapture.h"
#elif defined __APPLE__
#include "VideoCapture/QuicktimeCapture.h"
#else
#include "VideoCapture/Linux1394Capture2.h"
#endif

#include "Interfaces/MainWindowInterface.h"
#include "Interfaces/MainWindowEventInterface.h"
#include "Interfaces/ApplicationHandlerInterface.h"
#include "gui/GUIFactory.h"

#include "Image/ByteImage.h"

#include <stdio.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define MAXIMUM_CAMERAS 10


// ****************************************************************************
// CCaptureApp
// ****************************************************************************

class CCaptureApp : public CMainWindowEventInterface
{
public:
	// constructor
	CCaptureApp()
	{
		m_nSelectedCamera = -1;
	}

	// public methods
	void ButtonPushed(WIDGET_HANDLE widget)
	{
		if (widget == m_snapshotButton && m_nSelectedCamera != -1)
		{
			char szFilename[1024];
			if (m_pMainWindow->GetText(m_filenameTextEdit, szFilename, 1024))
			{
				if (!m_ppImages[m_nSelectedCamera]->SaveToFile(szFilename))
					printf("error: could not write image to '%s'\n", szFilename);
			}
		}
	}
	
	int Run()
	{
		// create capture object
		#if defined WIN32
		CVFWCapture capture(0);
		#elif defined __APPLE__
		CQuicktimeCapture capture(CVideoCaptureInterface::e640x480);
		#else
		CLinux1394Capture2 capture(-1, CVideoCaptureInterface::e640x480, CVideoCaptureInterface::eRGB24);
		#endif

		// open camera
		if (!capture.OpenCamera())
		{
			printf("error: could not open camera\n");
			printf("press return to quit\n");
			char szTemp[1024];
			scanf("%c", szTemp);
			return 1;
		}

		const int width = capture.GetWidth();
		const int height = capture.GetHeight();
		const CByteImage::ImageType type = capture.GetType();
		const int nCameras = capture.GetNumberOfCameras();

		int i;

		m_ppImages = new CByteImage*[nCameras];
		for (i = 0; i < nCameras; i++)
			m_ppImages[i] = new CByteImage(width, height, type);

		// create the application handler
		CApplicationHandlerInterface *pApplicationHandler = CreateApplicationHandler();
		pApplicationHandler->Reset();

		// create the main window
		m_pMainWindow = CreateMainWindow(0, 0, width, height + 50, "Capture Application");
		m_pMainWindow->SetEventCallback(this);

		// add an image widget to display an image
		WIDGET_HANDLE pImageWidget = m_pMainWindow->AddImage(0, 0, width, height);

		// add a combo box to choose which camera image to display (at most 10 cameras can be used)
		const char *ppEntries[MAXIMUM_CAMERAS] = { "Camera 0", "Camera 1", "Camera 2", "Camera 3", "Camera 4", "Camera 5", "Camera 6", "Camera 7", "Camera 8", "Camera 9" };
		WIDGET_HANDLE pComboBox = m_pMainWindow->AddComboBox(10, height + 10, 150, 30, nCameras < 10 ? nCameras : 10, ppEntries, 0);

		// add a button for triggering a snapshot
		m_snapshotButton = m_pMainWindow->AddButton(180, height + 10, 150, 25, "Take Snapshot");

		// add an text edit field for the file name
		m_filenameTextEdit = m_pMainWindow->AddTextEdit(350, height + 10, 200, 25, "snapshot.bmp");

		// make the main window visible
		m_pMainWindow->Show();

		while (!pApplicationHandler->ProcessEventsAndGetExit())
		{
			if (!capture.CaptureImage(m_ppImages))
				break;
			
			if (m_pMainWindow->GetValue(pComboBox, m_nSelectedCamera))
				m_pMainWindow->SetImage(pImageWidget, m_ppImages[m_nSelectedCamera]);
		}

		for (i = 0; i < nCameras; i++)
			delete m_ppImages[i];
		delete [] m_ppImages;

		delete m_pMainWindow;
		delete pApplicationHandler;

		return 0;
	}

private:
	// private attributes
	CMainWindowInterface *m_pMainWindow;
	WIDGET_HANDLE m_snapshotButton;
	WIDGET_HANDLE m_filenameTextEdit;

	CByteImage **m_ppImages;
	int m_nSelectedCamera;
};


int main()
{
	CCaptureApp app;
	return app.Run();
}
