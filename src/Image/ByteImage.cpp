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
// Filename:  ByteImage.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************
// Changes:   24.07.2008, Miguel Bernal Marin
//            * Added methods LoadFromFilePNM and SaveToFilePNM
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ByteImage.h"

#include "Helpers/helpers.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>




// ****************************************************************************
// Defines
// ****************************************************************************

// this is a workaround for a bug in OpenCV:
// on Mac, if not one byte extra is allocated for the pixel memory
// most of the OpenCV functions crash (segmentation fault), such as
// cvErode, cvDilate, cvSmooth, ... (after cvCreateImageHeader and
// cvSetImageData)
#define EXTRA_BYTES		1



// ****************************************************************************
// Constructors / Destructor
// ****************************************************************************

CByteImage::CByteImage()
{
	width = 0;
	height = 0;
	bytesPerPixel = 0;
	pixels = 0;
	type = eGrayScale;
	m_bOwnMemory = false;
}

CByteImage::CByteImage(int nImageWidth, int nImageHeight, ImageType imageType, bool bHeaderOnly)
{
	switch (imageType)
	{
		case eGrayScale:
			bytesPerPixel = 1;
		break;

		case eRGB24:
		case eRGB24Split:
			bytesPerPixel = 3;
		break;
	}

	width = nImageWidth;
	height = nImageHeight;
	type = imageType;

	if (bHeaderOnly)
	{
		pixels = 0;
		m_bOwnMemory = false;
	}
	else
	{
		pixels = new unsigned char[width * height * bytesPerPixel + EXTRA_BYTES];
		m_bOwnMemory = true;
	}
}

CByteImage::CByteImage(const CByteImage &image, bool bHeaderOnly)
{
	width = image.width;
	height = image.height;
	bytesPerPixel = image.bytesPerPixel;
	type = image.type;
	
	if (bHeaderOnly)
	{
		pixels = 0;
		m_bOwnMemory = false;
	}
	else
	{
		pixels = new unsigned char[width * height * bytesPerPixel + EXTRA_BYTES];
		m_bOwnMemory = true;
	}
}

CByteImage::CByteImage(const CByteImage *pImage, bool bHeaderOnly)
{
	width = pImage->width;
	height = pImage->height;
	bytesPerPixel = pImage->bytesPerPixel;
	type = pImage->type;
	
	if (bHeaderOnly)
	{
		pixels = 0;
		m_bOwnMemory = false;
	}
	else
	{
		pixels = new unsigned char[width * height * bytesPerPixel + EXTRA_BYTES];
		m_bOwnMemory = true;
	}
}

