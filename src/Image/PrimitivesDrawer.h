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
// Filename:  PrimitivesDrawer.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************
// Changes:   03.03.2010, Derick Beng Youh
//            * Added function FillPolygon
// ****************************************************************************

#ifndef _PRIMITIVES_DRAWER_H_
#define _PRIMITIVES_DRAWER_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Math/Math2d.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;
struct Circle2d;
struct Ellipse2d;
struct StraightLine2d;
struct PointPair2d;
struct MyRegion;




// ****************************************************************************
// PrimitivesDrawer
// ****************************************************************************

/*!
	\ingroup ImageProcessing
	\brief Functions for drawing 2D primitives into images.
*/
namespace PrimitivesDrawer
{
	/*!
		\brief Draws a point into a CByteImage.
	 
		The function is safe, i.e. it checks for image boundaries. If the point is outside the image, this function returns.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		\param[out] pImage The target image.
		\param[in] point The location of the point to be drawn.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
	*/
	void DrawPoint(CByteImage *pImage, const Vec2d &point, int r = 255, int g = 255, int b = 255);
	
	/*!
		\brief Draws a point into a CByteImage.
	 
		The function is safe, i.e. it checks for image boundaries. If the point is outside the image, this function returns.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		\param[out] pImage The target image.
		\param[in] x The x-coordinate of the point to be drawn.
		\param[in] y The y-coordinate of the point to be drawn.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
	*/
	void DrawPoint(CByteImage *pImage, int x, int y, int r = 255, int g = 255, int b = 255);

	/*!
		\brief Draws a point into a CByteImage, with blending.
	 
		The function is safe, i.e. it checks for image boundaries. If the point is outside the image, this function returns.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		\param[out] pImage The target image.
		\param[in] point The location of the point to be drawn.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] blend The parameter for adjusting the amount of blending. It must be 0.0f <= blend <= 1.0f. blend = 1.0f means that no transparency will be applied.
	*/
	void DrawPointBlended(CByteImage *pImage, const Vec2d &point, int r = 255, int g = 255, int b = 255, float blend = 1.0f);
	
	/*!
		\brief Draws a point into a CByteImage, with blending.
	 
		The function is safe, i.e. it checks for image boundaries. If the point is outside the image, this function returns.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		\param[out] pImage The target image.
		\param[in] x The x-coordinate of the point to be drawn.
		\param[in] y The y-coordinate of the point to be drawn.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] blend The parameter for adjusting the amount of blending. It must be 0.0f <= blend <= 1.0f. blend = 1.0f means that no transparency will be applied.
	*/
	void DrawPointBlended(CByteImage *pImage, int x, int y, int r = 255, int g = 255, int b = 255, float blend = 1.0f);

	/*!
		\brief Draws a list of points of type CVec2dArray into a CByteImage.
	 
		The function is safe, i.e. it checks for image boundaries. If the point is outside the image, this function returns.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		\param[out] pImage The target image.
		\param[in] points The list of type CVec2dArray, containing the points to be drawn.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
	*/
	void DrawPoints(CByteImage *pImage, const CVec2dArray &points, int r = 255, int g = 255, int b = 255);
	
	/*!
		\brief Draws a rectangle into a CByteImage.
	 
		The function is safe, i.e. it checks for image boundaries. Rectangle points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		\param[out] pImage The target image.
		\param[in] region This parameter specifies the location and size of the rectangle.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the lines.
	*/
	void DrawRegion(CByteImage *pImage, const MyRegion &region, int r = 255, int g = 255, int b = 255, int thickness = 1);

	/*!
		\brief Draws a circle into a CByteImage.
	 
		The function is safe, i.e. it checks for image boundaries. Circle points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		\param[out] pImage The target image.
		\param[in] mx The x-coordinate of the center of the circle.
		\param[in] my The x-coordinate of the center of the circle.
		\param[in] radius The radius of the circle.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the lines.
		\param[in] bAntiAlias If set to true, anti-aliasing will be applied for drawing.
	*/
	void DrawCircle(CByteImage *pImage, float mx, float my, float radius, int r = 255, int g = 255, int b = 255, int thickness = 1, bool bAntiAlias = false);

	/*!
		\brief Draws a circle into a CByteImage.

		The function is safe, i.e. it checks for image boundaries. Circle points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).

		Internally, the function PrimitivesDrawer::DrawCircle(CByteImage*, float, float, float, int, int, int, int, bool) is called.
	 
		\param[out] pImage The target image.
		\param[in] center The center of the circle.
		\param[in] radius The radius of the circle.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the lines.
		\param[in] bAntiAlias If set to true, anti-aliasing will be applied for drawing.
	*/
	void DrawCircle(CByteImage *pImage, const Vec2d &center, float radius, int r = 255, int g = 255, int b = 255, int thickness = 1, bool bAntiAlias = false);

	/*!
		\brief Draws a circle into a CByteImage.

		The function is safe, i.e. it checks for image boundaries. Circle points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).

		Internally, the function PrimitivesDrawer::DrawCircle(CByteImage*, float, float, float, int, int, int, int, bool) is called.
	 
		\param[out] pImage The target image.
		\param[in] circle The circle representation.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the lines.
		\param[in] bAntiAlias If set to true, anti-aliasing will be applied for drawing.
	*/
	void DrawCircle(CByteImage *pImage, const Circle2d &circle, int r = 255, int g = 255, int b = 255, int thickness = 1, bool bAntiAlias = false);

	/*!
		\brief Draws an ellipse into a CByteImage.
	 
		The function is safe, i.e. it checks for image boundaries. Ellipse points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		\param[out] pImage The target image.
		\param[in] ellipse This parameter specifies the location, orientation and shape of the ellipse.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the lines.
	*/
	void DrawEllipse(CByteImage *pImage, const Ellipse2d &ellipse, int r = 255, int g = 255, int b = 255, int thickness = 1);

