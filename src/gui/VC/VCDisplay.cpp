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
// Filename:  VCDisplay.cpp
// Author:    Moritz Hassert
// Date:      9.12.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#define NEW_IMAGE_VAR
#include <vcrt.h>
#include <macros.h>
#include <sysvar.h>


#include <algorithm>

#include <Helpers/helpers.h>
#include <Image/ByteImage.h>

#include "VCDisplay.h"



// ****************************************************************************
// Global functions
// ****************************************************************************

inline void wait_for_next_frame()
{
	int res;
	//wait for NEXT frame change, not the current one
	while ( (res=wait(DISPLAY_EVT, 1000))==2) //while last change is reported
	{
		printf("res: %d\n", res);
	}
	printf("res: %d\n", res);
}


// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CVCDisplay::CVCDisplay(bool doubleBuffer, bool show_overlay): m_width(DispGetColumns), m_height(DispGetRows), m_pitch(DispGetPitch), m_doubleBuffer(doubleBuffer)
{
	// alloc mem for display. for non VGA models the diplay is 0x0 sized!
	m_allocated_buffer = DRAMDisplayMalloc();
	//align display buffer to 1024 (hardware requirement)
	m_display_front = (U8*)((m_allocated_buffer+1023)&(~1023));
	//set everything to zero, including all border regions
	std::fill_n(m_display_front, m_height*m_pitch, 128);

	if (m_doubleBuffer) {
		// alloc mem for display. for non VGA models the diplay is 0x0 sized!
		m_allocated_backbuffer = DRAMDisplayMalloc();
		//align display buffer to 1024 (hardware requirement)
		m_display_back = (U8*)((m_allocated_backbuffer+1023)&(~1023));
		//set everything to zero, including all border regions
		std::fill_n( m_display_back, m_height*m_pitch, 128);
	}
	else {
		m_allocated_backbuffer = NULL;
		m_display_back = NULL;
	}

	make_current();

	if (show_overlay) {
		enable_overlay();
		std::fill_n( (unsigned char*)OvlGetPhysPage, OvlGetRows*OvlGetPitch, 0);
	}
	else {
		disable_overlay();
	}

	//printf("create: cols %d, rows %d, pitch %d, disp %x, front %x(%x), back %x(%x)\n", m_width, m_height, m_pitch, ScrGetDispPage, m_allocated_buffer, m_display_front, m_allocated_backbuffer, m_display_back);
}

CVCDisplay::~CVCDisplay()
{
	//restore default diplay buffer before freeing memory
	restore_default();

	//free this buffer
	DRAMByteFree(m_allocated_buffer);

	if (m_doubleBuffer) {
		DRAMByteFree(m_allocated_backbuffer);
	}
	
	disable_overlay();
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CVCDisplay::make_current() const
{
	//make private buffer the current display buffer
	ScrSetDispPage((int)m_display_front);
	display_update(UPDATE_DISP);
}
	
void CVCDisplay::restore_default() const
{
	//use capture buffer for display, live mode
	ScrSetDispPage(ScrGetCaptPage);
	display_update(UPDATE_DISP);
}



void CVCDisplay::enable_overlay() const
{
	setvar(OVLY_ACTIVE,1);
	sleep_ms(500);
}


void CVCDisplay::disable_overlay() const
{
	setvar(OVLY_ACTIVE,0);
	sleep_ms(500);
}

void CVCDisplay::show(const CByteImage* image)
{
	if (m_doubleBuffer) {
		std::swap(m_display_front, m_display_back);
	}

	const unsigned char* restrict img_pixels = image->pixels;
	unsigned char* restrict disp_pixels = m_display_front;

	int h = std::min(m_height, image->height);
	int w = std::min(m_width,  image->width);

	//copy row by row as width may differ and pitch most likely will
	for (int row=0; row<h; row++) {
		//for (int x=0; x<w; x++)
		//	disp_pixels[row*m_pitch + x] = img_pixels[row*image->width + x];
			
		std::copy(img_pixels, img_pixels+w, disp_pixels);
		//std::memcpy(disp_pixels, img_pixels, w);

		img_pixels  += image->width;
		disp_pixels += m_pitch;
	}

	//always set buffer address. someone might have changed it
	ScrSetDispPage((int)m_display_front);
	display_update(UPDATE_DISP);

	//printf("set: disp %x, front %x, back %x\n", ScrGetDispPage, m_display_front, m_display_back);
}

void CVCDisplay::overlay(const CByteImage *image)
{
	const unsigned char* restrict img_pixels = image->pixels;
	unsigned char*       restrict ovl_pixels = (unsigned char*)OvlGetPhysPage;


	int h = std::min(OvlGetRows,    image->height);
	int w = std::min(OvlGetColumns, image->width);
	int ovl_pitch = OvlGetPitch;

	//copy row by row as width may differ and pitch most likely will
	for (int row=0; row<h; row++) {
		std::copy(img_pixels, img_pixels+w, ovl_pixels);

		img_pixels += image->width;
		ovl_pixels += ovl_pitch;
	}
}
