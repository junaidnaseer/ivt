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
// Filename:  Configuration.cpp
// Author:    Kai Welke
// Date:      03.03.2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "Configuration.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// ****************************************************************************
// Constructors / Destructor
// ****************************************************************************

CConfiguration::CConfiguration()
{
	m_pchFileName = 0;
	m_nFileLength = 0;
	
	m_ppVariables = 0;
	m_nVariableCount = 0;
}

CConfiguration::CConfiguration(const char* pchFileName)
{
	m_pchFileName = 0;
	m_nFileLength = 0;
	
	m_ppVariables = 0;
	m_nVariableCount = 0;
	
	SetFileName(pchFileName);
}

CConfiguration::~CConfiguration()
{
	if (m_pchFileName)
		delete [] m_pchFileName;
		
	for(int v = 0 ; v < m_nVariableCount; v++)
	{
		delete [] m_ppVariables[v]->szName;
		delete [] m_ppVariables[v]->szValue;
		
		delete m_ppVariables[v];
	}
	
	delete m_ppVariables;
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CConfiguration::Read(const char* pchFileName)
{
	SetFileName(pchFileName);
	
	return Read();
}

bool CConfiguration::Read()
{
	bool bResult;
	
	// open config file for reading
	FILE* pCfgFile = fopen(m_pchFileName,"r");
	if (!pCfgFile)
	{
		printf("error: could not open config file '%s'\n",m_pchFileName);
		return false;
	}
	
	// obtain file size.
 	fseek(pCfgFile , 0 , SEEK_END);
  	m_nFileLength = ftell(pCfgFile);
  	rewind(pCfgFile);
	
	// create file buffer
	char *pchBuffer = new char[m_nFileLength];
	if (!pchBuffer) 
	{
		printf("error: out of memory\n");
		fclose(pCfgFile);
		return false;
	}
	
	// read whole file
	if (fread(pchBuffer, 1, m_nFileLength,pCfgFile) != m_nFileLength)
	{
		fclose(pCfgFile);
		return false;
	}

	// close config file
	fclose(pCfgFile);

	// parse the file
	bResult = ParseBuffer(pchBuffer);
	
	// release memory
	delete [] pchBuffer;
	
	return bResult;
	
}


void CConfiguration::SetFileName(const char* pchFileName)
{
	if (m_pchFileName)
		delete [] m_pchFileName;
		
	m_pchFileName = new char[strlen(pchFileName) + 1];
	strcpy(m_pchFileName, pchFileName);
}


bool CConfiguration::GetString(char* szName, char*& szReturnString )
{
	bool bResult = false;
	char* szString;
	
	bResult = GetVarByName( szName, szString  );
	
	if(bResult)
		szReturnString = szString;
	
	return bResult;
}

bool CConfiguration::GetInt(char* szName, int& nReturnInt )
{
	bool bResult = false;
	char* szInteger;
	
	bResult = GetVarByName( szName, szInteger  );
	
	if(bResult) nReturnInt = atoi(szInteger);
	
	return bResult;
}

bool CConfiguration::GetFloat(char* szName, float& fReturnFloat )
{
	bool bResult = false;
	char* szFloat;
	
	bResult = GetVarByName( szName, szFloat  );
	if (bResult) fReturnFloat = (float)atof(szFloat);
	
	return bResult;
}

bool CConfiguration::GetDouble(char* szName, double& dReturnDouble )
{
	bool bResult = false;
	char* szDouble;
	
	bResult = GetVarByName( szName, szDouble  );
	if(bResult) dReturnDouble = atof(szDouble);
	
	return bResult;
}

bool CConfiguration::GetBool(char* szName, bool& bReturnBool )
{
	char szOn[] = "on";
	char szTrue[] = "true";
	
	bool bResult = false;
	char* szBool;
	
	bResult = GetVarByName( szName, szBool  );
	
	if(bResult) 
	{
		if(strcmp(szBool,szOn) == 0 ||
		   strcmp(szBool,szTrue) == 0)
		{
			bReturnBool = true;
		} else {
			bReturnBool = false;
		
		}
	}
	return bResult;
}


bool CConfiguration::ParseBuffer(char* pchBuffer)
{
	// restart at beginning of buffer
	bool fExit = false;
	bool fError = false;
	int cnBufferPosition = 0;
	char* pchCurrentName = NULL;
	char* pchCurrentValue = NULL;
	
	while(!fExit)
	{
		// check if we have control chars for parser
		do
		{
			fExit = !SeekNextContent(pchBuffer,cnBufferPosition);
			if(fExit) break;
		} while(CheckControlCharacter(pchBuffer,cnBufferPosition));
		
		// read name
		fError = !ExtractName(pchBuffer,cnBufferPosition, pchCurrentName);
		if(fError)
		{
			printf("error: could not extract variable name in line %d\n", GetLineNumber(pchBuffer, cnBufferPosition));
			return false;
		}
		
		// seek to value	
		fExit = !SeekNextContent(pchBuffer,cnBufferPosition);
		
		// read value
		fError = !ExtractValue(pchBuffer,cnBufferPosition, pchCurrentValue);
		if (fError)
		{
			printf("error: could not extract value of variable '%s' in line %d\n", pchCurrentName, GetLineNumber(pchBuffer,cnBufferPosition));
			return false;
		}
		
		// add to map
		AddVariable(pchCurrentName,pchCurrentValue);
		
	}
	
	return true;
}

bool CConfiguration::CheckControlCharacter(char* pchBuffer, int& cnBufferPosition)
{
	switch(pchBuffer[cnBufferPosition])
	{
		case '#':
			while(cnBufferPosition < m_nFileLength && pchBuffer[cnBufferPosition] != '\n')
				cnBufferPosition++;
		return true;
			
		case '@':
			while (cnBufferPosition < m_nFileLength && pchBuffer[cnBufferPosition] != '\n')
				printf("%c", pchBuffer[cnBufferPosition++]);
			printf("\n");
		return true;
		
		default:
			break;
	}
	
	return false;
}

bool CConfiguration::SeekNextContent(char* pchBuffer,int& cnBufferPosition)
{
	while(	cnBufferPosition < m_nFileLength && 
	      ( pchBuffer[cnBufferPosition] == ' ' || 
		pchBuffer[cnBufferPosition] == '\t' || 
		pchBuffer[cnBufferPosition] == '\r' || 
		pchBuffer[cnBufferPosition] == '\n' || 
		pchBuffer[cnBufferPosition] == '\x27'
	      )
	     )
		cnBufferPosition++;
		
	return cnBufferPosition != m_nFileLength;
}

bool CConfiguration::ExtractName(char* pchBuffer, int& cnBufferPosition, char*& pchResultName)
{
	int nNameStart = cnBufferPosition;
	
	while(	cnBufferPosition < m_nFileLength && 
	      ( pchBuffer[cnBufferPosition] != ' ' && 
		pchBuffer[cnBufferPosition] != '\t' &&
		pchBuffer[cnBufferPosition] != '\r' && 
		pchBuffer[cnBufferPosition] != '\n' && 
		pchBuffer[cnBufferPosition] != '\x27'
	      )
	     )
		cnBufferPosition++;
		
	int nNameEnd = cnBufferPosition;
	
	// create new variable name
	pchResultName = new char[nNameEnd - nNameStart + 1];
	strncpy(pchResultName, pchBuffer + nNameStart, nNameEnd - nNameStart);
	pchResultName[nNameEnd - nNameStart] = '\0'; 
	
	return true;
}

bool CConfiguration::ExtractValue(char* pchBuffer, int& cnBufferPosition, char*& pchResultValue)
{
	int nValueStart = cnBufferPosition;
	bool fStringMode = false;
	
	while (cnBufferPosition < m_nFileLength && 
	      ( (pchBuffer[cnBufferPosition] != ' ' || fStringMode) && 
		(pchBuffer[cnBufferPosition] != '\t' || fStringMode) &&
		pchBuffer[cnBufferPosition] != '\r' && 
		pchBuffer[cnBufferPosition] != '\n' && 
		pchBuffer[cnBufferPosition] != '\x27' &&
		!(pchBuffer[cnBufferPosition] == '\"' && fStringMode)
	      )
	     )
	{
		if (pchBuffer[cnBufferPosition] == '\"')
			fStringMode = true;
			
		cnBufferPosition++;
	}
	
	int nValueEnd = cnBufferPosition;
	int nAdjust = 0;
	
	// skip hyphens if stringmode!
	if (fStringMode)
	{
		nAdjust = 1;
		cnBufferPosition++;
	}
	
	// create new variable name
	pchResultValue = new char[nValueEnd - nValueStart + 1 - nAdjust];
	strncpy(pchResultValue, pchBuffer + nValueStart + nAdjust, nValueEnd - nValueStart - nAdjust);
	pchResultValue[nValueEnd - nValueStart - nAdjust] = '\0'; 
	
	return true;
}

void CConfiguration::AddVariable(char* pchCurrentName, char* pchCurrentValue)
{
	if (pchCurrentName[0] == '\0' || pchCurrentValue[0] == '\0')
	{
		delete [] pchCurrentName;
		delete [] pchCurrentValue;
		return;
	}

	m_nVariableCount++;
	
	// allocate new array
	TVariableMap **ppNewMap = new TVariableMap*[m_nVariableCount];
	  	
	// copy old pointers
	if (m_ppVariables)
	{
		for (int v = 0 ; v < (m_nVariableCount - 1) ; v++)
			ppNewMap[v] = m_ppVariables[v];
	
		// get rid of old array
		delete [] m_ppVariables;
	}
	
	// create new variable	
	TVariableMap *pVariable = new TVariableMap();
	
	pVariable->szName = pchCurrentName;
	pVariable->szValue = pchCurrentValue;
	ppNewMap[m_nVariableCount - 1] = pVariable;
	
	// reassign pointer
	m_ppVariables = ppNewMap;
}

int CConfiguration::GetLineNumber(char* pchBuffer, int cnBufferPosition)
{
	int nLines = 0;
	
	for (int pos = 0 ; pos < cnBufferPosition ; pos++)
	{
		if (pchBuffer[pos] == '\r')
		{
			pos++;
			nLines++;
		}
		else if (pchBuffer[pos] == '\n')
		{
			nLines++;
		}
	}
	
	return nLines+1;
}

bool CConfiguration::GetVarByName(char* szName, char*& szReturnString)
{
	for (int v = 0 ; v < m_nVariableCount ; v++)
	{
		if (strcmp(szName,m_ppVariables[v]->szName) == 0)
		{
			szReturnString = m_ppVariables[v]->szValue;
			return true;
		}
	}
	
	return false;
}
