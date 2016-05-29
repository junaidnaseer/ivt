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
// Filename:  Matd.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "Vecd.h"
#include "Matd.h"

#include <math.h>
#include <stdio.h>
#include <string.h>



// ****************************************************************************
// Constructors / Destructor
// ****************************************************************************

CMatd::CMatd()
{
	m_nRows = 0;
	m_nColumns = 0;
	m_ppElements = 0;
}

CMatd::CMatd(int nRows, int nColumns)
{
	m_ppElements = 0;

	SetSize(nRows, nColumns);
}

CMatd::CMatd(const CMatd &m)
{
	m_ppElements = 0;

	SetSize(m.m_nRows, m.m_nColumns);

	for (int i = 0; i < m_nRows; i++)
		for (int j = 0; j < m_nColumns; j++)
			m_ppElements[i][j] = m.m_ppElements[i][j];
}

CMatd::~CMatd()
{
	if (m_ppElements)
	{
		for (int i = 0; i < m_nRows; i++)
			delete [] m_ppElements[i];

		delete [] m_ppElements;
	}
}


// ****************************************************************************
// Operators
// ****************************************************************************

double& CMatd::operator() (int nRow, int nColumn) const
{
	//_ASSERTE(nRow >= 0 && nRow < m_nRows && nColumn >= 0 && nColumn < m_nColumns);

	return m_ppElements[nRow][nColumn];
}

CMatd& CMatd::operator= (const CMatd &m)
{
	SetSize(m.m_nRows, m.m_nColumns);

	for (int i = 0; i < m_nRows; i++)
		for (int j = 0; j < m_nColumns; j++)
			m_ppElements[i][j] = m.m_ppElements[i][j];

	return *this;
}

void CMatd::operator*= (const double s)
{
	for (int i = 0; i < m_nRows; i++)
		for (int j = 0; j < m_nColumns; j++)
			m_ppElements[i][j] *= s;
}

CMatd CMatd::operator* (const CMatd &m)
{
	//_ASSERTE(m_nColumns == m.m_nRows && m_nColumns > 0);

	int i, j, k, newColumns = m.m_nColumns;
	double constantElement;
	CMatd result(m_nRows, newColumns);

	// ikj matrix multiplication
	for (i = 0; i < m_nRows; i++)
	{
		constantElement = m_ppElements[i][0];

		for (j = 0; j < newColumns; j++)
		{
			result.m_ppElements[i][j] = constantElement * m.m_ppElements[0][j];
		}
	}
	for (i = 0; i < m_nRows; i++)
	{
		for (k = 1; k < m_nColumns; k++)
		{
			constantElement = m_ppElements[i][k];

			for (j = 0; j < newColumns; j++)			
			{
				result.m_ppElements[i][j] += constantElement * m.m_ppElements[k][j];
			}
		}
	}

	return result;
}

CMatd CMatd::operator+ (const CMatd &m)
{
	if (m_nColumns != m.m_nColumns || m_nRows != m.m_nRows)
		printf("error: incompatible input in CMatd::operator+(const CMatd&)\n");

	CMatd result(m_nRows, m_nColumns);

	// ikj matrix multiplication
	for (int i = 0; i < m_nRows; i++)
		for (int j = 0; j < m_nColumns; j++)
			result.m_ppElements[i][j] = m_ppElements[i][j] + m.m_ppElements[i][j];

	return result;
}

CMatd CMatd::operator- (const CMatd &m)
{
	if (m_nColumns != m.m_nColumns || m_nRows != m.m_nRows)
		printf("error: incompatible input in CMatd::operator-(const CMatd&)\n");

	CMatd result(m_nRows, m_nColumns);

	// ikj matrix multiplication
	for (int i = 0; i < m_nRows; i++)
		for (int j = 0; j < m_nColumns; j++)
			result.m_ppElements[i][j] = m_ppElements[i][j] - m.m_ppElements[i][j];

	return result;
}

CMatd CMatd::operator* (double s)
{
	CMatd result(m_nRows, m_nColumns);

	// ikj matrix multiplication
	for (int i = 0; i < m_nRows; i++)
		for (int j = 0; j < m_nColumns; j++)
			result.m_ppElements[i][j] = s * m_ppElements[i][j];

	return result;
}

