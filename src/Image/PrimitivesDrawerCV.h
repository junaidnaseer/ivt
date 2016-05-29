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
// Filename:  PrimitivesDrawerCV.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


#ifndef _PRIMITIVES_DRAWER_CV_H_
#define _PRIMITIVES_DRAWER_CV_H_


// ****************************************************************************
// Forward declarations
// ****************************************************************************

struct Ellipse2d;
struct PointPair2d;
struct MyRegion;
struct Vec2d;
class CByteImage;


// ****************************************************************************
// PrimitivesDrawerCV
// ****************************************************************************

/*!
	\brief Functions for drawing 2D primitives into a CByteImage using OpenCV.
 
	Deprecated. Use PrimitivesDrawer instead. Only filling of some primitives and PrimitivesDrawerCV::PutText are not yet implemented in PrimitivesDrawer.
*/
namespace PrimitivesDrawerCV
{
	/*!
		\deprecated
		\brief Deprecated.
		
		Use PrimitivesDrawer::DrawCircle(CByteImage*, const Vec2d&, float, int, int, int, int, bool) instead.
	*/
	void DrawCircle(CByteImage *pImage, const Vec2d &mid_point, double radius, int r = 255, int g = 255, int b = 255, int thickness = 1);
	
	/*!
		\deprecated
		\brief Deprecated.

		Use PrimitivesDrawer::DrawEllipse(CByteImage*, const Ellipse2d&, int, int, int, int) instead.
	*/
	void DrawEllipse(CByteImage *pImage, const Ellipse2d &ellipse, int r = 255, int g = 255, int b = 255, int thickness = 1);
	
	/*!
		\deprecated
		\brief Deprecated.
		
		Use PrimitivesDrawer::DrawLine(CByteImage*, const PointPair2d&, int, int, int, int) instead.
	*/
	void DrawLine(CByteImage *pImage, const PointPair2d &line, int r = 255, int g = 255, int b = 255, int thickness = 1);
	
	/*!
		\deprecated
		\brief Deprecated.

		Use PrimitivesDrawer::DrawLine(CByteImage*, const Vec2d&, const Vec2d&, int, int, int, int) instead.
	*/
	void DrawLine(CByteImage *pImage, const Vec2d &p1, const Vec2d &p2, int r = 255, int g = 255, int b = 255, int thickness = 1);
	
	/*!
		\deprecated
		\brief Deprecated.
		
		Use PrimitivesDrawer::Region(CByteImage*, const MyRegion&, int, int, int, int) instead.
	*/
	void DrawRegion(CByteImage *pImage, const MyRegion &region, int r = 255, int g = 255, int b = 255, int thickness = 1);
	
	/*!
		\brief Draws a polygon into a CByteImage.
	 
		The provided points must build a polygon in the given order.
	 
		If the polygon is to be filled (thickness = -1), then the points must build a convex polygon.
	 
		If thickness >= 1, then it is recommended to use PrimitivesDrawer::DrawConvexPolygon(CByteImage*, const Vec2d*, int, int, int, int, int) instead.
	 
		@param[out] pImage The target image, the polygon will be drawn in here. Must be of type CByteImage::eGrayScale or CByteImage::eRGB24.
		@param[in] pPoints An array of points containing the 2D point coordinates in the following order: x-coordinate 1st point, y-coordinate 1st point, x-coordinate 2nd point, y-coordinate 2nd point, and so on. Must contain 2 * nPoints integer values.
		@param[in] nPoints The number of points. Must be >= 2.
		@param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		@param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		@param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		@param[in] thickness If thickness = -1, then the polygon will be filled. Otherwise (thickness >= 1) this parameter specifies the thickness of the lines to be drawn. If thickness >= 1, then the polygon will be closed if the first and last point are not equal.
	*/
	void DrawConvexPolygon(CByteImage *pImage, int *pPoints, int nPoints, int r = 255, int g = 255, int b = 255, int thickness = 1);
	
	/*!
		\brief Draws text into a CByteImage.
	 
		@param[out] pImage The target image, the text will be drawn in here. Must be of type CByteImage::eGrayScale or CByteImage::eRGB24.
		@param[in] pText The text to be written.
		@param[in] x The x-coordinate of the bootem left corner of the text area in the target image.
		@param[in] y The y-coordinate of the bottom left corner of the text area in the target image.
		@param[in] scale_x The scaling in x direction. Must be > 0. scale_x = 1.0 is a good value to start with.
		@param[in] scale_y The scaling in y direction. Must be > 0. scale_y = 1.0 is a good value to start with.
		@param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		@param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		@param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		@param[in] thickness The thickness of the letters. Must be >= 1.
	*/
	void PutText(CByteImage *pImage, const char *pText, double x, double y, double scale_x, double scale_y, int r = 255, int g = 255, int b = 255, int thickness = 1);
}



#endif /* _PRIMITIVES_DRAWER_CV_H_ */
