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
// Filename:  VFWCapture.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "VFWCapture.h"

#include "Image/ByteImage.h"
#include "Image/ImageProcessor.h"
#include "Helpers/helpers.h"

#include <vfw.h>
#include <wingdi.h>


// ****************************************************************************
// Defines
// ****************************************************************************

// FOURCC codes understood by the capture code
#define BI_I420 0x30323449
#define BI_YUY2 0x32595559


// ****************************************************************************
// Global and static variables
// ****************************************************************************

CRITICAL_SECTION critical_section;
static CByteImage *pCapturedImage = 0;


// ****************************************************************************
// Static functions
// ****************************************************************************

#define myClip(x) (((x) > 255) ? 255 : ((x) < 0) ? 0 : (unsigned char) (x))
static inline void yuv2rgb(unsigned char y, unsigned char u, unsigned char v, unsigned char *out)
{
	int C = (int) y - 16;
	int D = (int) u - 128;
	int E = (int) v - 128;

	int R = (( 298 * C + 409 * E + 128) >> 8);
	int G = (( 298 * C - 100 * D - 208 * E + 128) >> 8);
	int B = (( 298 * C + 516 * D + 128) >> 8);

	out[0] = myClip(R);
	out[1] = myClip(G);
	out[2] = myClip(B);
}
#undef myClip

static LRESULT CALLBACK CallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	EnterCriticalSection(&critical_section);

	if (!pCapturedImage)
	{
		LeaveCriticalSection(&critical_section);
		return (LRESULT) TRUE;
	}

	pCapturedImage->pixels = (unsigned char *) lpVHdr->lpData;

	LeaveCriticalSection(&critical_section);

	return (LRESULT) TRUE;
}


// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CVFWCapture::CVFWCapture(int nDriverIndex)
{
	pCapturedImage = 0;
	m_hCaptureWnd = 0;

	m_nWidth = -1;
	m_nHeight = -1;
	m_nBitsPerPixel = 0;
	m_type = CByteImage::eGrayScale;

	m_nDriverIndex = nDriverIndex;
	m_bFlipY = false;

	m_bCameraOpened = false;

	for (int i = 0; i < 1024; i++)
		m_clip[i] = i < 256 ? i : 255;

	InitializeCriticalSection(&critical_section);
}

