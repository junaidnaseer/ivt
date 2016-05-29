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
// Filename:  VideoReader.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "VideoReader.h"
#include "Image/ByteImage.h"
#include "Helpers/helpers.h"
#include <ctype.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define MAX_HEADER_LENGTH	8192



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CVideoReader::CVideoReader()
{
	m_file = 0;
	m_nBytesToRead = 0;
	m_nImageWidth = -1;
	m_nImageHeight = -1;
	m_pTempBuffer = new unsigned char[1];
	m_pImage = new CByteImage();
	
	header = new unsigned char[MAX_HEADER_LENGTH];
}

CVideoReader::~CVideoReader()
{
	delete [] header;
	delete m_pImage;
	Close();
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CVideoReader::OpenUncompressedAVI(const char *pFileName)
{
	// just to make sure
	Close();

	m_file = fopen(pFileName, "rb");
	if (!m_file)
		return false;

	// parse header
	if (!ParseUncompressedAVIHeader())
	{
		Close();
		return false;
	}
	
	// TODO (easy)
	m_nBytesToRead = m_nImageWidth * m_nImageHeight * 3;

	// create new image
	delete m_pImage;
	delete [] m_pTempBuffer;
	m_pImage = new CByteImage(m_nImageWidth, m_nImageHeight, CByteImage::eRGB24);
	m_pTempBuffer = new unsigned char[m_nBytesToRead];

	return true;
}

CByteImage *CVideoReader::ReadNextFrame()
{
	if (!m_file)
		return 0;

	if (fread(m_pTempBuffer, m_nBytesToRead, 1, m_file) != 1)
		return 0;
		
	const int width_bytes = 3 * m_nImageWidth;
		
	unsigned char *pHelper1 = m_pImage->pixels;
	unsigned char *pHelper2 = m_pTempBuffer + m_nBytesToRead - width_bytes;
		
	// convert from BGR to RGB, and from bottom-left to top-left
	for (int i = 0; i < m_nImageHeight; i++)
	{
		for (int j = 0; j < width_bytes; j += 3)
		{
			pHelper1[j] = pHelper2[j + 2];
			pHelper1[j + 1] = pHelper2[j + 1];
			pHelper1[j + 2] = pHelper2[j];
		}
		
		pHelper1 += width_bytes;
		pHelper2 -= width_bytes;
	}

	fseek(m_file, 8, SEEK_CUR);

	return m_pImage;
}

void CVideoReader::Close()
{
	if (m_file)
	{
		fclose(m_file);
		m_file = 0;
	}
	
	m_nImageWidth = -1;
	m_nImageHeight = -1;
}


static inline unsigned int ConvertFourByteCode(const char *pCode)
{
	return *((unsigned int *) pCode);
}

bool CVideoReader::ParseUncompressedAVIHeader()
{
	if (fread(header, 8192, 1, m_file) != 1)
		return false;

	unsigned int length, *p = (unsigned int *) header;

#ifdef IVT_BIG_ENDIAN
	if (*p++ != ConvertFourByteCode("RIFF"))
		return false;

	// skip length information
	p++;

	if (*p++ != ConvertFourByteCode("AVI "))
		return false;

	if (*p++ != ConvertFourByteCode("LIST"))
		return false;

	// skip length information
	p++;

	if (*p++ != ConvertFourByteCode("hdrl"))
		return false;

	if (*p++ != ConvertFourByteCode("avih"))
		return false;
		
	m_nImageWidth = invert_byte_order_int(p[9]);
	m_nImageHeight = invert_byte_order_int(p[10]);

	length = invert_byte_order_int(*p++);
	if (length > 256)
		return false;

	// skip avi header
	p = (unsigned int *) (((unsigned char *) p) + length);

	if (*p++ != ConvertFourByteCode("LIST"))
		return false;

	// skip length information
	p++;

	if (*p++ != ConvertFourByteCode("strl"))
		return false;

	if (*p++ != ConvertFourByteCode("strh"))
		return false;

	length = invert_byte_order_int(*p++);
	if (length > 256)
		return false;

	// skip stream header
	p = (unsigned int *) (((unsigned char *) p) + length);

	if (*p++ != ConvertFourByteCode("strf"))
		return false;

	length = invert_byte_order_int(*p++);

	// skip stream format
	p = (unsigned int *) (((unsigned char *) p) + length);
	if (length > 4096)
		return false;

	// now re-read the file
	unsigned int offset = (unsigned int) ((unsigned char *) p - header);
	fseek(m_file, offset, SEEK_SET);

	if (*p++ == ConvertFourByteCode("JUNK"))
	{
		// skip it
		length = invert_byte_order_int(*p);
		fseek(m_file, length + 8, SEEK_CUR);
	}

	if (fread(header, 20, 1, m_file) != 1)
		return false;

	p = (unsigned int *) header;

	if (*p++ != ConvertFourByteCode("LIST"))
		return false;

	// skip length information
	p++;

	if (*p++ != ConvertFourByteCode("movi"))
		return false;

	if (*p++ != ConvertFourByteCode("00db"))
		return false;

	if (invert_byte_order_int(*p) != m_nImageWidth * m_nImageHeight * 3)
		return false;
#else
	if (*p++ != ConvertFourByteCode("RIFF"))
		return false;
		
	// skip length information
	p++;

	if (*p++ != ConvertFourByteCode("AVI "))
		return false;
	
	if (*p++ != ConvertFourByteCode("LIST"))
		return false;
	
	// skip length information
	p++;

	if (*p++ != ConvertFourByteCode("hdrl"))
		return false;
	
	if (*p++ != ConvertFourByteCode("avih"))
		return false;
	
	m_nImageWidth = p[9];
	m_nImageHeight = p[10];
	
	length = *p++;
	if (length > 256)
		return false;

	// skip avi header
	p = (unsigned int *) (((unsigned char *) p) + length);

	if (*p++ != ConvertFourByteCode("LIST"))
		return false;

	// skip length information
	p++;

	if (*p++ != ConvertFourByteCode("strl"))
		return false;

	if (*p++ != ConvertFourByteCode("strh"))
		return false;

	length = *p++;
	if (length > 256)
		return false;

	// skip stream header
	p = (unsigned int *) (((unsigned char *) p) + length);

	if (*p++ != ConvertFourByteCode("strf"))
		return false;
	
	length = *p++;

	// skip stream format
	p = (unsigned int *) (((unsigned char *) p) + length);
	if (length > 4096)
		return false;
	
	// now re-read the file
	unsigned int offset = (unsigned int) ((unsigned char *) p - header);
	fseek(m_file, offset, SEEK_SET);
	
	if (*p++ == ConvertFourByteCode("JUNK"))
	{
		// skip it
		length = *p;
		fseek(m_file, length + 8, SEEK_CUR);
	}
	
	if (fread(header, 20, 1, m_file) != 1)
		return false;
	
	p = (unsigned int *) header;
	
	if (*p++ != ConvertFourByteCode("LIST"))
		return false;
	
	// skip length information
	p++;
	
	if (*p++ != ConvertFourByteCode("movi"))
		return false;
	
	if (*p++ != ConvertFourByteCode("00db"))
		return false;
	
	if (*p != (unsigned int) (m_nImageWidth * m_nImageHeight * 3))
		return false;
#endif
    
	return true;
}
