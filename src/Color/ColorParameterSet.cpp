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
// Filename:  ColorParameterSet.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ColorParameterSet.h"

#include <stdio.h>
#include <string.h>
#include <string>




// *****************************************************************
// Constructor / Destructor
// *****************************************************************

CColorParameterSet::CColorParameterSet()
{
    m_nColors = (int) eNumberOfColors;
	
	m_ppParameters = new int*[m_nColors];
	for (int i = 0; i < m_nColors; i++)
	{
		m_ppParameters[i] = new int[6];
		
		for (int j = 0; j < 6; j++)
			m_ppParameters[i][j] = 0;
	}
	
	SetColorParameters(eSkin, 1, 16, 20, 128, 25, 255);
	SetColorParameters(eYellow, 40, 26, 36, 255, 25, 255);
	SetColorParameters(eOrange, 21, 15, 128, 255, 25, 255);
	SetColorParameters(eRed, 0, 10, 140, 255, 25, 255);
	SetColorParameters(eBlue, 113, 13, 149, 255, 25, 255);
    SetColorParameters(eGreen, 65, 20, 100, 255, 25, 255);
    SetColorParameters(eWhite, 90, 90, 0, 120, 200, 255);
}

CColorParameterSet::~CColorParameterSet()
{
	for (int i = 0; i < m_nColors; i++)
		delete [] m_ppParameters[i];
	
	delete [] m_ppParameters;
}


// ****************************************************************************
// Copy Constructor
// ****************************************************************************

CColorParameterSet::CColorParameterSet(const CColorParameterSet &colorParameterSet)
{
	m_nColors = colorParameterSet.m_nColors;
	
	m_ppParameters = new int*[m_nColors];

	for (int i = 0; i < m_nColors; i++)
	{
		m_ppParameters[i] = new int[6];
		
		m_ppParameters[i][0] = colorParameterSet.m_ppParameters[i][0];
		m_ppParameters[i][1] = colorParameterSet.m_ppParameters[i][1];
		m_ppParameters[i][2] = colorParameterSet.m_ppParameters[i][2];
		m_ppParameters[i][3] = colorParameterSet.m_ppParameters[i][3];
		m_ppParameters[i][4] = colorParameterSet.m_ppParameters[i][4];
		m_ppParameters[i][5] = colorParameterSet.m_ppParameters[i][5];
	}
}


// ****************************************************************************
// Methods
// ****************************************************************************

CColorParameterSet& CColorParameterSet::operator=(const CColorParameterSet &colorParameterSet)
{
	if (this == &colorParameterSet)
		return *this;

	int i;
               
	for (i = 0; i < m_nColors; i++)
		delete [] m_ppParameters[i];
       
	delete [] m_ppParameters;
	
	// create new content
	m_nColors = colorParameterSet.m_nColors;
	
	m_ppParameters = new int*[m_nColors];
        
	for (i = 0; i < m_nColors; i++)
	{
		m_ppParameters[i] = new int[6];
               
        m_ppParameters[i][0] = colorParameterSet.m_ppParameters[i][0];
        m_ppParameters[i][1] = colorParameterSet.m_ppParameters[i][1];
        m_ppParameters[i][2] = colorParameterSet.m_ppParameters[i][2];
        m_ppParameters[i][3] = colorParameterSet.m_ppParameters[i][3];
        m_ppParameters[i][4] = colorParameterSet.m_ppParameters[i][4];
        m_ppParameters[i][5] = colorParameterSet.m_ppParameters[i][5];
	}

	return *this;
}

void CColorParameterSet::SetColorParameters(ObjectColor color, int par1, int par2, int par3, int par4, int par5, int par6)
{
	const int nIndex = (int) color;
	
	if (nIndex < 0 || nIndex >= m_nColors)
	{
		printf("error: tried to assign color '%i' with enum out of range (m_nColors = %i)\n", nIndex, m_nColors);
		return;
	}
	
	m_ppParameters[nIndex][0] = par1;
	m_ppParameters[nIndex][1] = par2;
	m_ppParameters[nIndex][2] = par3;
	m_ppParameters[nIndex][3] = par4;
	m_ppParameters[nIndex][4] = par5;
	m_ppParameters[nIndex][5] = par6;
}

