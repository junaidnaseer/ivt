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
#include "Interfaces/MainWindowInterface.h"

#include "gui/GUIFactory.h"

#include "Image/ByteImage.h"

#include <stdio.h>



// ****************************************************************************
// main
// ****************************************************************************

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("use: showimage [file path]\n");
		return 1;
	}

	CByteImage image;
	
	// load image
	if (!image.LoadFromFile(argv[1]))
	{
		printf("error: could not open input file\n");
		return 1;
	}
	
	// create an application handler
	CApplicationHandlerInterface *app = CreateApplicationHandler();
	app->Reset();
	
	// create a main window
	CMainWindowInterface *main_window = CreateMainWindow(0, 0, image.width, image.height, "Show Image");
	WIDGET_HANDLE image_widget = main_window->AddImage(0, 0, image.width, image.height);
	
	// make the window visible
	main_window->Show();
	
	// set the image to display in the window widget
	main_window->SetImage(image_widget, &image);
	
	// main loop
	while (!app->ProcessEventsAndGetExit())
	{
	}
	
	delete main_window;
	delete app;
	
	return 0;
}
