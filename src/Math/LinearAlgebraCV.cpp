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
// Filename:  LinearAlgebraCV.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "LinearAlgebraCV.h"
#include "LinearAlgebra.h"
#include "Math/FloatMatrix.h"
#include "Math/FloatVector.h"
#include "Math/DoubleMatrix.h"
#include "Math/DoubleVector.h"
#include "Math/Math2d.h"
#include "Math/Math3d.h"
#include "Helpers/helpers.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <cv.h>



// ****************************************************************************
// Functions
// ****************************************************************************

void LinearAlgebraCV::SolveLinearLeastSquaresHomogeneousSVD(const CFloatMatrix *A, CFloatVector *x)
{
	if (A->columns != x->dimension)
	{
		printf("error: A, b, x do not match LinearAlgebraCV::SolveLinearLeastSquaresHomogeneousSVD");
		return;
	}
	
	if (A->rows < A->columns)
	{
		printf("error: equation system is underdetermined in LinearAlgebraCV::SolveLinearLeastSquaresHomogeneousSVD\n");
		return;
	}
	
	const int m = A->rows;
	const int n = A->columns;

	CFloatMatrix W(n, m), V(n, n);
	
	SVD(A, &W, 0, &V, false, false, false);
	
	for (int i = 0, offset = n - 1; i < n; i++, offset += n)
		x->data[i] = V.data[offset];
}

void LinearAlgebraCV::SolveLinearLeastSquaresSVD(const CFloatMatrix *A, const CFloatVector *b, CFloatVector *x)
{
	if (A->columns != x->dimension || A->rows != b->dimension)
	{
		printf("error: A, b, x do not match LinearAlgebraCV::SolveLinearLeastSquaresSVD");
		return;
	}
	
	if (A->rows < A->columns)
	{
		printf("error: equation system is underdetermined in LinearAlgebraCV::SolveLinearLeastSquaresSVD\n");
		return;
	}

	CFloatMatrix A_(A->rows, A->columns);
	CalculatePseudoInverseSVD(A, &A_);
	LinearAlgebra::MulMatVec(&A_, b, x);
}

void LinearAlgebraCV::SolveLinearLeastSquaresSimple(const CFloatMatrix *A, const CFloatVector *b, CFloatVector *x)
{
	if (A->columns != x->dimension || A->rows != b->dimension)
	{
		printf("error: A, b, x do not match LinearAlgebraCV::SolveLinearLeastSquaresSimple");
		return;
	}
	
	if (A->rows < A->columns)
	{
		printf("error: equation system is underdetermined in LinearAlgebraCV::SolveLinearLeastSquaresSimple\n");
		return;
	}

	CFloatMatrix A_(A->rows, A->columns);
	CalculatePseudoInverseSimple(A, &A_);
	LinearAlgebra::MulMatVec(&A_, b, x);
}

void LinearAlgebraCV::CalculatePseudoInverseSVD(const CFloatMatrix *pInputMatrix, CFloatMatrix *pResultMatrix)
{
	if (pInputMatrix->columns != pResultMatrix->rows || pInputMatrix->rows != pResultMatrix->columns)
	{
		printf("error: input and output matrix do not match LinearAlgebraCV::CalculatePseudoInverseSVD");
		return;
	}
	
	// algorithm:
	// 1: compute U * W * V^T = A
	// 2: compute W' = W^T with all non-zero values inverted
	// 3: compute V * W' * U^T (=pseudoinverse of A)
	
	const CFloatMatrix *A = pInputMatrix;
	const int m = pInputMatrix->rows;
	const int n = pInputMatrix->columns;
	
	CFloatMatrix W(n, m), WT(m, n), UT(m, m), V(n, n);
	
	// calculate SVD
	SVD(A, &W, &UT, &V, false, true, false);
	
	// transpose middle diagonal matrix
	Transpose(&W, &WT);
	
	const int min = MY_MIN(m, n);

	int i;
	float fThreshold = 0.0f;

	for (i = 0; i < min; i++)
		fThreshold += WT(i, i);
	
	fThreshold *= 2 * FLT_EPSILON;
	
	// invert non-zero values (along diagonal)
	for (i = 0; i < min; i++)
		if (WT(i, i) < fThreshold)
			WT(i, i) = 0.0f;
		else
			WT(i, i) = 1.0f / WT(i, i);

	// calculate pseudoinverse
	CFloatMatrix temp(m, n);
	Multiply(&V, &WT, &temp);
	Multiply(&temp, &UT, pResultMatrix);
}

