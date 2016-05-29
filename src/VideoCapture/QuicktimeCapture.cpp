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
// Filename:  QuicktimeCapture.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "QuicktimeCapture.h"
#include "Image/ByteImage.h"

#include <QuickTime/QuickTimeStreaming.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CQuicktimeCapture::CQuicktimeCapture(VideoMode mode, const char *pComponentName) : m_mode(mode)
{
	grabber = 0;
	videoChannel = 0;
	gWorld = 0;
	m_pBuffer = 0;
	imgDesc = 0;
	
	width = -1;
	height = -1;
	
	m_sComponentName = "";
	
	if (pComponentName)
		m_sComponentName += pComponentName;
	
	m_bImagePending = false;
}

CQuicktimeCapture::~CQuicktimeCapture()
{
	CloseCamera();
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CQuicktimeCapture::OpenCamera()
{
	CloseCamera();
	
	switch (m_mode)
	{
		case e320x240:
			width = 320;
			height = 240;
		break;
		
		case e640x480:
			width = 640;
			height = 480;
		break;
		
		default:
			return false;
	}
	
	srcRect.left = 0;
	srcRect.right = width - 1;
	srcRect.top = 0;
	srcRect.bottom = height - 1;
	
	Component comp = 0;
	Handle grabberName = NewHandle(0);
	Handle info = NewHandle(0);
	ComponentDescription desc;
  
	desc.componentType = SeqGrabComponentType;
	desc.componentSubType = 0;
	desc.componentManufacturer = 0;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;  

	// find a sequence grabber that matches the given component name
	bool bFound = false;
	while (comp = FindNextComponent(comp, &desc))
	{
		if (GetComponentInfo(comp, &desc, grabberName, info, 0) != noErr)
		{
			printf("error: could not get info on component id = %i\n", comp);
			return false;
		}
		
		printf("grabber: '%s'\n", *grabberName + 1);

		if (m_sComponentName.length() == 0 || strcmp(m_sComponentName.c_str(), *grabberName + 1) == 0)
		{
			bFound = true;
			break;
		}
    }
 
	if (!bFound)
	{
		printf("error: could not find a sequence grabber named '%s'\n", m_sComponentName.c_str());
		return false;
	}
	
	// found grabber, open and initialize it
	grabber = OpenComponent(comp);
	
	if (!grabber)
	{
		printf("error: could not open sequence grabber\n");
		return false;
	}
	
	if (SGInitialize(grabber) != noErr)
	{
		printf("errror: could not initialize the sequence grabber\n");
		return false;
	}

	if (SGSetDataRef(grabber, 0, 0, seqGrabDontMakeMovie) != noErr)
	{
		printf("error: could not set data reference on sequence grabber\n");
		return false;
	}

	if (QTNewGWorld(&gWorld, k24RGBPixelFormat, &srcRect, 0, 0, 0) != noErr)
	{
		printf("error: could not create new gworld\n");
		return false;
	}

	if (!LockPixels(GetPortPixMap(gWorld)))
	{
		printf("error: could not lock pixels in new gworld\n");
		return false;
	}

	if (SGSetGWorld(grabber, gWorld, 0) != noErr)
	{
		printf("error: could not set gworld\n");
		return false;
	}
  
	// create video channel
	if (SGNewChannel(grabber, VideoMediaType, &videoChannel) != noErr || !videoChannel)
	{
		printf("error: could not open video channel\n");
		return false;
	}

	if (SGSetChannelUsage(videoChannel, seqGrabRecord) != noErr)
	{
		printf("error: could not set channel usage to 'record'\n");
		return false;
	}
			     
	if (SGSetChannelBounds(videoChannel, &srcRect) != noErr)
	{
		printf("error: could not set channel bounds to %ix%i\n", width, height);
		return false;
	}

	if (SGSetDataProc(grabber, NewSGDataUPP(sDataProc), (long)this) != noErr)
	{
		printf("error: could not set data proc\n");
		return false;
	}
	
	SGStartRecord(grabber);
	
	return true;
}

void CQuicktimeCapture::CloseCamera()
{
	if (grabber)
	{
		SGStop(grabber);
	
		if (videoChannel)
		{
			SGDisposeChannel(grabber, videoChannel);
			videoChannel = 0;
		}

		if (gWorld)
		{
			DisposeGWorld(gWorld);
			gWorld = 0;
		}
		
		grabber = 0;
	}
	
	m_bImagePending = false;
}

bool CQuicktimeCapture::CaptureImage(CByteImage **ppImages)
{
	if (!videoChannel)
		return false;
		
	CByteImage *pImage = ppImages[0];
	if (pImage->width != width || pImage->height != height || pImage->type != CByteImage::eRGB24)
		return false;
	
	while (!m_bImagePending)
		SGIdle(grabber);
		
	ImageSequence image_sequence;

	if (DecompressSequenceBegin(&image_sequence, imgDesc, gWorld, 0, &srcRect, 0, srcCopy, 0, 0, codecNormalQuality, bestSpeedCodec) != noErr)
	{
		printf("error: could not begin decompression\n");
		return false;
	}

	CodecFlags ignore;
  
	DecompressSequenceFrameS(image_sequence, m_pBuffer, m_lImageLength, 0, &ignore, NULL);

	const int nBytes = 3 * pImage->width * pImage->height;
	const unsigned char *input = (unsigned char *) GetPixBaseAddr(GetGWorldPixMap(gWorld));
	unsigned char *output = pImage->pixels;
	for (int i = 0, offset = 0; i < nBytes; i += 3, offset += 4)
	{
		output[i] = input[offset + 1];
		output[i + 1] = input[offset + 2];
		output[i + 2] = input[offset + 3];
	}
	
	CDSequenceEnd(image_sequence);
	
	m_bImagePending = false;
	
	return true;
}

pascal OSErr CQuicktimeCapture::sDataProc(SGChannel channel, Ptr p, long length, long *offset, long chRefCon, TimeValue time, short writeType, long refcon)
{
	return ((CQuicktimeCapture *) refcon)->dataProc(channel, p, length);
}

OSErr CQuicktimeCapture::dataProc(SGChannel channel, Ptr p, long length)
{
	if (!imgDesc)
	{
		imgDesc = (ImageDescriptionHandle) NewHandle(0);
		SGGetChannelSampleDescription(videoChannel, (Handle) imgDesc);
	}
	
	m_pBuffer = p;
	m_lImageLength = length;
	m_bImagePending = true;

	return noErr;
}
