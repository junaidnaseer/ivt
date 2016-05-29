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
// Filename:  GLContext.cpp
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "GLContext.h"

#include "Image/ByteImage.h"

#include <stdio.h>
#include <memory.h>



#ifdef WIN32

CGLContext::CGLContext()
{
	m_hDC = NULL;
	m_hBmp = NULL;
	m_hBmpOld = NULL;
	m_hGLRC = NULL;

	m_nWidth = 0;
	m_nHeight = 0;
	m_pPixels = NULL;
}
CGLContext::~CGLContext()
{
	DeleteContext();
}

bool CGLContext::CreateContext(int width, int height, void *shareContext)
{
	m_hDC = CreateCompatibleDC(NULL);
	if (!m_hDC)
	{
		printf("ERROR: couldn't allocate DC\n");

		return false;
	}

	BITMAPINFO bminfo;
	// create a bitmap with rgb pixels
	bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bminfo.bmiHeader.biWidth = width;
	bminfo.bmiHeader.biHeight = height;
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biBitCount = 24;
	bminfo.bmiHeader.biCompression = BI_RGB;
	bminfo.bmiHeader.biSizeImage = 0; // it's OK for RGB images
	bminfo.bmiHeader.biXPelsPerMeter = 1;
	bminfo.bmiHeader.biYPelsPerMeter = 1;
	bminfo.bmiHeader.biClrUsed = 0;
	bminfo.bmiHeader.biClrImportant = 0;

	m_hBmp = CreateDIBSection(m_hDC, &bminfo, DIB_RGB_COLORS, (void**)&m_pPixels, NULL, 0);
	if (!m_hBmp)
	{
		printf("ERROR: couldn't allocate Bitmap\n");

		DeleteDC(m_hDC);
		m_hDC = NULL;

		return false;
	}

	m_hBmpOld = (HBITMAP)SelectObject(m_hDC, m_hBmp);

	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd 
		1,                     // version number 
		PFD_DRAW_TO_BITMAP |   // support drawing to bitmap 
		PFD_SUPPORT_OPENGL,    // support OpenGL 
		PFD_TYPE_RGBA,         // RGBA type 
		24,                    // 24-bit color depth 
		0, 0, 0, 0, 0, 0,      // color bits ignored 
		0,                     // no alpha buffer 
		0,                     // shift bit ignored 
		0,                     // no accumulation buffer 
		0, 0, 0, 0,            // accum bits ignored 
		32,                    // 32-bit z-buffer 
		0,                     // no stencil buffer 
		0,                     // no auxiliary buffer 
		PFD_MAIN_PLANE,        // main layer 
		0,                     // reserved 
		0, 0, 0                // layer masks ignored 
	}; 
	
	int  iPixelFormat; 
 
	// get the best available match of pixel format for the device context  
	iPixelFormat = ChoosePixelFormat(m_hDC, &pfd); 
 
	// make that the pixel format of the device context 
	BOOL bResult = SetPixelFormat(m_hDC, iPixelFormat, &pfd);
	if (!bResult)
	{
		printf("ERROR: SetPixelFormat failed\n");

		SelectObject(m_hDC, m_hBmpOld);
		DeleteObject(m_hBmp);
		DeleteDC(m_hDC);
		m_hDC = NULL;

		return false;
	}
	
	m_hGLRC = wglCreateContext(m_hDC);
	if (!m_hGLRC)
	{
		printf("ERROR: couldn't allocate HGLRC\n");

		SelectObject(m_hDC, m_hBmpOld);
		DeleteObject(m_hBmp);
		DeleteDC(m_hDC);
		m_hDC = NULL;

		return NULL;
	}

	wglMakeCurrent(m_hDC, m_hGLRC);

	m_nWidth = width;
	m_nHeight = height;

	return true;
}