void LinearAlgebraCV::CalculatePseudoInverseSimple(const CFloatMatrix *pInputMatrix, CFloatMatrix *pResultMatrix)
{
	if (pInputMatrix->columns != pResultMatrix->rows || pInputMatrix->rows != pResultMatrix->columns)
	{
		printf("error: input and output matrix do not match LinearAlgebraCV::CalculatePseudoInverseSimple");
		return;
	}
	
	// algorithm:
	// compute (A * A^T)^-1 * A^T
	
	const CFloatMatrix *A = pInputMatrix;
	const int m = pInputMatrix->rows;
	const int n = pInputMatrix->columns;
	
	CFloatMatrix AT(m, n), ATA(n, n), ATA_inverted(n, n);
	
	Transpose(A, &AT);
	Multiply(&AT, A, &ATA);
	Invert(&ATA, &ATA_inverted);
	Multiply(&ATA_inverted, &AT, pResultMatrix);
}

void LinearAlgebraCV::Invert(const CFloatMatrix *pInputMatrix, const CFloatMatrix *pResultMatrix)
{
	if (pInputMatrix->columns != pInputMatrix->rows)
	{
		printf("error: input is not square matrix in LinearAlgebraCV::Invert");
		return;
	}
	
	if (pInputMatrix->columns != pResultMatrix->columns || pInputMatrix->rows != pResultMatrix->rows)
	{
		printf("error: input and output matrix are not the same in LinearAlgebraCV::Invert");
		return;
	}
	
	CvMat inputMatrix = cvMat(pInputMatrix->rows, pInputMatrix->columns, CV_32FC1, pInputMatrix->data);
	CvMat resultMatrix = cvMat(pResultMatrix->rows, pResultMatrix->columns, CV_32FC1, pResultMatrix->data);
	
	cvInvert(&inputMatrix, &resultMatrix);
}

void LinearAlgebraCV::Transpose(const CFloatMatrix *pInputMatrix, const CFloatMatrix *pResultMatrix)
{
	if (pInputMatrix->columns != pResultMatrix->rows || pInputMatrix->rows != pResultMatrix->columns)
	{
		printf("error: input and output matrix do not match LinearAlgebraCV::Transpose");
		return;
	}
	
	CvMat inputMatrix = cvMat(pInputMatrix->rows, pInputMatrix->columns, CV_32FC1, pInputMatrix->data);
	CvMat resultMatrix = cvMat(pResultMatrix->rows, pResultMatrix->columns, CV_32FC1, pResultMatrix->data);
	
	cvTranspose(&inputMatrix, &resultMatrix);
}

void LinearAlgebraCV::CalculateCovarianceMatrix(const CFloatMatrix *pMatrix, CFloatMatrix *pCovarianceMatrix)
{
	if (pCovarianceMatrix->columns != pMatrix->columns || pCovarianceMatrix->rows != pMatrix->columns)
		return;

	const int columns = pMatrix->columns;
	const int rows = pMatrix->rows;
	
	CvMat covarianceMatrix = cvMat(columns, columns, CV_32FC1, pCovarianceMatrix->data);
	
	CvMat **ppInput = new CvMat*[rows];
	for (int i = 0; i < rows; i++)
	{
		CvMat *vector = cvCreateMatHeader(1, columns, CV_32FC1);
		cvInitMatHeader(vector, 1, columns, CV_32FC1, pMatrix->data + i * columns);
		ppInput[i] = vector;
	}

	CvMat *avg = cvCreateMat(1, columns, CV_32FC1);

	#ifdef CV_COVAR_NORMAL
	cvCalcCovarMatrix((const CvArr **) ppInput, rows, &covarianceMatrix, avg, CV_COVAR_NORMAL);
	#else
	cvCalcCovarMatrix((const CvArr **) ppInput, &covarianceMatrix, avg);
	#endif

	cvReleaseMat(&avg);
}

