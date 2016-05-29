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
// Filename:  Unicap1394Capture.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "Unicap1394Capture.h"

#include "Image/ByteImage.h"

#include <stdio.h>
#include <string.h>
#include <string>


// ****************************************************************************
// Defines
// ****************************************************************************

#define UYVY 0x59565955 /* UYVY (packed, 16 bits) */
#define FOURCC(a,b,c,d) (unsigned int)((((unsigned int)d)<<24)+(((unsigned int)c)<<16)+(((unsigned int)b)<<8)+a)



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CUnicap1394Capture::CUnicap1394Capture(int nCameras, VideoMode mode) : m_nCameras(nCameras), m_mode(mode)
{
    source = 4;
    width = 0;
    height = 0;
    norm = 0;

    for (int i = 0; i < UNICAP_MAX_CAMERAS; i++)
	handle[i] = 0;
}

CUnicap1394Capture::~CUnicap1394Capture()
{
	CloseCamera();
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CUnicap1394Capture::OpenCamera()
{
	unicap_property_t property;
	unicap_format_t format;
	unicap_format_t format_spec;

	int nCamerasFound = 0;


	for (int i = 0; i < 10; i++)
	{
		if (SUCCESS(unicap_enumerate_devices(0, &device[i], i)))
		{
			printf("info: camera found at index %i\n", i);
			printf("info: using device for cam: %s\n", device[i].identifier);
			nCamerasFound++;
		}

		if (!SUCCESS(unicap_open(&handle[i], &device[i])))
		{
			printf("error2\n");
			return false;
		}

		unicap_void_property( &property );
		strcpy( property.identifier, "video norm" );
		unicap_get_property( handle[i], &property );
		strcpy( property.menu_item, property.menu.menu_items[this->norm] );
		if(!SUCCESS(unicap_set_property(handle[i], &property)))
		{
			printf("error3\n");
			return false;
		}

		unicap_void_property( &property );
		strcpy( property.identifier, "source" );
		unicap_get_property( handle[i], &property );
		strcpy( property.menu_item, property.menu.menu_items[this->source] );
		if(!SUCCESS(unicap_set_property(handle[i], &property)))
		{
			printf("error4\n");
			return false;
		}

		if(!SUCCESS(unicap_lock_stream(handle[i])))
		{
			printf("error5\n");
			return false;
		}

		unicap_status_t  status=0;
		unicap_void_format( &format_spec );

		format_spec.fourcc = FOURCC('U','Y','V','Y');
		int num= - 1;
		status = unicap_enumerate_formats(handle[i], &format_spec, &format, 0 );
		if(!SUCCESS(status))
		{
			printf("error: failed get video format\n");
			return false;
		}

		for (int j = 0; j < format.size_count; j++)
		{
			printf("format %i: width/height = %i/%i\n", j, format.sizes[j].width, format.sizes[j].height);

			switch (m_mode)
			{
				case e320x240:
					if (format.sizes[j].width == 320 && format.sizes[j].height == 240)
					{
						num = j;
						width = 320;
						height = 240;
					}
				break;

				case e640x480:
					if (format.sizes[j].width == 640 && format.sizes[j].height == 480)
					{
						num = j;
						width = 640;
						height = 480;
					}
				break;

				case e768x576:
					if (format.sizes[j].width == 768 && format.sizes[j].height == 576)
					{
						num = j;
						width = 768;
						height = 576;
					}
				break;
			}
		}

		if (num < 0)
		{
			printf("error: could not found matching mode\n");
			return false;
		}

		printf("info: took format %i with resolution %i/%i\n", num, width, height);

		format.size.width = format.sizes[num].width;
		format.size.height = format.sizes[num].height;

		if(!SUCCESS( unicap_set_format(handle[i], &format ) ) )
		{
			printf("error: Failed to set video format\n" );
			return false;
		}
		if( !SUCCESS( unicap_get_format(handle[i], &format ) ) )
		{
			printf("error: Failed to get video format\n" );
			return false;
		}
		memset(&buffer[i], 0x0, sizeof(unicap_data_buffer_t));
		buffer[i].format = format;
		buffer[i].buffer_size = format.buffer_size;
		buffer[i].type = format.buffer_type;
		
		printf("info: %s->Buffer size: %d\n",device[i].identifier,format.buffer_size);

		if (buffer[i].type == UNICAP_BUFFER_TYPE_USER)
			buffer[i].data = new unsigned char[buffer[i].buffer_size];

		status = unicap_start_capture(handle[i]);
		if(!SUCCESS(status))
		{
			printf("error: failed to start capture on device: %s (Error Code : %x)\n", device[i].identifier,status );
			return false;
		}

		printf("\n");

		if (nCamerasFound == m_nCameras)
			break;
	}

	if (nCamerasFound < 0 || (m_nCameras != -1 && nCamerasFound != m_nCameras))
		return false;

    return true;
}

void CUnicap1394Capture::CloseCamera()
{
	for (int i = 0; i < UNICAP_MAX_CAMERAS; i++)
	{
		if (handle[i])
		{
			if (!unicap_stop_capture(handle[i]))
				printf("error: close\n");

			unicap_unlock_stream(handle[i]);

			if(!SUCCESS(unicap_close(handle[i])))
			{
				printf("error: destructor\n");
			}

			handle[i] = 0;
		}
	}

}

void CUnicap1394Capture::Convert(const unsigned char *input, unsigned char *output)
{
	const int nBytes = width * height * 2;

    float u,y1,v,y2,r1,g1,b1,r2,g2,b2;

    for(unsigned int i = 0,j=0; i<nBytes;i+=4,j+=6)
    {
	u = input[i];
	y1 =input[i+1];
	v = input[i+2];
	y2 =input[i+3];

	u -= 128; //Werte zwischen -+0,5
	v -= 128; //Werte zwischen -+0,5
	y1 -= 16;
	y2 -= 16;

	r1 = 1.164f * y1 + 1.596f * v;
	g1 = 1.164f * y1 - 0.392f * u - 0.813f * v;
	b1 = 1.164f * y1 + 2.017f * u;

	r2 = 1.164f * y2 + 1.596f * v;
	g2 = 1.164f * y2 - 0.392f * u - 0.813f * v;
	b2 = 1.164f * y2 + 2.017f * u;
	

	if (r1 < 0) r1 = 0;
	if (g1 < 0) g1 = 0;
	if (b1 < 0) b1 = 0;
	if (r1 > 255) r1 = 255;
	if (g1 > 255) g1 = 255;
	if (b1 > 255) b1 = 255;

	if (r2 < 0) r2 = 0;
	if (g2 < 0) g2 = 0;
	if (b2 < 0) b2 = 0;
	if (r2 > 255) r2 = 255;
	if (g2 > 255) g2 = 255;
	if (b2 > 255) b2 = 255;

	output[j]  = (unsigned char)r1;
	output[j+1]= (unsigned char)g1;
	output[j+2]= (unsigned char)b1;
	output[j+3]= (unsigned char)r2;
	output[j+4]= (unsigned char)g2;
	output[j+5]= (unsigned char)b2;
    }
}

bool CUnicap1394Capture::CaptureImage(CByteImage **ppImages)
{
unicap_data_buffer_t *returned_buffer[10] = { 0 };
	for (int i = 0; i < m_nCameras; i++)
	{

		unicap_status_t status = unicap_queue_buffer(handle[i], &buffer[i]);
		if(!SUCCESS(status))
		{
			printf("error: failed to queue a buffer on device: %s (Error Code : %x)\n", device[i].identifier,status);
			return false;
		}

		status = unicap_wait_buffer(handle[i], &returned_buffer[i]);

		if (!SUCCESS(status))
		{
			printf("error: %x\n", status);
			printf("error: failed to wait for buffer on device: %s (Error Code : %x)\n", device[i].identifier,status );
		}

		Convert(returned_buffer[i]->data, ppImages[i]->pixels);
	}

    /*// alternative Conversion from YUV422 (YCbCr422) to RGB with IPP library
    IppiSize size;
    size.width=format.size.width;
    size.height=format.size.height;
    unsigned char *rgb_buffer = new unsigned char [format.size.width*format.size.height*6];
    ippiCbYCr422ToRGB_8u_C2C3R((Ipp8u*)returned_buffer->data,size.width*2,(Ipp8u*)rgb_buffer,size.width*3,size);*/

	return true;
}


/*
void CUnicap1394Capture::setContrast(unsigned int contrast)
{
    unicap_void_property( &property );
    strcpy( property.identifier, "contrast" ); 
    unicap_get_property( handle, &property );
    sprintf(property.menu_item,"%d",contrast); 
    if( !SUCCESS( unicap_set_property( handle, &property ) ) )
    {
      BGTHROWC(bg::error,"Failed to set contrast!" );
      exit(1);
    }

}

unsigned int CUnicap1394Capture::getContrast()
{
    unicap_void_property( &property ); 
    strcpy( property.identifier, "contrast" ); 
    unicap_get_property( handle, &property );
    bg::string contrast(property.menu.menu_items[0]);
    return contrast.toInt();
}


void CUnicap1394Capture::setBrightness(unsigned int brightness)
{
    unicap_void_property( &property ); 
    strcpy( property.identifier, "brightness" );
    unicap_get_property( handle, &property );
    sprintf(property.menu_item, "%d",brightness);
    if( !SUCCESS( unicap_set_property( handle, &property ) ) )
    {
      BGTHROWC(bg::error,"Failed to set brightness!" );
      exit(1);
    }
}

unsigned int CUnicap1394Capture::getBrightness()
{
    unicap_void_property( &property ); 
    strcpy( property.identifier, "brightness" );
    unicap_get_property( handle, &property );
    bg::string brightn(property.menu.menu_items[0]);
    return brightn.toInt();
}
*/