CVFWCapture::~CVFWCapture()
{
	CloseCamera();

	if (m_hCaptureWnd)
	{
		DestroyWindow(m_hCaptureWnd);
		m_hCaptureWnd = 0;
	}

	if (pCapturedImage)
		delete pCapturedImage;

	DeleteCriticalSection(&critical_section);
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CVFWCapture::OpenCamera()
{
	// prevent multiple init
	if (!m_hCaptureWnd)
	{
		// init attributes and create capture window
		m_hCaptureWnd = capCreateCaptureWindow("Capture Window", 0, 0, 0, 640, 480, NULL, 0);
		if (!m_hCaptureWnd)
			return false;

		// set callback function for single frame capturing
		if (!capSetCallbackOnFrame(m_hCaptureWnd, CallbackProc))
		{
			DestroyWindow(m_hCaptureWnd);
			m_hCaptureWnd = 0;
			return false;
		}
	}

	// connect to driver
	if (!capDriverConnect(m_hCaptureWnd, m_nDriverIndex))
	{
		DestroyWindow(m_hCaptureWnd);
		m_hCaptureWnd = 0;
		return false;
	}

	UpdateInformation();

	capOverlay(m_hCaptureWnd, FALSE);
	capPreview(m_hCaptureWnd, FALSE);

	CAPTUREPARMS s;
	s.dwRequestMicroSecPerFrame = 41667;
	s.fMakeUserHitOKToCapture = FALSE;
	s.wPercentDropForError = 10;
	s.fYield = TRUE;
	s.dwIndexSize = 34952;
	s.wChunkGranularity = 0;
	s.fUsingDOSMemory = FALSE;
	s.wNumVideoRequested = 10;
	s.fCaptureAudio = FALSE;
	//s.wNumAudioRequested = 0;
	s.vKeyAbort = VK_ESCAPE;
	s.fAbortLeftMouse = FALSE;
	s.fAbortRightMouse = FALSE;
	s.fLimitEnabled = FALSE;
	//s.wTimeLimit = 0;
	s.fMCIControl = FALSE;
	//s.fStepMCIDevice = FALSE;
	//s.dwMCIStartTime = 0;
	//s.dwMCIStopTime = 0;
	s.fStepCaptureAt2x = FALSE;
	s.wStepCaptureAverageFrames = 1;
	s.dwAudioBufferSize = 0;
	s.fDisableWriteCache = FALSE;
	s.AVStreamMaster = AVSTREAMMASTER_NONE;

	capCaptureSetSetup(m_hCaptureWnd, &s, sizeof(CAPTUREPARMS));
	//capCaptureSequenceNoFile(m_hCaptureWnd);

	m_bCameraOpened = true;
	
	return true;
}

void CVFWCapture::CloseCamera()
{
	if (m_bCameraOpened)
	{
		//capCaptureAbort(m_hCaptureWnd);
		//capCaptureStop(m_hCaptureWnd);
		capDriverDisconnect(m_hCaptureWnd);

		m_nWidth = 0;
		m_nHeight = 0;
		m_nBitsPerPixel = 0;

		m_bCameraOpened = false;
	}
}

bool CVFWCapture::GetDriverName(int nDriverIndex, std::string &sName)
{
	char szName[1000];
	char szVersion[1000];

	if (!capGetDriverDescription(nDriverIndex, szName, sizeof(szName), szVersion, sizeof(szVersion)))
		return false;

	sName = "";
	sName += szName;

	return true;
}

void CVFWCapture::ShowVideoFormatDialog()
{
	EnterCriticalSection(&critical_section);

	CAPDRIVERCAPS gCapDriverCaps;

	// get driver information
	if (m_hCaptureWnd && capDriverGetCaps(m_hCaptureWnd, &gCapDriverCaps, sizeof(CAPDRIVERCAPS)))
	{
		if (gCapDriverCaps.fHasDlgVideoFormat)
		{
			capDlgVideoFormat(m_hCaptureWnd);
			UpdateInformation();
		}
	}

	LeaveCriticalSection(&critical_section);
}

void CVFWCapture::ShowVideoSourceDialog()
{
	CAPDRIVERCAPS gCapDriverCaps;

	// get driver information
	if (m_hCaptureWnd && capDriverGetCaps(m_hCaptureWnd, &gCapDriverCaps, sizeof(CAPDRIVERCAPS)))
	{
		if (gCapDriverCaps.fHasDlgVideoSource)
		{
			capDlgVideoSource(m_hCaptureWnd);
			UpdateInformation();
		}
	}
}

void CVFWCapture::UpdateInformation()
{
	EnterCriticalSection(&critical_section);

	BITMAPINFO bitmapInfo;

	if (m_hCaptureWnd && capGetVideoFormat(m_hCaptureWnd, &bitmapInfo, sizeof(BITMAPINFO)))
	{
		m_nWidth = bitmapInfo.bmiHeader.biWidth;
		m_nHeight = abs(bitmapInfo.bmiHeader.biHeight);
		m_nBitsPerPixel = bitmapInfo.bmiHeader.biBitCount;
		m_nCompression = bitmapInfo.bmiHeader.biCompression;

		m_bFlipY = false;

		// For compressed video and YUV formats, this member is a FOURCC code, specified as a DWORD 
		// in little-endian order. For example, YUYV video has the FOURCC 'VYUY' or 0x56595559. For 
		// more information, see FOURCC Codes.
		// For uncompressed RGB formats, the following values are possible:
		// Value 			Description
		// BI_RGB 			Uncompressed RGB.
		// BI_BITFIELDS 	Uncompressed RGB with color masks. Valid for 16-bpp and 32-bpp bitmaps.
		if (m_nCompression != BI_RGB && m_nCompression != BI_BITFIELDS)
		{
			printf("fourcc = %08x (%c%c%c%c)\n", m_nCompression, (m_nCompression >> 0) & 0xff, (m_nCompression >> 8) & 0xff, (m_nCompression >> 16) & 0xff, (m_nCompression >> 24) & 0xff);
			m_type = CByteImage::eRGB24;
		}
		else
		{
			m_type = m_nBitsPerPixel > 8 ? CByteImage::eRGB24 : CByteImage::eGrayScale;
			
			if (bitmapInfo.bmiHeader.biHeight > 0)
			{
				// image is transmitted bottom-up (vertically flipped)
				m_bFlipY = true;
			}
		}

		if (pCapturedImage)
			delete pCapturedImage;
		
		pCapturedImage = new CByteImage(m_nWidth, m_nHeight, m_type, true);
	}

	LeaveCriticalSection(&critical_section);
}

bool CVFWCapture::CaptureImage(CByteImage **ppImages)
{
	capGrabFrameNoStop(m_hCaptureWnd);

	EnterCriticalSection(&critical_section);

	// check if input image matches format
	if (!m_bCameraOpened || !m_hCaptureWnd || !ppImages || !ppImages[0] || m_nWidth != ppImages[0]->width || m_nHeight != ppImages[0]->height || m_type != ppImages[0]->type)
	{
		LeaveCriticalSection(&critical_section);
		return false;
	}

	if (pCapturedImage && pCapturedImage->pixels)
	{
		if (pCapturedImage->type == CByteImage::eRGB24)
		{
			unsigned char *output = ppImages[0]->pixels;

			if (m_nCompression == BI_RGB)
			{
				if (m_nBitsPerPixel == 24)
				{
					const int nBytes = m_nWidth * m_nHeight * 3;
					const unsigned char *input = pCapturedImage->pixels;

					for (int i = 0; i < nBytes; i += 3)
					{
						output[i] = input[i + 2];
						output[i + 1] = input[i + 1];
						output[i + 2] = input[i];
					}
				}
			}
			else if (m_nCompression == BI_BITFIELDS)
			{
				printf("error: BI_BITFIELDS not yet implemented\n");
			}
			else
			{
				// thanks to Paul van Niekerk
				if (m_nCompression == BI_I420)
				{
					const int nBytes = m_nWidth * m_nHeight;
					const unsigned char *inputY = pCapturedImage->pixels;
					const unsigned char *inputU = inputY + nBytes;
					const unsigned char *inputV = inputU + nBytes / 4;

					int i = 0;
					for (int h = 0; h < m_nHeight; h += 1)
					{
						for (int w = 0; w < m_nWidth; w += 1)
						{
							int j = (h / 2) * (m_nWidth / 2) + (w / 2);
							yuv2rgb(inputY[i], inputU[j], inputV[j], &output[3 * i]);
							i += 1;
						}
					}
				}
				else if (m_nCompression == BI_YUY2)
				{
					const int nBytes = m_nWidth * m_nHeight * 2;
					const unsigned char *input = pCapturedImage->pixels;

					for (int i = 0; i < nBytes; i += 4)
					{
						unsigned char y0 = *input++;
						unsigned char u = *input++;
						unsigned char y1 = *input++;
						unsigned char v = *input++;

						yuv2rgb(y0, u, v, output);
						output += 3;
						yuv2rgb(y1, u, v, output);
						output += 3;
					}
				}
			}
		}
		else
			ImageProcessor::CopyImage(pCapturedImage, ppImages[0]);
	}

	LeaveCriticalSection(&critical_section);

	if (m_bFlipY)
		ImageProcessor::FlipY(ppImages[0], ppImages[0]);

	return true;
}

#undef BI_I420
#undef BI_YUY2
