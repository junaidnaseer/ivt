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
// Filename:  Configuration.h
// Author:    Kai Welke
// Date:      03.03.2005
// ****************************************************************************


#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_



// **************************************************************
// CConfiguration
// **************************************************************

class CConfiguration
{
public:
	/** read a configuration file 
	@param szFileName full filename of .cfg file
	@return bValid returns true if error occured, false otherwise
	*/
	bool Read(const char* szFileName);
	bool Read();
	
	void SetFileName(const char* szFileName);
	
	/// Construct a new Configuration object.
	CConfiguration();
	/// Construct a new Configuration object.
	CConfiguration(const char* szFileName);
	
	/// Destructor.
	virtual ~CConfiguration();

	/// Get string from configutation file.
	bool GetString(char* szName, char*& szReturnString );
	/// Get integer from configutation file.
	bool GetInt(char* szName, int& nReturnInt );
	/// Get double from configutation file.
	bool GetDouble(char* szName, double& dReturnDouble );
	/// Get float from configutation file.
	bool GetFloat(char* szName, float& fReturnFloat );
	/// Get bool from configutation file.
	bool GetBool(char* szName, bool& bReturnBool );

private:
	
	bool ParseBuffer(char* pchBuffer);

	bool SeekNextContent(char* pchBuffer,int& cnBufferPosition);
	
	bool CheckControlCharacter(char* pchBuffer, int& cnBufferPosition);
	
	bool ExtractName(char* pchBuffer, int& cnBufferPosition, char*& pchResultName);
	bool ExtractValue(char* pchBuffer, int& cnBufferPosition, char*& pchResultValue);
	
	void AddVariable(char* pchCurrentName, char* pchCurrentValue);
	
	int GetLineNumber(char* pchBuffer, int cnBufferPosition);


	/// Determine if variable with Name exists.
	bool GetVarByName(char* szName, char*& szReturnString);


	typedef struct VarMap
	{
		char* szName;
		char* szValue;
	} TVariableMap;
	
	TVariableMap** m_ppVariables;
	int m_nVariableCount;
	
	int m_nFileLength;
	char* m_pchFileName;	
};



#endif // _CONFIGURATION_H_
