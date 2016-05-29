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
// Filename:  LinearAlgebra.cpp
// Author:    Pedram Azad
// Date:      23.01.2008
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "LinearAlgebra.h"
#include "Image/ImageProcessor.h"
#include "Helpers/Quicksort.h"
#include "Helpers/helpers.h"
#include "Math/FloatMatrix.h"
#include "Math/FloatVector.h"
#include "Math/DoubleMatrix.h"
#include "Math/DoubleVector.h"

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>



// ****************************************************************************
// Functions
// ****************************************************************************

void LinearAlgebra::SelfProduct(const CFloatMatrix *pMatrix, CFloatMatrix *pResultMatrix, bool AAT)
{
	if (
		(AAT && (pResultMatrix->columns != pMatrix->rows || pResultMatrix->rows != pMatrix->rows)) ||
		(!AAT && (pResultMatrix->columns != pMatrix->columns || pResultMatrix->rows != pMatrix->columns))
	)
	{
		printf("error: matrix dimensions do not match for LinearAlgebra::SelfProduct\n");
		return;
	}

	if (!AAT)
	{
		CFloatMatrix *pTransposedMatrix = new CFloatMatrix(pMatrix->rows, pMatrix->columns);
		Transpose(pMatrix, pTransposedMatrix);
		pMatrix = pTransposedMatrix;
	}

	const int columns = pMatrix->columns;
	const int rows = pMatrix->rows;

	const float *data = pMatrix->data;
	float *result = pResultMatrix->data;

	if (data != result)
	{
		for (int i = 0, input_offset1 = 0; i < rows; i++, input_offset1 += columns)
		{
			for (int j = i, input_offset2 = input_offset1; j < rows; j++, input_offset2 += columns)
			{
				const float *data1 = data + input_offset1;
				const float *data2 = data + input_offset2;
				float sum = 0;

				for (int k = 0; k < columns; k++)
					sum += data1[k] * data2[k];

				result[i * rows + j] = result[j * rows + i] = sum;
			}
		}
	}
	else
	{
		CFloatMatrix temp(pResultMatrix);
		float *temp_data = temp.data;

		for (int i = 0, input_offset1 = 0; i < rows; i++, input_offset1 += columns)
		{
			for (int j = i, input_offset2 = input_offset1; j < rows; j++, input_offset2 += columns)
			{
				const float *data1 = data + input_offset1;
				const float *data2 = data + input_offset2;
				float sum = 0;

				for (int k = 0; k < columns; k++)
					sum += data1[k] * data2[k];

				temp_data[i * rows + j] = temp_data[j * rows + i] = sum;
			}
		}

		memcpy(pResultMatrix->data, temp_data, pResultMatrix->rows * pResultMatrix->columns * sizeof(float));
	}

	if (!AAT)
		delete pMatrix;
}

void LinearAlgebra::MulMatMat(const CFloatMatrix *A, const CFloatMatrix *B, CFloatMatrix *pResultMatrix, bool bTransposeB)
{
	if (!bTransposeB && (A->columns != B->rows || pResultMatrix->columns != B->columns || pResultMatrix->rows != A->rows))
	{
		printf("error: matrices A, B, and pResultMatrix do not satisfy requirements for LinearAlgebra::Multiply\n");
		return;
	}

	if (bTransposeB && (A->columns != B->columns || pResultMatrix->columns != B->rows || pResultMatrix->rows != A->rows))
	{
		printf("error: matrices A, B, and pResultMatrix do not satisfy requirements for LinearAlgebra::Multiply\n");
		return;
	}

	const float *data1 = A->data;
	const float *data2 = B->data;
	float *result = pResultMatrix->data;

	const int nSize = pResultMatrix->columns * pResultMatrix->rows;

	if (bTransposeB)
	{
		const int rowsA = A->rows;
		const int rowsB = B->rows;
		const int columns = A->columns;

		if (data1 != result && data2 != result)
		{
			for (int i = 0, input_offset1 = 0, output_offset = 0; i < rowsA; i++, input_offset1 += columns)
			{
				for (int j = 0, input_offset2 = 0; j < rowsB; j++, input_offset2 += columns, output_offset++)
				{
					const float *data1_ = data1 + input_offset1;
					const float *data2_ = data2 + input_offset2;
					float sum = 0;

					for (int k = 0; k < columns; k++)
						sum += data1_[k] * data2_[k];

					result[output_offset] = sum;
				}
			}
		}
		else
		{
			CFloatMatrix temp(pResultMatrix);
			float *temp_data = temp.data;

			int l;

			for (l = 0; l < nSize; l++)
				temp_data[l] = 0;

			for (int i = 0, input_offset1 = 0, output_offset = 0; i < rowsA; i++, input_offset1 += columns)
			{
				for (int j = 0, input_offset2 = input_offset1; j < rowsB; j++, input_offset2 += columns, output_offset++)
				{
					const float *data1_ = data1 + input_offset1;
					const float *data2_ = data2 + input_offset2;
					float sum = 0;

					for (int k = 0; k < columns; k++)
						sum += data1_[k] * data2_[k];

					temp_data[output_offset] = sum;
				}
			}

			for (l = 0; l < nSize; l++)
				result[l] = temp_data[l];
		}
	}
	else
	{
		const int i_max = A->rows;
		const int j_max = B->columns;
		const int k_max = A->columns;

		// not optimized yet
		if (data1 != result && data2 != result)
		{
			for (int l = 0; l < nSize; l++)
				result[l] = 0;

			for (int i = 0, input1_offset = 0; i < i_max; i++)
			{
				const int result_offset_start = i * j_max;

				for (int k = 0, input2_offset = 0; k < k_max; k++, input1_offset++)
					for (int j = 0, result_offset = result_offset_start; j < j_max; j++, result_offset++, input2_offset++)
						result[result_offset] += data1[input1_offset] * data2[input2_offset];
			}
		}
		else
		{
			CFloatMatrix temp(pResultMatrix);
			float *temp_data = temp.data;

			int l;

			for (l = 0; l < nSize; l++)
				temp_data[l] = 0;

			for (int i = 0, input1_offset = 0; i < i_max; i++)
			{
				const int result_offset_start = i * j_max;

				for (int k = 0, input2_offset = 0; k < k_max; k++, input1_offset++)
					for (int j = 0, result_offset = result_offset_start; j < j_max; j++, result_offset++, input2_offset++)
						temp_data[result_offset] += data1[input1_offset] * data2[input2_offset];
			}

			for (l = 0; l < nSize; l++)
				result[l] = temp_data[l];
		}
	}
}