void CGLContext::DeleteContext()
{
	if (m_hDC != NULL)
	{
		wglMakeCurrent(NULL, NULL);

		wglDeleteContext(m_hGLRC);
		m_hGLRC = NULL;

		SelectObject(m_hDC, m_hBmpOld);
		m_hBmpOld = NULL;

		DeleteObject(m_hBmp);
		m_hBmp = NULL;

		DeleteDC(m_hDC);
		m_hDC = NULL;
	}
}

void CGLContext::MakeCurrent()
{
	if (m_hDC != NULL)
		wglMakeCurrent(m_hDC, m_hGLRC);
}

void CGLContext::DoneCurrent()
{
	wglMakeCurrent(NULL, NULL);
}

bool CGLContext::GetImage(CByteImage *pImage)
{
	if (m_hDC == NULL)
		return false;

	if (pImage->width != m_nWidth || pImage->height != m_nHeight)
	{
		printf("ERROR: CGLContext::GetImage: Image dimensions do not match (%d, %d) vs. (%d, %d)\n", pImage->width, pImage->height, m_nWidth, m_nHeight);
		return false;
	}

	glFlush();

	if (pImage->type == CByteImage::eRGB24)
	{
		unsigned char *in = m_pPixels + 3 * m_nWidth * (m_nHeight-1);
		unsigned char *out = pImage->pixels;

		for (int y = 0; y < m_nHeight; y++, in -= 2*3*m_nWidth)
		{
			for (int x = 0; x < m_nWidth; x++, out += 3, in += 3)
			{
				// BGR -> RGB
				out[0] = in[2];
				out[1] = in[1];
				out[2] = in[0];
			}
		}
	}
	else
	{
		unsigned char *in = m_pPixels + 3 * m_nWidth * (m_nHeight-1);
		unsigned char *out = pImage->pixels;

		for (int y = 0; y < m_nHeight; y++, in -= 2*3*m_nWidth)
		{
			for (int x = 0; x < m_nWidth; x++, out += 3, in += 3)
			{
				*out = (in[0] + in[1] + in[2]) / 3;
			}
		}
	}

	return true;
}

#endif /* WIN32 */



#ifdef USE_QTGUI

#include <qapplication.h>
#include <qpixmap.h>

#include "Helpers/helpers.h"


class MyQGLContext : public QGLContext
{
public:
	MyQGLContext(const QGLFormat &format, QPaintDevice *device)
	: QGLContext(format, device)
	{
	}
	virtual ~MyQGLContext()
	{
	}
	
	void clearCurrent()
	{
		doneCurrent();
	}
};

CGLContext::CGLContext()
: m_pApp(NULL), m_pPixmap(NULL), m_pGLContext(NULL)
{
}
CGLContext::~CGLContext()
{
	DeleteContext();
}

static char app_name[] = "IVT_APPLICATION\0";
static int my_argc = 1;
static char *my_argv[2] = { app_name, NULL};

bool CGLContext::CreateContext(int width, int height, void *shareContext)
{
#ifdef USE_OPENGL
	if (qApp == NULL)
	{
		m_pApp = new QApplication(my_argc, my_argv);
	}
	
	m_pPixmap = new QPixmap(width, height);
	
	QGLFormat f = QGLFormat::defaultFormat();
	//f.setDoubleBuffer(FALSE); // this instruction disables the HW acceleration, so we have to use double buffering
	
	m_pGLContext = new MyQGLContext(f, m_pPixmap);
	
	m_pGLContext->create((QGLContext*)shareContext);
	if (!m_pGLContext->isValid())
	{
		printf("ERROR: CGLContext::CreateContext: couldn't create GLContext!\n");
		return false;
	}
	
	if (shareContext != NULL && !m_pGLContext->isSharing())
	{
		printf("WARNING: CGLContext::CreateContext: sharing of GLContext failed!\n");
	}
	
	m_pGLContext->makeCurrent();

	return true;
#else
	return false;
#endif
}

