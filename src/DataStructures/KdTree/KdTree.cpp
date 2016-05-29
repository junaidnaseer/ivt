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
// Filename:  KdTree.cpp
// Author:    Kai Welke
// Date:      14.04.2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "KdTree.h"
#include "KdUtils.h"
#include "KdPriorityQueue.h"

#include "Helpers/Quicksort.h"
#include "Helpers/BasicFileIO.h"

#include <stdio.h>
#include <float.h>
#include <string.h>



static inline float SquaredEuclideanDistance(const float *pVector1, const float *pVector2, int nDimension)
{
	register float sum = 0.0f;
	
	for (register int x = 0; x < nDimension; x++)
	{
		const float v = pVector1[x] - pVector2[x];
		sum += v * v;
	}
	
	return sum;
}

static inline float CrossCorrelation(const float *pVector1, const float *pVector2, int nDimension)
{
	register float sum = 0.0f;
	
	for (register int x = 0; x < nDimension; x++)
		sum += pVector1[x] * pVector2[x];
	
	return sum;
}



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CKdTree::CKdTree(int nMaximumNumberOfNodes)
{
	// init pointers
	m_pRoot = 0;
	m_EnclosingBox.pfLow = 0;
	m_EnclosingBox.pfHigh = 0;
	
	// create priority list for BFF search
	m_pNodeListBBF = new CKdPriorityQueue(nMaximumNumberOfNodes);
	
	// create priority list for second closest element
	m_pNearestNeighborList_ = new CKdPriorityQueue(nMaximumNumberOfNodes);
	m_pNearestNeighborList = 0;
	
	m_nLeaves = 0;
	m_nMaximumLeavesToVisit = 0;
}

