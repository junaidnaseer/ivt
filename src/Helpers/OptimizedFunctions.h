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
// Filename:  OptimizedFunctions.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************

#ifndef _OPTIMIZED_FUNCTIONS_H_
#define _OPTIMIZED_FUNCTIONS_H_



#ifdef OPT_DEFINE
#define OPTIMIZED_FUNCTION
#define OPTIMIZED_FUNCTION_END = 0
#else
#define OPTIMIZED_FUNCTION extern
#define OPTIMIZED_FUNCTION_END
#endif

// the header and footer have to be placed in each function that has an optimized version
// different macros for different number of parameters
#define OPTIMIZED_FUNCTION_HEADER_0(name) if (Optimized##name == NULL || Optimized##name() == 0) {
#define OPTIMIZED_FUNCTION_HEADER_1(name, p1) if (Optimized##name == NULL || Optimized##name(p1) == 0) {
#define OPTIMIZED_FUNCTION_HEADER_2(name, p1, p2) if (Optimized##name == NULL || Optimized##name(p1, p2) == 0) {
#define OPTIMIZED_FUNCTION_HEADER_2_ROI(name, p1, p2, p3) if (Optimized##name == NULL || p3 != 0 || Optimized##name(p1, p2) == 0) {
#define OPTIMIZED_FUNCTION_HEADER_3(name, p1, p2, p3) if (Optimized##name == NULL || Optimized##name(p1, p2, p3) == 0) {
#define OPTIMIZED_FUNCTION_HEADER_3_ROI(name, p1, p2, p3, p4) if (Optimized##name == NULL || p4 != 0 || Optimized##name(p1, p2, p3) == 0) {
#define OPTIMIZED_FUNCTION_HEADER_4(name, p1, p2, p3, p4) if (Optimized##name == NULL || Optimized##name(p1, p2, p3, p4) == 0) {
#define OPTIMIZED_FUNCTION_HEADER_5(name, p1, p2, p3, p4, p5) if (Optimized##name == NULL || Optimized##name(p1, p2, p3, p4, p5) == 0) {
#define OPTIMIZED_FUNCTION_HEADER_5_RET(name, p1, p2, p3, p4, p5) int nRet; if (Optimized##name != NULL && Optimized##name(p1, p2, p3, p4, p5, nRet) != 0) return nRet;
#define OPTIMIZED_FUNCTION_HEADER_8(name, p1, p2, p3, p4, p5, p6, p7, p8) if (Optimized##name == NULL || Optimized##name(p1, p2, p3, p4, p5, p6, p7, p8) == 0) {
#define OPTIMIZED_FUNCTION_HEADER_8_ROI(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) if (Optimized##name == NULL || p9 != 0 || Optimized##name(p1, p2, p3, p4, p5, p6, p7, p8) == 0) {
#define OPTIMIZED_FUNCTION_HEADER_9(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) if (Optimized##name == NULL || Optimized##name(p1, p2, p3, p4, p5, p6, p7, p8, p9) == 0) {

#define OPTIMIZED_FUNCTION_FOOTER }

// these macros create a type definition of function and declare a function pointer to that function
// different macros for different number of parameters
#define DECLARE_OPTIMIZED_FUNCTION_0(name) typedef int (*DefOptimized##name)(); OPTIMIZED_FUNCTION DefOptimized##name Optimized##name OPTIMIZED_FUNCTION_END;
#define DECLARE_OPTIMIZED_FUNCTION_1(name, p1) typedef int (*DefOptimized##name)(p1); OPTIMIZED_FUNCTION DefOptimized##name Optimized##name OPTIMIZED_FUNCTION_END;
#define DECLARE_OPTIMIZED_FUNCTION_2(name, p1, p2) typedef int (*DefOptimized##name)(p1, p2); OPTIMIZED_FUNCTION DefOptimized##name Optimized##name OPTIMIZED_FUNCTION_END;
#define DECLARE_OPTIMIZED_FUNCTION_3(name, p1, p2, p3) typedef int (*DefOptimized##name)(p1, p2, p3); OPTIMIZED_FUNCTION DefOptimized##name Optimized##name OPTIMIZED_FUNCTION_END;
#define DECLARE_OPTIMIZED_FUNCTION_4(name, p1, p2, p3, p4) typedef int (*DefOptimized##name)(p1, p2, p3, p4); OPTIMIZED_FUNCTION DefOptimized##name Optimized##name OPTIMIZED_FUNCTION_END;
#define DECLARE_OPTIMIZED_FUNCTION_5(name, p1, p2, p3, p4, p5) typedef int (*DefOptimized##name)(p1, p2, p3, p4, p5); OPTIMIZED_FUNCTION DefOptimized##name Optimized##name OPTIMIZED_FUNCTION_END;
#define DECLARE_OPTIMIZED_FUNCTION_5_RET(name, p1, p2, p3, p4, p5) typedef int (*DefOptimized##name)(p1, p2, p3, p4, p5, int &nRet); OPTIMIZED_FUNCTION DefOptimized##name Optimized##name OPTIMIZED_FUNCTION_END;
#define DECLARE_OPTIMIZED_FUNCTION_8(name, p1, p2, p3, p4, p5, p6, p7, p8) typedef int (*DefOptimized##name)(p1, p2, p3, p4, p5, p6, p7, p8); OPTIMIZED_FUNCTION DefOptimized##name Optimized##name OPTIMIZED_FUNCTION_END;
#define DECLARE_OPTIMIZED_FUNCTION_9(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) typedef int (*DefOptimized##name)(p1, p2, p3, p4, p5, p6, p7, p8, p9); OPTIMIZED_FUNCTION DefOptimized##name Optimized##name OPTIMIZED_FUNCTION_END;

// this include will generate the necessary declaration
// due to the DECLARE_OPTIMIZED_FUNCTION_x macros
#include "OptimizedFunctionsList.h"



#endif /* _OPTIMIZED_FUNCTIONS_H_ */