void LinearAlgebra::Transpose(const CFloatMatrix *pMatrix, CFloatMatrix *pResultMatrix)
{
	if (pMatrix->columns != pResultMatrix->rows || pMatrix->rows != pResultMatrix->columns)
	{
		printf("error: input and output matrix do not match LinearAlgebra::Transpose\n");
		return;
	}

	const int rows = pMatrix->rows;
	const int columns = pMatrix->columns;
	const float *input = pMatrix->data;
	float *output = pResultMatrix->data;

	if (input != output)
	{
		for (int i = 0, input_offset = 0; i < rows; i++)
			for (int j = 0, output_offset = i; j < columns; j++, input_offset++, output_offset += rows)
				output[output_offset] = input[input_offset];
	}
	else
	{
		// not optimized yet
		CFloatMatrix temp(pResultMatrix);
		float *temp_data = temp.data;

		int i, input_offset;

		for (i = 0, input_offset = 0; i < rows; i++)
			for (int j = 0, output_offset = i; j < columns; j++, input_offset++, output_offset += rows)
				temp_data[output_offset] = input[input_offset];

		const int nSize = rows * columns;

		for (i = 0; i < nSize; i++)
			output[i] = temp_data[i];
	}
}

void LinearAlgebra::MulMatVec(const CFloatMatrix *pMatrix, const CFloatVector *pVector, CFloatVector *pResultVector)
{
	if (pMatrix->columns != pVector->dimension || pMatrix->rows != pResultVector->dimension)
	{
		printf("error: input matrix and vector and output vector do not match for CFloatMatrix::Multiply\n");
		return;
	}
	
	const int rows = pMatrix->rows;
	const int columns = pMatrix->columns;
	const float *data_m = pMatrix->data;
	const float *data_v = pVector->data;
	float *data_r = pResultVector->data;
	
	if (data_r != data_v)
	{
		for (int i = 0, offset = 0; i < rows; i++)
		{
			float sum = 0.0f;
			
			for (int j = 0; j < columns; j++, offset++)
				sum += data_m[offset] * data_v[j];
				
			data_r[i] = sum;
		}
	}
	else
	{
		float *temp = new float[rows];
		int i, offset;

		for (i = 0, offset = 0; i < rows; i++)
		{
			float sum = 0.0f;
			
			for (int j = 0; j < columns; j++, offset++)
				sum += data_m[offset] * data_v[j];
				
			temp[i] = sum;
		}

		for (i = 0; i < rows; i++)
			data_r[i] = temp[i];

		delete [] temp;
	}
}

void LinearAlgebra::MulMatVec(const CFloatMatrix *pMatrix, const CFloatVector *pVector1, const CFloatVector *pVector2, CFloatVector *pResultVector)
{
	MulMatVec(pMatrix, pVector1, pResultVector);
	AddToVec(pResultVector, pVector2);
}

void LinearAlgebra::AddMatMat(const CFloatMatrix *pMatrix1, const CFloatMatrix *pMatrix2, CFloatMatrix *pResultMatrix)
{
	if (pMatrix1->rows != pMatrix2->rows || pMatrix1->columns != pMatrix2->columns ||
		pMatrix1->rows != pResultMatrix->rows || pMatrix1->columns != pResultMatrix->columns)
	{
		printf("error: matrix dimensions do not match in LinearAlgebra::AddMatMat\n");
		return;
	}

	const int nSize = pMatrix1->rows * pMatrix1->columns;
	const float *data1 = pMatrix1->data;
	const float *data2 = pMatrix2->data;
	float *result = pResultMatrix->data;

	for (int i = 0; i < nSize; i++)
		result[i] = data1[i] + data2[i];
}

void LinearAlgebra::SubtractMatMat(const CFloatMatrix *pMatrix1, const CFloatMatrix *pMatrix2, CFloatMatrix *pResultMatrix)
{
	if (pMatrix1->rows != pMatrix2->rows || pMatrix1->columns != pMatrix2->columns ||
		pMatrix1->rows != pResultMatrix->rows || pMatrix1->columns != pResultMatrix->columns)
	{
		printf("error: matrix dimensions do not match in LinearAlgebra::SubtractMatMat\n");
		return;
	}

	const int nSize = pMatrix1->rows * pMatrix1->columns;
	const float *data1 = pMatrix1->data;
	const float *data2 = pMatrix2->data;
	float *result = pResultMatrix->data;

	for (int i = 0; i < nSize; i++)
		result[i] = data1[i] - data2[i];
}

void LinearAlgebra::AddVecVec(const CFloatVector *pVector1, const CFloatVector *pVector2, CFloatVector *pResultVector)
{
	if (pVector1->dimension != pVector2->dimension || pVector1->dimension != pResultVector->dimension)
	{
		printf("error: vector dimensions do not match in LinearAlgebra::AddVecVec\n");
		return;
	}

	const int nDimension = pVector1->dimension;
	const float *data1 = pVector1->data;
	const float *data2 = pVector2->data;
	float *result = pResultVector->data;

	for (int i = 0; i < nDimension; i++)
		result[i] = data1[i] + data2[i];
}

void LinearAlgebra::SubtractVecVec(const CFloatVector *pVector1, const CFloatVector *pVector2, CFloatVector *pResultVector)
{
	if (pVector1->dimension != pVector2->dimension || pVector1->dimension != pResultVector->dimension)
	{
		printf("error: vector dimensions do not match in LinearAlgebra::SubtractVecVec\n");
		return;
	}

	const int nDimension = pVector1->dimension;
	const float *data1 = pVector1->data;
	const float *data2 = pVector2->data;
	float *result = pResultVector->data;

	for (int i = 0; i < nDimension; i++)
		result[i] = data1[i] - data2[i];
}

void LinearAlgebra::AddToMat(CFloatMatrix *pMatrix, const CFloatMatrix *pMatrixToAdd)
{
	if (pMatrix->rows != pMatrixToAdd->rows || pMatrix->columns != pMatrixToAdd->columns)
	{
		printf("error: matrix dimensions do not match in LinearAlgebra::AddToMat\n");
		return;
	}

	const int nSize = pMatrix->rows * pMatrix->columns;
	float *data = pMatrix->data;
	const float *data_to_add = pMatrixToAdd->data;

	for (int i = 0; i < nSize; i++)
		data[i] += data_to_add[i];
}

