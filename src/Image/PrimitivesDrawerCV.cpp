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
// Filename:  PrimitivesDrawerCV.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "PrimitivesDrawerCV.h"

#include "Math/Constants.h"
#include "Structs/Structs.h"
#include "Image/IplImageAdaptor.h"
#include "Image/ByteImage.h"

#include <cv.h>



// ****************************************************************************
// Functions
// ****************************************************************************

void PrimitivesDrawerCV::DrawCircle(CByteImage *pImage, const Vec2d &center, double radius, int r, int g, int b, int thickness)
{
	IplImage *pIplImage = IplImageAdaptor::Adapt(pImage);
	cvCircle(pIplImage, cvPoint(int(center.x + 0.5), int(center.y + 0.5)), int(radius + 0.5), CV_RGB(b, g, r), thickness);
	cvReleaseImageHeader(&pIplImage);
}

void PrimitivesDrawerCV::DrawEllipse(CByteImage *pImage, const Ellipse2d &ellipse, int r, int g, int b, int thickness)
{
	IplImage *pIplImage = IplImageAdaptor::Adapt(pImage);
	cvEllipse(pIplImage, cvPoint(int(ellipse.center.x + 0.5), int(ellipse.center.y + 0.5)), cvSize(int(ellipse.radius_x + 0.5), int(ellipse.radius_y + 0.5)), -ellipse.angle * FLOAT_RAD2DEG, 0.0, 360.0, CV_RGB(b, g, r), thickness);
	cvReleaseImageHeader(&pIplImage);
}

void PrimitivesDrawerCV::DrawLine(CByteImage *pImage, const PointPair2d &line, int r, int g, int b, int thickness)
{
	IplImage *pIplImage = IplImageAdaptor::Adapt(pImage);
	cvLine(pIplImage, cvPoint(int(line.p1.x + 0.5), int(line.p1.y + 0.5)), cvPoint(int(line.p2.x + 0.5), int(line.p2.y + 0.5)), CV_RGB(b, g, r), thickness);
	cvReleaseImageHeader(&pIplImage);
}

void PrimitivesDrawerCV::DrawLine(CByteImage *pImage, const Vec2d &p1, const Vec2d &p2, int r, int g, int b, int thickness)
{
	IplImage *pIplImage = IplImageAdaptor::Adapt(pImage);
	cvLine(pIplImage, cvPoint(int(p1.x + 0.5), int(p1.y + 0.5)), cvPoint(int(p2.x + 0.5), int(p2.y + 0.5)), CV_RGB(b, g, r), thickness);
	cvReleaseImageHeader(&pIplImage);
}

void PrimitivesDrawerCV::DrawRegion(CByteImage *pImage, const MyRegion &region, int r, int g, int b, int thickness)
{
	IplImage *pIplImage = IplImageAdaptor::Adapt(pImage);
	cvRectangle(pIplImage, cvPoint(region.min_x, region.min_y), cvPoint(region.max_x, region.max_y), CV_RGB(b, g, r), thickness);
	cvReleaseImageHeader(&pIplImage);
}

void PrimitivesDrawerCV::DrawConvexPolygon(CByteImage *pImage, int *pPoints, int nPoints, int r, int g, int b, int thickness)
{
	if (nPoints < 2)
	{
		printf("error: at least to points must be provided for PrimitivesDrawerCV::DrawConvexPolygon\n");
		return;
	}
	
	CvPoint *points = new CvPoint[nPoints];
	for (int i = 0, index = 0; i < nPoints; i++, index += 2)
	{
		points[i].x = pPoints[index];
		points[i].y = pPoints[index + 1];
	}


	IplImage *pIplImage = IplImageAdaptor::Adapt(pImage);
	if (thickness == -1)
		cvFillConvexPoly(pIplImage, points, nPoints, CV_RGB(b, g, r));
	else
	{
		for (int i = 0; i < nPoints - 1; i++)
			cvLine(pIplImage, points[i], points[i + 1], CV_RGB(b, g, r), thickness);
			
		if (points[0].x != points[nPoints - 1].x || points[0].y != points[nPoints - 1].y)
			cvLine(pIplImage, points[nPoints - 1], points[0], CV_RGB(b, g, r), thickness);
	}
	cvReleaseImageHeader(&pIplImage);
	
	delete [] points;
}

void PrimitivesDrawerCV::PutText(CByteImage *pImage, const char *pText, double x, double y, double scale_x, double scale_y, int r, int g, int b, int thickness)
{
	CvFont font;
	cvInitFont(&font, CV_FONT_VECTOR0, scale_x, scale_y, 0, thickness);
	IplImage *pIplImage = IplImageAdaptor::Adapt(pImage);
	cvPutText(pIplImage, pText, cvPoint(int(x + 0.5), int(y + 0.5)), &font, CV_RGB(b, g, r));
	cvReleaseImageHeader(&pIplImage);
}
