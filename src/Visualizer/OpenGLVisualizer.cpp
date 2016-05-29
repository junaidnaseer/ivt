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
// Filename:  OpenGLVisualizer.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************
// Changes:   25.10.2010, Zhixing Xue
//            * Added COpenGLVisualizer::GetDepthMatrix
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "OpenGLVisualizer.h"
#include "Calibration/Calibration.h"
#include "Image/ByteImage.h"
#include "Math/FloatMatrix.h"
#include "Math/Math3d.h"
#include "Math/Constants.h"

#if defined __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#elif defined WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glext.h"
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glext.h>
#endif

#include <math.h>
#include <stdio.h>


// ****************************************************************************
// Static variables
// ****************************************************************************

static GLuint bufferID;
static bool bExtensionInitialized;

#ifdef WIN32
static PFNGLGENBUFFERSARBPROC glGenBuffers = 0;
static PFNGLBINDBUFFERARBPROC glBindBuffer = 0;
static PFNGLBUFFERDATAARBPROC glBufferData = 0;
static PFNGLMAPBUFFERARBPROC glMapBuffer = 0;
static PFNGLUNMAPBUFFERARBPROC glUnmapBuffer = 0;
#endif

const float COpenGLVisualizer::red[] = { 0.75f, 0, 0 };
const float COpenGLVisualizer::green[] = { 0, 0.75f, 0 };
const float COpenGLVisualizer::blue[] = { 0, 0, 0.75f };
const float COpenGLVisualizer::yellow[] = { 0.65f, 0.5f, 0.49f };



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

COpenGLVisualizer::COpenGLVisualizer()
{
	bExtensionInitialized = false;
	m_pQuadric = 0;
	
	width = 0;
	height = 0;
	
	m_ViewMatrix.rotation = Math3d::unit_mat;
	m_ViewMatrix.translation = Math3d::zero_vec;
	m_bMatrixOnStack = false;
}

COpenGLVisualizer::~COpenGLVisualizer()
{
	if (m_pQuadric)
		gluDeleteQuadric(m_pQuadric);
}


// ****************************************************************************
// Methods
// ****************************************************************************

void COpenGLVisualizer::SetProjectionMatrix(const CCalibration *pCalibration)
{
	Mat3d K;
	
	if (pCalibration)
	{
		width = pCalibration->GetCameraParameters().width;
		height = pCalibration->GetCameraParameters().height;
		pCalibration->GetCalibrationMatrix(K);
	}
	else
	{
		// apply default values
		width = 640;
		height = 480;
		Math3d::SetMat(K, 580, 0, 320, 0, 580, 240, 0, 0, 1);
	}

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	float m[16];
	CalculateOpenGLProjectionMatrix(K, width, height, 1, 4000, m);
	glMultMatrixf(m);

	// model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(180, 1, 0, 0);

	// account for extrinsic calibration
	if (pCalibration)
	{
		float m[16];
		Transformation3d transformation;
		Math3d::SetMat(transformation.rotation, pCalibration->GetCameraParameters().rotation);
		Math3d::SetVec(transformation.translation, pCalibration->GetCameraParameters().translation);
		ConvertToOpenGLMatrix(transformation, m);
		glMultMatrixf(m);
	}
}

void COpenGLVisualizer::SetProjectionMatrix(int nWidth, int nHeight)
{
	Mat3d K;
	
	// apply default values for focal length and principal point
	width = nWidth;
	height = nHeight;
	const float w_factor = width / 640.0f;
	const float h_factor = height / 480.0f;
	Math3d::SetMat(K, 580 * w_factor, 0, 320 * w_factor, 0, 580 * h_factor, 240 * h_factor, 0, 0, 1);

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	float m[16];
	CalculateOpenGLProjectionMatrix(K, width, height, 1, 4000, m);
	glMultMatrixf(m);

	// model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(180, 1, 0, 0);
}

void COpenGLVisualizer::ActivateShading(bool bActivateShading)
{
	if (bActivateShading)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);
		glShadeModel(GL_SMOOTH);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_NORMALIZE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

bool COpenGLVisualizer::Init(int nWidth, int nHeight, bool bActivateShading)
{
	ActivateShading(bActivateShading);

	// set projection model
	SetProjectionMatrix(nWidth, nHeight);
	glViewport(0, 0, nWidth, nHeight);
	
	// create quadric object
	m_pQuadric = gluNewQuadric();
	
	// initialize extension for using pixel buffers for fast glReadPixels variant
	if (!InitExtension())
	{
		//printf("info: pixel buffer extension is not available (slow method for glReadPixels will be used)\n");
		return true;
	}
	
	#ifdef WIN32
	glGenBuffers(1, &bufferID);
	#endif
	
	return true;
}