void CGLContext::DeleteContext()
{
#ifdef USE_OPENGL
	DoneCurrent();
	
	if (m_pGLContext)
	{
		delete m_pGLContext;
		m_pGLContext = NULL;
	}
	
	if (m_pPixmap)
	{
		delete m_pPixmap;
		m_pPixmap = NULL;
	}
	
	if (m_pApp)
	{
		delete m_pApp;
		m_pApp = NULL;
	}
#endif
}

void CGLContext::MakeCurrent()
{
#ifdef USE_OPENGL
	if (m_pGLContext)
		m_pGLContext->makeCurrent();
#endif
}

void CGLContext::DoneCurrent()
{
#ifdef USE_OPENGL
	if (m_pGLContext)
		m_pGLContext->clearCurrent();
#endif
}

bool CGLContext::GetImage(CByteImage *pImage)
{
#ifdef USE_OPENGL
	if (!m_pGLContext)
		return false;

	int width = m_pPixmap->width();
	int height = m_pPixmap->height();
	
	if (pImage->width != width || pImage->height != height)
	{
		printf("ERROR: CGLContext::GetImage: Image dimensions do not match (%d, %d) vs. (%d, %d)\n", pImage->width, pImage->height, width, height);
		return false;
	}
	
	glFlush();
	m_pGLContext->swapBuffers();
	
	if (pImage->type == CByteImage::eRGB24)
	{
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pImage->pixels);
		
		unsigned char *tmp = new unsigned char[3*width];
	
		for (int i = 0; i < (height/2); i++)
		{
			memcpy(tmp, &pImage->pixels[3*i*width], 3*width);
			memcpy(&pImage->pixels[3*i*width], &pImage->pixels[3*(height - i - 1)*width], 3*width);
			memcpy(&pImage->pixels[3*(height - i - 1)*width], tmp, 3*width);
		}
		
		delete [] tmp;
	}
	else
	{
		glReadPixels(0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, pImage->pixels);
		
		unsigned char *tmp = new unsigned char[width];
	
		for (int i = 0; i < (height/2); i++)
		{
			memcpy(tmp, &pImage->pixels[i*width], width);
			memcpy(&pImage->pixels[i*width], &pImage->pixels[(height - i - 1)*width], width);
			memcpy(&pImage->pixels[(height - i - 1)*width], tmp, width);
		}
		
		delete [] tmp;
	}
	
	return true;
#else
	return false;
#endif
}

#endif /* USE_QTGUI */



#ifdef USE_COCOAGUI


// these functions are defined in GLContext.m
extern "C"
{
	void* CocoaCreateGLContext(int width, int height, unsigned char* buffer);
	void CocoaDeleteGLContext(void* ptr);
	void CocoaMakeCurrentGLContext(void* ptr);
	void CocoaDoneCurrentGLContext(void* ptr);
	void CocoaSwapBuffersGLContext(void* ptr);
}

CGLContext::CGLContext()
: m_pGLContext(NULL), m_pBuffer(NULL)
{
}
CGLContext::~CGLContext()
{
	DeleteContext();
}

bool CGLContext::CreateContext(int width, int height, void *shareContext)
{
#ifdef USE_OPENGL
	m_nWidth = width;
	m_nHeight = height;
	
	// RGBA buffer
	m_pBuffer = new unsigned char[width*height*4];
	
	m_pGLContext = CocoaCreateGLContext(width, height, m_pBuffer);
	if (!m_pGLContext)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
		
		return false;
	}
		
	return true;
#else
	return false;
#endif
}

