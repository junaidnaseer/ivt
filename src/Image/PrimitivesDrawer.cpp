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
// Filename:  PrimitivesDrawer.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************
// Changes:   03.03.2010, Derick Beng Youh
//            * Added function FillPolygon
// ****************************************************************************

// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

// IVT
#include "PrimitivesDrawer.h"
#include "Image/ByteImage.h"
#include "Math/Math2d.h"
#include "Structs/Structs.h"
#include "Helpers/helpers.h"

// system
#include <math.h>
#include <limits>



// ****************************************************************************
// Helper functions
// ****************************************************************************

static bool is_indeterminate(const float fValue)
{
	return fValue != fValue;
} 

static bool is_infinite(const float fValue)
{
	return fabsf(fValue) == std::numeric_limits<float>::infinity();
}


// ****************************************************************************
// Functions
// ****************************************************************************

void PrimitivesDrawer::DrawPoint(CByteImage *pImage, const Vec2d &point, int r, int g, int b)
{
	DrawPoint(pImage, int(point.x + 0.5f), int(point.y + 0.5f), r, g, b);
}

void PrimitivesDrawer::DrawPoint(CByteImage *pImage, int x, int y, int r, int g, int b)
{
	const int width = pImage->width;
	const int height = pImage->height;
	const int nPixels = width * height;

	if (x < 0 || x >= width || y < 0 || y >= height)
		return;
	
	if (pImage->type == CByteImage::eGrayScale)
	{
		pImage->pixels[y * width + x] = (r + (g << 1) + b) >> 2;
	}
	else if (pImage->type == CByteImage::eRGB24)
	{
		const int offset = 3 * (y * width + x);
		pImage->pixels[offset] = r;
		pImage->pixels[offset + 1] = g;
		pImage->pixels[offset + 2] = b;
	}
	else if (pImage->type == CByteImage::eRGB24Split)
	{
		const int offset = y * width + x;
		pImage->pixels[offset] = r;
		pImage->pixels[offset + nPixels] = g;
		pImage->pixels[offset + (nPixels << 1)] = b;
	}
}

void PrimitivesDrawer::DrawPointBlended(CByteImage *pImage, const Vec2d &point, int r, int g, int b, float blend)
{
	DrawPointBlended(pImage, int(point.x + 0.5f), int(point.y + 0.5f), r, g, b, blend);
}

void PrimitivesDrawer::DrawPointBlended(CByteImage *pImage, int x, int y, int r, int g, int b, float blend)
{
	const int width = pImage->width;
	const int height = pImage->height;
	const int nPixels = width * height;

	if (x < 0 || x >= width || y < 0 || y >= height)
		return;
	
	if (pImage->type == CByteImage::eGrayScale)
	{
		const float o = (float) pImage->pixels[y * width + x];
		const float n = (float) ((r + (g << 1) + b + 1) >> 2);

		pImage->pixels[y * width + x] = (int) ((1.0f - blend) * o + blend * n);
	}
	else if (pImage->type == CByteImage::eRGB24)
	{
		const int offset = 3 * (y * width + x);
		int ro = pImage->pixels[offset];
		int go = pImage->pixels[offset + 1];
		int bo = pImage->pixels[offset + 2];
		
		r = (int) ((1.0f - blend) * (float) ro + blend * (float) r);
		g = (int) ((1.0f - blend) * (float) go + blend * (float) g);
		b = (int) ((1.0f - blend) * (float) bo + blend * (float) b);
		
		pImage->pixels[offset] = r;
		pImage->pixels[offset + 1] = g;
		pImage->pixels[offset + 2] = b;
	}
	else if (pImage->type == CByteImage::eRGB24Split)
	{
		const int offset = y * width + x;

		int ro = pImage->pixels[offset];
		int go = pImage->pixels[offset + nPixels];
		int bo = pImage->pixels[offset + (nPixels << 1)];
		
		r = (int) ((1.0f - blend) * (float) ro + blend * (float) r);
		g = (int) ((1.0f - blend) * (float) go + blend * (float) g);
		b = (int) ((1.0f - blend) * (float) bo + blend * (float) b);
		
		pImage->pixels[offset] = r;
		pImage->pixels[offset + nPixels] = g;
		pImage->pixels[offset + (nPixels << 1)] = b;
	}
}

