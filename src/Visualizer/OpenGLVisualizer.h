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
// Filename:  OpenGLVisualizer.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************
// Changes:   25.10.2010, Zhixing Xue
//            * Added COpenGLVisualizer::GetDepthMatrix
// ****************************************************************************

#ifndef _OPENGL_VISUALIZER_SCENE_H_
#define _OPENGL_VISUALIZER_SCENE_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Math/Math3d.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class GLUquadric;
class CByteImage;
class CCalibration;
class CFloatMatrix;



// ****************************************************************************
// COpenGLVisualizer
// ****************************************************************************

class COpenGLVisualizer
{
public:
	// constructor
	COpenGLVisualizer();

	// destructor
	~COpenGLVisualizer();

	// static variables for colors
	static const float red[3];
	static const float green[3];
	static const float blue[3];
	static const float yellow[3];


	// public methods
	bool Init(int width = 640, int height = 480, bool bActivateShading = true);
	bool InitByCalibration(const CCalibration *pCalibration, bool bActivateShading = true);
	void ActivateShading(bool bActivateShading);
	void SetProjectionMatrix(const CCalibration *pCalibration);
	bool GetImage(CByteImage *pDestinationImage);
	bool GetDepthMatrix(CFloatMatrix *pDestinationMatrix);
	void SetViewMatrix(const Transformation3d &transformation)
	{
		m_ViewMatrix = transformation;
	}
	
	// methods for drawing
	void Clear();
	void DrawPoint(float x, float y, float z, const float *pColor = 0);
	void DrawPoints(Vec3dList &points, const float *pColor = 0);
	void DrawSphere(const Vec3d &point, float radius, const float *pColor = 0);
	void DrawCylinder(const Vec3d &point1, const Vec3d &point2, float radius1, float radius2, const float color[3]);
	void DrawObject(const CFloatMatrix *pMatrix, const Transformation3d &transformation);
	
	// static methods
	static void ConvertToOpenGLMatrix(const Transformation3d &transformation, float resultMatrix[16]);
	static void CalculateOpenGLProjectionMatrix(const Mat3d &K, int width, int height, float gnear, float gfar, float *m);
	static void CalculateOpenGLProjectionMatrixDefaultPrincipalPoint(const Mat3d &K, int width, int height, float gnear, float gfar, float *m);


private:
	// private methods
	void SetProjectionMatrix(int nWidth, int nHeight);
	bool InitExtension();
	
	// private attributes
	int width, height;

	GLUquadric *m_pQuadric;
	
	// current view matrix
	bool			m_bMatrixOnStack;
	Transformation3d	m_ViewMatrix;
};



#endif /* _OPENGL_VISUALIZER_SCENE_H_ */