void CGLContext::DeleteContext()
{
#ifdef USE_OPENGL
	if (m_pGLContext)
	{
		CocoaDeleteGLContext(m_pGLContext);
		m_pGLContext = NULL;
		
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
#endif
}

void CGLContext::MakeCurrent()
{
#ifdef USE_OPENGL
	if (m_pGLContext)
		CocoaMakeCurrentGLContext(m_pGLContext);
#endif
}

void CGLContext::DoneCurrent()
{
#ifdef USE_OPENGL
	if (m_pGLContext)
		CocoaDoneCurrentGLContext(m_pGLContext);
#endif
}

bool CGLContext::GetImage(CByteImage *pImage)
{
#ifdef USE_OPENGL
	if (!m_pGLContext)
		return false;
		
	if (pImage->width != m_nWidth || pImage->height != m_nHeight)
	{
		printf("error: CGLContext::GetImage: image dimensions do not match dimensions of the GL context!\n");
		return false;
	}
	
	glFlush();
	CocoaSwapBuffersGLContext(m_pGLContext);
	
	if (pImage->type == CByteImage::eRGB24)
	{
		glReadPixels(0, 0, m_nWidth, m_nHeight, GL_RGB, GL_UNSIGNED_BYTE, pImage->pixels);
	
		unsigned char *pixels = pImage->pixels;
		unsigned char *tmp = new unsigned char[3*m_nWidth];
	
		for (int i = 0; i < (m_nHeight/2); i++)
		{
			memcpy(tmp, &pixels[3*i*m_nWidth], 3*m_nWidth);
			memcpy(&pixels[3*i*m_nWidth], &pixels[3*(m_nHeight - i - 1)*m_nWidth], 3*m_nWidth);
			memcpy(&pixels[3*(m_nHeight - i - 1)*m_nWidth], tmp, 3*m_nWidth);
		}
		
		delete [] tmp;
	}
	else
	{
		glReadPixels(0, 0, m_nWidth, m_nHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, pImage->pixels);
		
		unsigned char *tmp = new unsigned char[m_nWidth];
	
		for (int i = 0; i < (m_nHeight/2); i++)
		{
			memcpy(tmp, &pImage->pixels[i*m_nWidth], m_nWidth);
			memcpy(&pImage->pixels[i*m_nWidth], &pImage->pixels[(m_nHeight - i - 1)*m_nWidth], m_nWidth);
			memcpy(&pImage->pixels[(m_nHeight - i - 1)*m_nWidth], tmp, m_nWidth);
		}
		
		delete [] tmp;
	}
		
	return true;
#else
	return false;
#endif
}


#endif /* USE_COCOAGUI */


#ifdef USE_GTKGUI

CGLContext::CGLContext()
: m_pXDisplay(NULL), m_glxpixmap(0), m_glxcontext(0)
{
}
CGLContext::~CGLContext()
{
	DeleteContext();
}

bool CGLContext::CreateContext(int width, int height, void *shareContext)
{
#ifdef USE_OPENGL
	m_pXDisplay = XOpenDisplay(NULL);
	if (!m_pXDisplay)
	{
		printf("CGLContext::CreateContext: couldn't open XServer connection.\n");
		return false;
	}
	
	if (glXQueryExtension(m_pXDisplay, NULL, NULL) == False)
	{
		printf("CGLContext::CreateContext: no OpenGL support.\n");
		return false;
	}
			
	m_nWidth = width;
	m_nHeight = height;
	
	int attrlist[] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 16, None};
	
	XVisualInfo *vi = glXChooseVisual(m_pXDisplay, DefaultScreen(m_pXDisplay), attrlist);
	if (!vi)
	{
		printf("CGLContext::CreateContext: couldn't find a suitable Visual\n");
		return false;
	}

	m_pFrontBuffer = XCreatePixmap(m_pXDisplay, RootWindow(m_pXDisplay, vi->screen), m_nWidth, m_nHeight, vi->depth);
	if (!m_pFrontBuffer)
	{
		XFree(vi);
		DeleteContext();
		printf("CGLContext::CreateContext: XCreatePixmap failed.\n");
		return false;
	}
	
	m_glxpixmap = glXCreateGLXPixmap(m_pXDisplay, vi, m_pFrontBuffer);
  	if (m_glxpixmap == None)
  	{
  		XFree(vi);
		DeleteContext();
		printf("CGLContext::CreateContext: glXCreateGLXPixmap failed.\n");
		return false;
  	}
  	
	if (shareContext)
		m_glxcontext = glXCreateContext(m_pXDisplay, vi, (GLXContext)shareContext, False);
	else
		m_glxcontext = glXCreateContext(m_pXDisplay, vi, 0, False);
	
	if (m_glxcontext == NULL)
	{
		XFree(vi);
		DeleteContext();
		printf("CGLContext::CreateContext: glXCreateContext failed.\n");
		return false;
	}
	
	XFree(vi);
	
	return true;
#else
	return false;
#endif
}

