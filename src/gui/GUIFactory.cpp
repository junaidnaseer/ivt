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
// Filename:  GUIFactory.cpp
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "Image/ByteImage.h"

#include <string.h>



#ifdef USE_QTGUI

#include "gui/Qt/QtApplicationHandler.h"
#include "gui/Qt/QtMainWindow.h"

CApplicationHandlerInterface *CreateApplicationHandler(int argc, char **argv)
{
	return new CQtApplicationHandler(argc, argv);
}

CMainWindowInterface *CreateMainWindow(int x, int y, int width, int height, const char *title)
{
	return new CQtMainWindow(x, y, width, height, title);
}

#include <qapplication.h>
#include <qfiledialog.h>

#if QT_VERSION >= 0x040000
#define GET_ASCII toAscii
#else
#define GET_ASCII ascii
#endif

static char app_name[] = "IVT_APPLICATION\0";
static int my_argc = 1;
static char *my_argv[2] = { app_name, NULL};

// opens a file dialog and returns true if a filename was selected. The compelete path is returned in filename (the buffer has to be big enough)
bool FileDialog(bool save_dialog, const char **filter, int num_filter, const char *caption, char *filename, int max_length)
{
	QApplication *pApp = NULL;
	if (qApp == NULL)
	{
		pApp = new QApplication(my_argc, my_argv);
	}
	
	char filter_str[1024];
	char *fs = filter_str;
	*fs = 0;

	for (int i = 0; i < num_filter; i++) 
	{
		if (i == num_filter - 1)
		{
			sprintf(fs, "%s (%s)", filter[(2*i)], filter[(2*i) + 1]);
			fs += strlen(filter[(2*i)]) + strlen(filter[(2*i) + 1]) + 3;
		}
		else
		{
			sprintf(fs, "%s (%s);;", filter[(2*i)], filter[(2*i) + 1]);
			fs += strlen(filter[(2*i)]) + strlen(filter[(2*i) + 1]) + 5;
		}
	}
	*fs++ = 0;
	
	if (save_dialog)
	{
		#if QT_VERSION >= 0x040000
		QString s = QFileDialog::getSaveFileName(0, caption, NULL, filter_str);
		#else
		QString s = QFileDialog::getSaveFileName(QString(filename), filter_str, 0, "Save File Dialog", caption);
		#endif
		
		if (s.length() > 0)
		{
			strncpy(filename, s.GET_ASCII(), max_length);
			
			if (pApp)
			{
				delete pApp;
				pApp = NULL;
			}
			
			return true;	
		}
	}
	else
	{
		#if QT_VERSION >= 0x040000
		QString s = QFileDialog::getOpenFileName(0, caption, NULL, filter_str);
		#else
		QString s = QFileDialog::getOpenFileName(QString(filename), filter_str, 0, "Open File Dialog", caption);
		#endif
		
		if (s.length() > 0)
		{
			strncpy(filename, s.GET_ASCII(), max_length);
			
			if (pApp)
			{
				delete pApp;
				pApp = NULL;
			}
			
			return true;	
		}
	}
	
	if (pApp)
	{
		delete pApp;
		pApp = NULL;
	}
	
	return false;
}

#include <qimage.h>

// load an image from file. The supported filetypes depend on the plattform
bool LoadImageFromFile(const char *filename, CByteImage *pImage)
{
	QApplication *pApp = NULL;
	if (qApp == NULL)
	{
		int argc = 1;
		char *argv[] = {app_name, NULL};
	
		pApp = new QApplication(argc, argv);
	}
	
	QImage img(filename, 0);
	
	if (!img.isNull())
	{
		if (img.depth() != 8 && img.depth() != 24 && img.depth() != 32)
		{
			printf("error: LoadImageFromFile: image '%s' has an unsupported bit depth of %d bpp\n", filename, img.depth());
			return false;
		}
		
		int width = img.width();
		int height = img.height();
		
		if (pImage->m_bOwnMemory)
			delete [] pImage->pixels;
		
		pImage->width = width;
		pImage->height = height;
		pImage->bytesPerPixel = (img.depth() == 8 ? 1 : 3);
		pImage->type = (img.depth() <= 8 ? CByteImage::eGrayScale : CByteImage::eRGB24);
		pImage->pixels = new unsigned char[pImage->bytesPerPixel * width*height];
		pImage->m_bOwnMemory = true;
		
		uchar *ptr = img.bits();
		if (img.depth() == 32)
		{
			unsigned char *output = pImage->pixels;
			
			int size = width * height;
			for (int i = 0; i < size; i++)
			{
				output[3*i    ] = ptr[4*i + 2];
				output[3*i + 1] = ptr[4*i + 1];
				output[3*i + 2] = ptr[4*i    ];
			}
		}
		else
		{
			memcpy(pImage->pixels, ptr, pImage->bytesPerPixel * width*height);
		}
		
		if (pApp)
		{
			delete pApp;
			pApp = NULL;
		}
	
		return true;
	}
	
	if (pApp)
	{
		delete pApp;
		pApp = NULL;
	}
	
	return false;
}

