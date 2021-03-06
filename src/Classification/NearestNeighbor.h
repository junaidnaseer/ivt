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
// Filename:  NearestNeighbor.h
// Author:    Pedram Azad
// Date:      06.10.2009
// ****************************************************************************

/** \defgroup Classificators Classificators */


#ifndef _NEAREST_NEIGHBOR_H_
#define _NEAREST_NEIGHBOR_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/ClassificatorInterface.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CKdTree;



// ****************************************************************************
// CNearestNeighbor
// ****************************************************************************

/*!
	\ingroup Classificators
	\brief Class containing different implementations of the nearest neighbor classificator.
*/
class CNearestNeighbor : public CClassificatorInterface
{
public:
	enum ComputationMethod
	{
		eBruteForce,
		eKdTree,
		eBruteForceGPU
	};
	
	// constructor
	CNearestNeighbor(ComputationMethod method);
	
	// destructor
	~CNearestNeighbor();
	
	
	// public methods
	void SetKdTreeMaxLeaves(int nKdTreeMaxLeaves) { m_nKdTreeMaxLeaves = nKdTreeMaxLeaves; }
	bool Train(const float *pData, int nDimension, int nDataSets);
	int Classify(const float *pQuery, int nDimension, float &fResultError);
	bool Classify(const float *pQueries, int nDimension, int nQueries, int *pResults, float *pResultErrors);


private:
	// private attributes
	int m_nDimension;
	int m_nDataSets;
	float *m_pData;
	CKdTree *m_pKdTree;
	int m_nKdTreeMaxLeaves;
	bool m_bTrained;
	ComputationMethod m_method;
};



#endif /* _NEAREST_NEIGHBOR_H_ */
