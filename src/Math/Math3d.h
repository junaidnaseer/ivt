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
// Filename:  Math3d.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


#ifndef _MATH_3D_H_
#define _MATH_3D_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include <vector>
#include "DataStructures/DynamicArrayTemplate.h"


// ****************************************************************************
// Structs and typedefs
// ****************************************************************************

/*!
	\brief Data structure for the representation of a 3D vector.

	This data structure the vector

	\f$\left(\begin{array}{c}
	x\\
	y\\
	z\\
	\end{array}\right)\f$.

	The namespace Math3d offers various functions operating on this data type.

	\ingroup MathData
*/
struct Vec3d
{
	float x, y, z;
};

/*!
	\brief Data structure for the representation of a 3x3 matrix.

	This data structure the matrix

	\f$\left(\begin{array}{ccc}
	r_1 & r_2 & r_3\\
	r_4 & r_5 & r_6\\
	r_7 & r_8 & r_9\\
	\end{array}\right)\f$.

	The namespace Math3d offers various functions operating on this data type.

	\ingroup MathData
*/
struct Mat3d
{
	float r1, r2, r3, r4, r5, r6, r7, r8, r9;
};

/*!
	\brief Data structure for the representation of a 3D rigid body transformation.

	The namespace Math3d offers various functions operating on this data type.

	\ingroup MathData
*/
struct Transformation3d
{
	Mat3d rotation;
	Vec3d translation;
};

/*!
	\brief Data structure for the representation of a quaternion.

	The namespace Math3d offers functions operating on this data type.

	\ingroup MathData
*/
struct Quaternion
{
	/*! The vector (imaginary) part. */
	Vec3d v;
	/*! The scalar (real) part. */
	float w;
};

typedef std::vector<Vec3d> Vec3dList;
typedef CDynamicArrayTemplate<Vec3d> CVec3dArray;



// ****************************************************************************
// Math3d
// ****************************************************************************

/*!
	\ingroup MathOperations
	\brief Efficient mathematic functions operating on the data types Vec3d, Mat3d, Transformation3d, and Quaternion.
*/
namespace Math3d
{
	bool LoadFromFile(Vec3d &vector, const char *pFilePath);
	bool LoadFromFile(Mat3d &matrix, const char *pFilePath);
	bool LoadFromFile(Transformation3d &transformation, const char *pFilePath);
	bool SaveToFile(const Vec3d &vector, const char *pFilePath);
	bool SaveToFile(const Mat3d &matrix, const char *pFilePath);
	bool SaveToFile(const Transformation3d &transformation, const char *pFilePath);
	
	void SetVec(Vec3d &vec, float x, float y, float z);
	void SetVec(Vec3d &vec, const Vec3d &sourceVector);
	void SetMat(Mat3d &matrix, float r1, float r2, float r3, float r4, float r5, float r6, float r7, float r8, float r9);
	void SetMat(Mat3d &matrix, const Mat3d &sourceMatrix);
	void SetRotationMat(Mat3d &matrix, const Vec3d &axis, float theta);
	void SetRotationMat(Mat3d &matrix, float alpha, float beta, float gamma);
	void SetRotationMat(Mat3d &matrix, const Vec3d &rotation);
	void SetRotationMatYZX(Mat3d &matrix, const Vec3d &rotation);
	void SetRotationMatX(Mat3d &matrix, float theta);
	void SetRotationMatY(Mat3d &matrix, float theta);
	void SetRotationMatZ(Mat3d &matrix, float theta);
	void SetRotationMatAxis(Mat3d &matrix, const Vec3d &axis, float theta);

	void MulMatVec(const Mat3d &matrix, const Vec3d &vec, Vec3d &result);
	void MulMatVec(const Mat3d &matrix, const Vec3d &vector1, const Vec3d &vector2, Vec3d &result);
	void MulMatMat(const Mat3d &matrix1, const Mat3d &matrix2, Mat3d &result);

	void MulVecTransposedVec(const Vec3d &vector1, const Vec3d &vector2, Mat3d &result);
	
	void RotateVec(const Vec3d &vec, const Vec3d &rotation, Vec3d &result);
	void RotateVecYZX(const Vec3d &vec, const Vec3d &rotation, Vec3d &result);
	void TransformVec(const Vec3d &vec, const Vec3d &rotation, const Vec3d &translation, Vec3d &result);
	void TransformVecYZX(const Vec3d &vec, const Vec3d &rotation, const Vec3d &translation, Vec3d &result);
	
	void MulVecScalar(const Vec3d &vec, float scalar, Vec3d &result);
	void MulMatScalar(const Mat3d &matrix, float scalar, Mat3d &result);

	void AddMatMat(const Mat3d &matrix1, const Mat3d &matrix2, Mat3d &matrix);
	void AddToMat(Mat3d &matrix, const Mat3d &matrixToAdd);
	void SubtractMatMat(const Mat3d &matrix1, const Mat3d &matrix2, Mat3d &result);
		
	void AddVecVec(const Vec3d &vector1, const Vec3d &vector2, Vec3d &result);
	void SubtractVecVec(const Vec3d &vector1, const Vec3d &vector2, Vec3d &result);
	void AddToVec(Vec3d &vec, const Vec3d &vectorToAdd);
	void SubtractFromVec(Vec3d &vec, const Vec3d &vectorToSubtract);
	
	void CrossProduct(const Vec3d &vector1, const Vec3d &vector2, Vec3d &result);
	float ScalarProduct(const Vec3d &vector1, const Vec3d &vector2);
	float SquaredLength(const Vec3d &vec);
	float Length(const Vec3d &vec);
	float Distance(const Vec3d &vector1, const Vec3d &vector2);
	float SquaredDistance(const Vec3d &vector1, const Vec3d &vector2);
	float Angle(const Vec3d &vector1, const Vec3d &vector2);
	float Angle(const Vec3d &vector1, const Vec3d &vector2, const Vec3d &axis);
	float EvaluateForm(const Vec3d &matrix1, const Mat3d &matrix2); // matrix1^T * matrix2 * matrix1
	
	void NormalizeVec(Vec3d &vec);
	
	void Transpose(const Mat3d &matrix, Mat3d &result);
	void Invert(const Mat3d &matrix, Mat3d &result);
	float Det(const Mat3d &matrix);

	void SetTransformation(Transformation3d &transformation, const Vec3d &rotation, const Vec3d &translation);
	void SetTransformation(Transformation3d &transformation, const Transformation3d &sourceTransformation);
	void Invert(const Transformation3d &input, Transformation3d &result);
	void MulTransTrans(const Transformation3d &transformation1, const Transformation3d &transformation2, Transformation3d &result);
	void MulTransVec(const Transformation3d &transformation, const Vec3d &vec, Vec3d &result);

	void MulQuatQuat(const Quaternion &quat1, const Quaternion &quat2, Quaternion &result);
	void RotateVecQuaternion(const Vec3d &vec, const Vec3d &axis, float theta, Vec3d &result);
	void RotateVecAngleAxis(const Vec3d &vec, const Vec3d &axis, float theta, Vec3d &result);
	void GetAxisAndAngle(const Mat3d &R, Vec3d &axis, float &angle);

	void Average(const Vec3d &vector1, const Vec3d &vector2, Vec3d &result);

	void Mean(const CVec3dArray &vectorList, Vec3d &result);
	void Mean(const Vec3d *pVectors, int nVectors, Vec3d &result);

	extern const Vec3d zero_vec;
	extern const Mat3d unit_mat;
	extern const Mat3d zero_mat;
}



#endif /* _MATH_3D_H_ */