void PrimitivesDrawer::DrawPoints(CByteImage *pImage, const CVec2dArray &points, int r, int g, int b)
{
	const int nPoints = points.GetSize();

	for (int i = 0; i < nPoints; i++)
		DrawPoint(pImage, points[i], r, g, b);
}


void PrimitivesDrawer::DrawRegion(CByteImage *pImage, const MyRegion &region, int r, int g, int b, int thickness)
{
	if (thickness == -1)
	{
		for (int y = region.min_y; y <= region.max_y; y++)
			for (int x = region.min_x; x <= region.max_x; x++)
				DrawPoint(pImage, x, y, r, g, b);
	}
	else
	{
		const Vec2d p1 = { float(region.min_x), float(region.min_y) };
		const Vec2d p2 = { float(region.max_x), float(region.min_y) };
		const Vec2d p3 = { float(region.max_x), float(region.max_y) };
		const Vec2d p4 = { float(region.min_x), float(region.max_y) };

		DrawLine(pImage, p1, p2, r, g, b, thickness);
		DrawLine(pImage, p2, p3, r, g, b, thickness);
		DrawLine(pImage, p3, p4, r, g, b, thickness);
		DrawLine(pImage, p4, p1, r, g, b, thickness);
	}
}

void PrimitivesDrawer::DrawLine(CByteImage *pImage, const Vec2d &p1, const Vec2d &p2, int r, int g, int b, int thickness)
{
	if (is_indeterminate(p1.x) || is_indeterminate(p1.y) || is_indeterminate(p2.x) || is_indeterminate(p2.y))
		return;
	
	if (is_infinite(p1.x) || is_infinite(p1.y) || is_infinite(p2.x) || is_infinite(p2.y))
		return;

	const float dx = p1.x - p2.x;
	const float dy = p1.y - p2.y;
	
	if (thickness == 1)
	{
		if (fabsf(dx) < 0.01f && fabsf(dy) < 0.01f)
		{
			PrimitivesDrawer::DrawPoint(pImage, p1, r, g, b);
		}
		else if (fabsf(dy) < fabsf(dx))
		{
			const float slope = dy / dx;
			const int max_x = int(p2.x + 0.5f);
			float y = p1.y + 0.5f;

			if (p1.x < p2.x)
			{
				for (int x = int(p1.x + 0.5f); x <= max_x; x++, y += slope)
					PrimitivesDrawer::DrawPoint(pImage, x, int(y), r, g, b);
			}
			else
			{
				for (int x = int(p1.x + 0.5f); x >= max_x; x--, y -= slope)
					PrimitivesDrawer::DrawPoint(pImage, x, int(y), r, g, b);
			}
		}
		else
		{
			const float slope = dx / dy;
			const int max_y = int(p2.y + 0.5f);
			float x = p1.x + 0.5f;

			if (p1.y < p2.y)
			{
				for (int y = int(p1.y + 0.5f); y <= max_y; y++, x += slope)
					PrimitivesDrawer::DrawPoint(pImage, int(x), y, r, g, b);
			}
			else
			{
				for (int y = int(p1.y + 0.5f); y >= max_y; y--, x -= slope)
					PrimitivesDrawer::DrawPoint(pImage, int(x), y, r, g, b);
			}
		}
	}
	else
	{
		const float radius = 0.5f * thickness;

		if (fabsf(dx) < 0.01f && fabsf(dy) < 0.01f)
		{
			PrimitivesDrawer::DrawCircle(pImage, p1, radius, r, g, b, -1);
		}
		else if (fabsf(dy) < fabsf(dx))
		{
			const float slope = dy / dx;
			const int max_x = int(p2.x + 0.5f);
			float y = p1.y + 0.5f;

			if (p1.x < p2.x)
			{
				for (int x = int(p1.x + 0.5f); x <= max_x; x++, y += slope)
					PrimitivesDrawer::DrawCircle(pImage, float(x), floor(y), radius, r, g, b, -1);
			}
			else
			{
				for (int x = int(p1.x + 0.5f); x >= max_x; x--, y -= slope)
					PrimitivesDrawer::DrawCircle(pImage, float(x), floor(y), radius, r, g, b, -1);
			}
		}
		else
		{
			const float slope = dx / dy;
			const int max_y = int(p2.y + 0.5f);
			float x = p1.x + 0.5f;

			if (p1.y < p2.y)
			{
				for (int y = int(p1.y + 0.5f); y <= max_y; y++, x += slope)
					PrimitivesDrawer::DrawCircle(pImage, floor(x), float(y), radius, r, g, b, -1);
			}
			else
			{
				for (int y = int(p1.y + 0.5f); y >= max_y; y--, x -= slope)
					PrimitivesDrawer::DrawCircle(pImage, floor(x), float(y), radius, r, g, b, -1);
			}
		}
	}
}