void LinearAlgebra::SubtractFromMat(CFloatMatrix *pMatrix, const CFloatMatrix *pMatrixToAdd)
{
	if (pMatrix->rows != pMatrixToAdd->rows || pMatrix->columns != pMatrixToAdd->columns)
	{
		printf("error: matrix dimensions do not match in LinearAlgebra::SubtractFromMat\n");
		return;
	}

	const int nSize = pMatrix->rows * pMatrix->columns;
	float *data = pMatrix->data;
	const float *data_to_subtract = pMatrixToAdd->data;

	for (int i = 0; i < nSize; i++)
		data[i] -= data_to_subtract[i];
}

void LinearAlgebra::AddToVec(CFloatVector *pVector, const CFloatVector *pVectorToAdd)
{
	if (pVector->dimension != pVectorToAdd->dimension)
	{
		printf("error: vector dimensions do not match in LinearAlgebra::AddToVec\n");
		return;
	}

	const int nDimension = pVector->dimension;
	float *data = pVector->data;
	const float *data_to_add = pVectorToAdd->data;

	for (int i = 0; i < nDimension; i++)
		data[i] += data_to_add[i];
}

void LinearAlgebra::SubtractFromVec(CFloatVector *pVector, const CFloatVector *pVectorToSubtract)
{
	if (pVector->dimension != pVectorToSubtract->dimension)
	{
		printf("error: vector dimensions do not match in LinearAlgebra::SubtractFromVec\n");
		return;
	}

	const int nDimension = pVector->dimension;
	float *data = pVector->data;
	const float *data_to_subtract = pVectorToSubtract->data;

	for (int i = 0; i < nDimension; i++)
		data[i] -= data_to_subtract[i];
}


void LinearAlgebra::SubtractMeanFromColumns(const CFloatMatrix *pMatrix, CFloatMatrix *pResultMatrix)
{
	if (pMatrix->columns != pResultMatrix->columns || pMatrix->rows != pResultMatrix->rows)
		return;

	const int columns = pMatrix->columns;
	const int rows = pMatrix->rows;
	const int max = rows * columns;
	const float *data = pMatrix->data;
	float *output = pResultMatrix->data;

	for (int i = 0; i < columns; i++)
	{
		float mean = 0;
		int j;

		for (j = 0; j < max; j += columns)
			mean += data[j];

		mean /= rows;

		for (j = 0; j < max; j += columns)
			output[j] = data[j] - mean;

		data++;
		output++;
	}
}

void LinearAlgebra::SubtractMeanFromRows(const CFloatMatrix *pMatrix, CFloatMatrix *pResultMatrix)
{
	if (pMatrix->columns != pResultMatrix->columns || pMatrix->rows != pResultMatrix->rows)
		return;

	const int columns = pMatrix->columns;
	const int rows = pMatrix->rows;
	const float *data = pMatrix->data;
	float *output = pResultMatrix->data;

	for (int i = 0; i < rows; i++)
	{
		float mean = 0;
		int j;

		for (j = 0; j < columns; j++)
			mean += data[j];

		mean /= columns;

		for (j = 0; j < columns; j++)
			output[j] = data[j] - mean;

		data += columns;
		output += columns;
	}
}

void LinearAlgebra::SolveLinearLeastSquaresHomogeneousSVD(const CFloatMatrix *A, CFloatVector *x)
{
	if (A->columns != x->dimension)
	{
		printf("error: A, x do not match LinearAlgebra::SolveLinearLeastSquaresHomogeneousSVD\n");
		return;
	}
	
	if (A->rows < A->columns)
	{
		printf("error: equation system is underdetermined in LinearAlgebra::SolveLinearLeastSquaresHomogeneousSVD\n");
		return;
	}
	
	const int m = A->rows;
	const int n = A->columns;

	CFloatMatrix W(n, m), V(n, n);
	
	SVD(A, &W, 0, &V, false, false, false);
	
	for (int i = 0, offset = n - 1; i < n; i++, offset += n)
		x->data[i] = V.data[offset];
}

void LinearAlgebra::SolveLinearLeastSquaresSVD(const CFloatMatrix *A, const CFloatVector *b, CFloatVector *x)
{
	if (A->columns != x->dimension || A->rows != b->dimension)
	{
		printf("error: A, b, x do not match LinearAlgebra::SolveLinearLeastSquaresSVD\n");
		return;
	}
	
	if (A->rows < A->columns)
	{
		printf("error: equation system is underdetermined in LinearAlgebra::SolveLinearLeastSquaresSVD\n");
		return;
	}

	CFloatMatrix A_(A->rows, A->columns);
	CalculatePseudoInverseSVD(A, &A_);
	LinearAlgebra::MulMatVec(&A_, b, x);
}

bool LinearAlgebra::SolveLinearLeastSquaresSimple(const CFloatMatrix *A, const CFloatVector *b, CFloatVector *x)
{
	if (A->columns != x->dimension || A->rows != b->dimension)
	{
		printf("error: A, b, x do not match LinearAlgebra::SolveLinearLeastSquaresSimple\n");
		return false;
	}
	
	if (A->rows < A->columns)
	{
		printf("error: equation system is underdetermined in LinearAlgebra::SolveLinearLeastSquaresSimple\n");
		return false;
	}

	CFloatMatrix A_(A->rows, A->columns);
	
	if (!CalculatePseudoInverseSimple(A, &A_))
		return false;
	
	LinearAlgebra::MulMatVec(&A_, b, x);
	
	return true;
}

