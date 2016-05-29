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
// Filename:  LinearAlgebra.h
// Author:    Pedram Azad
// Date:      23.01.2008
// ****************************************************************************

/** \defgroup MathOperations Mathematic functions and operations */


#ifndef _LINEAR_ALGEBRA_H_
#define _LINEAR_ALGEBRA_H_


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
// LinearAlgebra
// ****************************************************************************

/*!
	\ingroup MathOperations
	\brief Mathematic functions operating on the data types CFloatMatrix, CFloatVector, CDoubleMatrix, and CDoubleVector.
*/
namespace LinearAlgebra
{
	// operating on CFloatMatrix and CFloatVector
	void Zero(CFloatMatrix *pMatrix);
	void Zero(CFloatVector *pVector);
	
	void SelfProduct(const CFloatMatrix *pMatrix, CFloatMatrix *pResultMatrix, bool AAT = false);
	void MulMatMat(const CFloatMatrix *A, const CFloatMatrix *B, CFloatMatrix *pResultMatrix, bool bTransposeB = false);
	void Transpose(const CFloatMatrix *pMatrix, CFloatMatrix *pResultMatrix);

	void MulMatVec(const CFloatMatrix *pMatrix, const CFloatVector *pVector, CFloatVector *pResultVector);
	void MulMatVec(const CFloatMatrix *pMatrix, const CFloatVector *pVector1, const CFloatVector *pVector2, CFloatVector *pResultVector);

	void AddMatMat(const CFloatMatrix *pMatrix1, const CFloatMatrix *pMatrix2, CFloatMatrix *pResultMatrix);
	void SubtractMatMat(const CFloatMatrix *pMatrix1, const CFloatMatrix *pMatrix2, CFloatMatrix *pResultMatrix);
	void AddVecVec(const CFloatVector *pVector1, const CFloatVector *pVector2, CFloatVector *pResultVector);
	void SubtractVecVec(const CFloatVector *pVector1, const CFloatVector *pVector2, CFloatVector *pResultVector);
	
	void AddToMat(CFloatMatrix *pMatrix, const CFloatMatrix *pMatrixToAdd);
	void SubtractFromMat(CFloatMatrix *pMatrix, const CFloatMatrix *pMatrixToSubtract);
	void AddToVec(CFloatVector *pVector, const CFloatVector *pVectorToAdd);
	void SubtractFromVec(CFloatVector *pVector, const CFloatVector *pVectorToSubtract);
	
	void SubtractMeanFromColumns(const CFloatMatrix *pMatrix, CFloatMatrix *pResultMatrix);
	void SubtractMeanFromRows(const CFloatMatrix *pMatrix, CFloatMatrix *pResultMatrix);

	extern void SVD(const CFloatMatrix *A, CFloatMatrix *W, CFloatMatrix *U = 0, CFloatMatrix *V = 0,
		bool bAllowModifyA = false, bool bReturnUTransposed = false, bool bReturnVTransposed = false);

	// linear least squares
	bool SolveLinearLeastSquaresSimple(const CFloatMatrix *A, const CFloatVector *b, CFloatVector *x);
	void SolveLinearLeastSquaresSVD(const CFloatMatrix *A, const CFloatVector *b, CFloatVector *x);
	void SolveLinearLeastSquaresHomogeneousSVD(const CFloatMatrix *A, CFloatVector *x);
		
	// calculation of pseudoinverse
	void CalculatePseudoInverseSVD(const CFloatMatrix *pInputMatrix, CFloatMatrix *pOutputMatrix);
	bool CalculatePseudoInverseSimple(const CFloatMatrix *pInputMatrix, CFloatMatrix *pResultMatrix);

	// calculation of inverse
	bool Invert(const CFloatMatrix *pInputMatrix, CFloatMatrix *pResultMatrix);

	// PCA
	void PCA(const CFloatMatrix *pData, CFloatMatrix *pTransformationMatrix, CFloatMatrix *pTransformedData, int nTargetDimension);
	void PCA(const CFloatMatrix *pData, CFloatMatrix *pTransformationMatrix, CFloatMatrix *pEigenValues);


	// operating on CDoubleMatrix and CDoubleVector
	void Zero(CDoubleMatrix *pMatrix);
	void Zero(CDoubleVector *pVector);
	
	void SelfProduct(const CDoubleMatrix *pMatrix, CDoubleMatrix *pResultMatrix, bool AAT = false);
	void MulMatMat(const CDoubleMatrix *pMatrix1, const CDoubleMatrix *pMatrix2, CDoubleMatrix *pResultMatrix, bool bTransposeB = false);
	void Transpose(const CDoubleMatrix *pMatrix, CDoubleMatrix *pResultMatrix);

	void MulMatVec(const CDoubleMatrix *pMatrix, const CDoubleVector *pVector, CDoubleVector *pResultVector);
	void MulMatVec(const CDoubleMatrix *pMatrix, const CDoubleVector *pVector1, const CDoubleVector *pVector2, CDoubleVector *pResultVector);

