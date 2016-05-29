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
// Filename:  GLContext.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************


#ifndef _GL_CONTEXT_H_
#define _GL_CONTEXT_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#ifdef USE_QTGUI
#ifdef USE_OPENGL
#include <qgl.h>
#endif
class MyQGLContext;
class QPixmap;
class QApplication;
#endif

#ifdef USE_GTKGUI
#include <GL/glx.h>
#endif


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;

class CGLContext
{
public:
	CGLContext();
	~CGLContext();

	bool CreateContext(int width, int height, void *shareContext = 0);
	void DeleteContext();

	void MakeCurrent();
	void DoneCurrent();
	bool GetImage(CByteImage *pImage);

private:

#ifdef WIN32
	HDC				m_hDC;
	HBITMAP			m_hBmp;
	HBITMAP			m_hBmpOld;
	HGLRC			m_hGLRC;

	int				m_nWidth;
	int				m_nHeight;
	unsigned char	*m_pPixels;
#endif

#ifdef USE_QTGUI
	QApplication	*m_pApp;
	QPixmap		*m_pPixmap;
	MyQGLContext	*m_pGLContext;
#endif

#ifdef USE_COCOAGUI
	void *m_pGLContext;
	int	m_nWidth;
	int m_nHeight;
	unsigned char *m_pBuffer;
#endif

#ifdef USE_GTKGUI
	Display		*m_pXDisplay;
	GLXContext	m_glxcontext;
	int		m_nWidth;
	int		m_nHeight;
	GLXPixmap	m_glxpixmap;
	Pixmap		m_pFrontBuffer;
#endif

};



#endif /* _GL_CONTEXT_H_ */