CByteImage::~CByteImage()
{
    FreeMemory();
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CByteImage::Set(int nImageWidth, int nImageHeight, ImageType imageType, bool bHeaderOnly)
{
	FreeMemory();

	switch (imageType)
	{
		case eGrayScale:
			bytesPerPixel = 1;
		break;

		case eRGB24:
		case eRGB24Split:
			bytesPerPixel = 3;
		break;
	}

	width = nImageWidth;
	height = nImageHeight;
	type = imageType;

	if (bHeaderOnly)
	{
		pixels = 0;
		m_bOwnMemory = false;
	}
	else
	{
		pixels = new unsigned char[width * height * bytesPerPixel + EXTRA_BYTES];
		m_bOwnMemory = true;
	}
}

void CByteImage::FreeMemory()
{
	if (pixels)
	{
		if (m_bOwnMemory)
			delete [] pixels;

		pixels = 0;
		m_bOwnMemory = false;
	}
}

bool CByteImage::IsCompatible(const CByteImage *pImage) const
{
	return width == pImage->width && height == pImage->height && type == pImage->type;
}


static int my_strcmpi(const char *pString1, const char *pString2)
{
	for (int i = 0;; i++)
	{
		const unsigned char c1 = (unsigned char) toupper((int) pString1[i]);
		const unsigned char c2 = (unsigned char) toupper((int) pString2[i]);

		if (c1 == '\0' || c1 != c2)
			return c1 - c2;
	}
 
	return 0;
}

bool CByteImage::LoadFromFile(const char *pFileName)
{
	const char *pBeginEnding = pFileName + strlen(pFileName) - 4;
	if (my_strcmpi(pBeginEnding, ".bmp") ==  0)
		return LoadFromFileBMP(pFileName);
	else if (my_strcmpi(pBeginEnding, ".pgm") ==  0 || my_strcmpi(pBeginEnding, ".ppm") ==  0)
		return LoadFromFilePNM(pFileName);
		
	return false;
}

bool CByteImage::SaveToFile(const char *pFileName) const
{
	const char *pBeginEnding = pFileName + strlen(pFileName) - 4;
	if (my_strcmpi(pBeginEnding, ".bmp") ==  0)
		return SaveToFileBMP(pFileName);
	else if (my_strcmpi(pBeginEnding, ".pgm") ==  0 || my_strcmpi(pBeginEnding, ".ppm") ==  0)
		return SaveToFilePNM(pFileName);
		
	return false;
}


bool CByteImage::LoadFromFileBMP(const char *pFileName)
{
	const unsigned short MB = 0x4d42;

	// first free memory, in any case
	FreeMemory();

	FILE *f = fopen(pFileName, "rb");
	if (!f)
		return false;

	unsigned char szHeader[4096];

	if (fread(szHeader, 14, 1, f) != 1)
	{
		fclose(f);
		return false;
	}
	
	myBITMAPFILEHEADER bitmapFileHeader;
	
#ifdef IVT_BIG_ENDIAN
	bitmapFileHeader.bfType = invert_byte_order_short(*((unsigned short *) (szHeader)));
	bitmapFileHeader.bfSize = invert_byte_order_int(*((unsigned int *) (szHeader + 2)));
	bitmapFileHeader.bfReserved1 = invert_byte_order_short(*((unsigned short *) (szHeader + 6)));
	bitmapFileHeader.bfReserved2 = invert_byte_order_short(*((unsigned short *) (szHeader + 8)));
	bitmapFileHeader.bfOffBits = invert_byte_order_int(*((unsigned int *) (szHeader + 10)));
#else
	bitmapFileHeader.bfType = *((unsigned short *) (szHeader));
	bitmapFileHeader.bfSize = *((unsigned int *) (szHeader + 2));
	bitmapFileHeader.bfReserved1 = *((unsigned short *) (szHeader + 6));
	bitmapFileHeader.bfReserved2 = *((unsigned short *) (szHeader + 8));
	bitmapFileHeader.bfOffBits = *((unsigned int *) (szHeader + 10));
#endif

	if (bitmapFileHeader.bfType != MB || bitmapFileHeader.bfReserved1 != 0 || bitmapFileHeader.bfReserved2 != 0 || bitmapFileHeader.bfOffBits >= 4096)
	{
		fclose(f);
		return false;
	}

	if (fread(szHeader + 14, bitmapFileHeader.bfOffBits - 14, 1, f) != 1)
	{
		fclose(f);
		return false;
	}

	myBITMAPINFOHEADER bitmapInfoHeader;
	unsigned char *pBitmapInfoHeaderPosition = szHeader + 14;

#ifdef IVT_BIG_ENDIAN
	bitmapInfoHeader.biSize = invert_byte_order_int(*((unsigned int *) (pBitmapInfoHeaderPosition)));
	bitmapInfoHeader.biWidth = invert_byte_order_int(*((unsigned int *) (pBitmapInfoHeaderPosition + 4)));
	bitmapInfoHeader.biHeight = invert_byte_order_int(*((int *) (pBitmapInfoHeaderPosition + 8)));
	bitmapInfoHeader.biPlanes = invert_byte_order_short(*((unsigned short *) (pBitmapInfoHeaderPosition + 12)));
	bitmapInfoHeader.biBitCount = invert_byte_order_short(*((unsigned short *) (pBitmapInfoHeaderPosition + 14)));
	bitmapInfoHeader.biCompression = invert_byte_order_int(*((unsigned int *) (pBitmapInfoHeaderPosition + 16)));
	bitmapInfoHeader.biSizeImage = invert_byte_order_int(*((unsigned int *) (pBitmapInfoHeaderPosition + 20)));
	bitmapInfoHeader.biXPelsPerMeter = invert_byte_order_int(*((unsigned int *) (pBitmapInfoHeaderPosition + 24)));
	bitmapInfoHeader.biYPelsPerMeter = invert_byte_order_int(*((unsigned int *) (pBitmapInfoHeaderPosition + 28)));
	bitmapInfoHeader.biClrUsed = invert_byte_order_int(*((unsigned int *) (pBitmapInfoHeaderPosition + 32)));
	bitmapInfoHeader.biClrImportant = invert_byte_order_int(*((unsigned int *) (pBitmapInfoHeaderPosition + 36)));
#else
	bitmapInfoHeader.biSize = *((unsigned int *) (pBitmapInfoHeaderPosition));
	bitmapInfoHeader.biWidth = *((unsigned int *) (pBitmapInfoHeaderPosition + 4));
	bitmapInfoHeader.biHeight = *((int *) (pBitmapInfoHeaderPosition + 8));
	bitmapInfoHeader.biPlanes = *((unsigned short *) (pBitmapInfoHeaderPosition + 12));
	bitmapInfoHeader.biBitCount = *((unsigned short *) (pBitmapInfoHeaderPosition + 14));
	bitmapInfoHeader.biCompression = *((unsigned int *) (pBitmapInfoHeaderPosition + 16));
	bitmapInfoHeader.biSizeImage = *((unsigned int *) (pBitmapInfoHeaderPosition + 20));
	bitmapInfoHeader.biXPelsPerMeter = *((unsigned int *) (pBitmapInfoHeaderPosition + 24));
	bitmapInfoHeader.biYPelsPerMeter = *((unsigned int *) (pBitmapInfoHeaderPosition + 28));
	bitmapInfoHeader.biClrUsed = *((unsigned int *) (pBitmapInfoHeaderPosition + 32));
	bitmapInfoHeader.biClrImportant = *((unsigned int *) (pBitmapInfoHeaderPosition + 36));
#endif

	if ((bitmapInfoHeader.biBitCount != 8 && bitmapInfoHeader.biBitCount != 24 && bitmapInfoHeader.biBitCount != 32) || bitmapInfoHeader.biWidth <= 0)
	{
		fclose(f);
		return false;
	}

	if (bitmapInfoHeader.biCompression != 0)
	{
		fclose(f);
		return false;
	}

	if (bitmapInfoHeader.biBitCount != 8 && bitmapInfoHeader.biClrUsed > 256)
	{
		fclose(f);
		return false;
	}

	// allocate image
	width = bitmapInfoHeader.biWidth;
	height = abs(bitmapInfoHeader.biHeight);

	int nReadBytesPerPixel;

	const int nTableSize = bitmapInfoHeader.biClrUsed == 0 ? 1024 : 4 * bitmapInfoHeader.biClrUsed;
	unsigned char index_table[1024];
	memcpy(index_table, pBitmapInfoHeaderPosition + bitmapInfoHeader.biSize, nTableSize);

	bool bIndexed = false;

	if (bitmapInfoHeader.biBitCount == 8)
	{
		if (bitmapFileHeader.bfOffBits - 14 - bitmapInfoHeader.biSize != nTableSize)
		{
			fclose(f);
			return false;
		}

		bIndexed = false;

		bool bGrayScale = true;

		for (int i = 0; i < nTableSize; i += 4)
		{
			if (index_table[i] != index_table[i + 1] || index_table[i] != index_table[i + 2])
			{
				// contains non grayscale data
				bIndexed = true;
				bGrayScale = false;
				break;
			}

			if (index_table[i] != (i >> 2))
				bIndexed = true;
		}

		if (bGrayScale)
		{
			type = eGrayScale;
			bytesPerPixel = 1;
			nReadBytesPerPixel = 1;
		}
		else
		{
			type = eRGB24;
			bytesPerPixel = 3;
			nReadBytesPerPixel = 1;
		}
	}
	else
	{
		switch (bitmapInfoHeader.biBitCount)
		{
			case 24:
				type = eRGB24;
				bytesPerPixel = 3;
				nReadBytesPerPixel = 3;
			break;

			case 32:
				type = eRGB24;
				bytesPerPixel = 3;
				nReadBytesPerPixel = 4;
			break;

			default:
				fclose(f);
				return false;
			break;
		}
	}

	const int padding_bytes = ((width * nReadBytesPerPixel) % 4 == 0) ? 0 : 4 - ((width * nReadBytesPerPixel) % 4);

	const int nPixels = width * height;

	// allocate memory
	pixels = new unsigned char[nPixels * bytesPerPixel + EXTRA_BYTES];
	unsigned char *pTempBuffer = new unsigned char[(width * nReadBytesPerPixel + padding_bytes) * height + EXTRA_BYTES];
	m_bOwnMemory = true;

	// fill pixels from file
	if (fread(pTempBuffer, (width * nReadBytesPerPixel + padding_bytes) * height, 1, f) != 1)
	{
		FreeMemory();
		fclose(f);
		delete [] pTempBuffer;
		return false;
	}

	if (bitmapInfoHeader.biHeight < 0)
	{
		if (type == eRGB24)
		{
			const int write_width_bytes = 3 * width;
			const int width_bytes = nReadBytesPerPixel * width;
			const int aligned_width_bytes = width_bytes + padding_bytes;
						
			unsigned char *pHelper1 = pixels;
			unsigned char *pHelper2 = pTempBuffer;

			if (nReadBytesPerPixel == 1)
			{
				// indexed
				for (int i = 0; i < height; i++)
				{
					for (int j = 0, k = 0; j < width_bytes; j++, k += 3)
					{
						pHelper1[k] = index_table[pHelper2[j] << 2];
						pHelper1[k + 1] = index_table[(pHelper2[j] << 2) + 1];
						pHelper1[k + 2] = index_table[(pHelper2[j] << 2) + 2];
					}
				
					pHelper1 += write_width_bytes;
					pHelper2 += aligned_width_bytes;
				}
			}
			else
			{
				// convert from BGR or BGRA to RGB
				for (int i = 0; i < height; i++)
				{
					for (int j = 0, k = 0; j < width_bytes; j += nReadBytesPerPixel, k += 3)
					{
						pHelper1[k] = pHelper2[j + 2];
						pHelper1[k + 1] = pHelper2[j + 1];
						pHelper1[k + 2] = pHelper2[j];
					}
				
					pHelper1 += write_width_bytes;
					pHelper2 += aligned_width_bytes;
				}
			}
		}
		else if (type == eGrayScale)
		{
			const int aligned_width_bytes = width + padding_bytes;

			unsigned char *pHelper1 = pixels;
			unsigned char *pHelper2 = pTempBuffer;
			
			if (bIndexed)
			{
				// indexed
				for (int i = 0; i < height; i++)
				{
					for (int j = 0; j < width; j++)
						pHelper1[j] = index_table[pHelper2[j << 2]];
				
					pHelper1 += width;
					pHelper2 += aligned_width_bytes;
				}
			}
			else
			{
				// just copy
				for (int i = 0; i < height; i++)
				{
					memcpy(pHelper1, pHelper2, width);
				
					pHelper1 += width;
					pHelper2 += aligned_width_bytes;
				}
			}
		}
	}
	else
	{
		if (type == eRGB24)
		{
			const int write_width_bytes = 3 * width;
			const int width_bytes = nReadBytesPerPixel * width;
			const int aligned_width_bytes = width_bytes + padding_bytes;
			
			unsigned char *pHelper1 = pixels;
			unsigned char *pHelper2 = pTempBuffer + aligned_width_bytes * height - aligned_width_bytes;
			
			if (nReadBytesPerPixel == 1)
			{
				// indexed
				for (int i = 0; i < height; i++)
				{
					for (int j = 0, k = 0; j < width_bytes; j++, k += 3)
					{
						pHelper1[k] = index_table[pHelper2[j] << 2];
						pHelper1[k + 1] = index_table[(pHelper2[j] << 2) + 1];
						pHelper1[k + 2] = index_table[(pHelper2[j] << 2) + 2];
					}
					
					pHelper1 += write_width_bytes;
					pHelper2 -= aligned_width_bytes;
				}
			}
			else
			{
				// convert from BGR or BGRA to RGB, and from bottom-left to top-left
				for (int i = 0; i < height; i++)
				{
					for (int j = 0, k = 0; j < width_bytes; j += nReadBytesPerPixel, k += 3)
					{
						pHelper1[k] = pHelper2[j + 2];
						pHelper1[k + 1] = pHelper2[j + 1];
						pHelper1[k + 2] = pHelper2[j];
					}
					
					pHelper1 += write_width_bytes;
					pHelper2 -= aligned_width_bytes;
				}
			}
		}
		else if (type == eGrayScale)
		{
			const int aligned_width_bytes = width + padding_bytes;
			
			unsigned char *pHelper1 = pixels;
			unsigned char *pHelper2 = pTempBuffer + aligned_width_bytes * height - aligned_width_bytes;
			
			if (bIndexed)
			{
				// indexed
				for (int i = 0; i < height; i++)
				{
					for (int j = 0; j < width; j++)
						pHelper1[j] = index_table[pHelper2[j] << 2];
					
					pHelper1 += width;
					pHelper2 -= aligned_width_bytes;
				}
			}
			else
			{
				// convert from bottom-left to top-left
				for (int i = 0; i < height; i++)
				{
					memcpy(pHelper1, pHelper2, width);
					
					pHelper1 += width;
					pHelper2 -= aligned_width_bytes;
				}
			}
		}
	}

	fclose(f);
	delete [] pTempBuffer;

	return true;
}

bool CByteImage::SaveToFileBMP(const char *pFileName) const
{
	if (!pixels || !width || !height)
		return false;

	const unsigned short MB = 0x4d42;
	const int padding_bytes = ((width * bytesPerPixel) % 4 == 0) ? 0 : 4 - ((width * bytesPerPixel) % 4);

	myBITMAPFILEHEADER bitmapFileHeader;
	bitmapFileHeader.bfType = MB;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
		
	myBITMAPINFOHEADER bitmapInfoHeader;
	bitmapInfoHeader.biSize = 40;
	bitmapInfoHeader.biWidth = width;
	bitmapInfoHeader.biHeight = height;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biXPelsPerMeter = 0;
	bitmapInfoHeader.biYPelsPerMeter = 0;
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;

	switch (type)
	{
		case eGrayScale:
			bitmapInfoHeader.biBitCount = 8;
			bitmapInfoHeader.biSizeImage = (width + padding_bytes) * height;
			bitmapInfoHeader.biCompression = 0;
			bitmapFileHeader.bfOffBits = 14 + 40 + 1024;
			bitmapFileHeader.bfSize = bitmapFileHeader.bfOffBits + bitmapInfoHeader.biSizeImage;
		break;

		case eRGB24:
		case eRGB24Split:
			bitmapInfoHeader.biBitCount = 24;
			bitmapInfoHeader.biSizeImage = (width * 3 + padding_bytes) * height;
			bitmapInfoHeader.biCompression = 0;
			bitmapFileHeader.bfOffBits = 14 + 40;
			bitmapFileHeader.bfSize = bitmapFileHeader.bfOffBits + bitmapInfoHeader.biSizeImage;
		break;

		default:
			return false;
		break;
	}
	
	FILE *f = fopen(pFileName, "wb");
	if (!f)
		return false;

	unsigned char szHeader[14 + 40];
	unsigned char *pBitmapInfoHeaderPosition = szHeader + 14;

#ifdef IVT_BIG_ENDIAN
	*((unsigned short *) (szHeader)) = invert_byte_order_short(bitmapFileHeader.bfType);
	*((unsigned int *) (szHeader + 2)) = invert_byte_order_int(bitmapFileHeader.bfSize);
	*((unsigned short *) (szHeader + 6)) = invert_byte_order_short(bitmapFileHeader.bfReserved1);
	*((unsigned short *) (szHeader + 8)) = invert_byte_order_short(bitmapFileHeader.bfReserved2);
	*((unsigned int *) (szHeader + 10)) = invert_byte_order_int(bitmapFileHeader.bfOffBits);

	// write bitmap info header to file
	*((unsigned int *) (pBitmapInfoHeaderPosition)) = invert_byte_order_int(bitmapInfoHeader.biSize);
	*((unsigned int *) (pBitmapInfoHeaderPosition + 4)) = invert_byte_order_int(bitmapInfoHeader.biWidth);
	*((unsigned int *) (pBitmapInfoHeaderPosition + 8)) = invert_byte_order_int(bitmapInfoHeader.biHeight);
	*((unsigned short *) (pBitmapInfoHeaderPosition + 12)) = invert_byte_order_short(bitmapInfoHeader.biPlanes);
	*((unsigned short *) (pBitmapInfoHeaderPosition + 14)) = invert_byte_order_short(bitmapInfoHeader.biBitCount);
	*((unsigned int *) (pBitmapInfoHeaderPosition + 16)) = invert_byte_order_int(bitmapInfoHeader.biCompression);
	*((unsigned int *) (pBitmapInfoHeaderPosition + 20)) = invert_byte_order_int(bitmapInfoHeader.biSizeImage);
	*((unsigned int *) (pBitmapInfoHeaderPosition + 24)) = invert_byte_order_int(bitmapInfoHeader.biXPelsPerMeter);
	*((unsigned int *) (pBitmapInfoHeaderPosition + 28)) = invert_byte_order_int(bitmapInfoHeader.biYPelsPerMeter);
	*((unsigned int *) (pBitmapInfoHeaderPosition + 32)) = invert_byte_order_int(bitmapInfoHeader.biClrUsed);
	*((unsigned int *) (pBitmapInfoHeaderPosition + 36)) = invert_byte_order_int(bitmapInfoHeader.biClrImportant);
#else
	*((unsigned short *) (szHeader)) = bitmapFileHeader.bfType;
	*((unsigned int *) (szHeader + 2)) = bitmapFileHeader.bfSize;
	*((unsigned short *) (szHeader + 6)) = bitmapFileHeader.bfReserved1;
	*((unsigned short *) (szHeader + 8)) = bitmapFileHeader.bfReserved2;
	*((unsigned int *) (szHeader + 10)) = bitmapFileHeader.bfOffBits;

	// write bitmap info header to file
	*((unsigned int *) (pBitmapInfoHeaderPosition)) = bitmapInfoHeader.biSize;
	*((unsigned int *) (pBitmapInfoHeaderPosition + 4)) = bitmapInfoHeader.biWidth;
	*((unsigned int *) (pBitmapInfoHeaderPosition + 8)) = bitmapInfoHeader.biHeight;
	*((unsigned short *) (pBitmapInfoHeaderPosition + 12)) = bitmapInfoHeader.biPlanes;
	*((unsigned short *) (pBitmapInfoHeaderPosition + 14)) = bitmapInfoHeader.biBitCount;
	*((unsigned int *) (pBitmapInfoHeaderPosition + 16)) = bitmapInfoHeader.biCompression;
	*((unsigned int *) (pBitmapInfoHeaderPosition + 20)) = bitmapInfoHeader.biSizeImage;
	*((unsigned int *) (pBitmapInfoHeaderPosition + 24)) = bitmapInfoHeader.biXPelsPerMeter;
	*((unsigned int *) (pBitmapInfoHeaderPosition + 28)) = bitmapInfoHeader.biYPelsPerMeter;
	*((unsigned int *) (pBitmapInfoHeaderPosition + 32)) = bitmapInfoHeader.biClrUsed;
	*((unsigned int *) (pBitmapInfoHeaderPosition + 36)) = bitmapInfoHeader.biClrImportant;
#endif

	// write header to file
	if (fwrite(szHeader, 14 + 40, 1, f) != 1)
	{
		fclose(f);
		return false;
	}

	// if 8 bit grayscale then write color table
	if (type == eGrayScale)
	{
		char szColorTable[1024];

		for (int offset = 0, i = 0; i < 256; i++, offset += 4)
		{
			szColorTable[offset] = i;
			szColorTable[offset + 1] = i;
			szColorTable[offset + 2] = i;
			szColorTable[offset + 3] = 0;
		}

		// write color table to file
		if (fwrite(szColorTable, 1024, 1, f) != 1)
		{
			fclose(f);
			return false;
		}
	}

	unsigned char *pTempBuffer = new unsigned char[(width * bytesPerPixel + padding_bytes) * height];
	
	if (type == eRGB24)
	{
		const int width_bytes = 3 * width;
		
		unsigned char *pHelper1 = pTempBuffer;
		unsigned char *pHelper2 = pixels + width * height * 3 - width_bytes;
		
		// convert from RGB to BGR, and from top-left to bottom-left
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width_bytes; j += 3)
			{
				pHelper1[j] = pHelper2[j + 2];
				pHelper1[j + 1] = pHelper2[j + 1];
				pHelper1[j + 2] = pHelper2[j];
			}

			for (int j = width_bytes; j < width_bytes + padding_bytes; j++)
				pHelper1[j] = 0;
			
			pHelper1 += width_bytes + padding_bytes;
			pHelper2 -= width_bytes;
		}
	}
	else if (type == eRGB24Split)
	{
		const int width_bytes = 3 * width;

		unsigned char *pHelper = pTempBuffer;
		unsigned char *pHelperR = pixels + width * height - width;
		unsigned char *pHelperG = pHelperR + width * height;
		unsigned char *pHelperB = pHelperG + width * height;
		
		// convert from RGB to BGR, and from top-left to bottom-left
		for (int i = 0; i < height; i++)
		{
			for (int j = 0, offset = 0; j < width_bytes; j += 3, offset++)
			{
				pHelper[j] = pHelperB[offset];
				pHelper[j + 1] = pHelperG[offset];
				pHelper[j + 2] = pHelperR[offset];
			}

			for (int j = width_bytes; j < width_bytes + padding_bytes; j++)
				pHelper[j] = 0;
			
			pHelper += width_bytes + padding_bytes;
			pHelperR -= width;
			pHelperG -= width;
			pHelperB -= width;
		}
	}
	else if (type == eGrayScale)
	{
		unsigned char *pHelper1 = pTempBuffer;
		unsigned char *pHelper2 = pixels + width * height - width;
		
		// convert from top-left to bottom-left
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
				pHelper1[j] = pHelper2[j];

			for (int j = width; j < width + padding_bytes; j++)
				pHelper1[j] = 0;
				
			pHelper1 += width + padding_bytes;
			pHelper2 -= width;
		}

	}
	
	if (fwrite(pTempBuffer, (width * bytesPerPixel + padding_bytes) * height, 1, f) != 1)
	{
		delete [] pTempBuffer;
		fclose(f);
		return false;
	}
	
	delete [] pTempBuffer;
	fclose(f);

	return true;
}


