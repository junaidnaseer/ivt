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
// Filename:  LinearAlgebraCV.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _LINEAR_ALGEBRA_CV_H_
#define _LINEAR_ALGEBRA_CV_H_


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CFloatMatrix;
class CFloatVector;
class CDoubleMatrix;
class CDoubleVector;
struct Mat3d;
struct Vec2d;



// ****************************************************************************
// LinearAlgebraCV
// ****************************************************************************

/*!
	\namespace LinearAlgebraCV
	\brief Obsolete (encapsulated OpenCV calls). Use LinearAlgebra instead.
*/
namespace LinearAlgebraCV
{
	// operating on CFloatMatrix and CFloatVector

	// singular value decomposition
	void SVD(const CFloatMatrix *A, CFloatMatrix *W, CFloatMatrix *U = 0, CFloatMatrix *V = 0,
		bool bAllowModifyA = false, bool bReturnUTransposed = false, bool bReturnVTransposed = false);

	// linear least squares
	void SolveLinearLeastSquaresSimple(const CFloatMatrix *A, const CFloatVector *b, CFloatVector *x);
	void SolveLinearLeastSquaresSVD(const CFloatMatrix *A, const CFloatVector *b, CFloatVector *x);
	void SolveLinearLeastSquaresHomogeneousSVD(const CFloatMatrix *A, CFloatVector *x);
		
	// calculation of pseudoinverse
	void CalculatePseudoInverseSVD(const CFloatMatrix *pInputMatrix, CFloatMatrix *pOutputMatrix);
	void CalculatePseudoInverseSimple(const CFloatMatrix *pInputMatrix, CFloatMatrix *pResultMatrix);
	
	// basic operations on matrices
	void Multiply(const CFloatMatrix *A, const CFloatMatrix *B, CFloatMatrix *pResultMatrix, bool bTransposeB = false);
	void Invert(const CFloatMatrix *A, const CFloatMatrix *pResultMatrix);
	void Transpose(const CFloatMatrix *A, const CFloatMatrix *pResultMatrix);
	void SelfProduct(const CFloatMatrix *pMatrix, CFloatMatrix *pResultMatrix, bool bTransposeSecond = false);
	
	// 2d homographies
	bool DetermineAffineTransformation(const Vec2d *pSourcePoints, const Vec2d *pTargetPoints, int nPoints, Mat3d &A, bool bUseSVD = false);
	bool DetermineHomography(const Vec2d *pSourcePoints, const Vec2d *pTargetPoints, int nPoints, Mat3d &A, bool bUseSVD = false);
	
	// covariance matrix
	void CalculateCovarianceMatrix(const CFloatMatrix *pMatrix, CFloatMatrix *pCovarianceMatrix);
	
	// PCA
	void PCA(const CFloatMatrix *pData, CFloatMatrix *pTransformationMatrix, CFloatMatrix *pTransformedData, int nTargetDimension);
	void PCA(const CFloatMatrix *pData, CFloatMatrix *pTransformationMatrix, CFloatMatrix *pEigenValues);



	// operating on CDoubleMatrix and CDoubleVector

	// singular value decomposition
	void SVD(const CDoubleMatrix *A, CDoubleMatrix *W, CDoubleMatrix *U = 0, CDoubleMatrix *V = 0,
		bool bAllowModifyA = false, bool bReturnUTransposed = false, bool bReturnVTransposed = false);

	// linear least squares
	void SolveLinearLeastSquaresSimple(const CDoubleMatrix *A, const CDoubleVector *b, CDoubleVector *x);
	void SolveLinearLeastSquaresSVD(const CDoubleMatrix *A, const CDoubleVector *b, CDoubleVector *x);
	void SolveLinearLeastSquaresHomogeneousSVD(const CDoubleMatrix *A, CDoubleVector *x);
		
	// calculation of pseudoinverse
	void CalculatePseudoInverseSVD(const CDoubleMatrix *pInputMatrix, CDoubleMatrix *pOutputMatrix);
	void CalculatePseudoInverseSimple(const CDoubleMatrix *pInputMatrix, CDoubleMatrix *pResultMatrix);
	
	// basic operations on matrices
	void Multiply(const CDoubleMatrix *A, const CDoubleMatrix *B, CDoubleMatrix *pResultMatrix, bool bTransposeB = false);
	void Invert(const CDoubleMatrix *A, const CDoubleMatrix *pResultMatrix);
	void Transpose(const CDoubleMatrix *A, const CDoubleMatrix *pResultMatrix);
}



#endif /* _LINEAR_ALGEBRA_CV_H_ */