#else


#if defined(USE_GTKGUI) && !defined(USE_REMOTEGUI)

#include "gui/GTK/GTKApplicationHandler.h"
#include "gui/GTK/GTKMainWindow.h"

CApplicationHandlerInterface *CreateApplicationHandler(int argc, char **argv)
{
	return new CGTKApplicationHandler(argc, argv);
}

CMainWindowInterface *CreateMainWindow(int x, int y, int width, int height, const char *title)
{
	return new CGTKMainWindow(x, y, width, height, title);
}

// opens a file dialog and returns true if a filename was selected. The complete path is returned in filename (the buffer has to be big enough)
bool FileDialog(bool save_dialog, const char **filter, int num_filter, const char *caption, char *filename, int max_length)
{
	// TODO: implement for GTK
	GtkWidget *dialog;
	
	if (save_dialog)
	{
		dialog = gtk_file_chooser_dialog_new (caption,
						      NULL,
						      GTK_FILE_CHOOSER_ACTION_SAVE,
						      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
						      NULL);
	}
	else
	{
		dialog = gtk_file_chooser_dialog_new (caption,
						      NULL,
						      GTK_FILE_CHOOSER_ACTION_OPEN,
						      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						      NULL);
	}
	
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), filename);

	for (int i = 0; i < num_filter; i++)
	{
		GtkFileFilter *f = gtk_file_filter_new();
		gtk_file_filter_set_name(f, filter[2*i]);
		gtk_file_filter_add_pattern(f, filter[2*i+1]);
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), f);
	}
	
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *file;
		file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		strncpy(filename, file, max_length);
		g_free (file);
		
		gtk_widget_destroy (dialog);
		
		return true;
	}
	
	gtk_widget_destroy (dialog);
	
	return false;
}


// load an image from file. The supported filetypes depend on the plattform
bool LoadImageFromFile(const char *filename, CByteImage *pImage)
{
	bool success = false;
	
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
	if (pixbuf)
	{
		int channels = gdk_pixbuf_get_n_channels(pixbuf);
		int bits_per_sample = gdk_pixbuf_get_bits_per_sample(pixbuf);
		unsigned char *pixels = gdk_pixbuf_get_pixels(pixbuf);
		int width = gdk_pixbuf_get_width(pixbuf);
		int height = gdk_pixbuf_get_height(pixbuf);
		int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
		
		if (bits_per_sample == 8 && (channels == 1 || channels == 3  || channels == 4))
		{
			if (pImage->m_bOwnMemory)
				delete [] pImage->pixels;
		
			pImage->width = width;
			pImage->height = height;
			pImage->bytesPerPixel = channels;
			pImage->type = (channels == 3 ? CByteImage::eGrayScale : CByteImage::eRGB24);
			pImage->pixels = new unsigned char[pImage->bytesPerPixel * width * height];
			pImage->m_bOwnMemory = true;
			
			if (channels == 1)
			{
				for (int y = 0; y < height; y++)
				{
					memcpy(&pImage->pixels[y*width], pixels, width);
					pixels += rowstride;
				}
			}
			else
			{
				if (channels == 3)
				{
					for (int y = 0; y < height; y++)
					{
						memcpy(&pImage->pixels[3*y*width], pixels, 3*width);
						pixels += rowstride;
					}
				}
				else
				{
					// discard the alpha channel
					for (int y = 0; y < height; y++)
					{
						for (int x = 0; x < width; x++)
						{
							pImage->pixels[3*(y*width + x)] = pixels[4*x];
							pImage->pixels[3*(y*width + x)+1] = pixels[4*x+1];
							pImage->pixels[3*(y*width + x)+2] = pixels[4*x+2];
						}
						
						pixels += rowstride;
					}
				}
			}
			
			success = true;
		}

	}
	
	return success;
}

#else


#if defined(WIN32) && !defined(USE_REMOTEGUI)

#include "gui/win32/Win32ApplicationHandler.h"
#include "gui/win32/Win32MainWindow.h"

CApplicationHandlerInterface *CreateApplicationHandler(int argc, char **argv)
{
	return new CWin32ApplicationHandler();
}