bool CByteImage::LoadFromFilePNM(const char *pFileName)
{
	// first free memory, in any case
	FreeMemory();

	FILE *f = fopen(pFileName, "rb");
	if (!f)
		return false;

	int character, maxVal; 
	char sNumber[16];
	short idx;

	character = fgetc(f);
	if (character != 'P')
	{
		fclose(f);
		return false;
	}

	character = fgetc(f);
	if (character != '5' && character != '6' )
	{
		fclose(f);
		return false;
	}

	if (character == '5')
	{
		type = eGrayScale;
		bytesPerPixel = 1;
	}
	else
	{
		type = eRGB24;
		bytesPerPixel = 3;
	}

	do
	{
		// Read whitespaces
		character = fgetc(f);
	} while (character == ' ' || character == '\n' || character == '\r');

	// Skip comments
	while (character == '#')
	{
		while (fgetc(f) != '\n');
		character = fgetc(f);
	}

	// Read width
	idx = 0;
	while (character != ' ' && character != '\n' && character != '\r')
	{
		sNumber[idx++] =(char) character;
		character = fgetc(f);
	}
	sscanf(sNumber,"%d",&width);

	do
	{  // Read whitespaces
		character = fgetc(f);
	} while (character == ' ' || character == '\n' || character == '\r');

	// Skip comments
	while (character == '#')
	{
		while (fgetc(f) != '\n');
		character = fgetc(f);
	}

	// Read height
	idx = 0;
	while (character != ' ' && character != '\n' && character != '\r') {
	        sNumber[idx++] =(char) character;
		character = fgetc(f);
	}
	sscanf(sNumber,"%d",&height);

	do
	{
		// Read whitespaces
		character = fgetc(f);
	} while (character == ' ' || character == '\n' || character == '\r');

	// Skip comments
	while (character == '#')
	{
		while (fgetc(f) != '\n');
		character = fgetc(f);
	}

	// The maximum color value (Maxval)
	idx = 0;
	while (character != ' ' && character != '\n' && character != '\r')
	{
		sNumber[idx++] =(char) character;
		character = fgetc(f);
	}
	sscanf(sNumber,"%d",&maxVal);

	if (maxVal != 255)
	{
		fclose(f);
		return false;
	}

	pixels = new unsigned char[width * height * bytesPerPixel + EXTRA_BYTES];
	m_bOwnMemory = true;

	if (fread (pixels , width * height * bytesPerPixel, 1, f ) !=  1)
	{
		FreeMemory();
		fclose(f);
		return false;
	}

	fclose(f);

	return true;
}