void LinearAlgebra::CalculatePseudoInverseSVD(const CFloatMatrix *pInputMatrix, CFloatMatrix *pResultMatrix)
{
	if (pInputMatrix->columns != pResultMatrix->rows || pInputMatrix->rows != pResultMatrix->columns)
	{
		printf("error: input and output matrix do not match LinearAlgebra::CalculatePseudoInverseSVD\n");
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
	MulMatMat(&V, &WT, &temp);
	MulMatMat(&temp, &UT, pResultMatrix);
}

bool LinearAlgebra::CalculatePseudoInverseSimple(const CFloatMatrix *pInputMatrix, CFloatMatrix *pResultMatrix)
{
	if (pInputMatrix->columns != pResultMatrix->rows || pInputMatrix->rows != pResultMatrix->columns)
	{
		printf("error: input and output matrix do not match LinearAlgebra::CalculatePseudoInverseSimple\n");
		return false;
	}
	
	// algorithm:
	// compute (A * A^T)^-1 * A^T
	
	const CFloatMatrix *A = pInputMatrix;
	const int m = pInputMatrix->rows;
	const int n = pInputMatrix->columns;
	
	CFloatMatrix AT(m, n), ATA(n, n), ATA_inverted(n, n);
	
	Transpose(A, &AT);
	SelfProduct(A, &ATA);
	
	if (!Invert(&ATA, &ATA_inverted))
		return false;
	
	MulMatMat(&ATA_inverted, &AT, pResultMatrix);
	
	return true;
}

void LinearAlgebra::PCA(const CFloatMatrix *pData, CFloatMatrix *pTransformationMatrix, CFloatMatrix *pTransformedData, int nTargetDimension)
{
	if (nTargetDimension > pData->columns)
	{
		printf("error: target dimension is greater than number of columns in training data matrix in LinearAlgebra::PCA\n");
		return;
	}

	const int samples = pData->rows;
	const int dimension = pData->columns;

	if (pTransformationMatrix->columns != dimension || pTransformationMatrix->rows != nTargetDimension ||
		pTransformedData->columns != samples || pTransformedData->rows != nTargetDimension)
	{
		printf("error: input to LinearAlgebra::PCA does not match\n");
		return;
	}

	CFloatMatrix adjustedData(pData);
	CFloatMatrix covarianceMatrix(dimension, dimension);
	CFloatMatrix eigenValues(dimension, dimension);
	CFloatMatrix eigenVectors(dimension, dimension);
	
	//printf("info: subtracting mean from columns...\n");
	SubtractMeanFromColumns(pData, &adjustedData);
	
	//printf("info: calculating covariance matrix...\n");
	SelfProduct(&adjustedData, &covarianceMatrix);
	
	//printf("info: calculating SVD on %ix%i matrix...\n", dimension, dimension);
	SVD(&covarianceMatrix, &eigenValues, &eigenVectors, 0, true, true);
	
	//printf("info: SVD calculated\n");

	for (int i = 0, offset = 0; i < nTargetDimension; i++)
	{
		for (int j = 0; j < dimension; j++)
		{
			pTransformationMatrix->data[offset] = eigenVectors.data[i * dimension + j];
			offset++;
		}
	}

	MulMatMat(pTransformationMatrix, pData, pTransformedData, true);
}

void LinearAlgebra::PCA(const CFloatMatrix *pData, CFloatMatrix *pTransformationMatrix, CFloatMatrix *pEigenValues)
{
	const int dimension = pData->columns;

	if (pTransformationMatrix->columns != dimension || pTransformationMatrix->rows != dimension || pEigenValues->columns != 1 || pEigenValues->rows != dimension)
	{
		printf("error: input to LinearAlgebra::PCA does not match\n");
		return;
	}

	CFloatMatrix adjustedData(pData);
	CFloatMatrix covarianceMatrix(dimension, dimension);
	CFloatMatrix eigenValues(dimension, dimension);
	CFloatMatrix eigenVectors(dimension, dimension);
	
	//printf("info: subtracting mean from columns...\n");
	SubtractMeanFromColumns(pData, &adjustedData);
	
	//printf("info: calculating covariance matrix...\n");
	SelfProduct(&adjustedData, &covarianceMatrix);
	
	//printf("info: calculating SVD on %ix%i matrix...\n", dimension, dimension);
	SVD(&covarianceMatrix, &eigenValues, pTransformationMatrix, 0, true, true);
	
	//printf("info: SVD calculated\n");
	
	for (int i = 0; i < dimension; i++)
		pEigenValues->data[i] = eigenValues.data[i * dimension + i];
}

bool LinearAlgebra::Invert(const CFloatMatrix *pInputMatrix, CFloatMatrix *pResultMatrix)
{
	if (pInputMatrix->data == pResultMatrix->data)
	{
		printf("error: pInputMatrix and pResultMatrix may not point to the same data in LinearAlgebra::Invert\n");
		return false;
	}
	
	if (pInputMatrix->rows != pInputMatrix->columns)
	{
		printf("error: input matrix must be square matrix for LinearAlgebra::Invert\n");
		return false;
	}

	if (pInputMatrix->columns != pResultMatrix->columns || pInputMatrix->rows != pResultMatrix->rows)
	{
		printf("error: input and output matrix do not match LinearAlgebra::Invert\n");
		return false;
	}

	const int n = pInputMatrix->columns;
	int i;

	CFloatMatrix copiedMatrix(pInputMatrix);
	CFloatMatrix tempMatrix(pInputMatrix);
	ImageProcessor::CopyMatrix(pInputMatrix, &copiedMatrix);

	ImageProcessor::Zero(&tempMatrix);
	for (i = 0; i < n; i++)
		tempMatrix[i][i] = 1;

	int *pPivotRows = new int[n];
	for (i = 0; i < n; i++)
		pPivotRows[i] = 0;
	
	// invert by gauss elimination
	for (i = 0; i < n; i++)
	{
		int j, nPivotColumn = 0;

		float *helper1 = copiedMatrix[i];
		float *helper2 = tempMatrix[i];

		// determine pivot element
		float max = 0;
		for (j = 0; j < n; j++)
			if (fabsf(helper1[j]) > max)
			{
				max = fabsf(helper1[j]);
				nPivotColumn = j;
			}

		pPivotRows[nPivotColumn] = i;

		const float fPivotElement = copiedMatrix[i][nPivotColumn];

		if (fabsf(fPivotElement) < 0.00001f)
		{
			//printf("error: input matrix is not regular for LinearAlgebra::Invert\n");
			delete [] pPivotRows;
			return false;
		}

		const float fFactor = 1.0f / fPivotElement;
		
		for (j = 0; j < n; j++)
		{
			helper1[j] *= fFactor;
			helper2[j] *= fFactor;
		}

		for (j = 0; j < n; j++)
		{
			if (i != j)
			{
				const float v = copiedMatrix[j][nPivotColumn];
				int k;

				helper1 = copiedMatrix[j];
				helper2 = copiedMatrix[i];
				for (k = 0; k < n; k++)
					helper1[k] -= v * helper2[k];
				helper1[nPivotColumn] = 0; // explicitly set to zero

				helper1 = tempMatrix[j];
				helper2 = tempMatrix[i];
				for (k = 0; k < n; k++)
					helper1[k] -= v * helper2[k];
			}
		}
	}

	// copy result rows in pivot order
	for (i = 0; i < n; i++)
	{
		float *helper1 = (*pResultMatrix)[i];
		const float *helper2 = tempMatrix[pPivotRows[i]];

		for (int j = 0; j < n; j++)
			helper1[j] = helper2[j];
	}

	delete [] pPivotRows;
	
	return true;
}

void LinearAlgebra::Zero(CFloatMatrix *pMatrix)
{
	memset(pMatrix->data, 0, pMatrix->columns * pMatrix->rows * sizeof(float));
}

void LinearAlgebra::Zero(CFloatVector *pVector)
{
	memset(pVector->data, 0, pVector->dimension * sizeof(float));
}





// copy & paste implementation for CDoubleMatrix and CDoubleVector
// not nice, but i don't like templates.

void LinearAlgebra::SelfProduct(const CDoubleMatrix *pMatrix, CDoubleMatrix *pResultMatrix, bool AAT)
{
	if (
		(AAT && (pResultMatrix->columns != pMatrix->rows || pResultMatrix->rows != pMatrix->rows)) ||
		(!AAT && (pResultMatrix->columns != pMatrix->columns || pResultMatrix->rows != pMatrix->columns))
	)
	{
		printf("error: matrix dimensions do not match for LinearAlgebra::SelfProduct\n");
		return;
	}

	if (!AAT)
	{
		CDoubleMatrix *pTransposedMatrix = new CDoubleMatrix(pMatrix->rows, pMatrix->columns);
		Transpose(pMatrix, pTransposedMatrix);
		pMatrix = pTransposedMatrix;
	}

	const int columns = pMatrix->columns;
	const int rows = pMatrix->rows;

	const double *data = pMatrix->data;
	double *result = pResultMatrix->data;

	if (data != result)
	{
		for (int i = 0, input_offset1 = 0; i < rows; i++, input_offset1 += columns)
		{
			for (int j = i, input_offset2 = input_offset1; j < rows; j++, input_offset2 += columns)
			{
				const double *data1 = data + input_offset1;
				const double *data2 = data + input_offset2;
				double sum = 0;

				for (int k = 0; k < columns; k++)
					sum += data1[k] * data2[k];

				result[i * rows + j] = result[j * rows + i] = sum;
			}
		}
	}
	else
	{
		CDoubleMatrix temp(pResultMatrix);
		double *temp_data = temp.data;

		for (int i = 0, input_offset1 = 0; i < rows; i++, input_offset1 += columns)
		{
			for (int j = i, input_offset2 = input_offset1; j < rows; j++, input_offset2 += columns)
			{
				const double *data1 = data + input_offset1;
				const double *data2 = data + input_offset2;
				double sum = 0;

				for (int k = 0; k < columns; k++)
					sum += data1[k] * data2[k];

				temp_data[i * rows + j] = temp_data[j * rows + i] = sum;
			}
		}

		memcpy(pResultMatrix->data, temp_data, pResultMatrix->rows * pResultMatrix->columns * sizeof(double));
	}

	if (!AAT)
		delete pMatrix;
}

void LinearAlgebra::MulMatMat(const CDoubleMatrix *A, const CDoubleMatrix *B, CDoubleMatrix *pResultMatrix, bool bTransposeB)
{
	if (!bTransposeB && (A->columns != B->rows || pResultMatrix->columns != B->columns || pResultMatrix->rows != A->rows))
	{
		printf("error: matrices A, B, and pResultMatrix do not satisfy requirements for LinearAlgebra::Multiply\n");
		return;
	}

	if (bTransposeB && (A->columns != B->columns || pResultMatrix->columns != B->rows || pResultMatrix->rows != A->rows))
	{
		printf("error: matrices A, B, and pResultMatrix do not satisfy requirements for LinearAlgebra::Multiply\n");
		return;
	}

	const double *data1 = A->data;
	const double *data2 = B->data;
	double *result = pResultMatrix->data;

	const int nSize = pResultMatrix->columns * pResultMatrix->rows;

	if (bTransposeB)
	{
		const int rowsA = A->rows;
		const int rowsB = B->rows;
		const int columns = A->columns;

		if (data1 != result && data2 != result)
		{
			for (int i = 0, input_offset1 = 0, output_offset = 0; i < rowsA; i++, input_offset1 += columns)
			{
				for (int j = 0, input_offset2 = input_offset1; j < rowsB; j++, input_offset2 += columns, output_offset++)
				{
					const double *data1_ = data1 + input_offset1;
					const double *data2_ = data2 + input_offset2;
					double sum = 0;

					for (int k = 0; k < columns; k++)
						sum += data1_[k] * data2_[k];

					result[output_offset] = sum;
				}
			}
		}
		else
		{
			CDoubleMatrix temp(pResultMatrix);
			double *temp_data = temp.data;

			int l;

			for (l = 0; l < nSize; l++)
				temp_data[l] = 0;

			for (int i = 0, input_offset1 = 0, output_offset = 0; i < rowsA; i++, input_offset1 += columns)
			{
				for (int j = 0, input_offset2 = input_offset1; j < rowsB; j++, input_offset2 += columns, output_offset++)
				{
					const double *data1_ = data1 + input_offset1;
					const double *data2_ = data2 + input_offset2;
					double sum = 0;

					for (int k = 0; k < columns; k++)
						sum += data1_[k] * data2_[k];

					temp_data[output_offset] = sum;
				}
			}

			for (l = 0; l < nSize; l++)
				result[l] = temp_data[l];
		}
	}
	else
	{
		const int i_max = A->rows;
		const int j_max = B->columns;
		const int k_max = A->columns;

		// not optimized yet
		if (data1 != result && data2 != result)
		{
			for (int l = 0; l < nSize; l++)
				result[l] = 0;

			for (int i = 0, input1_offset = 0; i < i_max; i++)
			{
				const int result_offset_start = i * j_max;

				for (int k = 0, input2_offset = 0; k < k_max; k++, input1_offset++)
					for (int j = 0, result_offset = result_offset_start; j < j_max; j++, result_offset++, input2_offset++)
						result[result_offset] += data1[input1_offset] * data2[input2_offset];
			}
		}
		else
		{
			CDoubleMatrix temp(pResultMatrix);
			double *temp_data = temp.data;

			int l;

			for (l = 0; l < nSize; l++)
				temp_data[l] = 0;

			for (int i = 0, input1_offset = 0; i < i_max; i++)
			{
				const int result_offset_start = i * j_max;

				for (int k = 0, input2_offset = 0; k < k_max; k++, input1_offset++)
					for (int j = 0, result_offset = result_offset_start; j < j_max; j++, result_offset++, input2_offset++)
						temp_data[result_offset] += data1[input1_offset] * data2[input2_offset];
			}

			for (l = 0; l < nSize; l++)
				result[l] = temp_data[l];
		}
	}
}

void LinearAlgebra::Transpose(const CDoubleMatrix *pMatrix, CDoubleMatrix *pResultMatrix)
{
	if (pMatrix->columns != pResultMatrix->rows || pMatrix->rows != pResultMatrix->columns)
	{
		printf("error: input and output matrix do not match LinearAlgebra::Transpose\n");
		return;
	}

	const int rows = pMatrix->rows;
	const int columns = pMatrix->columns;
	const double *input = pMatrix->data;
	double *output = pResultMatrix->data;

	if (input != output)
	{
		for (int i = 0, input_offset = 0; i < rows; i++)
			for (int j = 0, output_offset = i; j < columns; j++, input_offset++, output_offset += rows)
				output[output_offset] = input[input_offset];
	}
	else
	{
		// not optimized yet
		CDoubleMatrix temp(pResultMatrix);
		double *temp_data = temp.data;

		int i, input_offset;

		for (i = 0, input_offset = 0; i < rows; i++)
			for (int j = 0, output_offset = i; j < columns; j++, input_offset++, output_offset += rows)
				temp_data[output_offset] = input[input_offset];

		const int nSize = rows * columns;

		for (i = 0; i < nSize; i++)
			output[i] = temp_data[i];
	}
}

void LinearAlgebra::MulMatVec(const CDoubleMatrix *pMatrix, const CDoubleVector *pVector, CDoubleVector *pResultVector)
{
	if (pMatrix->columns != pVector->dimension || pMatrix->rows != pResultVector->dimension)
	{
		printf("error: input matrix and vector and output vector do not match for CDoubleMatrix::Multiply\n");
		return;
	}
	
	const int rows = pMatrix->rows;
	const int columns = pMatrix->columns;
	const double *data_m = pMatrix->data;
	const double *data_v = pVector->data;
	double *data_r = pResultVector->data;
	
	if (data_r != data_v)
	{
		for (int i = 0, offset = 0; i < rows; i++)
		{
			double sum = 0.0f;
			
			for (int j = 0; j < columns; j++, offset++)
				sum += data_m[offset] * data_v[j];
				
			data_r[i] = sum;
		}
	}
	else
	{
		double *temp = new double[rows];
		int i, offset;

		for (i = 0, offset = 0; i < rows; i++)
		{
			double sum = 0.0f;
			
			for (int j = 0; j < columns; j++, offset++)
				sum += data_m[offset] * data_v[j];
				
			temp[i] = sum;
		}

		for (i = 0; i < rows; i++)
			data_r[i] = temp[i];

		delete [] temp;
	}
}

void LinearAlgebra::MulMatVec(const CDoubleMatrix *pMatrix, const CDoubleVector *pVector1, const CDoubleVector *pVector2, CDoubleVector *pResultVector)
{
	MulMatVec(pMatrix, pVector1, pResultVector);
	AddToVec(pResultVector, pVector2);
}

void LinearAlgebra::AddMatMat(const CDoubleMatrix *pMatrix1, const CDoubleMatrix *pMatrix2, CDoubleMatrix *pResultMatrix)
{
	if (pMatrix1->rows != pMatrix2->rows || pMatrix1->columns != pMatrix2->columns ||
		pMatrix1->rows != pResultMatrix->rows || pMatrix1->columns != pResultMatrix->columns)
	{
		printf("error: matrix dimensions do not match in LinearAlgebra::AddMatMat\n");
		return;
	}

	const int nSize = pMatrix1->rows * pMatrix1->columns;
	const double *data1 = pMatrix1->data;
	const double *data2 = pMatrix2->data;
	double *result = pResultMatrix->data;

	for (int i = 0; i < nSize; i++)
		result[i] = data1[i] + data2[i];
}

void LinearAlgebra::SubtractMatMat(const CDoubleMatrix *pMatrix1, const CDoubleMatrix *pMatrix2, CDoubleMatrix *pResultMatrix)
{
	if (pMatrix1->rows != pMatrix2->rows || pMatrix1->columns != pMatrix2->columns ||
		pMatrix1->rows != pResultMatrix->rows || pMatrix1->columns != pResultMatrix->columns)
	{
		printf("error: matrix dimensions do not match in LinearAlgebra::SubtractMatMat\n");
		return;
	}

	const int nSize = pMatrix1->rows * pMatrix1->columns;
	const double *data1 = pMatrix1->data;
	const double *data2 = pMatrix2->data;
	double *result = pResultMatrix->data;

	for (int i = 0; i < nSize; i++)
		result[i] = data1[i] - data2[i];
}

void LinearAlgebra::AddVecVec(const CDoubleVector *pVector1, const CDoubleVector *pVector2, CDoubleVector *pResultVector)
{
	if (pVector1->dimension != pVector2->dimension || pVector1->dimension != pResultVector->dimension)
	{
		printf("error: vector dimensions do not match in LinearAlgebra::AddVecVec\n");
		return;
	}

	const int nDimension = pVector1->dimension;
	const double *data1 = pVector1->data;
	const double *data2 = pVector2->data;
	double *result = pResultVector->data;

	for (int i = 0; i < nDimension; i++)
		result[i] = data1[i] + data2[i];
}

void LinearAlgebra::SubtractVecVec(const CDoubleVector *pVector1, const CDoubleVector *pVector2, CDoubleVector *pResultVector)
{
	if (pVector1->dimension != pVector2->dimension || pVector1->dimension != pResultVector->dimension)
	{
		printf("error: vector dimensions do not match in LinearAlgebra::SubtractVecVec\n");
		return;
	}

	const int nDimension = pVector1->dimension;
	const double *data1 = pVector1->data;
	const double *data2 = pVector2->data;
	double *result = pResultVector->data;

	for (int i = 0; i < nDimension; i++)
		result[i] = data1[i] - data2[i];
}

void LinearAlgebra::AddToMat(CDoubleMatrix *pMatrix, const CDoubleMatrix *pMatrixToAdd)
{
	if (pMatrix->rows != pMatrixToAdd->rows || pMatrix->columns != pMatrixToAdd->columns)
	{
		printf("error: matrix dimensions do not match in LinearAlgebra::AddToMat\n");
		return;
	}

	const int nSize = pMatrix->rows * pMatrix->columns;
	double *data = pMatrix->data;
	const double *data_to_add = pMatrixToAdd->data;

	for (int i = 0; i < nSize; i++)
		data[i] += data_to_add[i];
}

void LinearAlgebra::SubtractFromMat(CDoubleMatrix *pMatrix, const CDoubleMatrix *pMatrixToAdd)
{
	if (pMatrix->rows != pMatrixToAdd->rows || pMatrix->columns != pMatrixToAdd->columns)
	{
		printf("error: matrix dimensions do not match in LinearAlgebra::SubtractFromMat\n");
		return;
	}

	const int nSize = pMatrix->rows * pMatrix->columns;
	double *data = pMatrix->data;
	const double *data_to_subtract = pMatrixToAdd->data;

	for (int i = 0; i < nSize; i++)
		data[i] -= data_to_subtract[i];
}

void LinearAlgebra::AddToVec(CDoubleVector *pVector, const CDoubleVector *pVectorToAdd)
{
	if (pVector->dimension != pVectorToAdd->dimension)
	{
		printf("error: vector dimensions do not match in LinearAlgebra::AddToVec\n");
		return;
	}

	const int nDimension = pVector->dimension;
	double *data = pVector->data;
	const double *data_to_add = pVectorToAdd->data;

	for (int i = 0; i < nDimension; i++)
		data[i] += data_to_add[i];
}

void LinearAlgebra::SubtractFromVec(CDoubleVector *pVector, const CDoubleVector *pVectorToSubtract)
{
	if (pVector->dimension != pVectorToSubtract->dimension)
	{
		printf("error: vector dimensions do not match in LinearAlgebra::SubtractFromVec\n");
		return;
	}

	const int nDimension = pVector->dimension;
	double *data = pVector->data;
	const double *data_to_subtract = pVectorToSubtract->data;

	for (int i = 0; i < nDimension; i++)
		data[i] -= data_to_subtract[i];
}


void LinearAlgebra::SubtractMeanFromColumns(const CDoubleMatrix *pMatrix, CDoubleMatrix *pResultMatrix)
{
	if (pMatrix->columns != pResultMatrix->columns || pMatrix->rows != pResultMatrix->rows)
		return;

	const int columns = pMatrix->columns;
	const int rows = pMatrix->rows;
	const int max = rows * columns;
	const double *data = pMatrix->data;
	double *output = pResultMatrix->data;

	for (int i = 0; i < columns; i++)
	{
		double mean = 0;
		int j;

		for (j = 0; j < max; j += columns)
			mean += data[j];

		mean /= rows;

		for (j = 0; j < max; j += columns)
			output[j] = data[j] - mean;

		data++;
		output++;
	}
}

void LinearAlgebra::SubtractMeanFromRows(const CDoubleMatrix *pMatrix, CDoubleMatrix *pResultMatrix)
{
	if (pMatrix->columns != pResultMatrix->columns || pMatrix->rows != pResultMatrix->rows)
		return;

	const int columns = pMatrix->columns;
	const int rows = pMatrix->rows;
	const double *data = pMatrix->data;
	double *output = pResultMatrix->data;

	for (int i = 0; i < rows; i++)
	{
		double mean = 0;
		int j;

		for (j = 0; j < columns; j++)
			mean += data[j];

		mean /= columns;

		for (j = 0; j < columns; j++)
			output[j] = data[j] - mean;

		data += columns;
		output += columns;
	}
}

bool LinearAlgebra::Invert(const CDoubleMatrix *pInputMatrix, CDoubleMatrix *pResultMatrix)
{
	if (pInputMatrix->data == pResultMatrix->data)
	{
		printf("error: pInputMatrix and pResultMatrix may not point to the same data in LinearAlgebra::Invert\n");
		return false;
	}
	
	if (pInputMatrix->rows != pInputMatrix->columns)
	{
		printf("error: input matrix must be square matrix for LinearAlgebra::Invert\n");
		return false;
	}

	if (pInputMatrix->columns != pResultMatrix->columns || pInputMatrix->rows != pResultMatrix->rows)
	{
		printf("error: input and output matrix do not match LinearAlgebra::Invert\n");
		return false;
	}

	const int n = pInputMatrix->columns;
	int i;

	CDoubleMatrix copiedMatrix(pInputMatrix);
	CDoubleMatrix tempMatrix(pInputMatrix);
	ImageProcessor::CopyMatrix(pInputMatrix, &copiedMatrix);

	ImageProcessor::Zero(&tempMatrix);
	for (i = 0; i < n; i++)
		tempMatrix[i][i] = 1;

	int *pPivotRows = new int[n];
	for (i = 0; i < n; i++)
		pPivotRows[i] = 0;
	
	// invert by gauss elimination
	for (i = 0; i < n; i++)
	{
		int j, nPivotColumn = 0;

		double *helper1 = copiedMatrix[i];
		double *helper2 = tempMatrix[i];

		// determine pivot element
		double max = 0;
		for (j = 0; j < n; j++)
			if (fabs(helper1[j]) > max)
			{
				max = fabs(helper1[j]);
				nPivotColumn = j;
			}

		pPivotRows[nPivotColumn] = i;

		const double dPivotElement = copiedMatrix[i][nPivotColumn];

		if (fabs(dPivotElement) < 0.00001)
		{
			//printf("error: input matrix is not regular for LinearAlgebra::Invert\n");
			delete [] pPivotRows;
			return false;
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
				const double v = copiedMatrix[j][nPivotColumn];
				int k;

				helper1 = copiedMatrix[j];
				helper2 = copiedMatrix[i];
				for (k = 0; k < n; k++)
					helper1[k] -= v * helper2[k];
				helper1[nPivotColumn] = 0; // explicitly set to zero

				helper1 = tempMatrix[j];
				helper2 = tempMatrix[i];
				for (k = 0; k < n; k++)
					helper1[k] -= v * helper2[k];
			}
		}
	}

	// copy result rows in pivot order
	for (i = 0; i < n; i++)
	{
		double *helper1 = (*pResultMatrix)[i];
		const double *helper2 = tempMatrix[pPivotRows[i]];

		for (int j = 0; j < n; j++)
			helper1[j] = helper2[j];
	}

	delete [] pPivotRows;
	
	return true;
}

