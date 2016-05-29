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
// Filename:  Math2d.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _MATH_2D_H_
#define _MATH_2D_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "DataStructures/DynamicArrayTemplate.h"

// system
#include <vector>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

struct Mat3d;
struct Rectangle2d;


// ****************************************************************************
// Structs and typedefs
// ****************************************************************************

/*!
	\brief Data structure for the representation of a 2D vector.

	This data structure the vector

	\f$\left(\begin{array}{c}
	x\\
	y\\
	\end{array}\right)\f$.

	The namespace Math2d offers various functions operating on this data type.

	\ingroup MathData
*/
struct Vec2d
{
	float x, y;
};

/*!
	\brief Data structure for the representation of a 2x2 matrix.

	This data structure the matrix

	\f$\left(\begin{array}{cc}
	r_1 & r_2\\
	r_3 & r_4\\
	\end{array}\right)\f$.

	The namespace Math2d offers various functions operating on this data type.

	\ingroup MathData
*/
struct Mat2d
{
	float r1, r2, r3, r4;
};

typedef std::vector<Vec2d> Vec2dList;
typedef CDynamicArrayTemplate<Vec2d> CVec2dArray;



// ****************************************************************************
// Math2d
// ****************************************************************************

/*!
	\ingroup MathOperations
	\brief Efficient mathematic functions operating on the data types Vec2d and Mat2d.
*/
namespace Math2d
{
	void SetVec(Vec2d &vec, float x, float y);
	void SetVec(Vec2d &vec, const Vec2d &sourceVector);
	
	void SetRotationMat(Mat2d &matrix, float angle);
	
	void MulMatMat(const Mat2d &matrix1, const Mat2d &matrix2, Mat2d &result);
	void MulMatVec(const Mat2d &matrix, const Vec2d &vec, Vec2d &result);
	void MulMatVec(const Mat2d &matrix, const Vec2d &vector1, const Vec2d &vector2, Vec2d &result);
	void MulMatScalar(const Mat2d &matrix, float scalar, Mat2d &result);
	
	void MulVecScalar(const Vec2d &vec, float scalar, Vec2d &result);

	void RotateVec(const Vec2d &vec, float angle, Vec2d &result);
	void RotateVec(const Vec2d &point, const Vec2d &center, float angle, Vec2d &result);
	
	void AddVecVec(const Vec2d &vector1, const Vec2d &vector2, Vec2d &result);
	void SubtractVecVec(const Vec2d &vector1, const Vec2d &vector2, Vec2d &result);
	void AddToVec(Vec2d &vec, const Vec2d &vectorToAdd);
	void SubtractFromVec(Vec2d &vec, const Vec2d &vectorToSubtract);
	
	float ScalarProduct(const Vec2d &vector1, const Vec2d &vector2);
	float SquaredLength(const Vec2d &vec);
	float Length(const Vec2d &vec);
	float Distance(const Vec2d &vector1, const Vec2d &vector2);
	float SquaredDistance(const Vec2d &vector1, const Vec2d &vector2);
	float Angle(const Vec2d &vector1, const Vec2d &vector2);
	
	void Transpose(const Mat2d &matrix, Mat2d &result);
	void Invert(const Mat2d &matrix, Mat2d &result);
	
	void NormalizeVec(Vec2d &vec);
	
	void ApplyHomography(const Mat3d &A, const Vec2d &p, Vec2d &result);

	void Average(const Vec2d &vector1, const Vec2d &vector2, Vec2d &result);

	void Mean(const CVec2dArray &vectorList, Vec2d &result);
	void Mean(const Vec2d *pVectors, int nVectors, Vec2d &result);

	void ComputeRectangleCornerPoints(const Rectangle2d &rectangle, Vec2d resultCornerPoints[4]);

	extern const Vec2d zero_vec;
}



#endif /* _MATH_2D_H_ */