CMainWindowInterface *CreateMainWindow(int x, int y, int width, int height, const char *title)
{
	return new CWin32MainWindow(x, y, width, height, title);
}

#include <windows.h>

// opens a file dialog and returns true if a filename was selected. The compelete path is returned in filename (the buffer has to be big enough)
bool FileDialog(bool save_dialog, const char **filter, int num_filter, const char *caption, char *filename, int max_length)
{
	char filter_str[1024];
	char *fs = filter_str;
	*fs = 0;

	for (int i = 0; i < num_filter; i++) 
	{
		sprintf(fs, "%s", filter[(2*i)]);
		fs += strlen(filter[(2*i)]) + 1;

		sprintf(fs, "%s", filter[(2*i) + 1]);
		fs += strlen(filter[(2*i) + 1]) + 1;
	}
	*fs++ = 0;

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = filter_str;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = max_length;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = caption;
	ofn.Flags = 0; //OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = NULL;
	ofn.lCustData = 0;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;

	if (save_dialog)
	{
		if (GetSaveFileName(&ofn))
		{
			return true;
		}
	}
	else
	{
		if (GetOpenFileName(&ofn))
		{
			return true;
		}
	}

	return false;
}

#include <olectl.h>

// load an image from file. The supported filetypes depend on the plattform
bool LoadImageFromFile(const char *filename, CByteImage *pImage)
{
	HDC			hdcTemp;												// The DC To Hold Our Bitmap
	HBITMAP		hbmpTemp;												// Holds The Bitmap Temporarily
	IPicture	*pPicture;												// IPicture Interface
	OLECHAR		wszPath[MAX_PATH+1];									// Full Path To Picture (WCHAR)
	long		lWidth;													// Width In Logical Units
	long		lHeight;												// Height In Logical Units
	long		lWidthPixels;											// Width In Pixels
	long		lHeightPixels;											// Height In Pixels

	MultiByteToWideChar(CP_ACP, 0, filename, -1, wszPath, MAX_PATH);	// Convert From ASCII To Unicode
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if(FAILED(hr))														// If Loading Failed
		return false;													// Return False

	hdcTemp = CreateCompatibleDC(GetDC(0));								// Create The Windows Compatible Device Context
	if(!hdcTemp)														// Did Creation Fail?
	{
		pPicture->Release();											// Decrements IPicture Reference Count
		return false;													// Return False (Failure)
	}
	
	pPicture->get_Width(&lWidth);										// Get IPicture Width (Convert To Pixels)
	lWidthPixels	= MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);										// Get IPicture Height (Convert To Pixels)
	lHeightPixels	= MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);
	
	//	Create A Temporary Bitmap
	BITMAPINFO	bi = {0};												// The Type Of Bitmap We Request
	DWORD		*pBits = 0;												// Pointer To The Bitmap Bits

	bi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);				// Set Structure Size
	bi.bmiHeader.biBitCount		= 32;									// 32 Bit
	bi.bmiHeader.biWidth		= lWidthPixels;							// Power Of Two Width
	bi.bmiHeader.biHeight		= lHeightPixels;						// Make Image Top Up (Positive Y-Axis)
	bi.bmiHeader.biCompression	= BI_RGB;								// RGB Encoding
	bi.bmiHeader.biPlanes		= 1;									// 1 Bitplane

	//	Creating A Bitmap This Way Allows Us To Specify Color Depth And Gives Us Imediate Access To The Bits
	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);
	
	if(!hbmpTemp)														// Did Creation Fail?
	{
		DeleteDC(hdcTemp);												// Delete The Device Context
		pPicture->Release();											// Decrements IPicture Reference Count
		return false;													// Return False (Failure)
	}

	SelectObject(hdcTemp, hbmpTemp);									// Select Handle To Our Temp DC And Our Temp Bitmap Object

	// Render The IPicture On To The Bitmap
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, 0, lWidth, lHeight, 0);

	
	if (pImage->m_bOwnMemory)
		delete [] pImage->pixels;
	
	pImage->width = lWidthPixels;
	pImage->height = lHeightPixels;
	pImage->bytesPerPixel = 3;
	pImage->type = CByteImage::eRGB24;
	pImage->pixels = new unsigned char[pImage->bytesPerPixel * lWidthPixels*lHeightPixels];
	pImage->m_bOwnMemory = true;

	unsigned char *output = pImage->pixels;

	// Convert From BGR To RGB Format And Add An Alpha Value Of 255
	for(long i = 0; i < lWidthPixels * lHeightPixels; i++)				// Loop Through All Of The Pixels
	{
		BYTE* pPixel	= (BYTE*)(&pBits[i]);							// Grab The Current Pixel
		output[3*i]		= pPixel[2];									// Store 1st Color In Temp Variable (Blue)
		output[3*i + 1]	= pPixel[1];									// Move Red Value To Correct Position (1st)
		output[3*i + 2]	= pPixel[0];									// Move Temp Value To Correct Blue Position (3rd)
	}

	DeleteObject(hbmpTemp);												// Delete The Object
	DeleteDC(hdcTemp);													// Delete The Device Context

	pPicture->Release();												// Decrements IPicture Reference Count

	return true;
}