void LinearAlgebra::SolveLinearLeastSquaresHomogeneousSVD(const CDoubleMatrix *A, CDoubleVector *x)
{
	if (A->columns != x->dimension)
	{
		printf("error: A, x do not match LinearAlgebra::SolveLinearLeastSquaresHomogeneousSVD\n");
		return;
	}
	
	if (A->rows < A->columns)
	{
		printf("error: equation system is underdetermined in LinearAlgebra::SolveLinearLeastSquaresHomogeneousSVD\n");
		return;
	}
	
	const int m = A->rows;
	const int n = A->columns;

	CDoubleMatrix W(n, m), V(n, n);
	
	SVD(A, &W, 0, &V, false, false, false);
	
	for (int i = 0, offset = n - 1; i < n; i++, offset += n)
		x->data[i] = V.data[offset];
}

void LinearAlgebra::SolveLinearLeastSquaresSVD(const CDoubleMatrix *A, const CDoubleVector *b, CDoubleVector *x)
{
	if (A->columns != x->dimension || A->rows != b->dimension)
	{
		printf("error: A, b, x do not match LinearAlgebra::SolveLinearLeastSquaresSVD\n");
		return;
	}
	
	if (A->rows < A->columns)
	{
		printf("error: equation system is underdetermined in LinearAlgebra::SolveLinearLeastSquaresSVD\n");
		return;
	}

	CDoubleMatrix A_(A->rows, A->columns);
	CalculatePseudoInverseSVD(A, &A_);
	MulMatVec(&A_, b, x);
}

