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
// Filename:  helpers.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


#ifndef _HELPERS_H_
#define _HELPERS_H_

#if defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MISPEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
    defined(__sparc__)
    
#ifndef IVT_BIG_ENDIAN
#define IVT_BIG_ENDIAN 1
#endif
   
#endif



// ****************************************************************************
// Defines
// ****************************************************************************

#define MY_MAX(a, b)    (((a) > (b)) ? (a) : (b))
#define MY_MIN(a, b)    (((a) < (b)) ? (a) : (b))


// ****************************************************************************
// Declarations
// ****************************************************************************

extern float invert_byte_order_float(float x);
extern unsigned long invert_byte_order_long(unsigned long x);
extern unsigned int invert_byte_order_int(unsigned int x);
extern unsigned short invert_byte_order_short(unsigned short x);
extern long invert_byte_order_long(long x);
extern int invert_byte_order_int(int x);
extern short invert_byte_order_short(short x);
extern void get_timer_value(unsigned int &sec, unsigned int &usec);
extern unsigned int get_timer_value(bool bResetTimer = false);
extern int my_round(double x);
extern int my_round(float x);
extern double uniform_random();
extern double gaussian_random();
extern float uniform_random_float();
extern float gaussian_random_float();
extern void sleep_ms(unsigned int ms);
extern void hsv2rgb(int h, int s, int v, int &r, int &g, int &b);
extern void rgb2hsv(int r, int g, int b, int &h, int &s, int &v);
extern void *aligned_malloc(unsigned int size, unsigned int align_size = 16);
extern void aligned_free(void *ptr);
extern const char *GetVersionIVT();



#endif /* _HELPERS_H_ */
