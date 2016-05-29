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
// Filename:  helpers.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************
// Changes:   01.12.2008, Moritz Hassert
//            * added time functions for VC cameras
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "helpers.h"

#include <stdlib.h>
#include <math.h>

#ifdef WIN32
#include <windows.h>
#ifndef _ATL_VER
#define _ATL_VER 0x0400 // set this to 0x0300 if getting an error with Visual Studio 6
#endif
#elif defined _TMS320C6X
#include <sysvar.h>
#include <vcrt.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

const char *pVersion = "1.3.22";



float invert_byte_order_float(float x)
{
	const unsigned int nResult = invert_byte_order_int(*((unsigned int *) &x));
	return *((float *) &nResult);
}

unsigned long invert_byte_order_long(unsigned long x)
{
	unsigned long result = 0;
	result |= (x & 0x000000ff) << 24;
	result |= (x & 0x0000ff00) << 8;
	result |= (x & 0x00ff0000) >> 8;
	result |= (x & 0xff000000) >> 24;
	return result;
}

unsigned int invert_byte_order_int(unsigned int x)
{
	unsigned int result = 0;
	result |= (x & 0x000000ff) << 24;
	result |= (x & 0x0000ff00) << 8;
	result |= (x & 0x00ff0000) >> 8;
	result |= (x & 0xff000000) >> 24;
	return result;
}

unsigned short invert_byte_order_short(unsigned short x)
{
	unsigned short result = 0;
	result |= (x & 0x00ff) << 8;
	result |= (x & 0xff00) >> 8;
	return result;
}

long invert_byte_order_long(long x)
{
	long result = 0;
	result |= (x & 0x000000ff) << 24;
	result |= (x & 0x0000ff00) << 8;
	result |= (x & 0x00ff0000) >> 8;
	result |= (x & 0xff000000) >> 24;
	return result;
}

int invert_byte_order_int(int x)
{
	int result = 0;
	result |= (x & 0x000000ff) << 24;
	result |= (x & 0x0000ff00) << 8;
	result |= (x & 0x00ff0000) >> 8;
	result |= (x & 0xff000000) >> 24;
	return result;
}

short invert_byte_order_short(short x)
{
	short result = 0;
	result |= (x & 0x00ff) << 8;
	result |= (x & 0xff00) >> 8;
	return result;
}

void get_timer_value(unsigned int &sec, unsigned int &usec)
{
	#ifdef WIN32
		// set the define _ATL_VER to 0x0300 if getting an error with Visual Studio 6
		#if _ATL_VER > 0x0300
		DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0); // necessary on some Multicore systems
		#endif

		static bool bInit = true;
		static LARGE_INTEGER startTime;
		static LARGE_INTEGER freq;

		if (bInit)
		{
			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&startTime);
			bInit = false;
		}

		LARGE_INTEGER tempTime;
		QueryPerformanceCounter(&tempTime);
		tempTime.QuadPart = tempTime.QuadPart - startTime.QuadPart;
		sec = (unsigned int) (tempTime.QuadPart / freq.QuadPart);
		__int64 remainder = (__int64) tempTime.QuadPart % (__int64) freq.QuadPart;
   		usec = (unsigned int) (remainder * (1000000.0 / freq.QuadPart));

		#if _ATL_VER > 0x0300
		SetThreadAffinityMask(GetCurrentThread(), oldmask); // necessary on some Multicore systems
		#endif

	#elif defined(_TMS320C6X)
		sec  =  (unsigned int) getlvar(SEC);
		usec = ((unsigned int) getvar(MSEC))*1000;
	#else
		timeval t;
		
		// get time and fill timeval structure
		gettimeofday(&t, 0);
		
		// if first time save tv_sec
		sec = t.tv_sec;
		usec = t.tv_usec;
	#endif
}

unsigned int get_timer_value(bool bResetTimer)
{
	static int nStaticSec = 0;
	static int nStaticUSec = 0;

	#ifdef WIN32
		#if _ATL_VER > 0x0300
		DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0); // necessary on some Multicore systems
		#endif
		
		static bool bInit = true;
		static LARGE_INTEGER startTime;
		static LARGE_INTEGER freq;

		if (bInit)
		{
			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&startTime);
			bInit = false;
		}

		LARGE_INTEGER tempTime;
		QueryPerformanceCounter(&tempTime);
		tempTime.QuadPart = tempTime.QuadPart - startTime.QuadPart;
		unsigned int sec = (unsigned int) (tempTime.QuadPart / freq.QuadPart);
		__int64 remainder = (__int64) tempTime.QuadPart % (__int64) freq.QuadPart;
   		unsigned int usec = (unsigned int) (remainder * (1000000.0 / freq.QuadPart));
		
		// if first time save tv_sec
		if (bResetTimer)
		{
			nStaticSec = sec;
			nStaticUSec = usec;
		}

		#if _ATL_VER > 0x0300
		SetThreadAffinityMask(GetCurrentThread(), oldmask); // necessary on some Multicore systems
		#endif

		return (sec - nStaticSec) * 1000000 + ((int) usec - nStaticUSec);

	#elif defined(_TMS320C6X)
		int tv_sec  =  getlvar(SEC);
		int tv_usec =  getvar(MSEC)*1000;

		// if first time save tv_sec
		if (bResetTimer)
		{
			nStaticSec  = tv_sec;
			nStaticUSec = tv_usec;
		}
		
		return (tv_sec - nStaticSec) * 1000000 + ((int) tv_usec - nStaticUSec);

	#else
		timeval t;
		
		// get time and fill timeval structure
		gettimeofday(&t, 0);
		
		// if first time save tv_sec
		if (bResetTimer)
		{
			nStaticSec = t.tv_sec;
			nStaticUSec = t.tv_usec;
		}
		
		return (t.tv_sec - nStaticSec) * 1000000 + ((int) t.tv_usec - nStaticUSec);
	#endif
}