bool LinearAlgebra::SolveLinearLeastSquaresSimple(const CDoubleMatrix *A, const CDoubleVector *b, CDoubleVector *x)
{
	if (A->columns != x->dimension || A->rows != b->dimension)
	{
		printf("error: A, b, x do not match LinearAlgebra::SolveLinearLeastSquaresSimple\n");
		return false;
	}
	
	if (A->rows < A->columns)
	{
		printf("error: equation system is underdetermined in LinearAlgebra::SolveLinearLeastSquaresSimple\n");
		return false;
	}

	CDoubleMatrix A_(A->rows, A->columns);
	
	if (!CalculatePseudoInverseSimple(A, &A_))
		return false;
	
	MulMatVec(&A_, b, x);
	
	return true;
}

void LinearAlgebra::CalculatePseudoInverseSVD(const CDoubleMatrix *pInputMatrix, CDoubleMatrix *pResultMatrix)
{
	if (pInputMatrix->columns != pResultMatrix->rows || pInputMatrix->rows != pResultMatrix->columns)
	{
		printf("error: input and output matrix do not match LinearAlgebra::CalculatePseudoInverseSVD\n");
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
	MulMatMat(&V, &WT, &temp);
	MulMatMat(&temp, &UT, pResultMatrix);
}

bool LinearAlgebra::CalculatePseudoInverseSimple(const CDoubleMatrix *pInputMatrix, CDoubleMatrix *pResultMatrix)
{
	if (pInputMatrix->columns != pResultMatrix->rows || pInputMatrix->rows != pResultMatrix->columns)
	{
		printf("error: input and output matrix do not match LinearAlgebra::CalculatePseudoInverseSimple\n");
		return false;
	}
	
	// algorithm:
	// compute (A * A^T)^-1 * A^T
	
	const CDoubleMatrix *A = pInputMatrix;
	const int m = pInputMatrix->rows;
	const int n = pInputMatrix->columns;
	
	CDoubleMatrix AT(m, n), ATA(n, n), ATA_inverted(n, n);
	
	Transpose(A, &AT);
	SelfProduct(A, &ATA);
	
	if (!Invert(&ATA, &ATA_inverted))
		return false;
	
	MulMatMat(&ATA_inverted, &AT, pResultMatrix);
	
	return true;
}

void LinearAlgebra::Zero(CDoubleVector *pVector)
{
	memset(pVector->data, 0, pVector->dimension * sizeof(double));
}

void LinearAlgebra::Zero(CDoubleMatrix *pMatrix)
{
	memset(pMatrix->data, 0, pMatrix->columns * pMatrix->rows * sizeof(double));
}




bool LinearAlgebra::DetermineAffineTransformation(const Vec2d *pSourcePoints, const Vec2d *pTargetPoints, int nPoints, Mat3d &A, bool bUseSVD)
{
	if (nPoints < 3)
	{
		printf("error: not enough input point pairs for LinearAlgebra::DetermineAffineTransformation (must be at least 3)\n");
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
	
	for (int i = 0; i < 6; i++)
		x.data[i] = 0.0f;
	
	bool bRet = true;
	
	if (bUseSVD)
		SolveLinearLeastSquaresSVD(&M, &b, &x);
	else
		bRet = SolveLinearLeastSquaresSimple(&M, &b, &x);
	
	if (!bRet)
		return false;
	
	Math3d::SetMat(A, x.data[0], x.data[1], x.data[2], x.data[3], x.data[4], x.data[5], 0.0f, 0.0f, 1.0f);
	
	return true;
}

bool LinearAlgebra::DetermineHomography(const Vec2d *pSourcePoints, const Vec2d *pTargetPoints, int nPoints, Mat3d &A, bool bUseSVD)
{
	if (nPoints < 4)
	{
		printf("error: not enough input point pairs for LinearAlgebra::DetermineHomography (must be at least 4)\n");
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
	
	for (int i = 0; i < 8; i++)
		x.data[i] = 0.0;
	
	bool bRet = true;
	
	if (bUseSVD)
		SolveLinearLeastSquaresSVD(&M, &b, &x);
	else
		bRet = SolveLinearLeastSquaresSimple(&M, &b, &x);
	
	if (!bRet)
		return false;
	
	Math3d::SetMat(A, float(x.data[0]), float(x.data[1]), float(x.data[2]), float(x.data[3]), float(x.data[4]), float(x.data[5]), float(x.data[6]), float(x.data[7]), 1.0f);
	
	return true;
}
