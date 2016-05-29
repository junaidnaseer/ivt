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
// Filename:  BitmapFont.cpp
// Copyright: Keyetech UG (haftungsbeschraenkt)
// Author:    Pedram Azad
// Date:      17.12.2012
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "BitmapFont.h"

#include "Image/ByteImage.h"
#include "Image/PrimitivesDrawer.h"

#include <stdio.h>
#include <string.h>


// ****************************************************************************
// Defines
// ****************************************************************************

#define PCF_PROPERTIES		(1<<0)
#define PCF_ACCELERATORS	(1<<1)
#define PCF_METRICS		(1<<2)
#define PCF_BITMAPS		(1<<3)
#define PCF_INK_METRICS		(1<<4)
#define	PCF_BDF_ENCODINGS	(1<<5)
#define PCF_SWIDTHS		(1<<6)
#define PCF_GLYPH_NAMES		(1<<7)
#define PCF_BDF_ACCELERATORS	(1<<8)

#define PCF_DEFAULT_FORMAT	0x00000000
#define PCF_INKBOUNDS		0x00000200
#define PCF_ACCEL_W_INKBOUNDS	0x00000100
#define PCF_COMPRESSED_METRICS	0x00000100

#define PCF_GLYPH_PAD_MASK	(3<<0)		/* See the bitmap table for explanation */
#define PCF_BYTE_MASK		(1<<2)		/* If set then Most Sig Byte First */
#define PCF_BIT_MASK		(1<<3)		/* If set then Most Sig Bit First */
#define PCF_SCAN_UNIT_MASK	(3<<4)		/* See the bitmap table for explanation */


// ****************************************************************************
// Struct definitions
// ****************************************************************************

struct TOCEntry
{
    int type;		// indicates which table
    int format;		// indicates how the data are formatted in the table
    int size;		// in bytes
    int offset;		// from start of file
};

struct PCFMetricsTable
{
	int format;
	short metrics_count;
};

struct PCFEncodingTable
{
	int format; 						// always stored with least significant byte first
	short min_char_or_byte2;
	short max_char_or_byte2;
	short min_byte1;
	short max_byte1;
	short default_char;
};

struct PCFAcceleratorTable
{
	int format; 						// always stored with least significant byte first
	unsigned char noOverlap;
	unsigned char constantMetrics;
	unsigned char terminalFont;
	unsigned char constantWidth;		// monospace font like courier
	unsigned char inkInside;
	unsigned char inkMetrics;
	unsigned char drawDirection;		// 0 =>left to right, 1 => right to left
	unsigned char padding;
	int fontAscent;
	int fontDescent;
	int maxOverlap;
};

struct PCFMetricsEntry
{
	int left_sided_bearing;
	int right_side_bearing;
	int character_width;
	int character_ascent;
	int character_descent;
	int character_attributes;
};


struct BitmapCharacter
{
	int *pCoordinatesX;
	int *pCoordinatesY;
	int nCoordinates;
	int nWidth;
	int nAscent;
	int nDescent;
};



// ****************************************************************************
// CBitmapFont
// ****************************************************************************

CBitmapFont::CBitmapFont()
{
	m_pCharacters = 0;
	m_pEncodingTable = 0;

	Reset();
}

CBitmapFont::~CBitmapFont()
{
	Reset();
}

void CBitmapFont::Reset()
{
	if (m_pCharacters)
	{
		for (int i = 0; i < m_nCharacters; i++)
		{
			if (m_pCharacters[i].pCoordinatesX)
			{
				delete [] m_pCharacters[i].pCoordinatesX;
				delete [] m_pCharacters[i].pCoordinatesY;
			}
		}

		delete [] m_pCharacters;
		m_pCharacters = 0;
	}

	delete [] m_pEncodingTable;
	m_pEncodingTable = 0;

	m_nCharacters = 0;
	m_nMaxEncoding = 0;
	m_nFontHeight = 0;
}

void CBitmapFont::Init(int nCharacters)
{
	Reset();

	m_nCharacters = nCharacters;

	m_pCharacters = new BitmapCharacter[m_nCharacters];

	int i;

	for (int i = 0; i < m_nCharacters; i++)
	{
		m_pCharacters[i].pCoordinatesX = 0;
		m_pCharacters[i].pCoordinatesY = 0;
		m_pCharacters[i].nCoordinates = 0;
		m_pCharacters[i].nWidth = 0;
	}

	m_nMaxEncoding = 256;
	m_pEncodingTable = new int[m_nMaxEncoding];

	for (i = 0; i < m_nMaxEncoding; i++)
		m_pEncodingTable[i] = -1;
}

