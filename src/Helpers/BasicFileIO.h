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
// Filename:  BasicFileIO.h
// Author:    Kai Welke
// Date:      25.03.2005
// ****************************************************************************


#ifndef _BASICFILEIO_H_
#define _BASICFILEIO_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include <stdio.h>
#include <string>



// ****************************************************************************
// CBasicFileIO
// ****************************************************************************

class CBasicFileIO
{
public:
	// read basic types
	static bool ReadBool(char*& pchBuffer);
	static char ReadChar(char*& pchBuffer);
	static std::string ReadString(char*& pchBuffer);
	static short ReadShort(char*& pchBuffer);
	static int ReadInt(char*& pchBuffer);
	static float ReadFloat(char*& pchBuffer);
	static double ReadDouble(char*& pchBuffer);
	static void ReadBytes(char*& pchBuffer, void* pDest, int nSize);
	
	// write basic types
	static bool WriteBool(FILE *fp, bool bValue);
	static bool WriteChar(FILE *fp, char chValue);
	static bool WriteString(FILE *fp, std::string szValue);
	static bool WriteInt(FILE *fp, int nValue);
	static bool WriteFloat(FILE *fp, float fValue);
	static bool WriteDouble(FILE *fp, double dValue);
	static bool WriteBytes(FILE *fp, void* pSrc, int nSize);
	
	// others
	static int GetFileSize(FILE *fp);
};



#endif // _BASICFILEIO_H_