void CGLContext::DeleteContext()
{
#ifdef USE_OPENGL
	if (!m_glxcontext)
		return;
		
	glFinish();
	glXWaitX();
		
	if (m_glxcontext == glXGetCurrentContext())
		glXMakeCurrent(m_pXDisplay, None, NULL);

	if (m_glxpixmap != None) {
		glXDestroyGLXPixmap(m_pXDisplay, m_glxpixmap);
		glXWaitGL();
		m_glxpixmap = None;
	}
	
	if (m_pFrontBuffer) {
		XFreePixmap(m_pXDisplay, m_pFrontBuffer);
		glXWaitX();
		m_pFrontBuffer = 0;
	}
  
	if (m_glxcontext)
	{
		glXDestroyContext(m_pXDisplay, m_glxcontext);
		m_glxcontext = NULL;
	}
	
	if (m_pXDisplay)
	{
		XCloseDisplay(m_pXDisplay);
		m_pXDisplay = NULL;
	}
#endif
}

void CGLContext::MakeCurrent()
{
#ifdef USE_OPENGL
	if (m_glxcontext)
	{
		if (m_glxcontext != glXGetCurrentContext())
			glXMakeCurrent(m_pXDisplay, m_glxpixmap, m_glxcontext);
	}
#endif
}

void CGLContext::DoneCurrent()
{
#ifdef USE_OPENGL
	glXMakeCurrent(m_pXDisplay, None, NULL);
#endif
}

bool CGLContext::GetImage(CByteImage *pImage)
{
#ifdef USE_OPENGL
	if (!m_glxcontext)
		return false;
	
	if (pImage->width != m_nWidth || pImage->height != m_nHeight)
	{
		printf("ERROR: CGLContext::GetImage: Image dimensions do not match (%d, %d) vs. (%d, %d)\n", pImage->width, pImage->height, m_nWidth, m_nHeight);
		return false;
	}
	
	glFlush();
	glXSwapBuffers(m_pXDisplay, m_glxpixmap);
	
	if (pImage->type == CByteImage::eRGB24)
	{
		glReadPixels(0, 0, m_nWidth, m_nHeight, GL_RGB, GL_UNSIGNED_BYTE, pImage->pixels);
		
		unsigned char *tmp = new unsigned char[3*m_nWidth];
	
		for (int i = 0; i < (m_nHeight/2); i++)
		{
			memcpy(tmp, &pImage->pixels[3*i*m_nWidth], 3*m_nWidth);
			memcpy(&pImage->pixels[3*i*m_nWidth], &pImage->pixels[3*(m_nHeight - i - 1)*m_nWidth], 3*m_nWidth);
			memcpy(&pImage->pixels[3*(m_nHeight - i - 1)*m_nWidth], tmp, 3*m_nWidth);
		}
		
		delete [] tmp;
	}
	else
	{
		glReadPixels(0, 0, m_nWidth, m_nHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, pImage->pixels);
		
		unsigned char *tmp = new unsigned char[m_nWidth];
	
		for (int i = 0; i < (m_nHeight/2); i++)
		{
			memcpy(tmp, &pImage->pixels[i*m_nWidth], m_nWidth);
			memcpy(&pImage->pixels[i*m_nWidth], &pImage->pixels[(m_nHeight - i - 1)*m_nWidth], m_nWidth);
			memcpy(&pImage->pixels[(m_nHeight - i - 1)*m_nWidth], tmp, m_nWidth);
		}
		
		delete [] tmp;
	}
	
	return true;
#else
	return false;
#endif
}

#endif /* USE_GTKGUI */
