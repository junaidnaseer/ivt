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
// Filename:  KdTree.h
// Author:    Kai Welke
// Date:      14.04.2005
// ****************************************************************************


#ifndef _KD_TREE_H_
#define _KD_TREE_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "KdStructs.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CKdPriorityQueue;


// ****************************************************************************
// CKdTreeNode
// ****************************************************************************

class CKdTreeNode
{
public:
	CKdTreeNode()
	{
		m_pLeftChild = 0; 
		m_pRightChild = 0; 
		m_nCutDimension = 0;
		m_bIsLeaf = 0;
		m_Bounding.fLow = 0;
		m_Bounding.fHigh = 0;
		m_nSize = 0;
		m_pValues = 0;
	}
	
	~CKdTreeNode()
	{
		if (m_pValues)
			delete [] m_pValues;
	}
	

	// public methods
	CKdTreeNode *m_pLeftChild, *m_pRightChild;
    
	float m_fMedianValue;
	
	KdBounding m_Bounding;
	
	int m_bIsLeaf;
	int m_nCutDimension;
		
	int m_nSize;
	float *m_pValues;
};



// ****************************************************************************
// CKdTree
// ****************************************************************************

class CKdTree
{
public:
	// constructor
	CKdTree(int nMaximumNumberOfNodes = 10000);
	
	// destructor
	~CKdTree();
	
	
	// build a kd-tree
	// Use userdata to attach metadata to the
	// values. Only the dimensions of the vectors
	// are used to build tree and search.
	// In the NN results, you get the full vector, with
	// the userdata in the upper elements of the vector.
	// Casting of float values to the appropriate type
	// has to be handled by user.
	void Build(float **ppfValues, int nLow, int nHigh, int nBucketSize, int nDimensions, int nUserDataSize);

	// nearest neighbor only
	void NearestNeighbor(const float *pQuery, float &fError, float*& pfNN, int nMaximumLeavesToVisit = -1);
	void NearestNeighborBBF(const float *pfQuery, float &fError, float *&pfNN, int nMaximumLeavesToVisit = -1); // with best bin first strategy
	
	// nearest neighbor with list of best elements
	void NearestNeighbor(const float *pQuery, float &fError, CKdPriorityQueue *&pNNList, int nMaximumLeavesToVisit = -1);
	void NearestNeighborBBF(const float *pQuery, float &fError, CKdPriorityQueue *&pNNList, int nMaximumLeavesToVisit = -1); // with best bin first strategy
	

private:
	// private methods
	void Dispose();
	CKdTreeNode* BuildRecursive(float** ppfValues,int nLow, int nHigh, KdBoundingBox* pCurrentBoundingBox, int nCurrentDepth);
	void DisposeRecursive(CKdTreeNode *pNode);
	void NearestNeighborRecursive(CKdTreeNode* pNode, const float *pQuery);
	void NearestNeighborRecursiveBBF(CKdTreeNode* pNode, const float *pQuery, float fDistanceBB);
	
	
	// root node
	CKdTreeNode *m_pRoot;
	// maximum bucket size in tree
	int m_nBucketSize;
	// number of dimensions of vectors
	int m_nDimensions;
	// size of userdata in vectors
	int	m_nUserDataSize;
	// m_nDimensions + m_nUserDataSize
	int m_nTotalVectorSize;
	// number of leaves
	int m_nLeaves;
	// bounding box of whole tree (calculated in build process)
	KdBoundingBox m_EnclosingBox;
	
	// used for query recursion
	float m_fCurrentMinDistance;
	float *m_pNearestNeighbor;
	// used when a maximum number of nodes to visit is provided
	int m_nMaximumLeavesToVisit;
	// queue to enable second closest element
	CKdPriorityQueue *m_pNearestNeighborList;
	CKdPriorityQueue *m_pNearestNeighborList_;
	// needed for BBF
	CKdPriorityQueue *m_pNodeListBBF;
};



#endif // _KD_TREE_H_
