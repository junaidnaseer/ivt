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
// Filename:  QTWindow.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "QTWindow.h"
#include "Interfaces/WindowEventInterface.h"
#include "Image/ByteImage.h"

#include <qpainter.h>
#include <qimage.h>
#include <qlcdnumber.h>
#include <qcheckbox.h>

#if QT_VERSION >= 0x040000
#include <QtGui/QMouseEvent>
#endif

#include <memory.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CQTWindow::CQTWindow(int nWidth, int nHeight, CWindowEventInterface *pWindowEventInterface, QWidget *pParent) : QWidget(pParent)
{
	setFixedSize(nWidth, nHeight);

	m_nImageWidth = -1;
	m_nImageHeight = -1;
	
	m_pBuffer = 0;	
	
	m_type = -1;
	m_bMarkRectangle = pWindowEventInterface != 0;
	
	m_bGetRect = false;
	m_nRectX0 = m_nRectY0 = m_nRectX1 = m_nRectY1 = 0;
	
	m_pWindowEventInterface = pWindowEventInterface;
}

CQTWindow::~CQTWindow()
{
	if (m_pBuffer)
		delete [] m_pBuffer;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CQTWindow::paintEvent(QPaintEvent *e)
{
	if (m_type != -1)
	{
		QPainter painter(this);

		#if QT_VERSION >= 0x040000
		QImage image(m_pBuffer, m_nImageWidth, m_nImageHeight, QImage::Format_RGB32);
		#else
		QImage image(m_pBuffer, m_nImageWidth, m_nImageHeight, 32, 0, 0, QImage::BigEndian);
		#endif

		painter.drawImage(m_nImageX, m_nImageY, image);

		if (m_bMarkRectangle && m_bGetRect)
		{
			QPen pen(Qt::blue, 1);
			painter.setPen(pen);
			painter.drawRect(m_nRectX0, m_nRectY0, m_nRectX1 - m_nRectX0, m_nRectY1 - m_nRectY0);
		}
	}
}

void CQTWindow::Show()
{
	show();
}

void CQTWindow::Hide()
{
	hide();
}

void CQTWindow::DrawImage(const CByteImage *pImage, int x, int y)
{
	if (m_nImageWidth != pImage->width || m_nImageHeight != pImage->height)
	{
		m_nImageWidth = pImage->width;
		m_nImageHeight = pImage->height;

		if (m_pBuffer)
			delete [] m_pBuffer;

		m_pBuffer = new unsigned char[m_nImageWidth * m_nImageHeight * 4];
	}
	
	m_nImageX = x;
	m_nImageY = y;
	
	if (pImage->type == CByteImage::eGrayScale)
	{
		const int nPixels = m_nImageWidth * m_nImageHeight;
		unsigned char *pixels = pImage->pixels;
		int *output = (int *) m_pBuffer;

		for (int i = 0; i < nPixels; i++)
			output[i] = 255 << 24 | pixels[i] << 16 | pixels[i] << 8 | pixels[i];
			
		m_type = CByteImage::eGrayScale;
	}
	else if (pImage->type == CByteImage::eRGB24)
	{
		const int nPixels = m_nImageWidth * m_nImageHeight;
		unsigned char *pixels = pImage->pixels;
		int *output = (int *) m_pBuffer;

		for (int offset = 0, i = 0; i < nPixels; i++)
		{
			output[i] = 255 << 24 | pixels[offset] << 16 | pixels[offset + 1] << 8 | pixels[offset + 2];
			offset += 3;
		}
			
		m_type = CByteImage::eRGB24;
	}
	
	#if QT_VERSION >= 0x040000
	repaint(x, y, m_nImageWidth, m_nImageHeight);
	#else
	repaint(x, y, m_nImageWidth, m_nImageHeight, false);
	#endif
}

void CQTWindow::mousePressEvent(QMouseEvent *e)
{
	if (e->button() != Qt::LeftButton)
		return;
		
	if (m_pWindowEventInterface)
		m_pWindowEventInterface->PointClicked(e->x(), e->y());
		
	if (m_bGetRect)
	{
		// rect is already being captured => abort
		m_bGetRect = false;
		return;
	}
	
	m_bGetRect = true;
	
	m_nRectX0 = m_nRectX1 = e->x();
	m_nRectY0 = m_nRectY1 = e->y();
}

void CQTWindow::mouseMoveEvent(QMouseEvent *e)
{
	if (!m_bGetRect)
		return;

	m_nRectX1 = e->x();
	m_nRectY1 = e->y();
}

void CQTWindow::mouseReleaseEvent(QMouseEvent *e)
{
	if (!m_bGetRect)
		return;
		
	m_bGetRect = false;
		
	if (m_nRectX0 == m_nRectX1 || m_nRectY0 == m_nRectY1)
		return;
		
	if (m_nRectX1 < m_nRectX0)
	{
		int temp = m_nRectX1;
		m_nRectX1 = m_nRectX0;
		m_nRectX0 = temp;
	}
	
	if (m_nRectY1 < m_nRectY0)
	{
		int temp = m_nRectY1;
		m_nRectY1 = m_nRectY0;
		m_nRectY0 = temp;
	}
		
	if (m_pWindowEventInterface)
		m_pWindowEventInterface->RectSelected(m_nRectX0, m_nRectY0, m_nRectX1, m_nRectY1);
}