void LinearAlgebraCV::Multiply(const CFloatMatrix *A, const CFloatMatrix *B, CFloatMatrix *pResultMatrix, bool bTransposeB)
{
	if (!bTransposeB && (A->columns != B->rows || pResultMatrix->columns != B->columns || pResultMatrix->rows != A->rows))
	{
		printf("error: matrices A, B, and pResultMatrix do not satisfy requirements for LinearAlgebraCV::Multiply\n");
		return;
	}

	if (bTransposeB && (A->columns != B->columns || pResultMatrix->columns != B->rows || pResultMatrix->rows != A->rows))
	{
		printf("error: matrices A, B, and pResultMatrix do not satisfy requirements for LinearAlgebraCV::Multiply\n");
		return;
	}

	int flags = 0;

	if (bTransposeB)
		flags =  CV_GEMM_B_T;

	CvMat matrixA = cvMat(A->rows, A->columns, CV_32FC1, A->data);
	CvMat matrixB = cvMat(B->rows, B->columns, CV_32FC1, B->data);
	CvMat result_matrix = cvMat(pResultMatrix->rows, pResultMatrix->columns, CV_32FC1, pResultMatrix->data);

	cvGEMM(&matrixA, &matrixB, 1, 0, 1, &result_matrix, flags);
}

void LinearAlgebraCV::SelfProduct(const CFloatMatrix *pMatrix, CFloatMatrix *pResultMatrix, bool bTransposeSecond)
{
	if (pResultMatrix->columns != pMatrix->columns || pResultMatrix->rows != pMatrix->columns)
		return;

	CvMat matrix = cvMat(pMatrix->rows, pMatrix->columns, CV_32FC1, pMatrix->data);
	CvMat result_matrix = cvMat(pResultMatrix->rows, pResultMatrix->columns, CV_32FC1, pResultMatrix->data);

	if (bTransposeSecond)
        cvGEMM(&matrix, &matrix, 1, 0, 1, &result_matrix, CV_GEMM_B_T);
	else
		cvGEMM(&matrix, &matrix, 1, 0, 1, &result_matrix, CV_GEMM_A_T);
}

void LinearAlgebraCV::SVD(const CFloatMatrix *A, CFloatMatrix *W, CFloatMatrix *U, CFloatMatrix *V, bool bAllowModifyA, bool bReturnUTransposed, bool bReturnVTransposed)
{
	const int columns = A->columns;
	const int rows = A->rows;

	if (W->columns != columns || W->rows != rows)
	{
		printf("error: W should have %i columns and %i rows for LinearAlgebra::SVD\n", columns, rows);
		return;
	}

	if (U && (U->columns != rows || U->rows != rows))
	{
		printf("error: U should have %i columns and %i rows for LinearAlgebra::SVD\n", rows, rows);
		return;
	}

	if (V && (V->columns != columns || V->rows != columns))
	{
		printf("error: V should have %i columns and %i rows for LinearAlgebra::SVD\n", columns, columns);
		return;
	}

	int flags = 0;

	if (bAllowModifyA)
		flags |= CV_SVD_MODIFY_A;

	if (bReturnUTransposed)
		flags |= CV_SVD_U_T;

	if (bReturnVTransposed)
		flags |= CV_SVD_V_T;

	CvMat matrixA = cvMat(rows, columns, CV_32FC1, A->data);
	CvMat matrixW = cvMat(rows, columns, CV_32FC1, W->data);

	if (U && V)
	{
		CvMat matrixU = cvMat(rows, rows, CV_32FC1, U->data);
		CvMat matrixV = cvMat(columns, columns, CV_32FC1, V->data);

		cvSVD(&matrixA, &matrixW, &matrixU, &matrixV, flags);
	}
	else if (U)
	{
		CvMat matrixU = cvMat(rows, rows, CV_32FC1, U->data);

		cvSVD(&matrixA, &matrixW, &matrixU, 0, flags);
	}
	else if (V)
	{
		CvMat matrixV = cvMat(columns, columns, CV_32FC1, V->data);

		cvSVD(&matrixA, &matrixW, 0, &matrixV, flags);
	}
	else
	{
		cvSVD(&matrixA, &matrixW, 0, 0, flags);
	}
}

