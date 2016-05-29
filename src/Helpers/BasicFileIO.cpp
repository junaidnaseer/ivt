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
// Filename:  BasicFileIO.cpp
// Author:    Kai Welke
// Date:      25.03.2005
// ****************************************************************************
// Changes:   20.03.2009, Moritz Hassert:
//            * ReadInt etc.: replaced type-cast-loads with memcpy() to avoid
//              problems on platforms that expect aligned pointers
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "BasicFileIO.h"
#include <string.h>
#include <stdlib.h>



// ****************************************************************************
// CBasicFileIO reading
// ****************************************************************************

bool CBasicFileIO::ReadBool(char*& pchBuffer)
{
	bool bReturn = (*((char *) pchBuffer)) ? true : false;
	pchBuffer += sizeof(char);
	return bReturn;
}

char CBasicFileIO::ReadChar(char*& pchBuffer)
{
	char chReturn = *((char*)pchBuffer);
	pchBuffer += sizeof(char);
	return chReturn;
}

std::string CBasicFileIO::ReadString(char*& pchBuffer)
{
	int nLength = ReadInt(pchBuffer);
	
	if(nLength == 0) return "";
	
	std::string szReturn(pchBuffer, nLength);
	pchBuffer += nLength;
	
	return szReturn;
}

short CBasicFileIO::ReadShort(char*& pchBuffer)
{
	short nReturn;
	memcpy(&nReturn, pchBuffer, sizeof(short));
	pchBuffer += sizeof(short);
	return nReturn;
}

int CBasicFileIO::ReadInt(char*& pchBuffer)
{
	int nReturn;
	memcpy(&nReturn, pchBuffer, sizeof(int));
	pchBuffer += sizeof(int);
	return nReturn;
}

float CBasicFileIO::ReadFloat(char*& pchBuffer)
{
	float fReturn;
	memcpy(&fReturn, pchBuffer, sizeof(float));
	pchBuffer += sizeof(float);
	return fReturn;
}

double CBasicFileIO::ReadDouble(char*& pchBuffer)
{
	double dReturn;
	memcpy(&dReturn, pchBuffer, sizeof(double));
	pchBuffer += sizeof(double);
	return dReturn;
}

void CBasicFileIO::ReadBytes(char*& pchBuffer, void* pDest, int nSize)
{
	memcpy(pDest,(void*) pchBuffer,nSize);
	pchBuffer += nSize;
}


// ****************************************************************************
// CBasicFileIO writing
// ****************************************************************************

bool CBasicFileIO::WriteBool(FILE *fp, bool bValue)
{
	char chValue = (char) bValue;
	return fwrite(&chValue, 1, 1, fp) == 1;
}

bool CBasicFileIO::WriteChar(FILE *fp, char chValue)
{
	return fwrite(&chValue, 1, 1, fp) == 1;
}

bool CBasicFileIO::WriteString(FILE *fp, std::string szValue)
{
	if (szValue == "")
		return WriteInt(fp,0);

	return WriteInt(fp, (int) szValue.length()) && fwrite(szValue.data(), szValue.length(), 1, fp) == 1;
}

bool CBasicFileIO::WriteInt(FILE *fp, int nValue)
{
	return fwrite(&nValue, sizeof(int), 1, fp) == 1;
}

bool CBasicFileIO::WriteFloat(FILE *fp, float fValue)
{
	return fwrite(&fValue, sizeof(float), 1, fp) == 1;
}

bool CBasicFileIO::WriteDouble(FILE *fp, double dValue)
{
	return fwrite(&dValue, sizeof(double), 1, fp) == 1;
}

bool CBasicFileIO::WriteBytes(FILE *fp, void* pSrc, int nSize)
{
	return fwrite(pSrc, nSize, 1, fp) == 1;
}


int CBasicFileIO::GetFileSize(FILE *fp)
{
	const int nCurrentFilePos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	const int nFileSize = ftell(fp);
	fseek(fp, nCurrentFilePos, SEEK_SET);
	return nFileSize;
}