void PrimitivesDrawer::DrawLine(CByteImage *pImage, const PointPair2d &line, int r, int g, int b, int thickness)
{
	DrawLine(pImage, line.p1, line.p2, r, g, b, thickness);
}

void DrawCircleAA(CByteImage *pImage, float mx, float my, float radius, int r, int g, int b, int thickness)
{
	const int min_x = int(mx - radius);
	const int min_y = int(my - radius);
	const int max_x = int(mx + radius);
	const int max_y = int(my + radius);
	int i;
	
	const float r2 = radius * radius;
				
	if (thickness == -1)
	{
		for (i = min_y; i <= max_y; i++)
		{
			const float d = r2 - (my - (float)i) * (my - (float)i);

			if (d >= 0)
			{
				const float x = sqrtf((float)d);

				int sx = (int)ceil(mx - x);
				int ex = (int)floor(mx + x);
				
				float sb = (float)sx - (mx - x);
				float eb = (mx + x) - (float)ex;
				
				PrimitivesDrawer::DrawPointBlended(pImage, sx, i, r, g, b, sb);

				for (int j = sx+1; j <= ex-1; j++)
					PrimitivesDrawer::DrawPoint(pImage, j, i, r, g, b);

				PrimitivesDrawer::DrawPointBlended(pImage, ex, i, r, g, b, eb);
			}
		}
	}
	else
	{
		if (thickness == 1)
		{
			for (i = min_y; i <= max_y; i++)
			{
				const float d = r2 - (my - (float)i) * (my - (float)i);

				if (d >= 0)
				{
					const float x = sqrtf(d);

					int sx = (int)ceil(mx - x);
					int ex = (int)floor(mx + x);
					
					float sb = (float)sx - (mx - x);
					float eb = (mx + x) - (float)ex;
					
					PrimitivesDrawer::DrawPointBlended(pImage, sx-1, i, r, g, b, sb);
					PrimitivesDrawer::DrawPointBlended(pImage, sx, i, r, g, b, 1.0f - sb);
					
					PrimitivesDrawer::DrawPointBlended(pImage, ex, i, r, g, b, 1.0f - eb);
					PrimitivesDrawer::DrawPointBlended(pImage, ex+1, i, r, g, b, eb);
				}
			}
			
			for (i = min_x; i <= max_x; i++)
			{
				const float d = r2 - (mx - (float)i) * (mx - (float)i);

				if (d >= 0)
				{
					const float y = sqrtf(d);

					int sy = (int)ceil(my - y);
					int ey = (int)floor(my + y);
					
					float sb = (float)sy - (my - y);
					float eb = (my + y) - (float)ey;
				
					PrimitivesDrawer::DrawPointBlended(pImage, i, sy-1, r, g, b, sb);
					PrimitivesDrawer::DrawPointBlended(pImage, i, sy, r, g, b, 1.0f - sb);
					
					PrimitivesDrawer::DrawPointBlended(pImage, i, ey, r, g, b, 1.0f - eb);
					PrimitivesDrawer::DrawPointBlended(pImage, i, ey+1, r, g, b, eb);
				}
			}
		}
		else
		{
			const float radius = 0.5f * (float)thickness;

			for (i = min_x; i <= max_x; i++)
			{
				const float d = r2 - (mx - i) * (mx - i);

				if (d >= 0.0f)
				{
					const float y = sqrtf(d);
					DrawCircleAA(pImage, float(i), floor(my + y), radius, r, g, b, -1);
					DrawCircleAA(pImage, float(i), floor(my - y), radius, r, g, b, -1);
				}
			}
			
			for (i = min_y; i <= max_y; i++)
			{
				const float d = r2 - (my - i) * (my - i);

				if (d >= 0.0f)
				{
					const float x = sqrtf(d);
					DrawCircleAA(pImage, floor(mx + x), float(i), radius, r, g, b, -1);
					DrawCircleAA(pImage, floor(mx - x), float(i), radius, r, g, b, -1);
				}
			}
		}
	}
}