bool CBitmapFont::GetCharacterInformation(unsigned char encoding, int *&pCoordinatesX, int *&pCoordinatesY, int &nCoordinates, int &nWidth) const
{
	const int nIndex = m_pEncodingTable[encoding];

	if (nIndex == -1)
		return false;

	pCoordinatesX = m_pCharacters[nIndex].pCoordinatesX;
	pCoordinatesY = m_pCharacters[nIndex].pCoordinatesY;
	nCoordinates = m_pCharacters[nIndex].nCoordinates;
	nWidth = m_pCharacters[nIndex].nWidth;

	return true;
}

void CBitmapFont::SetGlyph(int nIndex, int *pCoordinatesX, int *pCoordinatesY, int nCoordinates, int nWidth, int nAscent, int nDescent)
{
	if (m_pCharacters[nIndex].pCoordinatesY)
	{
		delete [] m_pCharacters[nIndex].pCoordinatesY;
		delete [] m_pCharacters[nIndex].pCoordinatesY;
	}

	m_pCharacters[nIndex].pCoordinatesX = new int[nCoordinates];
	m_pCharacters[nIndex].pCoordinatesY = new int[nCoordinates];
	m_pCharacters[nIndex].nCoordinates = nCoordinates;
	m_pCharacters[nIndex].nWidth = nWidth;

	// copy coordinates
	for (int i = 0; i < nCoordinates; i++)
	{
		m_pCharacters[nIndex].pCoordinatesX[i] = pCoordinatesX[i];
		m_pCharacters[nIndex].pCoordinatesY[i] = pCoordinatesY[i];
	}
}

void CBitmapFont::AddEncoding(int nEncoding, int nIndex)
{
	if (nIndex >= m_nCharacters)
	{
		printf("error: encoding %i indexed invalid glyph index %i (%i glyphs)\n", nEncoding, nIndex, m_nCharacters);
		return;
	}

	if (nEncoding < 0 || nEncoding >= m_nMaxEncoding)
	{
		printf("error: encoding %i is not valid\n", nEncoding);
		return;
	}

	m_pEncodingTable[nEncoding] = nIndex;
}



static short my_invert_byte_order_short(short x, int bInvert = false)
{
	if (!bInvert)
		return x;

	unsigned short result = 0;

	result |= (x & 0x00ff) << 8;
	result |= (x & 0xff00) >> 8;
	
	return result;
}

static unsigned int my_invert_byte_order_int(unsigned int x, int bInvert = false)
{
	if (!bInvert)
		return x;

	unsigned int result = 0;
	
	result |= (x & 0x000000ff) << 24;
	result |= (x & 0x0000ff00) << 8;
	result |= (x & 0x00ff0000) >> 8;
	result |= (x & 0xff000000) >> 24;
	
	return result;
}