	void AddMatMat(const CDoubleMatrix *pMatrix1, const CDoubleMatrix *pMatrix2, CDoubleMatrix *pResultMatrix);
	void SubtractMatMat(const CDoubleMatrix *pMatrix1, const CDoubleMatrix *pMatrix2, CDoubleMatrix *pResultMatrix);
	void AddVecVec(const CDoubleVector *pVector1, const CDoubleVector *pVector2, CDoubleVector *pResultVector);
	void SubtractVecVec(const CDoubleVector *pVector1, const CDoubleVector *pVector2, CDoubleVector *pResultVector);
	
	void AddToMat(CDoubleMatrix *pMatrix, const CDoubleMatrix *pMatrixToAdd);
	void SubtractFromMat(CDoubleMatrix *pMatrix, const CDoubleMatrix *pMatrixToSubtract);
	void AddToVec(CDoubleVector *pVector, const CDoubleVector *pVectorToAdd);
	void SubtractFromVec(CDoubleVector *pVector, const CDoubleVector *pVectorToSubtract);
	
	void SubtractMeanFromColumns(const CDoubleMatrix *pMatrix, CDoubleMatrix *pResultMatrix);
	void SubtractMeanFromRows(const CDoubleMatrix *pMatrix, CDoubleMatrix *pResultMatrix);

	extern void SVD(const CDoubleMatrix *A, CDoubleMatrix *W, CDoubleMatrix *U = 0, CDoubleMatrix *V = 0,
		bool bAllowModifyA = false, bool bReturnUTransposed = false, bool bReturnVTransposed = false);

	// linear least squares
	bool SolveLinearLeastSquaresSimple(const CDoubleMatrix *A, const CDoubleVector *b, CDoubleVector *x);
	void SolveLinearLeastSquaresSVD(const CDoubleMatrix *A, const CDoubleVector *b, CDoubleVector *x);
	void SolveLinearLeastSquaresHomogeneousSVD(const CDoubleMatrix *A, CDoubleVector *x);
		
	// calculation of pseudoinverse
	void CalculatePseudoInverseSVD(const CDoubleMatrix *pInputMatrix, CDoubleMatrix *pOutputMatrix);
	bool CalculatePseudoInverseSimple(const CDoubleMatrix *pInputMatrix, CDoubleMatrix *pResultMatrix);

	// calculation of inverse
	bool Invert(const CDoubleMatrix *pInputMatrix, CDoubleMatrix *pResultMatrix);



	// 2d homographies

	/*!
		\brief Determines an affine transformation based on a set of 2d-2d point correspondences.

		The affine transformation is determined by solving the over-determined system
		\f$
			\left(
				\begin{array}{cccccc}
					u_1 & v_1 & 1 & 0 & 0 & 0 \\
					0 & 0 & 0 & u_1 & v_1 & 1 \\
					\vdots & \vdots & \vdots & \vdots & \vdots & \vdots \\
					u_n & v_n & 1 & 0 & 0 & 0 \\
					0 & 0 & 0 & u_n & v_n & 1 
				\end{array}
			\right)
			\left( \begin{array}{c} a_1 \\ a_2 \\ a_3 \\ a_4 \\ a_5 \\ a_6 \end{array} \right)
			=
			\left( \begin{array}{c} u_1' \\ v_1' \\ \vdots \\ u_n' \\ v_n' \end{array} \right)
		\f$.

		This is solved optimally in the sense of the Euclidean norm by using the method of least squares.

		The n-th entry in pSourcePoints must correspond with the n-th entry in pTargetPoints.

		@param pSourcePoints The source points.
		@param pTargetPoints The target points.
		@param nPoints Number of points.
	*/
	bool DetermineAffineTransformation(const Vec2d *pSourcePoints, const Vec2d *pTargetPoints, int nPoints, Mat3d &A, bool bUseSVD = false);

	/*!
		\brief Determines a homography based on a set of 2d-2d point correspondences.

		The homography is determined by solving the over-determined system
		\f$
			\left(
				\begin{array}{cccccccc}
					u_1 & v_1 & 1 & 0 & 0 & 0 & -u_1 u_1' & -v_1 u_1 ' \\
					0 & 0 & 0 & u_1 & v_1 & 1 & -u_1 v_1' & -v_1 v_1' \\
					\vdots & \vdots & \vdots & \vdots & \vdots & \vdots & \vdots & \vdots \\
					u_n & v_n & 1 & 0 & 0 & 0 & -u_n u_n' & -v_n u_n' \\
					0 & 0 & 0 & u_n & v_n & 1 & -u_n v_n' & -v_n v_n'
				\end{array}
			\right)
			\left( \begin{array}{c} a_1 \\ a_2 \\ a_3 \\ a_4 \\ a_5 \\ a_6 \\ a_7 \\ a_8 \end{array} \right)
			=
			\left( \begin{array}{c} u_1' \\ v_1' \\ \vdots \\ u_n' \\ v_n' \end{array} \right)
		\f$.
		
		This is solved optimally in the sense of the Euclidean norm by using the method of least squares.

		The n-th entry in pSourcePoints must correspond with the n-th entry in pTargetPoints.

		@param pSourcePoints The source points.
		@param pTargetPoints The target points.
		@param nPoints Number of points. Must be at least 4.
	*/
	bool DetermineHomography(const Vec2d *pSourcePoints, const Vec2d *pTargetPoints, int nPoints, Mat3d &A, bool bUseSVD = false);
}



#endif /* _LINEAR_ALGEBRA_H_ */
