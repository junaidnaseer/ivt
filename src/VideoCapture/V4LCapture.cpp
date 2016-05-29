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
// Filename:  V4LCapture.cpp
// Author:    Alexander Kaspar, Tilo Gockel, Pedram Azad
// Date:      19.03.2009
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "V4LCapture.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CV4LCapture::CV4LCapture(const char *pDeviceName, int nChannel, CVideoCaptureInterface::VideoMode videoMode)
{
	m_sDeviceName = "";
	m_sDeviceName += pDeviceName;
	
	m_nChannel = nChannel;
	m_videoMode = videoMode;

	width = -1;
	height = -1;
}

CV4LCapture::~CV4LCapture()
{
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CV4LCapture::OpenCamera()
{
	video_capability capability;
	video_channel queryChannel;
	video_channel selectedChannel;
	video_window captureWindow;
	video_picture imageProperties;
	v4l2_capability cap;
	

	selectedChannel.channel = m_nChannel;

        if ((m_nDeviceHandle = open(m_sDeviceName.c_str(), O_RDWR)) == -1)
        {       // could not open device
                printf ("Could not open device %s - %s\n", m_sDeviceName.c_str(), strerror(errno));
                return false;
        }

	// get device capabilities
        if (ioctl(m_nDeviceHandle, VIDIOCGCAP, &capability) == -1)
        {       // query failed
                printf ("could not obtain device capabilities\n");
                return false;
        }

        if ((capability.type & VID_TYPE_CAPTURE) == 0)
        {       // this device cannot capture video to memory, exit
                printf ("this device cannot capture video to memory\n");
                return false;
        }

        // enumerate and print out the channels
        int i = 0;
        while (i < capability.channels)
        {
                queryChannel.channel = i;
                if (ioctl (m_nDeviceHandle, VIDIOCGCHAN, &queryChannel) != -1)
                {       // ioctl success, queryChannel contains information about this channel
                        printf ("%d. %s\n", queryChannel.channel, queryChannel.name);
                }

                i++;
        }

        selectedChannel.norm = VIDEO_MODE_NTSC;
        if (ioctl (m_nDeviceHandle, VIDIOCSCHAN, &selectedChannel) == -1)
        {
		// could not set the selected channel
		printf ("Could not set channel #%d\nNot a fatal error.", selectedChannel.channel);
        }

	// set width and height attributes
	switch (m_videoMode)
	{
		case e320x240: width = 320; height = 240; break;
		case e640x480: width = 640; height = 480; break;
		case e800x600: width = 800; height = 600; break;
		case e768x576: width = 768; height = 576; break;
		case e1024x768: width = 1024; height = 768; break;
		case e1280x960: width = 1280; height = 960; break;
		default: return false;
	}

	// set the desired width and height
        if ((capability.type & VID_TYPE_SCALES) != 0)
        {       // supports the ability to scale captured images
                captureWindow.x = 0;
                captureWindow.y = 0;
                captureWindow.width = width;
                captureWindow.height = height;
                captureWindow.chromakey = 0;
                captureWindow.flags = 0;
                captureWindow.clips = 0;
                captureWindow.clipcount = 0;

		if (ioctl (m_nDeviceHandle, VIDIOCSWIN, &captureWindow) == -1)
                {
			// could not set window values for capture
                        printf ("Could not set desired dimensions\nNot a fatal error.\n");
                }
        }
	
	// retrieve the actual width and height of the capturing images
        if (ioctl(m_nDeviceHandle, VIDIOCGWIN, &captureWindow) == -1)
        {       // could not obtain specifics of capture window
                printf ("Could not obtain capture window dimensions.\n");
        }
        width = captureWindow.width;
        height = captureWindow.height;
        printf ("Capturing dimensions are : %d, %d\n", width, height);

	// request that we capture to 24bit RGB

        // get image properties
        if (ioctl (m_nDeviceHandle, VIDIOCGPICT, &imageProperties) != -1)
        {       // successfully retrieved the default image properties

                // the following values are for requesting 24bit RGB
                imageProperties.depth = 24;
                imageProperties.palette = VIDEO_PALETTE_RGB24;
                if (ioctl (m_nDeviceHandle, VIDIOCSPICT, &imageProperties) == -1)
                {       // failed to set the image properties
                        printf ("Could not set the video depth and palette.\nPerhaps not a fatal error.\n");
                }
        }

        // verify that the requested capture pixel depth and palette succeeded
        if (ioctl (m_nDeviceHandle, VIDIOCGPICT, &imageProperties) == -1)
        {       // failed to retrieve default image properties
                printf ("Failed to retrieve the video depth and palette.\n");
                return false;
        }

	if ((imageProperties.depth != 24) || (imageProperties.palette != VIDEO_PALETTE_RGB24))
        {       // not a format our program supports
                printf ("Format is not 24bit RGB.\n");
                return false;
        }
	
        printf ("Capture depth is 24bit RGB\n");

        // obtain memory about capture space
        if (ioctl (m_nDeviceHandle, VIDIOCGMBUF, &memoryBuffer) == -1)
        {       // failed to retrieve information about capture memory space
                printf ("Failed to retrieve information about MMIO space.\n");
                return false;
        }


        // obtain memory mapped area
        memoryMap = (char*)mmap (0, memoryBuffer.size, PROT_READ | PROT_WRITE, MAP_SHARED, m_nDeviceHandle, 0);
        if ((intptr_t) memoryMap == -1)
        {       // failed to retrieve pointer to memory mapped area
                printf ("Failed to obtain MMIO space.\n");
                return false;
        }


	printf("Allocating structures...\n");
        // allocate structures
        mmaps = (struct video_mmap*)(malloc (memoryBuffer.frames * sizeof (struct video_mmap)));

        // fill out the fields
        i = 0;
        while (i < memoryBuffer.frames)
        {
                mmaps[i].frame = i;
                mmaps[i].width = captureWindow.width;
                mmaps[i].height = captureWindow.height;
                mmaps[i].format = imageProperties.palette;
                ++ i;
        }

	printf("Requesting capture buffers...\n");
        // request capture to each buffer except the last one
        i = 0;
        while (i < (memoryBuffer.frames-1))
        {
                if (ioctl (m_nDeviceHandle, VIDIOCMCAPTURE, &mmaps[i]) == -1)
                {       // capture request failed
                }
                ++ i;
        }

	printf("Set index buffer...\n");
        // set our index to the last buffer
        bufferIndex = memoryBuffer.frames-1;

	return true;
}

bool CV4LCapture::CaptureImage(CByteImage **ppImages)
{
	// send a request to begin capturing to the currently indexed buffer
        if (ioctl (m_nDeviceHandle, VIDIOCMCAPTURE, &mmaps[bufferIndex]) == -1)
        {
		       // capture request failed
        }

        // move bufferIndex to the next frame
        if (++bufferIndex == memoryBuffer.frames)
        {
		// bufferIndex is indexing beyond the last buffer
                // set it to index the first buffer
                bufferIndex = 0;
        }

        // wait for the currently indexed frame to complete capture
        if (ioctl(m_nDeviceHandle, VIDIOCSYNC, &mmaps[bufferIndex]) == -1)
        {       // sync request failed
        }

        // return the address of the frame data for the current buffer index
        const unsigned char *frame = (const unsigned char *) (memoryMap + memoryBuffer.offsets[bufferIndex]);
	unsigned char *output = ppImages[0]->pixels;

	const int nBytes = ppImages[0]->width * ppImages[0]->height * 3;

	for (int i = 0; i < nBytes; i += 3)
	{
		output[i] = frame[i + 2];
		output[i + 1] = frame[i + 1];
		output[i + 2] = frame[i];
	}

	return true;
}

void CV4LCapture::CloseCamera()
{
	// free the video_mmap structures
        free (mmaps);

        // unmap the capture memory
        munmap (memoryMap, memoryBuffer.size);

        // close the device
        close (m_nDeviceHandle);
}