void LinearAlgebraCV::PCA(const CFloatMatrix *pData, CFloatMatrix *pTransformationMatrix, CFloatMatrix *pTransformedData, int nTargetDimension)
{
	if (nTargetDimension > pData->columns)
	{
		printf("error: target dimension is greater than number of columns in training data matrix in LinearAlgebraCV::PCA\n");
		return;
	}

	const int samples = pData->rows;
	const int dimension = pData->columns;

	if (pTransformationMatrix->columns != dimension || pTransformationMatrix->rows != nTargetDimension ||
		pTransformedData->columns != samples || pTransformedData->rows != nTargetDimension)
	{
		printf("error: input to LinearAlgebraCV::PCA does not match\n");
		return;
	}

	CFloatMatrix adjustedData(pData);
	CFloatMatrix covarianceMatrix(dimension, dimension);
	CFloatMatrix eigenValues(dimension, dimension);
	CFloatMatrix eigenVectors(dimension, dimension);
	
	printf("subtracting mean from columns...\n");
	LinearAlgebra::SubtractMeanFromColumns(pData, &adjustedData);
	
	printf("calculating covariance matrix...\n");
	LinearAlgebraCV::SelfProduct(&adjustedData, &covarianceMatrix);
	
	printf("calculating SVD on %ix%i matrix...\n", dimension, dimension);
	LinearAlgebraCV::SVD(&covarianceMatrix, &eigenValues, &eigenVectors, 0, true, true);
	
	printf("SVD calculated\n");

	for (int i = 0, offset = 0; i < nTargetDimension; i++)
	{
		for (int j = 0; j < dimension; j++)
		{
			pTransformationMatrix->data[offset] = eigenVectors.data[i * dimension + j];
			offset++;
		}
	}

	LinearAlgebraCV::Multiply(pTransformationMatrix, pData, pTransformedData, true);
}

void LinearAlgebraCV::PCA(const CFloatMatrix *pData, CFloatMatrix *pTransformationMatrix, CFloatMatrix *pEigenValues)
{
	const int samples = pData->rows;
	const int dimension = pData->columns;

	if (pTransformationMatrix->columns != dimension || pTransformationMatrix->rows != dimension || pEigenValues->columns != 1 || pEigenValues->rows != dimension)
		return;

	CFloatMatrix adjustedData(pData);
	CFloatMatrix covarianceMatrix(dimension, dimension);
	CFloatMatrix eigenValues(dimension, dimension);
	CFloatMatrix eigenVectors(dimension, dimension);
	
	printf("subtracting mean from columns...\n");
	LinearAlgebra::SubtractMeanFromColumns(pData, &adjustedData);
	
	printf("calculating covariance matrix...\n");
	LinearAlgebraCV::SelfProduct(&adjustedData, &covarianceMatrix);
	
	printf("calculating SVD on %ix%i matrix...\n", dimension, dimension);
	LinearAlgebraCV::SVD(&covarianceMatrix, &eigenValues, pTransformationMatrix, 0, true, true);
	
	printf("SVD calculated\n");
	
	for (int i = 0; i < dimension; i++)
		pEigenValues->data[i] = eigenValues.data[i * dimension + i];
}

bool LinearAlgebraCV::DetermineAffineTransformation(const Vec2d *pSourcePoints, const Vec2d *pTargetPoints, int nPoints, Mat3d &A, bool bUseSVD)
{
	if (nPoints < 3)
	{
		printf("error: not enough input point pairs for LinearAlgebraCV::DetermineAffineTransformation (must be at least 3)\n");
		return false;
	}
	
	CFloatMatrix M(6, 2 * nPoints);
	CFloatVector b(2 * nPoints);
	
	float *data = M.data;
	
	for (int i = 0, offset = 0; i < nPoints; i++, offset += 12)
	{
		data[offset] = pSourcePoints[i].x;
		data[offset + 1] = pSourcePoints[i].y;
		data[offset + 2] = 1;
		data[offset + 3] = 0;
		data[offset + 4] = 0;
		data[offset + 5] = 0;
		
		data[offset + 6] = 0;
		data[offset + 7] = 0;
		data[offset + 8] = 0;
		data[offset + 9] = pSourcePoints[i].x;
		data[offset + 10] = pSourcePoints[i].y;
		data[offset + 11] = 1;

		const int index = 2 * i;
		b.data[index] = pTargetPoints[i].x;
		b.data[index + 1] = pTargetPoints[i].y;
	}
	
	CFloatVector x(6);
	
	if (bUseSVD)
		LinearAlgebraCV::SolveLinearLeastSquaresSVD(&M, &b, &x);
	else
		LinearAlgebraCV::SolveLinearLeastSquaresSimple(&M, &b, &x);
	
	Math3d::SetMat(A, x.data[0], x.data[1], x.data[2], x.data[3], x.data[4], x.data[5], 0, 0, 1);
	
	return true;
}

