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
// Filename:  Structs.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


#ifndef __STRUCTS_H__
#define __STRUCTS_H__


// ****************************************************************************
// Necessary includes
// ****************************************************************************

// IVT
#include "Math/Math2d.h"
#include "Math/Math3d.h"
#include "DataStructures/DynamicArrayTemplate.h"

// system
#include <string.h>
#include <math.h>
#include <vector>



// ****************************************************************************
// Structs
// ****************************************************************************

struct ROI
{
	int min_x, min_y, max_x, max_y;
};

struct PointPair2d
{
	Vec2d p1;
	Vec2d p2;
};

struct PointPair3d
{
	Vec3d p1;
	Vec3d p2;
};

/*!
	\brief Data structure for the representation of a 2D rectangle.

	The function PrimitivesDrawer::DrawRectangle draws a circle, given its specification in this struct.
	Other functions (e.g. detectors) can use this struct as well.
*/
struct Rectangle2d
{
	/*!
		\brief The center of the rectangle.
	*/
	Vec2d center;

	/*!
		\brief The width of the rectangle.
	*/
	float width;
	
	/*!
		\brief The height of the rectangle.
	*/
	float height;

	/*!
		\brief The angle of the rectangle.
	*/
	float angle;
};

/*!
	\brief Data structure for the representation of a 2D circle.

	The function PrimitivesDrawer::DrawCircle draws a circle, given its specification in this struct.
	Other functions (e.g. detectors) can use this struct as well.
*/
struct Circle2d
{
	/*!
		\brief The center of the circle.
	*/
	Vec2d center;
	
	/*!
		\brief The radius of the circle.
	*/
	float radius;
};

/*!
	\brief Data structure for the representation of a 2D ellipse.

	The function PrimitivesDrawer::DrawEllipse draws an ellipse, given its specification in this struct.
	Other functions (e.g. detectors) can use this struct as well.
*/
struct Ellipse2d
{
	/*!
		\brief The center of the ellipse.
	*/
	Vec2d center;
	
	/*!
		\brief The radius in horizontal direction of the ellipse (horizontal refers to when angle = 0).
	*/
	float radius_x;
	
	/*!
		\brief The radius in vertical direction of the ellipse (vertical refers to when angle = 0).
	*/
	float radius_y;
	
	/*!
		\brief The rotiation angle of the ellipse, given in radians.
	
		Note that a positive angle will rotate the ellipse clockwise, as the y-axis of the
		image coordinate system is oriented downward. In the IVt, the rotation angles are consistent for
		all algorithms/functions that involve rotations.
	*/
	float angle;
};

/*!
	\brief Data structure for the representation of a 2D straight line.

	The function PrimitivesDrawer::DrawLine draws a straight line, given its specification in this struct.
	Other functions (e.g. detectors) can use this struct as well.

	The two independent straight line equations are:

	1. \f$\vec{n} \cdot (\vec{x} - \vec{a}) = \vec{n} \cdot \vec{x} - \vec{n} \cdot \vec{a} = \vec{n} \cdot \vec{x} + c\f$.

	2. \f$\vec{x} = \vec{a} + r \cdot \vec{u},\ \ r \in R\f$

	where any \f$\vec{x}\f$ satisfying one of these equations belongs to the straight line,
	\f$\vec{a}\f$ = point, \f$\vec{u}\f$ = directionVector, and \f$\vec{n}\f$ = normalVector.
*/
struct StraightLine2d
{
	StraightLine2d()
	{
		Math2d::SetVec(point, Math2d::zero_vec);
		Math2d::SetVec(directionVector, Math2d::zero_vec);
		Math2d::SetVec(normalVector, Math2d::zero_vec);
		c = 0.0f;
	}

	StraightLine2d(const Vec2d &point1, const Vec2d &point2)
	{
		// choose first point as reference point
		Math2d::SetVec(point, point1);

		// compute direction vector
		Math2d::SubtractVecVec(point2, point1, directionVector);

		// normalize direction vector
		Math2d::NormalizeVec(directionVector);

		// compute normal vector (will be normalized, since u is normalized)
		Math2d::SetVec(normalVector, -directionVector.y, directionVector.x);

		// compute - n * a
		c = -Math2d::ScalarProduct(normalVector, point);
	}

	StraightLine2d(float angle, float c)
	{
		// set c
		this->c = c;

		// compute normal vector (will be normalized, since sin^2 + cos^2 = 1
		Math2d::SetVec(normalVector, cosf(angle), sinf(angle));

		// compute direction vector (will be normalized, since n is normalized)
		Math2d::SetVec(directionVector, -normalVector.y, normalVector.x);

		// compute an arbitrary point belonging to the straight line
		if (fabsf(normalVector.x) > fabsf(normalVector.y))
		{
			Math2d::SetVec(point, -c / normalVector.x, 0.0f);
		}
		else
		{
			Math2d::SetVec(point, 0.0f, -c / normalVector.y);
		}
	}

	StraightLine2d(const PointPair2d &pointPair)
	{
		StraightLine2d(pointPair.p1, pointPair.p2);
	}

	/*!
		\brief An aribtrary point belonging to the straight line.
	*/
	Vec2d point;

	/*!
		\brief The normalized direction vector of the straight line.
	*/
	Vec2d directionVector;

	/*!
		\brief The normalized normal vector of the straight line.
	*/
	Vec2d normalVector;
	
	/*!
		\brief The negative scalar product of normalVector and point.
	*/
	float c;
};

struct MyRegion
{
	// constructor
	MyRegion()
	{
		pPixels = 0;
	}

	// copy constructor
	MyRegion(const MyRegion &region)
	{
		nPixels = region.nPixels;

		if (region.pPixels)
		{
			pPixels = new int[nPixels];
			memcpy(pPixels, region.pPixels, nPixels * sizeof(int));
		}
		else
			pPixels = 0;

		nSeedOffset = region.nSeedOffset;

		Math2d::SetVec(centroid, region.centroid);

		min_x = region.min_x;
		min_y = region.min_y;
		max_x = region.max_x;
		max_y = region.max_y;

		ratio = region.ratio;
	}
	
	// destructor
	~MyRegion()
	{
		if (pPixels)
			delete [] pPixels;
	}

	// assign operator
	MyRegion& operator= (const MyRegion &region)
	{
		nPixels = region.nPixels;

		if (pPixels)
			delete [] pPixels;

		if (region.pPixels)
		{
			pPixels = new int[nPixels];
			memcpy(pPixels, region.pPixels, nPixels * sizeof(int));
		}
		else
			pPixels = 0;

		nSeedOffset = region.nSeedOffset;

		Math2d::SetVec(centroid, region.centroid);

		min_x = region.min_x;
		min_y = region.min_y;
		max_x = region.max_x;
		max_y = region.max_y;

		ratio = region.ratio;

		return *this;
	}
	
	// attributes
	int *pPixels;
	int nPixels;

	int nSeedOffset;
	
	Vec2d centroid;
	
	int min_x;
	int min_y;
	int max_x;
	int max_y;
	
	float ratio;
};


// ****************************************************************************
// Typedefs
// ****************************************************************************

typedef std::vector<MyRegion> RegionList;
typedef CDynamicArrayTemplate<MyRegion> CRegionArray;
typedef CDynamicArrayTemplate<Circle2d> CCircle2dArray;
typedef CDynamicArrayTemplate<Ellipse2d> CEllipse2dArray;
typedef CDynamicArrayTemplate<StraightLine2d> CStraightLine2dArray;
typedef CDynamicArrayTemplate<int> CIntArray;



#endif /* __STRUCTS_H__ */
