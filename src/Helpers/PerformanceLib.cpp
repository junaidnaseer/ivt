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
// Filename:  PerformanceLib.cpp
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "PerformanceLib.h"


// this is needed so that the function pointers are not declared extern
// and are defined here
#define OPT_DEFINE

// include all the function prototypes and function pointers
#include "OptimizedFunctions.h"

// include the necessary headers to load a dynamic library
#ifdef WIN32
#include <windows.h>
#elif defined(LOAD_KPP)
#include <dlfcn.h>
#endif

#include <stdio.h>

// this is the only instance that should be created ever
// it's global so it executes before the main function
#ifdef WIN32
//CPerformanceLibInitializer performanceLibInitializer; // activate this line under Windows to automatically load the KPP
#else
CPerformanceLibInitializer performanceLibInitializer;
#endif


#if defined(WIN32)
#define LOAD_OPTIMIZED_FUNCTION(name) Optimized##name = (DefOptimized##name) GetProcAddress(hMod, #name);
#elif defined(LOAD_KPP)
#define LOAD_OPTIMIZED_FUNCTION(name) Optimized##name = (DefOptimized##name) dlsym(m_pLibHandle, #name);
#else
// dummy
#define LOAD_OPTIMIZED_FUNCTION(name) Optimized##name = 0;
#endif

// these macros load a function from the shared library
// different macros for different number of parameters
#undef DECLARE_OPTIMIZED_FUNCTION_0
#undef DECLARE_OPTIMIZED_FUNCTION_1
#undef DECLARE_OPTIMIZED_FUNCTION_2
#undef DECLARE_OPTIMIZED_FUNCTION_3
#undef DECLARE_OPTIMIZED_FUNCTION_4
#undef DECLARE_OPTIMIZED_FUNCTION_5
#undef DECLARE_OPTIMIZED_FUNCTION_5_RET
#undef DECLARE_OPTIMIZED_FUNCTION_8
#undef DECLARE_OPTIMIZED_FUNCTION_9

#define DECLARE_OPTIMIZED_FUNCTION_0(name) LOAD_OPTIMIZED_FUNCTION(name)
#define DECLARE_OPTIMIZED_FUNCTION_1(name, p1) LOAD_OPTIMIZED_FUNCTION(name)
#define DECLARE_OPTIMIZED_FUNCTION_2(name, p1, p2) LOAD_OPTIMIZED_FUNCTION(name)
#define DECLARE_OPTIMIZED_FUNCTION_3(name, p1, p2, p3) LOAD_OPTIMIZED_FUNCTION(name)
#define DECLARE_OPTIMIZED_FUNCTION_4(name, p1, p2, p3, p4) LOAD_OPTIMIZED_FUNCTION(name)
#define DECLARE_OPTIMIZED_FUNCTION_5(name, p1, p2, p3, p4, p5) LOAD_OPTIMIZED_FUNCTION(name)
#define DECLARE_OPTIMIZED_FUNCTION_5_RET(name, p1, p2, p3, p4, p5) LOAD_OPTIMIZED_FUNCTION(name)
#define DECLARE_OPTIMIZED_FUNCTION_8(name, p1, p2, p3, p4, p5, p6, p7, p8) LOAD_OPTIMIZED_FUNCTION(name)
#define DECLARE_OPTIMIZED_FUNCTION_9(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) LOAD_OPTIMIZED_FUNCTION(name)


CPerformanceLibInitializer::CPerformanceLibInitializer() : m_pLibHandle(0)
{
	LoadPerformanceLib();
}

CPerformanceLibInitializer::~CPerformanceLibInitializer()
{
	FreePerformanceLib();
}

void CPerformanceLibInitializer::LoadPerformanceLib()
{
	// only load once
	if (m_pLibHandle != 0)
		return;
		
	#if defined(WIN32)
	HMODULE hMod = LoadLibrary("KPP.dll");
	if (hMod == NULL)
		return;
	m_pLibHandle = hMod;
	#elif defined(LOAD_KPP)
	#ifdef __APPLE__
	m_pLibHandle = dlopen("libkpp.dylib", RTLD_NOW);
	#else
	m_pLibHandle = dlopen("libkpp.so", RTLD_NOW);
	#endif
	if (m_pLibHandle == 0)
		return;
	#else
	return;
	#endif
	
	// this include will generate the necessary loading functions
	// due to the DECLARE_OPTIMIZED_FUNCTION_x macros
	#include "OptimizedFunctionsList.h"
	
	// activate with license key
	typedef bool (*ActivateFunction) (const char *);
	ActivateFunction f = 0;
	
	#ifdef WIN32
	f = (ActivateFunction) GetProcAddress(hMod, "Activate");
	#elif defined(LOAD_KPP)
	f = (ActivateFunction) dlsym(m_pLibHandle, "Activate");
	#endif
	
	if (f)
	{
		printf("info: loaded Keyetech Performance Primitives (KPP)\n");
		
		if (f("license_kpp.txt"))
			printf("info: activated KPP with license key\n");
		else
		{
			printf("info: KPP activation with licensey key failed\n");
			printf("info: The KPP thus run as a demo version.\n");
		}
	}
}


// these macros clear the function pointers
#undef DECLARE_OPTIMIZED_FUNCTION_0
#undef DECLARE_OPTIMIZED_FUNCTION_1
#undef DECLARE_OPTIMIZED_FUNCTION_2
#undef DECLARE_OPTIMIZED_FUNCTION_3
#undef DECLARE_OPTIMIZED_FUNCTION_4
#undef DECLARE_OPTIMIZED_FUNCTION_5
#undef DECLARE_OPTIMIZED_FUNCTION_5_RET
#undef DECLARE_OPTIMIZED_FUNCTION_8
#undef DECLARE_OPTIMIZED_FUNCTION_9

#define DECLARE_OPTIMIZED_FUNCTION_0(name) Optimized##name = 0;
#define DECLARE_OPTIMIZED_FUNCTION_1(name, p1) Optimized##name = 0;
#define DECLARE_OPTIMIZED_FUNCTION_2(name, p1, p2) Optimized##name = 0;
#define DECLARE_OPTIMIZED_FUNCTION_3(name, p1, p2, p3) Optimized##name = 0;
#define DECLARE_OPTIMIZED_FUNCTION_4(name, p1, p2, p3, p4) Optimized##name = 0;
#define DECLARE_OPTIMIZED_FUNCTION_5(name, p1, p2, p3, p4, p5) Optimized##name = 0;
#define DECLARE_OPTIMIZED_FUNCTION_5_RET(name, p1, p2, p3, p4, p5) Optimized##name = 0;
#define DECLARE_OPTIMIZED_FUNCTION_8(name, p1, p2, p3, p4, p5, p6, p7, p8) Optimized##name = 0;
#define DECLARE_OPTIMIZED_FUNCTION_9(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) Optimized##name = 0;

void CPerformanceLibInitializer::FreePerformanceLib()
{
	if (m_pLibHandle != 0)
	{
		#if defined(WIN32)
		HMODULE hMod = (HMODULE) m_pLibHandle;
		FreeLibrary(hMod);
		#elif defined(LOAD_KPP)
		dlclose(m_pLibHandle);
		#endif
		
		m_pLibHandle = 0;
	}
	
	// this include will generate the necessary clear operations
	// due to the DECLARE_OPTIMIZED_FUNCTION_x macros
	#include "OptimizedFunctionsList.h"
}