CKdTree::~CKdTree()
{
	Dispose();
	
	// delete priotiry queues
	delete m_pNodeListBBF;
	delete m_pNearestNeighborList_;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CKdTree::Build(float **ppfValues, int nLow, int nHigh, int nBucketSize, int nDimensions, int nUserDataSize)
{
	// set members
	m_nBucketSize =	nBucketSize;
	m_nDimensions =	nDimensions;
    m_nUserDataSize = nUserDataSize;
	m_nTotalVectorSize = m_nDimensions + m_nUserDataSize;
	m_nLeaves = nHigh - nLow + 1;
	
	// find enclosing bounding box
	m_EnclosingBox = CalculateEnclosingBoundingBox(ppfValues, m_nDimensions, m_nLeaves);
	
	// init members for recursion
	m_pRoot = BuildRecursive(ppfValues, nLow, nHigh, &m_EnclosingBox, 0);
}

CKdTreeNode* CKdTree::BuildRecursive(float **ppfValues,int nLow, int nHigh, KdBoundingBox* pCurrentBoundingBox, int nCurrentDepth)
{
	// **************************************
	// case1: create a leaf
	// **************************************
	if ((nHigh-nLow) <= (m_nBucketSize - 1))
	{
		// create leaf
		CKdTreeNode* pNewNode = new CKdTreeNode();
		
		pNewNode->m_bIsLeaf = 1;
		
		// create and fill bucket
		const int nSize = nHigh - nLow + 1;
				
		pNewNode->m_pValues = new float[nSize * m_nTotalVectorSize];
		pNewNode->m_nSize = nSize;
		
		float *pValues = pNewNode->m_pValues;
		
		for (int n = 0 ; n <  nSize; n++)
		{
			for (int d = 0; d < m_nTotalVectorSize; d++)
				pValues[d] = ppfValues[nLow + n][d];
			
			pValues += m_nTotalVectorSize;
		}
		
		pNewNode->m_nCutDimension = nCurrentDepth % m_nDimensions;
		pNewNode->m_Bounding.fLow = pCurrentBoundingBox->pfLow[pNewNode->m_nCutDimension];
		pNewNode->m_Bounding.fHigh = pCurrentBoundingBox->pfHigh[pNewNode->m_nCutDimension];
		
		return pNewNode;
	} 

	
	// *********************************************
	// case2: partition the set and start recursion
	// *********************************************
	
	const int nDimension = nCurrentDepth % m_nDimensions;
	
	// Quicksort array to find median and patition sets
	Quicksort::QuicksortByElementOfVector(ppfValues, nLow, nHigh, nDimension);
	
	// retrieve median value
	const int nMedianIndex = (nHigh - nLow) / 2 + nLow;
	const float fMedianValue = ppfValues[nMedianIndex][nDimension];
	
	// remember original boundings in dimension
	const float fHigh = pCurrentBoundingBox->pfHigh[nDimension];
	const float fLow = pCurrentBoundingBox->pfLow[nDimension];
	
	// *********************************************
	// start next recursion
	// *********************************************
	// note that median is element of left branch
	// adjust bounding box to mirror lower part of interval
	pCurrentBoundingBox->pfHigh[nDimension] = fMedianValue;
	
	// create left node
	CKdTreeNode* pNodeLeft = BuildRecursive(ppfValues, nLow, nMedianIndex, pCurrentBoundingBox, nCurrentDepth + 1);
	
	// adjust bounding box to mirror higher part of interval
	pCurrentBoundingBox->pfHigh[nDimension] = fHigh;
	pCurrentBoundingBox->pfLow[nDimension] = fMedianValue;
	
	// create right node
	CKdTreeNode* pNodeRight = BuildRecursive(ppfValues, nMedianIndex+1, nHigh, pCurrentBoundingBox, nCurrentDepth + 1);
	
	// readjust bounding box
	pCurrentBoundingBox->pfLow[nDimension] = fLow;
	
	// *********************************************
	// create inner nodes
	// *********************************************
	CKdTreeNode* pNewNode = new CKdTreeNode();
	// set cut dimension
	pNewNode->m_nCutDimension = nCurrentDepth % m_nDimensions;
	// set value of median
	pNewNode->m_fMedianValue = fMedianValue;
	// set children
	pNewNode->m_pLeftChild = pNodeLeft;
	pNewNode->m_pRightChild = pNodeRight;
	// set boundings in dimension
	pNewNode->m_Bounding.fLow = pCurrentBoundingBox->pfLow[pNewNode->m_nCutDimension];
	pNewNode->m_Bounding.fHigh = pCurrentBoundingBox->pfHigh[pNewNode->m_nCutDimension];
	// this is no leaf!
	pNewNode->m_bIsLeaf = 0;
	
	return pNewNode;
}

void CKdTree::Dispose()
{
	if (m_pRoot)
		DisposeRecursive(m_pRoot);
		
	if (m_EnclosingBox.pfLow)
		delete [] m_EnclosingBox.pfLow;
	
	if (m_EnclosingBox.pfHigh)
		delete [] m_EnclosingBox.pfHigh;
}

void CKdTree::DisposeRecursive(CKdTreeNode *pNode)
{
	if (pNode->m_bIsLeaf)
	{
		delete pNode;
		return;
	}
	
	// recursion
	if (pNode->m_pLeftChild) DisposeRecursive(pNode->m_pLeftChild);	
	if (pNode->m_pRightChild) DisposeRecursive(pNode->m_pRightChild);

	delete pNode;
}



void CKdTree::NearestNeighbor(const float *pQuery, float &fError, float *&pfNN, int nMaximumLeavesToVisit)
{
	// not used in this call
	m_pNearestNeighborList = 0;
	
	m_nMaximumLeavesToVisit = nMaximumLeavesToVisit <= 0 ? m_nLeaves : nMaximumLeavesToVisit;
	m_fCurrentMinDistance = FLT_MAX;
	
	// start recursion
	NearestNeighborRecursive(m_pRoot, pQuery);	
	
	// set return values
	fError = m_fCurrentMinDistance;
	pfNN = m_pNearestNeighbor;
}

void CKdTree::NearestNeighborBBF(const float *pQuery, float &fError, float *&pfNN, int nMaximumLeavesToVisit)
{
	// not used in this call
	m_pNearestNeighborList = 0;
	
	// set recursive parameter
	m_nMaximumLeavesToVisit = nMaximumLeavesToVisit <= 0 ? m_nLeaves : nMaximumLeavesToVisit;
	m_fCurrentMinDistance = FLT_MAX;
	
	// empty bbf node list for this run
	m_pNodeListBBF->Empty();

	// start recursion
	const float fDistanceBB = GetDistanceFromBox(m_EnclosingBox, pQuery, m_nDimensions);
	NearestNeighborRecursiveBBF(m_pRoot, pQuery, fDistanceBB);	
	
	// return result
	fError = m_fCurrentMinDistance;
	pfNN = m_pNearestNeighbor;
}

void CKdTree::NearestNeighbor(const float *pQuery, float &fError, CKdPriorityQueue *&pNNList, int nMaximumLeavesToVisit)
{
	// empty neighbor list
	m_pNearestNeighborList = m_pNearestNeighborList_;
	m_pNearestNeighborList->Empty();
	
	// member recursive params for this search
	m_nMaximumLeavesToVisit = nMaximumLeavesToVisit <= 0 ? m_nLeaves : nMaximumLeavesToVisit;
	m_fCurrentMinDistance = FLT_MAX;
	
	// start recursion
	NearestNeighborRecursive(m_pRoot, pQuery);	
	
	// set return values
	fError = m_fCurrentMinDistance;
	pNNList = m_pNearestNeighborList;
}

void CKdTree::NearestNeighborBBF(const float *pQuery, float &fError, CKdPriorityQueue *&pNNList, int nMaximumLeavesToVisit)
{
	// empty neighbor list
	m_pNearestNeighborList = m_pNearestNeighborList_;
	m_pNearestNeighborList->Empty();
	
	// set recursive parameter
	m_nMaximumLeavesToVisit = nMaximumLeavesToVisit <= 0 ? m_nLeaves : nMaximumLeavesToVisit;
	m_fCurrentMinDistance = FLT_MAX;
	
	// empty bbf node list for this run
	m_pNodeListBBF->Empty();

	// start recursion	
	const float fDistanceBB = GetDistanceFromBox(m_EnclosingBox, pQuery, m_nDimensions);
	NearestNeighborRecursiveBBF(m_pRoot, pQuery, fDistanceBB);	
	
	// return result
	fError = m_fCurrentMinDistance;
	pNNList = m_pNearestNeighborList;
}


void CKdTree::NearestNeighborRecursive(CKdTreeNode *pNode, const float *pQuery)
{
	// maximum number of leaves searched
	if (m_nMaximumLeavesToVisit == 0)
		return;
	
	if (pNode->m_bIsLeaf)
	{
		// go through bucket and find nearest neighbor with linear search
		float *pValues = pNode->m_pValues;
		const int nSize = pNode->m_nSize;
		
		for (int i = 0 ; i < nSize; i++)
		{
			// check distance
			const float fDistance = SquaredEuclideanDistance(pValues, pQuery, m_nDimensions);
			
			if (fDistance < m_fCurrentMinDistance)
			{
				m_pNearestNeighbor = pValues;
				m_fCurrentMinDistance = fDistance;
				
				// list for second closest element
				if (m_pNearestNeighborList)
					m_pNearestNeighborList->Push(m_fCurrentMinDistance, pValues);
			}
		}
        
		m_nMaximumLeavesToVisit--;
			
		// go up one level
		return;
	}
    
	// step down
	const float fCutDifference = pQuery[pNode->m_nCutDimension] - pNode->m_fMedianValue;
	const bool bFromLeft = fCutDifference < 0.0f;
	
	NearestNeighborRecursive(bFromLeft ? pNode->m_pLeftChild : pNode->m_pRightChild, pQuery);

	// worst-case estimation
	if (m_fCurrentMinDistance >= fCutDifference * fCutDifference)
	{
		// search other sub-tree
		NearestNeighborRecursive(bFromLeft ? pNode->m_pRightChild : pNode->m_pLeftChild, pQuery);
	}
}

void CKdTree::NearestNeighborRecursiveBBF(CKdTreeNode* pNode, const float *pQuery, float fDistanceBB)
{
	// maximum number of leaves searched
	if (m_nMaximumLeavesToVisit == 0)
		return;
	
	if (pNode->m_bIsLeaf)
	{
		// go through bucket and find nearest neighbor with linear search
		float *pValues = pNode->m_pValues;
		const int nSize = pNode->m_nSize;

		for (int i = 0; i < nSize; i++)
		{
			const float fDistance = SquaredEuclideanDistance(pValues, pQuery, m_nDimensions);
			
			if (fDistance < m_fCurrentMinDistance)
			{
				m_pNearestNeighbor = pValues;       				
				m_fCurrentMinDistance = fDistance;
				
				// list for second closest element
				if (m_pNearestNeighborList)
					m_pNearestNeighborList->Push(m_fCurrentMinDistance, pValues);
			}
			
			pValues += m_nTotalVectorSize;
		}

		m_nMaximumLeavesToVisit--;
		
		return;
	}
	    
	
	// step down
	const int nDimension = pNode->m_nCutDimension;

	// difference from query to cut plane
	const float fCutDifference = pQuery[nDimension] - pNode->m_fMedianValue;
	
	if (fCutDifference < 0.0f)
	{
		// difference from box to point (was the old distance for this node)
		float fBoxDifference = pNode->m_Bounding.fLow - pQuery[nDimension];

		// point inside? -> no difference
		if (fBoxDifference < 0.0f)
			fBoxDifference = 0.0f;
			
		// adjust distance:
		// subtract old distance (fBoxDifference)
		// add new distance (fCutDifference = new lower bound of right child)
		const float fChildDistanceBB = fDistanceBB - fBoxDifference * fBoxDifference + fCutDifference * fCutDifference;
			
		// store node in BBF node list
		m_pNodeListBBF->Push(fChildDistanceBB, pNode->m_pRightChild);
		
		// step down left
		NearestNeighborRecursiveBBF(pNode->m_pLeftChild, pQuery,  fDistanceBB);
	} 
	else
	{
		// difference from box to point (was the old distance for this node)
		float fBoxDifference = pQuery[nDimension] - pNode->m_Bounding.fHigh;

		// point inside? -> no difference
		if (fBoxDifference < 0.0f)
			fBoxDifference = 0.0f;
			
		// adjust distance:
		// subtract old distance (fBoxDifference)
		// add new distance (fCutDifference = new lower bound of right child)
		const float fChildDistanceBB = fDistanceBB - fBoxDifference * fBoxDifference + fCutDifference * fCutDifference;
			
		// store node in BBF node list
		m_pNodeListBBF->Push(fChildDistanceBB, pNode->m_pLeftChild);
		
		// step down right
		NearestNeighborRecursiveBBF(pNode->m_pRightChild, pQuery, fDistanceBB);
	}

	// step up (using best node - BBF)
	// take best node from priority queue as next possible node
	float fChildDistanceBB;
	m_pNodeListBBF->Pop(fChildDistanceBB, (void *&) pNode);
	
	// pruefe ob Distanz zwischen Median und Anfrage in der Dimension 
	// des Knotens groesser ist, als die minimale distanz bisher
	if (m_fCurrentMinDistance >= fChildDistanceBB)
		NearestNeighborRecursiveBBF(pNode, pQuery, fChildDistanceBB);
}