bool CBitmapFont::LoadPCFFont(const char *pFilePath)
{
	FILE *f = fopen(pFilePath, "rb");
	if (!f)
	{
		printf("error: could not open file for reading\n");
		return false;
	}
	
	fseek(f, 0, SEEK_END);
	const int nFileSize = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	// allocate memory for file contents
	unsigned char *pBuffer = new unsigned char[nFileSize];
	
	// read file into buffer
	if (fread(pBuffer, nFileSize, 1, f) != 1)
	{
		printf("error: could not read file\n");
		delete [] pBuffer;
		return false;
	}
	
	// compare
	if (strncmp((char *) pBuffer, "\1fcp", 4) != 0)
	{
		printf("error: file header is not a valid PCF header\n");
		delete [] pBuffer;
		return false;
	}
	
	const int nTableCount = my_invert_byte_order_int(*((int *) (pBuffer + 4)));
	
	TOCEntry *pTOCEntries = (TOCEntry *) (pBuffer + 8);
	TOCEntry *pBitmapTOCEntry = 0;
	TOCEntry *pAcceleratorTOCEntry = 0;
	TOCEntry *pMetricsTOCEntry = 0;
	TOCEntry *pEncodingTOCEntry = 0;

	int i;
	
	for (i = 0; i < nTableCount; i++)
	{
		if ((pTOCEntries[i].format & PCF_BIT_MASK) == 0)
		{
			printf("error: inverse bit order not implmeented\n");
			delete [] pBuffer;
			return false;
		}

		switch (pTOCEntries[i].type)
		{
			case PCF_BITMAPS: pBitmapTOCEntry = pTOCEntries + i; break;
			case PCF_ACCELERATORS: pAcceleratorTOCEntry = pTOCEntries + i; break;
			case PCF_METRICS: pMetricsTOCEntry = pTOCEntries + i; break;
			case PCF_BDF_ENCODINGS: pEncodingTOCEntry = pTOCEntries + i; break;
			default: break;
		}
	}
	
	if (!pBitmapTOCEntry || !pAcceleratorTOCEntry || !pMetricsTOCEntry || !pEncodingTOCEntry)
	{
		printf("error: PCF does not contain all tables\n");
		delete [] pBuffer;
		return false;
	}

	// read accelerator table
	PCFAcceleratorTable *pAcceleratorTable = (PCFAcceleratorTable *) (pBuffer + pAcceleratorTOCEntry->offset);

	pAcceleratorTable->format = my_invert_byte_order_int(pAcceleratorTable->format);
	pAcceleratorTable->fontAscent = my_invert_byte_order_int(pAcceleratorTable->fontAscent, pAcceleratorTable->format & PCF_BYTE_MASK);
	pAcceleratorTable->fontDescent = my_invert_byte_order_int(pAcceleratorTable->fontDescent, pAcceleratorTable->format & PCF_BYTE_MASK);

	// read metrics table
	PCFMetricsTable *pMetricsTable = (PCFMetricsTable *) (pBuffer + pMetricsTOCEntry->offset);

	pMetricsTable->format = my_invert_byte_order_int(pMetricsTable->format);
	pMetricsTable->metrics_count = my_invert_byte_order_short(pMetricsTable->metrics_count, pMetricsTable->format & PCF_BYTE_MASK);

	// read encoding table
	PCFEncodingTable *pEncodingTable = (PCFEncodingTable *) (pBuffer + pEncodingTOCEntry->offset);

	pEncodingTable->format = my_invert_byte_order_int(pEncodingTable->format);
	pEncodingTable->min_char_or_byte2 = my_invert_byte_order_short(pEncodingTable->min_char_or_byte2, pEncodingTable->format & PCF_BYTE_MASK);
	pEncodingTable->max_char_or_byte2 = my_invert_byte_order_short(pEncodingTable->max_char_or_byte2, pEncodingTable->format & PCF_BYTE_MASK);
	pEncodingTable->min_byte1 = my_invert_byte_order_short(pEncodingTable->min_byte1, pEncodingTable->format & PCF_BYTE_MASK);
	pEncodingTable->max_byte1 = my_invert_byte_order_short(pEncodingTable->max_byte1, pEncodingTable->format & PCF_BYTE_MASK);

	if (pEncodingTable->min_byte1 != 0 || pEncodingTable->max_byte1 != 0)
	{
		printf("error: 2 byte encoding not implemented\n");
		delete [] pBuffer;
		return false;
	}

	const short *pEncodingData = (const short *) (pBuffer + pEncodingTOCEntry->offset + 14);

	// read bitmap table
	int *pBitmapTable = (int *) (pBuffer + pBitmapTOCEntry->offset);
	
	const int nBitmapFormat = my_invert_byte_order_int(pBitmapTable[0]);
	const int nGlyphs = my_invert_byte_order_int(pBitmapTable[1], nBitmapFormat & PCF_BYTE_MASK);
	const int nBitmapSize = my_invert_byte_order_int(pBitmapTable[2 + nGlyphs + (nBitmapFormat & 3)], nBitmapFormat & PCF_BYTE_MASK);
	const int *pGlyphOffsets = pBitmapTable + 2;
	
	int nBytesPerLine = 0;
	switch (nBitmapFormat & 3)
	{
		case 0: nBytesPerLine = 1;
		case 1: nBytesPerLine = 2;
		case 2: nBytesPerLine = 4;
	}
	
	if (nBytesPerLine == 0)
	{
		printf("error: bitmap format invalid\n");
		delete [] pBuffer;
		return false;
	}
	
	// invert byte order if needed
	//for (int i = 0; i < nGlyphs; i++)
	//	pGlyphOffsets[i] = my_invert_byte_order_int(pGlyphOffsets[i], nBitmapFormat & PCF_BYTE_MASK);
	
	if (nBitmapSize % nGlyphs != 0)
	{
		printf("error: irregular glyph size\n");
		delete [] pBuffer;
		return false;
	}
	
	const int nBytesPerGlyph = nBitmapSize / nGlyphs;
	
	if (nBytesPerGlyph % nBytesPerLine != 0)
	{
		printf("error: irregular line size\n");
		delete [] pBuffer;
		return false;
	}
	
	const int nLinesPerGlyph = nBytesPerGlyph / nBytesPerLine;

	if (pMetricsTable->metrics_count != nGlyphs)
	{
		printf("error: glyph count does not match metrics count\n");
		delete [] pBuffer;
		return false;
	}

	// initialize
	Init(nGlyphs);

	// allocate memory for maximum number of coordinates
	int *pCoordinatesX = new int[nLinesPerGlyph * nBytesPerLine * 8];
	int *pCoordinatesY = new int[nLinesPerGlyph * nBytesPerLine * 8];
	
	for (int n = 0; n < nGlyphs; n++)
	{
		const unsigned char *pGlyphBitmapData = ((const unsigned char *) (pBitmapTable + 6 + nGlyphs)) + my_invert_byte_order_int(pGlyphOffsets[n], nBitmapFormat & PCF_BYTE_MASK);

		// collect coordinates
		int nCoordinates = 0;
		
		for (int i = 0; i < nLinesPerGlyph; i++)
		{
			for (int j = 0; j < nBytesPerLine; j++)
			{
				const unsigned char value = pGlyphBitmapData[i * nBytesPerLine + j];
				
				for (int k = 0; k < 8; k++)
					if ((value << k) & 128)
					{
						pCoordinatesX[nCoordinates] = j * 8 + k;
						pCoordinatesY[nCoordinates] = i;
						nCoordinates++;
					}
			}
		}

		// read metric data
		int nWidth, nAscent, nDescent;

		if (pMetricsTable->format & PCF_COMPRESSED_METRICS)
		{
			const unsigned char *pMetricsData = (unsigned char *) (pBuffer + pMetricsTOCEntry->offset + 6);
			nWidth = pMetricsData[n * 5 + 2] - 0x80;
			nAscent = pMetricsData[n * 5 + 3] - 0x80;
			nDescent = pMetricsData[n * 5 + 4] - 0x80;
		}
		else
		{
			const short *pMetricsData = (short *) (pBuffer + pMetricsTOCEntry->offset + 6);
			nWidth = my_invert_byte_order_short(pMetricsData[n * 5 + 2], pMetricsTable->format & PCF_BYTE_MASK);
			nAscent = my_invert_byte_order_short(pMetricsData[n * 5 + 3], pMetricsTable->format & PCF_BYTE_MASK);
			nDescent = my_invert_byte_order_short(pMetricsData[n * 5 + 4], pMetricsTable->format & PCF_BYTE_MASK);
		}

		// store coordinates and other information
		SetGlyph(n, pCoordinatesX, pCoordinatesY, nCoordinates, nWidth, nAscent, nDescent);
	}

	// set encoding table
	for (i = pEncodingTable->min_char_or_byte2; i <= pEncodingTable->max_char_or_byte2; i++)
	{
		const int nIndex = my_invert_byte_order_short(pEncodingData[i], pEncodingTable->format & PCF_BYTE_MASK);
		AddEncoding(i, nIndex);
	}
	
	//printf("\n%i glyphs with %i lines each with %i bytes each (%i = %i)\n", nGlyphs, nLinesPerGlyph, nBytesPerLine, nGlyphs * nLinesPerGlyph * nBytesPerLine, nBitmapSize);

	// set some attributes
	m_nFontHeight = pAcceleratorTable->fontAscent + pAcceleratorTable->fontDescent;
	
	// free memory
	delete [] pCoordinatesX;
	delete [] pCoordinatesY;
	delete [] pBuffer;
	
	return true;
}

void CBitmapFont::DrawText(CByteImage *pImage, const char *pText, int x0, int y0, unsigned char r, unsigned char g, unsigned char b) const
{
	const int nLength = (int) strlen(pText);

	int x = x0, y = y0;

	for (int i = 0; i < nLength; i++)
	{
		int *pCoordinatesX, *pCoordinatesY;
		int nCoordinates, nWidth;

		if (pText[i] == '\n')
		{
			x = x0;
			y += m_nFontHeight;
		}
		else
			if (GetCharacterInformation(pText[i], pCoordinatesX, pCoordinatesY, nCoordinates, nWidth))
			{
				for (int j = 0; j < nCoordinates; j++)
				{
					// DrawPoint is fail safe
					PrimitivesDrawer::DrawPoint(pImage, x + pCoordinatesX[j], y + pCoordinatesY[j], r, g, b);
				}
				
				x += nWidth;
			}
	}
}

void CBitmapFont::DrawText(CByteImage *pImage, const char *pText, int x, int y, Color::Color color) const
{
	unsigned char r, g, b;

	Color::GetRGBValues(color, r, g, b);	

	DrawText(pImage, pText, x, y, r, g, b);
}