void PrimitivesDrawer::DrawCircle(CByteImage *pImage, float mx, float my, float radius, int r, int g, int b, int thickness, bool antiAlias)
{
	if (antiAlias)
	{
		DrawCircleAA(pImage, mx, my, radius, r, g, b, thickness);
		return;
	}

	const int mx_int = my_round(mx);
	const int my_int = my_round(my);
	const int radius_int = my_round(radius);

	mx = float(mx_int);
	my = float(my_int);
	
	const int min_x = mx_int - radius_int;
	const int min_y = my_int - radius_int;
	const int max_x = mx_int + radius_int;
	const int max_y = my_int + radius_int;
	
	const int r2 = radius_int * radius_int;
	
	if (thickness == -1)
	{
		int i;
		
		for (i = min_y; i <= max_y; i++)
		{
			const int d = r2 - (my_int - i) * (my_int - i);

			if (d >= 0)
			{
				const int x = int(sqrtf(float(d)) + 0.5f);

				const int sx = mx_int - x;
				const int ex = mx_int + x;
				
				for (int j = sx; j <= ex; j++)
					PrimitivesDrawer::DrawPoint(pImage, j, i, r, g, b);
			}
		}
		
		for (i = min_x; i <= max_x; i++)
		{
			const int d = r2 - (mx_int - i) * (mx_int - i);

			if (d >= 0)
			{
				const int y = int(sqrtf(float(d)) + 0.5f);

				PrimitivesDrawer::DrawPoint(pImage, i, my_int - y, r, g, b);
				PrimitivesDrawer::DrawPoint(pImage, i, my_int + y, r, g, b);
			}
		}
	}
	else
	{
		if (thickness == 1)
		{
			int i;

			for (i = min_y; i <= max_y; i++)
			{
				const int d = r2 - (my_int - i) * (my_int - i);

				if (d >= 0)
				{
					const int x = int(sqrtf(float(d)) + 0.5f);
				
					PrimitivesDrawer::DrawPoint(pImage, mx_int - x, i, r, g, b);
					PrimitivesDrawer::DrawPoint(pImage, mx_int + x, i, r, g, b);
				}
			}
			
			for (i = min_x; i <= max_x; i++)
			{
				const int d = r2 - (mx_int - i) * (mx_int - i);

				if (d >= 0)
				{
					const int y = int(sqrtf(float(d)) + 0.5f);

					PrimitivesDrawer::DrawPoint(pImage, i, my_int - y, r, g, b);
					PrimitivesDrawer::DrawPoint(pImage, i, my_int + y, r, g, b);
				}
			}
		}
		else
		{
			const float fThicknessRadius = 0.5f * thickness;

			int i;

			for (i = min_x; i <= max_x; i++)
			{
				const int d = r2 - (mx_int - i) * (mx_int - i);

				if (d >= 0)
				{
					const int y = int(sqrtf(float(d)) + 0.5f);

					PrimitivesDrawer::DrawCircle(pImage, float(i), float(my_int - y), fThicknessRadius, r, g, b, -1);
					PrimitivesDrawer::DrawCircle(pImage, float(i), float(my_int + y), fThicknessRadius, r, g, b, -1);
				}
			}
			
			for (i = min_y; i <= max_y; i++)
			{
				const int d = r2 - (my_int - i) * (my_int - i);

				if (d >= 0)
				{
					const int x = int(sqrtf(float(d)) + 0.5f);

					PrimitivesDrawer::DrawCircle(pImage, float(mx_int - x), float(i), fThicknessRadius, r, g, b, -1);
					PrimitivesDrawer::DrawCircle(pImage, float(mx_int + x), float(i), fThicknessRadius, r, g, b, -1);
				}
			}
		}
	}
}

void PrimitivesDrawer::DrawCircle(CByteImage *pImage, const Vec2d &center, float radius, int r, int g, int b, int thickness, bool antiAlias)
{
	DrawCircle(pImage, center.x, center.y, radius, r, g, b, thickness, antiAlias);
}

void PrimitivesDrawer::DrawCircle(CByteImage *pImage, const Circle2d &circle, int r, int g, int b, int thickness, bool antiAlias)
{
	DrawCircle(pImage, circle.center.x, circle.center.y, circle.radius, r, g, b, thickness, antiAlias);
}