	/*!
		\brief Draws a line segment into a CByteImage, given its two end points.
	 
		The function is safe, i.e. it checks for image boundaries. Line points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).

		Internally, the function DrawLine(CByteImage*, const Vec2d&, const Vec2d&, int, int, int, int) is called.
	 
		\param[out] pImage The target image.
		\param[in] line This parameter specifies the two end points of the straight line segment.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the line.
	*/
	void DrawLine(CByteImage *pImage, const PointPair2d &line, int r = 255, int g = 255, int b = 255, int thickness = 1);

	/*!
		\brief Draws a line segment into a CByteImage, given its two end points.
	 
		The function is safe, i.e. it checks for image boundaries. Line points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).

		\param[out] pImage The target image.
		\param[in] p1 The first end point of the straight line segment.
		\param[in] p2 The second end point of the straight line segment.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the line.
	*/
	void DrawLine(CByteImage *pImage, const Vec2d &p1, const Vec2d &p2, int r = 255, int g = 255, int b = 255, int thickness = 1);

	/*!
		\brief Draws a straight line into a CByteImage, given its parameters in polar form.

		The equation of the straight line reads:

		\f$r = x\cdot\cos\theta + y\cdot\sin\theta\f$.
	 
		The function is safe, i.e. it checks for image boundaries. Line points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).

		Internally, the function PrimitivesDrawer::DrawLineNormal(CByteImage*, float, float, float, int, int, int) is called.
	 
		\param[out] pImage The target image.
		\param[in] theta The angle of the normal to the line in radians. theta = 0.0f draws a vertical line.
		\param[in] r The distance of the straight line to the origin in pixels.
		\param[in] color_r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] color_g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] color_b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the line.
	*/
	void DrawLinePolar(CByteImage *pImage, float theta, float r, int color_r, int color_g, int color_b, int thickness = 1);

	/*!
		\brief Draws a line into a CByteImage, given its parameters in normal form.
		
		The equation of the straight line reads:

		\f$c = x \cdot n_x + y \cdot n_y\f$
	 
		The function is safe, i.e. it checks for image boundaries. Line points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		\param[out] pImage The target image.
		\param[in] nx The x-component of the normal vector.
		\param[in] ny The y-component of the normal vector.
		\param[in] c If nx and ny are normalized, this parameter specifies the distance of the straight line to the origin in pixels.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the line.
	*/
	void DrawLineNormal(CByteImage *pImage, float nx, float ny, float c, int r, int g, int b, int thickness = 1);

	/*!
		\brief Draws a line into a CByteImage, given its representation as StraightLine2d.

		The function is safe, i.e. it checks for image boundaries. Line points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
		
		Internally, the function PrimitivesDrawer::DrawLineNormal(CByteImage*, float, float, float, int, int, int) is called.
	 
		\param[out] pImage The target image.
		\param[in] line The line representation.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the line.
	*/
	void DrawLine(CByteImage *pImage, const StraightLine2d &line, int r, int g, int b, int thickness = 1);

	/*!
		\brief Draws a small cross into a CByteImage.
		
		The cross consists of two line segments (a vertical and a horizontal one) of same size intersecting each other in the center of the cross.
	 
		The function is safe, i.e. it checks for image boundaries. Points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		\param[out] pImage The target image.
		\param[in] point The center of the cross.
		\param[in] length The length of each of the two lines of the cross.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
	*/
	void DrawCross(CByteImage *pImage, const Vec2d &point, int length, int r, int g, int b);

	/*!
		\brief Draws a polygon into a CByteImage.
	 
		The provided points must build a polygon in the given order.

		The function is safe, i.e. it checks for image boundaries. Polygon points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		\param[out] pImage The target image.
		\param[in] pPoints An array of points containing the points of the polygon in order.
		\param[in] nPoints The number of points. Must be >= 3.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the lines. The polygon will be closed if the first and last point are not equal. If set to -1 the polygon is filled, otherwise must be >= 1.
	*/
	void DrawPolygon(CByteImage *pImage, const Vec2d *pPoints, int nPoints, int r, int g, int b, int thickness = 1);

	/*!
		\brief Draws a polygon into a CByteImage.

		The provided points must build a polygon in the given order.
	 
		The function is safe, i.e. it checks for image boundaries. Polygon points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).

		This is a convenience function calling DrawPolygon(CByteImage*, const Vec2d*, int, int, int, int, int);
	 
		\param[out] pImage The target image.
		\param[in] points The list of points containing the points of the polygon in order.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the lines. The polygon will be closed if the first and last point are not equal. If set to -1 the polygon is filled, otherwise must be >= 1.
	*/
	void DrawPolygon(CByteImage *pImage, const CVec2dArray &points, int r, int g, int b, int thickness = 1);

	/*!
		\brief Draws a rectangle into a CByteImage.

		The function is safe, i.e. it checks for image boundaries. Polygon points lying outside the image will not be drawn.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).

		\param[out] pImage The target image.
		\param[in] rectangle The rectangle representation.
		\param[in] r The red component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] g The green component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] b The blue component (RGB) of the color to be used for drawing. It must be 0 <= r <= 255.
		\param[in] thickness This parameter specifies the thickness of the lines. The polygon will be closed if the first and last point are not equal. If set to -1 the polygon is filled, otherwise must be >= 1.
	*/
	void DrawRectangle(CByteImage *pImage, const Rectangle2d &rectangle, int r, int g, int b, int thickness = 1);
}



#endif /* _PRIMITIVES_DRAWER_H_ */