bool CByteImage::SaveToFilePNM(const char *pFileName) const
{
	if (!pixels || !width || !height)
		return false;

	FILE *f = fopen(pFileName, "wb");
	if (!f)
		return false;

	switch (type)
	{
		case eGrayScale:
			if (fprintf(f,"P5\n%d %d\n255\n", width, height) < 10)
			{
				fclose(f);
				return false;
			}
		break;
		
		case eRGB24:
		case eRGB24Split:
			if (fprintf(f,"P6\n%d %d\n255\n",width , height) < 10)
			{
				fclose(f);
				return false;
			}
		break;
		
		default:
			fclose(f);
			return false;
		break;
	}
	
	if (type == eGrayScale || type == eRGB24)
	{
		if (fwrite(pixels, width * height * bytesPerPixel, 1, f ) != 1)
		{
			fclose(f);
			return false;
		}
	}
	else if (type == eRGB24Split)
	{
		const int nPixels = width * height;
		
		unsigned char *pTempBuffer = new unsigned char[nPixels * 3];
		const unsigned char *pHelperR = pixels;
		const unsigned char *pHelperG = pHelperR + nPixels;
		const unsigned char *pHelperB = pHelperG + nPixels;
		
		for (int i = 0, offset = 0; i < nPixels; i++, offset += 3)
		{
			pTempBuffer[offset] = pHelperR[i];
			pTempBuffer[offset + 1] = pHelperG[i];
			pTempBuffer[offset + 2] = pHelperB[i];
		}

		if (fwrite(pTempBuffer, nPixels * 3, 1, f ) != 1)
		{
			delete [] pTempBuffer;
			fclose(f);
			return false;
		}

		delete [] pTempBuffer;
	}
	
	fclose(f);

	return true;
}