void PrimitivesDrawer::DrawLineNormal(CByteImage *pImage, float nx, float ny, float c, int r, int g, int b, int thickness)
{
	if (thickness == 1)
	{
		if (fabsf(nx) > fabsf(ny))
		{
			const float m = -ny / nx;
			const float cc = -c / nx;
			
			for (int y = 0; y < pImage->height; y++)
			{
				Vec2d p = { floor(m * y + cc + 0.5f), float(y) };
				PrimitivesDrawer::DrawPoint(pImage, p, r, g, b);
			}
		}
		else
		{
			const float m = -nx / ny;
			const float cc = -c / ny;
			
			for (int x = 0; x < pImage->width; x++)
			{
				Vec2d p = { float(x), floor(m * x + cc + 0.5f) };
				PrimitivesDrawer::DrawPoint(pImage, p, r, g, b);
			}
		}
	}
	else
	{
		const float radius = 0.5f * thickness;

		if (fabsf(nx) > fabsf(ny))
		{
			const float m = -ny / nx;
			const float cc = -c / nx;
			
			for (int y = 0; y < pImage->height; y++)
			{
				Vec2d p = { floor(m * y + cc + 0.5f), float(y) };
				PrimitivesDrawer::DrawCircle(pImage, p, radius, r, g, b);
			}
		}
		else
		{
			const float m = -nx / ny;
			const float cc = -c / ny;
			
			for (int x = 0; x < pImage->width; x++)
			{
				Vec2d p = { float(x), floor(m * x + cc + 0.5f) };
				PrimitivesDrawer::DrawCircle(pImage, p, radius, r, g, b);
			}
		}
	}
}

void PrimitivesDrawer::DrawLinePolar(CByteImage *pImage, float theta, float r, int color_r, int color_g, int color_b, int thickness)
{
	DrawLineNormal(pImage, cosf(theta), sinf(theta), -r, color_r, color_g, color_b, thickness);
}

void PrimitivesDrawer::DrawLine(CByteImage *pImage, const StraightLine2d &line, int r, int g, int b, int thickness)
{
	DrawLineNormal(pImage, line.normalVector.x, line.normalVector.y, line.c, r, g, b, thickness);
}

void PrimitivesDrawer::DrawCross(CByteImage *pImage, const Vec2d &point, int length, int r, int g, int b)
{
	const int x = int(point.x + 0.5f);
	const int y = int(point.y + 0.5f);
	
	const int l = length / 2;
	
	for (int i = -l; i <= l; i++)
	{
		PrimitivesDrawer::DrawPoint(pImage, x + i, y, r, g, b);
		PrimitivesDrawer::DrawPoint(pImage, x, y + i, r, g, b);
	}
}




struct Edge 
{
	Edge()
	{
		next = 0;
	}

	// maximum y-coordinate between p1 and p2
	int max_y;

	// point of intersection between an edge and a scanline
	float x_intersection;
	
	// inverse of the gradient of an edge dx / dy
	float inverse_gradient;

	// the next edge in the linked list
	Edge *next; 
};

struct Vec2dInt 
{
	int x;
	int y;
};

static void InsertEdge(Edge *ppEdgeList, Edge *pEdge) 
{
	// inserts edge into linked list in order of increasing x_intersection field
	Edge *q = ppEdgeList;
	Edge *p = q->next;
     
    while (p)
	{
		if (pEdge->x_intersection < p->x_intersection)
			break;

		q = p;
		p = p->next;
	}

	pEdge->next = q->next; 
	q->next = pEdge;
}

// ****************************************************************************
// Store lower-y coordinate and inverse slope for each edge.
// Adjust and store max_y coordinate for edges that are the lower member
// of a monotically increasing or decreasing pair of edges.
// ****************************************************************************
static void MakeEdgeRecord(const Vec2dInt &lower, const Vec2dInt &upper, int yComp, Edge *pEdge, Edge **ppEdgeList, int height)
{
	pEdge->inverse_gradient = float(upper.x - lower.x) / float(upper.y - lower.y);
	pEdge->x_intersection = float(lower.x);
   
	if (upper.y < yComp)
		pEdge->max_y = upper.y - 1;
	else
		pEdge->max_y = upper.y;
    
	if (lower.y >= 0 && lower.y < height)
		InsertEdge(ppEdgeList[lower.y], pEdge);
}