double uniform_random()
{
	return rand() / double(RAND_MAX);
}

float uniform_random_float()
{
	return rand() / float(RAND_MAX);
}


// This Gaussian routine is taken from Numerical Recipes and is their copyright
double gaussian_random()
{
	static int next_gaussian = 0;
	static double saved_gaussian_value;

	double fac, rsq, v1, v2;

	if (next_gaussian == 0)
	{
		do
		{
			v1 = 2.0 * uniform_random() - 1.0;
			v2 = 2.0 * uniform_random() - 1.0;
			rsq = v1 * v1 + v2 * v2;
		}
		while (rsq >= 1.0 || rsq == 0.0);
    
		fac = sqrt(-2.0 * log(rsq) / rsq);
		saved_gaussian_value = v1 * fac;
		next_gaussian = 1;
    
		return v2 * fac;
	}
	else
	{
		next_gaussian = 0;
		return saved_gaussian_value;
	}
}

// This Gaussian routine is taken from Numerical Recipes and is their copyright
float gaussian_random_float()
{
	static int next_gaussian = 0;
	static float saved_gaussian_value;

	float fac, rsq, v1, v2;

	if (next_gaussian == 0)
	{
		do
		{
			v1 = 2.0f * uniform_random_float() - 1.0f;
			v2 = 2.0f * uniform_random_float() - 1.0f;
			rsq = v1 * v1 + v2 * v2;
		}
		while (rsq >= 1.0f || rsq == 0.0f);
    
		fac = sqrtf(-2.0f * logf(rsq) / rsq);
		saved_gaussian_value = v1 * fac;
		next_gaussian = 1;
    
		return v2 * fac;
	}
	else
	{
		next_gaussian = 0;
		return saved_gaussian_value;
	}
}


int my_round(double x)
{
	return (x > 0) ? int(x + 0.5) : int(x - 0.5);
}

int my_round(float x)
{
	return (x > 0) ? int(x + 0.5f) : int(x - 0.5f);
}


void sleep_ms(unsigned int ms)
{
	#ifdef WIN32
	Sleep(ms);
	#elif defined(_TMS320C6X)
	time_delay(ms);
	#else
	usleep((useconds_t) (1000 * ms));
	#endif
}


void hsv2rgb(int h, int s, int v, int &r, int &g, int &b)
{
	float R, G, B;
	float S = s / 255.0f, V = v / 255.0f;
	
	if (h < 120)
	{
		R = (120 - h) / 60.0f;
		G = h / 60.0f;
		B = 0;
	}
	else if (h < 240)
	{
		R = 0;
		G = (240 - h) / 60.0f;
		B = (h - 120) / 60.0f;
	}
	else
	{
		R = (h - 240) / 60.0f;
		G = 0.0f;
		B = (360 - h) / 60.0f;
	}
	
	if (R > 1) R = 1.0f;
	if (G > 1) G = 1.0f;
	if (B > 1) B = 1.0f;
	
	r = int(255.0f * (1.0f + S * (R - 1.0f)) * V + 0.5f);
	g = int(255.0f * (1.0f + S * (G - 1.0f)) * V + 0.5f);
	b = int(255.0f * (1.0f + S * (B - 1.0f)) * V + 0.5f);
}

void rgb2hsv(int r, int g, int b, int &h, int &s, int &v)
{
	v = MY_MAX(MY_MAX(r, g), b);
	const int min = MY_MIN(MY_MIN(r, g), b);
	const int delta = v - min;

	if (delta == 0)
	{
		h = 0;
		s = 0;
	}
	else
	{
		s = 255 * delta / v;

		if (r == v)
			h = 60 * (g - b) / delta;
		else if (g == v)
			h = 120 + (60 * (b - r) / delta);
		else
			h = 240 + (60 * (r - g) / delta);
	}

	while (h < 0) h += 360;
	while (h > 360) h -= 360;
}


void *aligned_malloc(unsigned int size, unsigned int align_size)
{
	const int align_mask = align_size - 1;

	char *ptr = (char *) malloc(size + align_size + sizeof(int));
	if (ptr == 0)
		return 0;

	char *ptr2 = ptr + sizeof(int);
	char *aligned_ptr = ptr2 + (align_size - ((size_t)ptr2 & align_mask));

	ptr2 = aligned_ptr - sizeof(int);
	*((int *) ptr2) = (int) (aligned_ptr - ptr);

	return aligned_ptr;
}

void aligned_free(void *ptr)
{
	int *ptr2 = (int *) ptr - 1;
	char *ptr3 = (char *) ptr;
	ptr3  -= *ptr2;
	free(ptr3);
}


const char *GetVersionIVT()
{
	return pVersion;
}