bool LinearAlgebraCV::DetermineHomography(const Vec2d *pSourcePoints, const Vec2d *pTargetPoints, int nPoints, Mat3d &A, bool bUseSVD)
{
	if (nPoints < 4)
	{
		printf("error: not enough input point pairs for LinearAlgebraCV::DetermineHomography (must be at least 4)\n");
		return false;
	}

	// this least squares problem becomes numerically instable when
	// using float instead of double!!
	CDoubleMatrix M(8, 2 * nPoints);
	CDoubleVector b(2 * nPoints);
	
	double *data = M.data;
	
	for (int i = 0, offset = 0; i < nPoints; i++, offset += 16)
	{
		data[offset] = pSourcePoints[i].x;
		data[offset + 1] = pSourcePoints[i].y;
		data[offset + 2] = 1;
		data[offset + 3] = 0;
		data[offset + 4] = 0;
		data[offset + 5] = 0;
		data[offset + 6] = -pSourcePoints[i].x * pTargetPoints[i].x;
		data[offset + 7] = -pSourcePoints[i].y * pTargetPoints[i].x;
		
		data[offset + 8] = 0;
		data[offset + 9] = 0;
		data[offset + 10] = 0;
		data[offset + 11] = pSourcePoints[i].x;
		data[offset + 12] = pSourcePoints[i].y;
		data[offset + 13] = 1;
		data[offset + 14] = -pSourcePoints[i].x * pTargetPoints[i].y;
		data[offset + 15] = -pSourcePoints[i].y * pTargetPoints[i].y;

		const int index = 2 * i;
		b.data[index] = pTargetPoints[i].x;
		b.data[index + 1] = pTargetPoints[i].y;
	}
	
	CDoubleVector x(8);
	
	if (bUseSVD)
		LinearAlgebraCV::SolveLinearLeastSquaresSVD(&M, &b, &x);
	else
		LinearAlgebraCV::SolveLinearLeastSquaresSimple(&M, &b, &x);
	
	Math3d::SetMat(A, float(x.data[0]), float(x.data[1]), float(x.data[2]), float(x.data[3]), float(x.data[4]), float(x.data[5]), float(x.data[6]), float(x.data[7]), 1);
	
	return true;
}





// copy & paste implementation for CDoubleMatrix and CDoubleVector
// not nice, but i don't like templates.

void LinearAlgebraCV::SolveLinearLeastSquaresHomogeneousSVD(const CDoubleMatrix *A, CDoubleVector *x)
{
	if (A->columns != x->dimension)
	{
		printf("error: A, b, x do not match LinearAlgebraCV::SolveLinearLeastSquaresHomogeneousSVD");
		return;
	}
	
	if (A->rows < A->columns)
	{
		printf("error: equation system is underdetermined in LinearAlgebraCV::SolveLinearLeastSquaresHomogeneousSVD\n");
		return;
	}
	
	const int m = A->rows;
	const int n = A->columns;

	CDoubleMatrix W(n, m), V(n, n);
	
	SVD(A, &W, 0, &V, false, false, false);
	
	for (int i = 0, offset = n - 1; i < n; i++, offset += n)
		x->data[i] = V.data[offset];
}

void LinearAlgebraCV::SolveLinearLeastSquaresSVD(const CDoubleMatrix *A, const CDoubleVector *b, CDoubleVector *x)
{
	if (A->columns != x->dimension || A->rows != b->dimension)
	{
		printf("error: A, b, x do not match LinearAlgebraCV::SolveLinearLeastSquaresSVD");
		return;
	}
	
	if (A->rows < A->columns)
	{
		printf("error: equation system is underdetermined in LinearAlgebraCV::SolveLinearLeastSquaresSVD\n");
		return;
	}

#if 0
	CvMat *AA = cvCreateMat(A->rows, A->columns, CV_64FC1);
    CvMat *BB = cvCreateMat(b->dimension, 1, CV_64FC1);
    CvMat *XX = cvCreateMat(x->dimension, 1, CV_64FC1);

	int i;

	for (i = 0; i < A->rows * A->columns; i++)
		AA->data.db[i] = A->data[i];

	for (i = 0; i < b->dimension; i++)
		BB->data.db[i] = b->data[i];

	cvSolve(AA, BB, XX, CV_SVD);
	
	for (int k = 0; k < 8; k++)
		x->data[k] = XX->data.db[k];
	
	cvReleaseMat(&AA);
	cvReleaseMat(&BB);
	cvReleaseMat(&XX);
#else
	CDoubleMatrix A_(A->rows, A->columns);
	CalculatePseudoInverseSVD(A, &A_);
	LinearAlgebra::MulMatVec(&A_, b, x);
#endif
}