// ****************************************************************************
// For nIndex, return y-coordinate of the next non-horizontal edge which
// can be created by connecting the vertices in pPoints in their respective
// order to one another.
//
// nIndex: index of Vec2dInt to start with
// pPoints: array of points
// nPoints: number of points
// ****************************************************************************
static int NextY(int nIndex, const Vec2dInt *pPoints, int nVertices)
{
	//j = k % count - 1 
	int j = nIndex + 1 > nVertices - 1 ? 0 : nIndex + 1;

	while (pPoints[nIndex].y == pPoints[j].y)
	{
		if (j + 1 > nVertices - 1)
			j = 0;
		else
			j++;
	}
           
	return pPoints[j].y;
}

// ****************************************************************************
// Function used for creating a list of all edges, according to the points in
// in pPoints, respecting their order, that is pts[0]->pts[1]->..pts[n-1]->pts[0]
// 
// pPoints: array of points of the polygon with order of connection
// nPoints: the number of points of the polygon
// ppEdges: result parameter for the list of edges to be computed
//****************************************************************************
static void BuildEdgeList(const Vec2dInt *pPoints, int nPoints, Edge **ppResultEdges, int height)
{
	int nPreviousY = pPoints[nPoints - 2].y;
	Vec2dInt v1 = { pPoints[nPoints - 1].x, pPoints[nPoints - 1].y };

	for (int i = 0; i < nPoints; i++) 
	{
		Vec2dInt v2 = pPoints[i];
       
		if (v1.y != v2.y) // non-horizontal line
		{
			Edge *pEdge = new Edge();
			
			if (v1.y < v2.y)
				MakeEdgeRecord(v1, v2, NextY(i, pPoints, nPoints), pEdge, ppResultEdges, height); // up-going edge
			else
				MakeEdgeRecord(v2, v1, nPreviousY, pEdge, ppResultEdges, height); // down-going edge
		}
       
		nPreviousY = v1.y;
		v1 = v2;
	}
}




// ****************************************************************************
// During the algorithm, when a scanline intersects an edge on its 
// upper-coordinates, when the scanning is done from up downward, then the edge
// is said to become active, so this method is used to always build a list of
// active edges during the scanning procedure.
//
// y: the y-coordinate of the scan line at every stage of scanning
// pActiveEdge: list of edges that are already active
// ppEdges: list of all the edges of the polygon each of which is tested,
//          if it intersects the scanline (y)
// ****************************************************************************
static void BuildActiveList(int y, Edge *pActiveEdge, Edge **ppEdges)
{
	Edge *p = ppEdges[y]->next;
	
	while (p) 
	{
		Edge *q = p->next;
		InsertEdge(pActiveEdge, p);
		p = q;
	}
}

// ****************************************************************************
// Having all the active edges of each scan stage, all the pixels that lie 
// horizontally between each pair of intersection points are filled with the
// desired color.
// ****************************************************************************
static void FillScan(CByteImage *pImage, int y, Edge *pActiveEdge, int r, int g, int b) 
{
	const int width = pImage->width;
	const int height = pImage->height;
	const int nPixels = width * height;

	if (y < 0 || y >= height)
		return;

	unsigned char *pixels = pImage->type == CByteImage::eRGB24 ? pImage->pixels + 3 * y * width : pImage->pixels + y * width;
	
    Edge *p1 = pActiveEdge->next;

	while (p1 && p1->next) 
	{
		const Edge *p2 = p1->next;

		const int x1 = MY_MIN(MY_MAX(int(p1->x_intersection + 0.5f), 0), pImage->width - 1);
		const int x2 = MY_MAX(MY_MIN(int(p2->x_intersection + 0.5f), pImage->width - 1), 0);
		
		if (pImage->type == CByteImage::eRGB24)
		{
			for (int x = x1, offset = 3 * x1; x <= x2; x++, offset += 3)
			{
				pixels[offset] = r;
				pixels[offset + 1] = g;
				pixels[offset + 2] = b;
			}
		}
		else if (pImage->type == CByteImage::eRGB24Split)
		{
			memset(pixels + x1, r, x2 - x1 + 1);
			memset(pixels + x1 + nPixels, g, x2 - x1 + 1);
			memset(pixels + x1 + (nPixels << 1), b, x2 - x1 + 1);
		}
          
		p1 = p2->next;
	}
}