const int* CColorParameterSet::GetColorParameters(ObjectColor color) const
{
	const int nIndex = (int) color;
	
	if (nIndex < 0 || nIndex >= m_nColors)
	{
		printf("error: tried to retrieve color '%i' with enum out of range (m_nColors = %i)\n", nIndex, m_nColors);
		return 0;
	}
	
	return m_ppParameters[nIndex];
}

ObjectColor CColorParameterSet::Translate(const char *pColorName)
{
	if (strcmp(pColorName, "skin") == 0) return eSkin;
	else if (strcmp(pColorName, "yellow") == 0) return eYellow;
	else if (strcmp(pColorName, "yellow2") == 0) return eYellow2;
	else if (strcmp(pColorName, "yellow3") == 0) return eYellow3;
	else if (strcmp(pColorName, "orange") == 0) return eOrange;
	else if (strcmp(pColorName, "orange2") == 0) return eOrange2;
	else if (strcmp(pColorName, "orange3") == 0) return eOrange3;
	else if (strcmp(pColorName, "red") == 0) return eRed;
	else if (strcmp(pColorName, "red2") == 0) return eRed2;
	else if (strcmp(pColorName, "red3") == 0) return eRed3;
	else if (strcmp(pColorName, "blue") == 0) return eBlue;
	else if (strcmp(pColorName, "blue2") == 0) return eBlue2;
	else if (strcmp(pColorName, "blue3") == 0) return eBlue3;
	else if (strcmp(pColorName, "green") == 0) return eGreen;
	else if (strcmp(pColorName, "green2") == 0) return eGreen2;
    else if (strcmp(pColorName, "green3") == 0) return eGreen3;
    else if (strcmp(pColorName, "white") == 0) return eWhite;
    else if (strcmp(pColorName, "none") == 0) return eNone;
    else if (strcmp(pColorName, "colored") == 0) return eColored;
    else return eNone;
}

void CColorParameterSet::Translate(ObjectColor color, std::string &sName)
{
	switch (color)
	{
		case eSkin: sName = "skin"; break;
		case eYellow: sName = "yellow"; break;
		case eYellow2: sName = "yellow2"; break;
		case eYellow3: sName = "yellow3"; break;
		case eOrange: sName = "orange"; break;
		case eOrange2: sName = "orange2"; break;
		case eOrange3: sName = "orange3"; break;
		case eRed: sName = "red"; break;
		case eRed2: sName = "red2"; break;
		case eRed3: sName = "red3"; break;
		case eBlue: sName = "blue"; break;
		case eBlue2: sName = "blue2"; break;
		case eBlue3: sName = "blue3"; break;
		case eGreen: sName = "green"; break;
		case eGreen2: sName = "green2"; break;
        case eGreen3: sName = "green3"; break;
        case eWhite: sName = "white"; break;
        case eNone: sName = "none"; break;
        case eColored: sName = "colored"; break;
        default: sName = ""; printf("error: (internal error) color %i not handled\n", (int) color); break;
	}
}


bool CColorParameterSet::LoadFromFile(const char *pFileName)
{
	FILE *f = fopen(pFileName, "r");
	if (!f)
		return false;
		
	char colorName[100];
	int par1, par2, par3, par4, par5, par6;
	int **pp = m_ppParameters;
		
	while (true)
	{
		if (fscanf(f, "%s%i%i%i%i%i%i", colorName, &par1, &par2, &par3, &par4, &par5, &par6) == EOF)
			break;
		
		ObjectColor color = Translate(colorName);
		
		if (color < eNone || color >= eNumberOfColors)
		{
			printf("error: color name '%s' is not handled\n", colorName);
		}
		else
		{
			const int nIndex = (int) color;
			
			pp[nIndex][0] = par1;
			pp[nIndex][1] = par2;
			pp[nIndex][2] = par3;
			pp[nIndex][3] = par4;
			pp[nIndex][4] = par5;
			pp[nIndex][5] = par6;
		}
	}
	
	fclose(f);
	
	return true;
}

bool CColorParameterSet::SaveToFile(const char *pFileName)
{
	FILE *f = fopen(pFileName, "w");
	if (!f)
		return false;
		
	int **pp = m_ppParameters;
	
	for (int i = 0; i < m_nColors; i++)
	{
		ObjectColor c = (ObjectColor) i;
		std::string sName;
		Translate(c, sName);
		fprintf(f, "%s %i %i %i %i %i %i\n", sName.c_str(), pp[c][0], pp[c][1], pp[c][2], pp[c][3], pp[c][4], pp[c][5]);
	}
	
	fclose(f);

	return true;
}