CVecd CMatd::operator* (const CVecd &v)
{
	//_ASSERTE(m_nColumns == v.GetSize());

	CVecd result(m_nRows);
	
	for (int i = 0; i < m_nRows; i++)
	{
		result[i] = 0.0;

		for (int j = 0; j < m_nColumns; j++)
		{
			result[i] += m_ppElements[i][j] * v[j];
		}
	}

	return result;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CMatd::Zero()
{
	for (int i = 0; i < m_nRows; i++)
		for (int j = 0; j < m_nColumns; j++)
			m_ppElements[i][j] = 0.0;
}

bool CMatd::Unit()
{
	if (m_nRows != m_nColumns)
		return false;

	Zero();

	for (int i = 0; i < m_nRows; i++)
		m_ppElements[i][i] = 1.0;


	return true;
}

CMatd CMatd::Invert() const
{
	if (m_nRows != m_nColumns)
	{
		printf("error: input matrix must be square matrix for CMatd::Invert\n");
		CMatd null(0, 0);
		return null;
	}

	const int n = m_nColumns;
	int i;

	CMatd copiedMatrix(*this);
	CMatd resultMatrix(n, n);

	resultMatrix.Unit();

	int *pPivotRows = new int[n];
	for (i = 0; i < n; i++)
		pPivotRows[i] = 0;
	
	// invert by gauss elimination
	for (i = 0; i < n; i++)
	{
		int j, nPivotColumn = 0;

		double *helper1 = copiedMatrix.m_ppElements[i];
		double *helper2 = resultMatrix.m_ppElements[i];

		// determine pivot element
		double max = 0;
		for (j = 0; j < n; j++)
			if (fabs(helper1[j]) > max)
			{
				max = fabs(helper1[j]);
				nPivotColumn = j;
			}

		pPivotRows[nPivotColumn] = i;

		const double dPivotElement = copiedMatrix.m_ppElements[i][nPivotColumn];

		if (fabs(dPivotElement) < 0.00001)
		{
			printf("error: input matrix is not regular for CMatd::Invert\n");
			delete [] pPivotRows;
			CMatd null(0, 0);
			return null;
		}

		const double dFactor = 1.0 / dPivotElement;
		
		for (j = 0; j < n; j++)
		{
			helper1[j] *= dFactor;
			helper2[j] *= dFactor;
		}

		for (j = 0; j < n; j++)
		{
			if (i != j)
			{
				const double v = copiedMatrix.m_ppElements[j][nPivotColumn];
				int k;

				helper1 = copiedMatrix.m_ppElements[j];
				helper2 = copiedMatrix.m_ppElements[i];
				for (k = 0; k < n; k++)
					helper1[k] -= v * helper2[k];
				helper1[nPivotColumn] = 0; // explicitly set to zero

				helper1 = resultMatrix.m_ppElements[j];
				helper2 = resultMatrix.m_ppElements[i];
				for (k = 0; k < n; k++)
					helper1[k] -= v * helper2[k];
			}
		}
	}

	// bring result rows in pivot order
	double **ppTemp = new double*[n];
	for (i = 0; i < n; i++)
		ppTemp[i] = resultMatrix.m_ppElements[i];
	
	for (i = 0; i < n; i++)
		resultMatrix.m_ppElements[i] = ppTemp[pPivotRows[i]];

	delete [] pPivotRows;

	return resultMatrix;
}

void CMatd::SetSize(int nRows, int nColumns)
{
	//_ASSERTE(nRows >= 0 && nColumns >= 0);

	// free memory first
	if (m_ppElements)
	{
		for (int i = 0; i < m_nRows; i++)
			delete [] m_ppElements[i];
		delete [] m_ppElements;
	}

	// allocate memory for matrix
	m_ppElements = new double*[nRows];
	for (int i = 0; i < nRows; i++)
	{
		m_ppElements[i] = new double[nColumns];

		for (int j = 0; j < nColumns;  j++)
			m_ppElements[i][j] = 0.0;
	}

	// save size of matrix
	m_nRows = nRows;
	m_nColumns = nColumns;
}

CMatd CMatd::GetTransposed()
{
	CMatd result(m_nColumns, m_nRows);

	for (int i = 0; i < m_nRows; i++)
		for (int j = 0; j < m_nColumns; j++)
			result.m_ppElements[j][i] = m_ppElements[i][j];

	return result;
}
