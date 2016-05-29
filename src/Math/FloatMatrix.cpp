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
// Filename:  FloatMatrix.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "FloatMatrix.h"

#include "Helpers/helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// ****************************************************************************
// Constructors / Destructor
// ****************************************************************************

CFloatMatrix::CFloatMatrix()
{
	columns = 0;
	rows = 0;
	data = 0;
	m_bOwnMemory = false;
}

CFloatMatrix::CFloatMatrix(int nColumns, int nRows, bool bHeaderOnly)
{
	columns = nColumns;
	rows = nRows;

	if (bHeaderOnly)
	{
		data = 0;
		m_bOwnMemory = false;
	}
	else
	{
        data = new float[columns * rows];
		m_bOwnMemory = true;
	}
}

CFloatMatrix::CFloatMatrix(const CFloatMatrix &matrix, bool bHeaderOnly)
{
	columns = matrix.columns;
	rows = matrix.rows;
	
	if (bHeaderOnly)
	{
		data = 0;
		m_bOwnMemory = false;
	}
	else
	{
		data = new float[columns * rows];
		m_bOwnMemory = true;
	}
}

CFloatMatrix::CFloatMatrix(const CFloatMatrix *pMatrix, bool bHeaderOnly)
{
	columns = pMatrix->columns;
	rows = pMatrix->rows;
	
	if (bHeaderOnly)
	{
		data = 0;
		m_bOwnMemory = false;
	}
	else
	{
		data = new float[columns * rows];
		m_bOwnMemory = true;
	}
}

CFloatMatrix::~CFloatMatrix()
{
    FreeMemory();
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CFloatMatrix::FreeMemory()
{
	if (data)
	{
		if (m_bOwnMemory)
            delete [] data;

		data = 0;
		m_bOwnMemory = false;
	}
}

bool CFloatMatrix::LoadFromFile(const char *pFileName)
{
	// try binary format. if not successful try text format
	FILE *f = fopen(pFileName, "rb");
	if (!f)
		return false;
		
	char header[30];
	header[29] = '\0';
	
	if (fread(header, 29, 1, f) != 1)
	{
		fclose(f);
		return false;
	}
	
	if (strstr(header, "MATRIX") != header)
	{
		fclose(f);
		return false;
	}
	
	const int nColumns = atoi(header + 7);
	const int nRows = atoi(header + 18);
	
	if (nColumns <= 0 || nRows <= 0)
	{
		fclose(f);
		return false;
	}
	
	// allocate memory for data and read data
	columns = nColumns;
	rows = nRows;
	if (data && m_bOwnMemory)
		delete [] data;
		
	data = new float[columns * rows];
	m_bOwnMemory = true;
	
	if (fread(data, columns * rows * sizeof(float), 1, f) != 1)
	{
		fclose(f);
		return false;
	}
	
	fclose(f);
	
	#ifdef IVT_BIG_ENDIAN
	const int nWords = columns * rows;
	for (int i = 0; i < nWords; i++)
		data[i] = invert_byte_order_float(data[i]);
	#endif
	
	return true;
}

bool CFloatMatrix::SaveToFile(const char *pFileName)
{
	if (!data || !rows || !columns)
		return false;

	FILE *f = fopen(pFileName, "wb");
	if (!f)
		return false;
		
	#ifdef IVT_BIG_ENDIAN
	const int nWords = columns * rows;
	int i;
	for (i = 0; i < nWords; i++)
		data[i] = invert_byte_order_float(data[i]);
	#endif
		
	fprintf(f, "MATRIX %.10i %.10i ", columns, rows);
	if (fwrite(data, columns * rows * sizeof(float), 1, f) != 1)
	{
		fclose(f);
		return false;
	}
	fclose(f);
	
	#ifdef IVT_BIG_ENDIAN
	for (i = 0; i < nWords; i++)
		data[i] = invert_byte_order_float(data[i]);
	#endif
	
	return true;
}