bool COpenGLVisualizer::InitByCalibration(const CCalibration *pCalibration, bool bActivateShading)
{
	ActivateShading(bActivateShading);

	// set projection model
	SetProjectionMatrix(pCalibration);
	glViewport(0, 0, width, height);
	
	// create quadric object
	m_pQuadric = gluNewQuadric();
	
	// initialize extension for using pixel buffers for fast glReadPixels variant
	if (!InitExtension())
	{
		//printf("info: pixel buffer extension is not available (slow method for glReadPixels will be used)\n");
		return true;
	}
	
	#ifdef WIN32
	glGenBuffers(1, &bufferID);
	#endif
	
	return true;
}

bool COpenGLVisualizer::GetImage(CByteImage *pDestinationImage)
{
	if (pDestinationImage->width != width || pDestinationImage->height != height)
	{
		printf("error: destination image does not match current mode\n");
		return false;
	}
	
	if (bExtensionInitialized)
	{
		#ifdef WIN32
		// fast method
		const int nBytes = width * height *pDestinationImage->bytesPerPixel;
		glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, bufferID);
		glBufferData(GL_PIXEL_PACK_BUFFER_ARB, nBytes, NULL, GL_STREAM_READ);
		
		if (pDestinationImage->type == CByteImage::eRGB24)
			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0);
		else
			glReadPixels(0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);
		
		void *pData = glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY);   
		memcpy(pDestinationImage->pixels, pData, nBytes);
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
		glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
		#endif
	}
	else
	{
		// slow method
		if (pDestinationImage->type == CByteImage::eRGB24)
			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pDestinationImage->pixels);
		else
			glReadPixels(0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, pDestinationImage->pixels);
	}
	
	return true;
}

bool COpenGLVisualizer::GetDepthMatrix(CFloatMatrix *pDestinationMatrix)
{
	if (pDestinationMatrix->columns != width || pDestinationMatrix->rows != height)
	{
		printf("error: destination matrix does not match current mode\n");
		return false;
	}
	
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, pDestinationMatrix->data);
	
	return true;
}

void COpenGLVisualizer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	float m[16];
	ConvertToOpenGLMatrix(m_ViewMatrix, m);
	
	glMatrixMode(GL_MODELVIEW);
	
	if (m_bMatrixOnStack)
		glPopMatrix();
		
	glPushMatrix();
	glMultMatrixf(m);
	
	m_bMatrixOnStack = true;
}

void COpenGLVisualizer::DrawPoint(float x, float y, float z, const float *pColor)
{
	if (pColor)
		glColor3fv(pColor);

	glDisable(GL_LIGHTING);

	glBegin(GL_POINTS);
	glVertex3f(x, y, z);
	glEnd();

	glEnable(GL_LIGHTING);
}

void COpenGLVisualizer::DrawPoints(Vec3dList &points, const float *pColor)
{
	if (pColor)
		glColor3fv(pColor);

	glDisable(GL_LIGHTING);

	glBegin(GL_POINTS);

	const int c = (int) points.size();
	for (int i = 0; i < c; i++)
	{
		const Vec3d &v = points.at(i);
		glVertex3f(v.x, v.y, v.z);

		if (c % 10000 == 0)
		{
			glEnd();
			glBegin(GL_POINTS);
		}
	}

	glEnd();

	glEnable(GL_LIGHTING);
}

void COpenGLVisualizer::DrawSphere(const Vec3d &point, float radius, const float *pColor)
{
	glPushMatrix();

	glTranslated(point.x, point.y, point.z);
	if (pColor) glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pColor);
	gluSphere(m_pQuadric, radius, 50, 50);
	
	glPopMatrix();
}