#else


#ifdef USE_COCOAGUI

#include "gui/Cocoa/CocoaApplicationHandler.h"
#include "gui/Cocoa/CocoaMainWindow.h"

CApplicationHandlerInterface *CreateApplicationHandler(int argc, char **argv)
{
	return new CCocoaApplicationHandler();
}

CMainWindowInterface *CreateMainWindow(int x, int y, int width, int height, const char *title)
{
	return new CCocoaMainWindow(x, y, width, height, title);
}

// these functions are defined in GUIFactory.m
extern "C"
{
	bool CocoaFileDialog(bool save_dialog, const char **filter, int num_filter, const char *caption, char *filename, int max_length);
	bool CocoaLoadImage(const char *filename, unsigned char **ptr, int *width, int *height, int *type);
	void CocoaFreeImageMem(unsigned char *ptr);
}

// opens a file dialog and returns true if a filename was selected. The compelete path is returned in filename (the buffer has to be big enough)
bool FileDialog(bool save_dialog, const char **filter, int num_filter, const char *caption, char *filename, int max_length)
{
	char buf[64];
	
	char **cocoa_filters = NULL;
	if (filter != NULL) {
		cocoa_filters = new char*[num_filter];
		for (int i = 0; i < num_filter; i++)
		{
			sscanf(filter[2*i + 1], "*.%s", buf);
			cocoa_filters[i] = new char[strlen(buf) + 1];
			strcpy(cocoa_filters[i], buf);
		}
	}
	
	bool result = CocoaFileDialog(save_dialog, (const char**)cocoa_filters, num_filter, caption, filename, max_length);
	
	if (cocoa_filters != NULL)
	{
		for (int i = 0; i < num_filter; i++)
		{
			delete [] cocoa_filters[i];
		}
		delete [] cocoa_filters;
	}

	return result;
}

// load an image from file. The supported filetypes depend on the plattform
bool LoadImageFromFile(const char *filename, CByteImage *pImage)
{
	unsigned char *ptr = NULL;
	int width = -1;
	int height = -1;
	int type = -1;
	
	if (CocoaLoadImage(filename, &ptr, &width, &height, &type))
	{
		if (pImage->m_bOwnMemory)
			delete [] pImage->pixels;
	
		pImage->width = width;
		pImage->height = height;
		pImage->bytesPerPixel = (type == 1 ? 1 : 3);
		pImage->type = (type == 1 ? CByteImage::eGrayScale : CByteImage::eRGB24);
		pImage->pixels = new unsigned char[pImage->bytesPerPixel * width * height];
		pImage->m_bOwnMemory = true;
		
		memcpy(pImage->pixels, ptr, pImage->bytesPerPixel * width * height);
	
		CocoaFreeImageMem(ptr);
		
		return true;
	}
	
	return false;
}

#else


#ifdef USE_REMOTEGUI

#include "gui/Remote/RemoteApplicationHandler.h"
#include "gui/Remote/RemoteMainWindow.h"

CApplicationHandlerInterface *CreateApplicationHandler(int argc, char **argv)
{
	return new CRemoteApplicationHandler(argc, argv);
}

CMainWindowInterface *CreateMainWindow(int x, int y, int width, int height, const char *title)
{
	CRemoteApplicationHandler *app_handler = CRemoteApplicationHandler::GetApplicationHandler();
	
	if (!app_handler)
		return NULL;
		
	return app_handler->CreateMainWindow(x, y, width, height, title);
}

// opens a file dialog and returns true if a filename was selected. The compelete path is returned in filename (the buffer has to be big enough)
bool FileDialog(bool save_dialog, const char **filter, int num_filter, const char *caption, char *filename, int max_length)
{
	// not available for the remote gui system
	return false;
}

// load an image from file. The supported filetypes depend on the plattform
bool LoadImageFromFile(const char *filename, CByteImage *pImage)
{
	// not available for the remote gui system
	return false;
}

#endif /* ifdef USE_REMOTEGUI */


#endif /* ifdef USE_COCOAGUI */


#endif /* ifdef WIN32 */


#endif /* ifdef USE_GTKGUI */


#endif /* ifdef USE_QTGUI */