void LinearAlgebraCV::SolveLinearLeastSquaresSimple(const CDoubleMatrix *A, const CDoubleVector *b, CDoubleVector *x)
{
	if (A->columns != x->dimension || A->rows != b->dimension)
	{
		printf("error: A, b, x do not match LinearAlgebraCV::SolveLinearLeastSquaresSimple");
		return;
	}
	
	if (A->rows < A->columns)
	{
		printf("error: equation system is underdetermined in LinearAlgebraCV::SolveLinearLeastSquaresSimple\n");
		return;
	}

	CDoubleMatrix A_(A->rows, A->columns);
	CalculatePseudoInverseSimple(A, &A_);
	LinearAlgebra::MulMatVec(&A_, b, x);
}

void LinearAlgebraCV::CalculatePseudoInverseSVD(const CDoubleMatrix *pInputMatrix, CDoubleMatrix *pResultMatrix)
{
	if (pInputMatrix->columns != pResultMatrix->rows || pInputMatrix->rows != pResultMatrix->columns)
	{
		printf("error: input and output matrix do not match LinearAlgebraCV::CalculatePseudoInverseSVD");
		return;
	}
	
	// algorithm:
	// 1: compute U * W * V^T = A
	// 2: compute W' = W^T with all non-zero values inverted
	// 3: compute V * W' * U^T (=pseudoinverse of A)
	
	const CDoubleMatrix *A = pInputMatrix;
	const int m = pInputMatrix->rows;
	const int n = pInputMatrix->columns;
	
	CDoubleMatrix W(n, m), WT(m, n), UT(m, m), V(n, n);
	
	// calculate SVD
	SVD(A, &W, &UT, &V, false, true, false);
	
	// transpose middle diagonal matrix
	Transpose(&W, &WT);
	
	const int min = MY_MIN(m, n);

	int i;
	double dThreshold = 0.0;

	for(i = 0; i < min; i++)
        dThreshold += WT(i, i);
    
	dThreshold *= 2 * DBL_EPSILON;
	
	// invert non-zero values (along diagonal)
	for (i = 0; i < min; i++)
		if (WT(i, i) < dThreshold)
			WT(i, i) = 0;
		else
			WT(i, i) = 1.0 / WT(i, i);

	// calculate pseudoinverse
	CDoubleMatrix temp(m, n);
	Multiply(&V, &WT, &temp);
	Multiply(&temp, &UT, pResultMatrix);
}

void LinearAlgebraCV::CalculatePseudoInverseSimple(const CDoubleMatrix *pInputMatrix, CDoubleMatrix *pResultMatrix)
{
	if (pInputMatrix->columns != pResultMatrix->rows || pInputMatrix->rows != pResultMatrix->columns)
	{
		printf("error: input and output matrix do not match LinearAlgebraCV::CalculatePseudoInverseSimple");
		return;
	}
	
	// algorithm:
	// compute (A * A^T)^-1 * A^T
	
	const CDoubleMatrix *A = pInputMatrix;
	const int m = pInputMatrix->rows;
	const int n = pInputMatrix->columns;
	
	CDoubleMatrix AT(m, n), ATA(n, n), ATA_inverted(n, n);
	
	Transpose(A, &AT);
	Multiply(&AT, A, &ATA);
	Invert(&ATA, &ATA_inverted);
	Multiply(&ATA_inverted, &AT, pResultMatrix);
}

void LinearAlgebraCV::Invert(const CDoubleMatrix *pInputMatrix, const CDoubleMatrix *pResultMatrix)
{
	if (pInputMatrix->columns != pInputMatrix->rows)
	{
		printf("error: input is not square matrix in LinearAlgebraCV::Invert");
		return;
	}
	
	if (pInputMatrix->columns != pResultMatrix->columns || pInputMatrix->rows != pResultMatrix->rows)
	{
		printf("error: input and output matrix are not the same in LinearAlgebraCV::Invert");
		return;
	}
	
	CvMat inputMatrix = cvMat(pInputMatrix->rows, pInputMatrix->columns, CV_64FC1, pInputMatrix->data);
	CvMat resultMatrix = cvMat(pResultMatrix->rows, pResultMatrix->columns, CV_64FC1, pResultMatrix->data);
	
	cvInvert(&inputMatrix, &resultMatrix);
}