static void FillScan(CByteImage *pImage, int y, Edge *pActiveEdge, int g) 
{
	if (y < 0 || y >= pImage->height)
		return;

	unsigned char *pixels = pImage->pixels + y * pImage->width;

    Edge *p1 = pActiveEdge->next;

	while (p1 && p1->next)
	{
		const Edge *p2 = p1->next;

		const int x1 = MY_MIN(MY_MAX(int(p1->x_intersection + 0.5f), 0), pImage->width - 1);
		const int x2 = MY_MAX(MY_MIN(int(p2->x_intersection + 0.5f), pImage->width - 1), 0);

		memset(pixels + x1, g, x2 - x1 + 1);
          
		p1 = p2->next;
	}
}

static void DeleteAfter(Edge *pEdge) 
{
	// delete edge from linked list
	Edge *p = pEdge->next;
	pEdge->next = p->next;
	delete p;
}

static void UpdateActiveList(int y, Edge *pActiveEdge) 
{
	// delete completed edges
	// update attribute x_intersection field for others which are still active
	Edge *q = pActiveEdge;
	Edge *p = pActiveEdge->next;
      
	while (p)
	{
		if (y >= p->max_y) 
		{
			p = p->next;
			DeleteAfter(q);
		}
		else
		{
			p->x_intersection += p->inverse_gradient;
			q = p;
			p = p->next;
		}
	}
}

static void ResortActiveList(Edge *pActiveEdge) 
{
	// resort active list after it has been updated
	Edge *p = pActiveEdge->next;
	pActiveEdge->next = 0;
	
	while (p) 
	{
		Edge *q = p->next;
		InsertEdge(pActiveEdge, p);
		p = q;
	}
}


// ****************************************************************************
// Implementation of the scan line fill algorithm by Derick Beng Yuh.
//
// pImage: the target image
// pPoints: array of points of the polygon with order of connection
// nPoints: the number of points of the polygon
// r, g, b: target color in RGB
// ****************************************************************************
static void FillPolygon(CByteImage *pImage, const Vec2d *pPoints, int nPoints, int r, int g, int b) 
{
	const int height = pImage->height;

	int i;
	
	// allocate memory for edges
	Edge **ppEdges = new Edge*[height];
	for (i = 0; i < height; i++)
		ppEdges[i] = new Edge();

	// convert float points to int
	Vec2dInt *pPointsInt = new Vec2dInt[nPoints];
	for (i = 0; i < nPoints; i++)
	{
		pPointsInt[i].x = int(pPoints[i].x + 0.5f);
		pPointsInt[i].y = int(pPoints[i].y + 0.5f);
	}

	if (pPoints[0].x == pPoints[nPoints - 1].x && pPoints[0].y == pPoints[nPoints - 1].y)
		nPoints--;
   
	// constuct edges 
	BuildEdgeList(pPointsInt, nPoints, ppEdges, height);

	// free memory
	delete [] pPointsInt;

	Edge *pActiveEdge = new Edge();

	// run algorithm
	for (i = 0; i < height; i++) 
	{
		BuildActiveList(i, pActiveEdge, ppEdges);
       
		if (pActiveEdge->next) 
		{
			if (pImage->type == CByteImage::eGrayScale)
				FillScan(pImage, i, pActiveEdge, (r + (g << 1) + b + 2) >> 2);
			else
				FillScan(pImage, i, pActiveEdge, r, g, b);

			UpdateActiveList(i, pActiveEdge);
			ResortActiveList(pActiveEdge);
		}
	}
   
	// free memory for edges
	for (i = 0; i < height; i++)
		delete ppEdges[i];
	delete [] ppEdges;
	delete pActiveEdge;
}

void PrimitivesDrawer::DrawPolygon(CByteImage *pImage, const Vec2d *pPoints, int nPoints, int r, int g, int b, int thickness)
{
	if (nPoints < 3)
	{
		printf("error: at least three points must be provided for PrimitivesDrawer::DrawPolygon\n");
		return;
	}
	
	if (thickness == -1)
	{
		FillPolygon(pImage, pPoints, nPoints, r, g, b);
	}
	else
	{
		for (int i = 0; i < nPoints - 1; i++)
			DrawLine(pImage, pPoints[i], pPoints[i + 1], r, g, b, thickness);
			
		if (pPoints[0].x != pPoints[nPoints - 1].x || pPoints[0].y != pPoints[nPoints - 1].y)
			DrawLine(pImage, pPoints[nPoints - 1], pPoints[0], r, g, b, thickness);
	}
}

