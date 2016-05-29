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
// Filename:  ContourHelper.h
// Author:    Derick Beng Yuh, Pedram Azad
// Date:      03.08.2010
// ****************************************************************************

/** \defgroup Contours Algorithms related to contours. */

#ifndef _CONTOUR_HELPER_H_
#define _CONTOUR_HELPER_H_



// ****************************************************************************
// Forward declarations
// ****************************************************************************

struct Vec2d;



// ****************************************************************************
// ContourHelper
// ****************************************************************************

/*!
	\ingroup Contours
	\brief Collection of functions for contour processing.
*/
namespace ContourHelper
{
	/*!
		\brief Computes the convex hull for a set of contour points.
	 
		The algorithm used for computing the convex hull is Graham's scan.
	 
		The input points do not have to be sorted in any way. The result points will be sorted.
		The function PrimitivesDrawer::DrawPolygon(CByteImage*, const Vec2d*, int, int, int, int, int) can be used for visualizing the resulting contour.
	 
		@param[in] pPoints The input contour points.
		@param[in] nPoints The number of input contour points.
		@param[out] pResultPoints The result contour points. Memory for at least nPoints Vec2d entries must have been allocated by the user.
		@param[out] nResultPoints The number of result contour points.
	*/
	void ComputeConvexHull(const Vec2d *pPoints, int nPoints, Vec2d *pResultPoints, int &nResultPoints);
}



#endif /* _CONTOUR_HELPER_H_ */
