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
// Filename:  QTPainter.cpp
// Author:    Pedram Azad, Moritz Ritter
// Date:      08.05.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "QTPainter.h"
#include "Image/ByteImage.h"

#include <stdio.h>
#include <qimage.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CQTPainter::CQTPainter(QPaintDevice *pd) : QPainter(pd)
{
	m_pBuffer = 0;

	m_nImageWidth = -1;
	m_nImageHeight = -1;
}

CQTPainter::~CQTPainter()
{
	if (m_pBuffer)
		delete [] m_pBuffer;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CQTPainter::DrawImage(const CByteImage *pImage, int x, int y)
{
	WriteToBuffer(pImage);

	#if QT_VERSION >= 0x040000
	QImage image(m_pBuffer, m_nImageWidth, m_nImageHeight, QImage::Format_RGB32);
	#else
	QImage image(m_pBuffer, m_nImageWidth, m_nImageHeight, 32, 0, 0, QImage::BigEndian);
	#endif

	drawImage(x, y, image);
}

void CQTPainter::WriteToBuffer(const CByteImage *pImage)
{
	if (m_nImageWidth != pImage->width || m_nImageHeight != pImage->height)
	{
		m_nImageWidth = pImage->width;
		m_nImageHeight = pImage->height;

		if (m_pBuffer)
			delete [] m_pBuffer;

		m_pBuffer = new unsigned char[m_nImageWidth * m_nImageHeight * sizeof(int)];
	}
	
	if (pImage->type == CByteImage::eGrayScale)
	{
		const int nPixels = m_nImageWidth * m_nImageHeight;
		unsigned char *pixels = pImage->pixels;
		int *output = (int *) m_pBuffer;

		for (int i = 0; i < nPixels; i++)
			output[i] = 255 << 24 | pixels[i] << 16 | pixels[i] << 8 | pixels[i];
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
	}
}