void PrimitivesDrawer::DrawPolygon(CByteImage *pImage, const CVec2dArray &points, int r, int g, int b, int thickness)
{
	const int nPoints = points.GetSize();
	
	Vec2d *pPoints = new Vec2d[nPoints];
	
	for (int i = 0; i < nPoints; i++)
		Math2d::SetVec(pPoints[i], points[i]);
	
	DrawPolygon(pImage, pPoints, nPoints, r, g, b, thickness);

	delete [] pPoints;
}

void PrimitivesDrawer::DrawEllipse(CByteImage *pImage, const Ellipse2d &ellipse, int r, int g, int b, int thickness)
{
	const float mx = ellipse.center.x;
	const float my = ellipse.center.y;
	
	const int min_x = int(mx - ellipse.radius_x + 0.5f);
	const int min_y = int(my - ellipse.radius_y + 0.5f);
	const int max_x = int(mx + ellipse.radius_x + 0.5f);
	const int max_y = int(my + ellipse.radius_y + 0.5f);
	
	const int rx = int(ellipse.radius_x + 0.5f);
	const int ry = int(ellipse.radius_y + 0.5f);
	
	int i;
	
	const float a2 = ellipse.radius_x * ellipse.radius_x;
	const float b2 = ellipse.radius_y * ellipse.radius_y;
	const float a2divb2 = a2 / b2;
	const float b2diva2 = b2 / a2;
	
	const float ca = cosf(ellipse.angle);
	const float sa = sinf(ellipse.angle);
	
	if (thickness == -1)
	{
		printf("error: filling of ellipses has not been implemented yet in PrimitivesDrawer::DrawEllipse\n");
	}
	else
	{
		if (thickness == 1)
		{
			for (i = -ry; i <= ry; i++)
			{
				const float y = float(i);
				const float d = a2 - a2divb2 * y * y;
				
				if (d >= 0.0f)
				{
					const float x = sqrtf(d);
					
					const float xr = ca * x - sa * y;
					const float yr = sa * x + ca * y;
					
					PrimitivesDrawer::DrawPoint(pImage, int(mx + xr + 0.5f), int(my + yr + 0.5f), r, g, b);
					PrimitivesDrawer::DrawPoint(pImage, int(mx - xr + 0.5f), int(my - yr + 0.5f) , r, g, b);
				}
			}
			
			for (i = -rx; i <= rx; i++)
			{
				const float x = float(i);
				const float d = b2 - b2diva2 * x * x;
				
				if (d >= 0.0f)
				{
					const float y = sqrtf(d);
					
					const float xr = ca * x - sa * y;
					const float yr = sa * x + ca * y;
					
					PrimitivesDrawer::DrawPoint(pImage, int(mx + xr + 0.5f), int(my + yr + 0.5f), r, g, b);
					PrimitivesDrawer::DrawPoint(pImage, int(mx - xr + 0.5f), int(my - yr + 0.5f), r, g, b);
				}
			}
		}
		else
		{
			const float thickness_radius = 0.5f * thickness;
			
			for (i = -ry; i <= ry; i++)
			{
				const float y = float(i);
				const float d = a2 - a2divb2 * y * y;
				
				if (d >= 0.0f)
				{
					const float x = sqrtf(d);
					
					const float xr = ca * x - sa * y;
					const float yr = sa * x + ca * y;
					
					PrimitivesDrawer::DrawCircle(pImage, mx + xr, my + yr, thickness_radius, r, g, b, -1);
					PrimitivesDrawer::DrawCircle(pImage, mx - xr, my - yr, thickness_radius, r, g, b, -1);
				}
			}
			
			for (i = -rx; i <= rx; i++)
			{
				const float x = float(i);
				const float d = b2 - b2diva2 * x * x;
				
				if (d >= 0.0f)
				{
					const float y = sqrtf(d);
					
					const float xr = ca * x - sa * y;
					const float yr = sa * x + ca * y;
					
					PrimitivesDrawer::DrawCircle(pImage, mx + xr, my + yr, thickness_radius, r, g, b, -1);
					PrimitivesDrawer::DrawCircle(pImage, mx - xr, my - yr, thickness_radius, r, g, b, -1);
				}
			}
		}
	}
}

void PrimitivesDrawer::DrawRectangle(CByteImage *pImage, const Rectangle2d &rectangle, int r, int g, int b, int thickness)
{
	Vec2d cornerPoints[4];
	Math2d::ComputeRectangleCornerPoints(rectangle, cornerPoints);

	DrawPolygon(pImage, cornerPoints, 4, r, g, b, thickness);
}