void LinearAlgebraCV::Transpose(const CDoubleMatrix *pInputMatrix, const CDoubleMatrix *pResultMatrix)
{
	if (pInputMatrix->columns != pResultMatrix->rows || pInputMatrix->rows != pResultMatrix->columns)
	{
		printf("error: input and output matrix do not match LinearAlgebraCV::Transpose");
		return;
	}
	
	CvMat inputMatrix = cvMat(pInputMatrix->rows, pInputMatrix->columns, CV_64FC1, pInputMatrix->data);
	CvMat resultMatrix = cvMat(pResultMatrix->rows, pResultMatrix->columns, CV_64FC1, pResultMatrix->data);
	
	cvTranspose(&inputMatrix, &resultMatrix);
}

void LinearAlgebraCV::Multiply(const CDoubleMatrix *A, const CDoubleMatrix *B, CDoubleMatrix *pResultMatrix, bool bTransposeB)
{
	if (!bTransposeB && (A->columns != B->rows || pResultMatrix->columns != B->columns || pResultMatrix->rows != A->rows))
	{
		printf("error: matrices A, B, and pResultMatrix do not satisfy requirements for LinearAlgebraCV::Multiply\n");
		return;
	}

	if (bTransposeB && (A->columns != B->columns || pResultMatrix->columns != B->rows || pResultMatrix->rows != A->rows))
	{
		printf("error: matrices A, B, and pResultMatrix do not satisfy requirements for LinearAlgebraCV::Multiply\n");
		return;
	}

	int flags = 0;

	if (bTransposeB)
		flags =  CV_GEMM_B_T;

	CvMat matrixA = cvMat(A->rows, A->columns, CV_64FC1, A->data);
	CvMat matrixB = cvMat(B->rows, B->columns, CV_64FC1, B->data);
	CvMat result_matrix = cvMat(pResultMatrix->rows, pResultMatrix->columns, CV_64FC1, pResultMatrix->data);

	cvGEMM(&matrixA, &matrixB, 1, 0, 1, &result_matrix, flags);
}

void LinearAlgebraCV::SVD(const CDoubleMatrix *A, CDoubleMatrix *W, CDoubleMatrix *U, CDoubleMatrix *V, bool bAllowModifyA, bool bReturnUTransposed, bool bReturnVTransposed)
{
	const int columns = A->columns;
	const int rows = A->rows;

	if (W->columns != columns || W->rows != rows)
	{
		printf("error: W should have %i columns and %i rows for LinearAlgebra::SVD\n", columns, rows);
		return;
	}

	if (U && (U->columns != rows || U->rows != rows))
	{
		printf("error: U should have %i columns and %i rows for LinearAlgebra::SVD\n", rows, rows);
		return;
	}

	if (V && (V->columns != columns || V->rows != columns))
	{
		printf("error: V should have %i columns and %i rows for LinearAlgebra::SVD\n", columns, columns);
		return;
	}

	int flags = 0;

	if (bAllowModifyA)
		flags |= CV_SVD_MODIFY_A;

	if (bReturnUTransposed)
		flags |= CV_SVD_U_T;

	if (bReturnVTransposed)
		flags |= CV_SVD_V_T;

	CvMat matrixA = cvMat(rows, columns, CV_64FC1, A->data);
	CvMat matrixW = cvMat(rows, columns, CV_64FC1, W->data);

	if (U && V)
	{
		CvMat matrixU = cvMat(rows, rows, CV_64FC1, U->data);
		CvMat matrixV = cvMat(columns, columns, CV_64FC1, V->data);

		cvSVD(&matrixA, &matrixW, &matrixU, &matrixV, flags);
	}
	else if (U)
	{
		CvMat matrixU = cvMat(rows, rows, CV_64FC1, U->data);

		cvSVD(&matrixA, &matrixW, &matrixU, 0, flags);
	}
	else if (V)
	{
		CvMat matrixV = cvMat(columns, columns, CV_64FC1, V->data);

		cvSVD(&matrixA, &matrixW, 0, &matrixV, flags);
	}
	else
	{
		cvSVD(&matrixA, &matrixW, 0, 0, flags);
	}
}
