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
// Filename:  NearestNeighbor.cpp
// Author:    Pedram Azad
// Date:      06.10.2009
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "NearestNeighbor.h"
#include "DataStructures/KdTree/KdTree.h"
#include "Helpers/OptimizedFunctions.h"

#include <string.h>
#include <float.h>
#include <math.h>
#include <stdio.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CNearestNeighbor::CNearestNeighbor(ComputationMethod method)
{
	m_pData = 0;
	m_pKdTree = 0;
	m_nDimension = 0;
	m_nDataSets = 0;
	m_nKdTreeMaxLeaves = -1;
	m_method = method;
	m_bTrained = false;
}

CNearestNeighbor::~CNearestNeighbor()
{
	if (m_pData)
		delete [] m_pData;
	
	if (m_pKdTree)
		delete m_pKdTree;

	OPTIMIZED_FUNCTION_HEADER_0(NearestNeighbor_CleanupGPU)
	OPTIMIZED_FUNCTION_FOOTER
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CNearestNeighbor::Train(const float *pData, int nDimension, int nDataSets)
{
	if (nDataSets < 1)
	{
		m_bTrained = false;
		return false;
	}
	
	m_nDimension = nDimension;
	m_nDataSets = nDataSets;
	m_bTrained = false;
	
	if (m_method == eBruteForce)
	{
		if (m_pData)
			delete [] m_pData;
		
		m_pData = new float[nDimension * nDataSets];
		
		memcpy(m_pData, pData, nDimension * nDataSets * sizeof(float));
	}
	else if (m_method == eKdTree)
	{
		const int nOverallDimension = nDimension + 2;
		int i;
		
		float **ppValues = new float*[nDataSets];
		
		// build values for search tree generation
		for (i = 0; i < nDataSets; i++)
		{
			ppValues[i] = new float[nOverallDimension];
			
			// copy data
			memcpy(ppValues[i], pData + i * nDimension, nDimension * sizeof(float));
			
			// set user data
			memcpy(&ppValues[i][nDimension], &i, sizeof(int));
		}
		
		if (m_pKdTree)
			delete m_pKdTree;
		
		m_pKdTree = new CKdTree();
		m_pKdTree->Build(ppValues, 0, nDataSets - 1, 3, nDimension, 2);
		
		for (i = 0; i < nDataSets; i++)
			delete [] ppValues[i];
		
		delete [] ppValues;
	}
	else if (m_method == eBruteForceGPU)
	{
		OPTIMIZED_FUNCTION_HEADER_3(NearestNeighbor_TrainGPU, pData, nDimension, nDataSets)
		return false;
		OPTIMIZED_FUNCTION_FOOTER
	}
	
	m_bTrained = true;
	
	return true;
}

int CNearestNeighbor::Classify(const float *pQuery, int nDimension, float &fResultError)
{
	if (!m_bTrained)
	{
		printf("error: classifier not trained in CNearestNeighbor::Classify\n");
		return -1;
	}
	
	if (m_nDimension != nDimension)
	{
		printf("error: query dimension and trained dimension do not match in CNearestNeighbor::Classify\n");
		return -1;
	}
	
	if (m_method == eBruteForce)
	{
		const float *pData = m_pData;
		
		int best_i = -1;
		float min = FLT_MAX;
		
		for (int i = 0; i < m_nDataSets; i++)
		{
			float error = 0.0f;
			
			for (int j = 0; j < m_nDimension; j++)
			{
				register float v = pQuery[j] - pData[j];
				error += v * v;
			}
			
			if (error < min)
			{
				min = error;
				best_i = i;
			}
			
			pData += m_nDimension;
		}
		
		fResultError = min;
		
		return best_i;
	}
	else if (m_method == eKdTree)
	{
		float *pData, error;
		m_pKdTree->NearestNeighborBBF(pQuery, error, pData, m_nKdTreeMaxLeaves);
		
		int nResultIndex;
		memcpy(&nResultIndex, pData + m_nDimension, sizeof(int));
		
		fResultError = error;
		
		return nResultIndex;
	}
	else if (m_method == eBruteForceGPU)
	{
		int nResult;
		OPTIMIZED_FUNCTION_HEADER_4(NearestNeighbor_ClassifyGPU, pQuery, nDimension, nResult, fResultError)
		return nResult;
		OPTIMIZED_FUNCTION_FOOTER
	}
	
	return -1; // will never happen
}

bool CNearestNeighbor::Classify(const float *pQueries, int nDimension, int nQueries, int *pResults, float *pResultErrors)
{
	if (!m_bTrained)
	{
		printf("error: classifier not trained in CNearestNeighbor::Classify\n");
		return false;
	}
	
	if (m_nDimension != nDimension)
	{
		printf("error: query dimension and trained dimension do not match in CNearestNeighbor::Classify\n");
		return false;
	}
	
	if (m_method == eBruteForce)
	{
		const float *pQuery = pQueries;
		
		for (int k = 0; k < nQueries; k++)
		{
			const float *pData = m_pData;
			
			int best_i = -1;
			float min = FLT_MAX;
			
			for (int i = 0; i < m_nDataSets; i++)
			{
				float error = 0.0f;
				
				for (int j = 0; j < m_nDimension; j++)
				{
					register float v = pQuery[j] - pData[j];
					error += v * v;
				}
				
				if (error < min)
				{
					min = error;
					best_i = i;
				}
				
				pData += m_nDimension;
			}
			
			pResults[k] = best_i;
			pResultErrors[k] = min;
			
			pQuery += m_nDimension;
		}
		
		return true;
	}
	else if (m_method == eKdTree)
	{
		const float *pQuery = pQueries;
		
		for (int k = 0; k < nQueries; k++)
		{
			float *pData, error;
			m_pKdTree->NearestNeighborBBF(pQuery, error, pData, m_nKdTreeMaxLeaves);
		
			memcpy(pResults + k, pData + m_nDimension, sizeof(int)); // index
			pResultErrors[k] = error;
		
			pQuery += m_nDimension;
		}
		
		return true;
	}
	else if (m_method == eBruteForceGPU)
	{
		OPTIMIZED_FUNCTION_HEADER_5(NearestNeighbor_ClassifyBundleGPU, pQueries, nDimension, nQueries, pResults, pResultErrors)
		return false;
		OPTIMIZED_FUNCTION_FOOTER
		return true;
	}
	
	return false; // will never happen
}