void COpenGLVisualizer::DrawCylinder(const Vec3d &point1, const Vec3d &point2, float radius1, float radius2, const float color[3])
{
	Vec3d u, _point1;
	Math3d::SubtractVecVec(point2, point1, u);
		
	Math3d::SetVec(_point1, u);
	Math3d::MulVecScalar(_point1, 0.15f, _point1);
	Math3d::AddToVec(_point1, point1);

	const float length = Math3d::Length(u);
	Math3d::NormalizeVec(u);
	
	glPushMatrix();

	//glTranslated(_point1.x, _point1.y, _point1.z);
	glTranslated(point1.x, point1.y, point1.z);
	glRotatef(acosf(u.z) * 180.0f / FLOAT_PI, -u.y, u.x, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
	gluCylinder(m_pQuadric, radius1, radius2, length, 20, 20);

	glPopMatrix();
}

void COpenGLVisualizer::ConvertToOpenGLMatrix(const Transformation3d &transformation, float resultMatrix[16])
{
	resultMatrix[0] = transformation.rotation.r1;
	resultMatrix[1] = transformation.rotation.r4;
	resultMatrix[2] = transformation.rotation.r7;
	resultMatrix[3] = 0;
	resultMatrix[4] = transformation.rotation.r2;
	resultMatrix[5] = transformation.rotation.r5;
	resultMatrix[6] = transformation.rotation.r8;
	resultMatrix[7] = 0;
	resultMatrix[8] = transformation.rotation.r3;
	resultMatrix[9] = transformation.rotation.r6;
	resultMatrix[10] = transformation.rotation.r9;
	resultMatrix[11] = 0;
	resultMatrix[12] = transformation.translation.x;
	resultMatrix[13] = transformation.translation.y;
	resultMatrix[14] = transformation.translation.z;
	resultMatrix[15] = 1;
}

void COpenGLVisualizer::DrawObject(const CFloatMatrix *pMatrix, const Transformation3d &transformation)
{
	const int nTriangles = pMatrix->rows / 3;
	const float *data = pMatrix->data;
	float m[16];

	ConvertToOpenGLMatrix(transformation, m);

	glPushMatrix();
	glMultMatrixf(m);

	for (int i = 0, offset = 0; i < nTriangles; i++, offset += 18)
	{
		glBegin(GL_TRIANGLES);
		glNormal3f(data[offset], data[offset + 1], data[offset + 2]);
		glVertex3f(data[offset + 3], data[offset + 4], data[offset + 5]);
		glNormal3f(data[offset + 6], data[offset + 7], data[offset + 8]);
		glVertex3f(data[offset + 9], data[offset + 10], data[offset + 11]);
		glNormal3f(data[offset + 12], data[offset + 13], data[offset + 14]);
		glVertex3f(data[offset + 15], data[offset + 16], data[offset + 17]);
		glEnd();
	}

	glPopMatrix();
}


void COpenGLVisualizer::CalculateOpenGLProjectionMatrix(const Mat3d &K, int width, int height, float gnear, float gfar, float *m)
{
	// note: for OpenGL, matrix must have column-major order!
	m[0] = (2.0f * K.r1 / width);
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;
	m[4] = (2.0f * K.r2 / width);
	m[5] = (2.0f * K.r5 / height);
	m[6] = 0.0f;
	m[7] = 0.0f;
	m[8] = -((2.0f * K.r3 / width) - 1.0f);
	m[9] = ((2.0f * K.r6 / height) - 1.0f);
	m[10] = -(gfar + gnear) / (gfar - gnear);
	m[11] = -1.0f;
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = -2.0f * gfar * gnear / (gfar - gnear);
	m[15] = 0.0f;
}

void COpenGLVisualizer::CalculateOpenGLProjectionMatrixDefaultPrincipalPoint(const Mat3d &K, int width, int height, float gnear, float gfar, float *m)
{
	// note: for OpenGL, matrix must have column-major order!
	m[0] = (2.0f * K.r1 / width);
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;
	m[4] = (2.0f * K.r2 / width);
	m[5] = (2.0f * K.r5 / height);
	m[6] = 0.0f;
	m[7] = 0.0f;
	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = -(gfar + gnear) / (gfar - gnear);
	m[11] = -1.0f;
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = -2.0f * gfar * gnear / (gfar - gnear);
	m[15] = 0.0f;
}

bool COpenGLVisualizer::InitExtension()
{
	if (bExtensionInitialized)
		return true;
	
	#ifdef WIN32
	glGenBuffers = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
	glBindBuffer = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
	glBufferData = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
	glMapBuffer = (PFNGLMAPBUFFERARBPROC) wglGetProcAddress("glMapBufferARB");
	glUnmapBuffer = (PFNGLUNMAPBUFFERARBPROC) wglGetProcAddress("glUnmapBufferARB");
	bExtensionInitialized = glGenBuffers && glBindBuffer && glBufferData && glMapBuffer && glUnmapBuffer;
	#else
	/*glGenBuffers = (PFNGLGENBUFFERSARBPROC) glXGetProcAddress((const GLubyte *) "glGenBuffersARB");
	glBindBuffer = (PFNGLBINDBUFFERARBPROC) glXGetProcAddress((const GLubyte *) "glBindBufferARB");
	glBufferData = (PFNGLBUFFERDATAARBPROC) glXGetProcAddress((const GLubyte *) "glBufferDataARB");
	glMapBuffer = (PFNGLMAPBUFFERARBPROC) glXGetProcAddress((const GLubyte *) "glMapBufferARB");
	glUnmapBuffer = (PFNGLUNMAPBUFFERARBPROC) glXGetProcAddress((const GLubyte *) "glUnmapBufferARB");*/
	bExtensionInitialized = false;
	#endif

	return bExtensionInitialized;
}
