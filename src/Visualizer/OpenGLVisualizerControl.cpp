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
// Filename:  OpenGLVisualizerControl.cpp
// Author:    Florian Hecht
// Date:      2009
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "OpenGLVisualizerControl.h"

#include "Interfaces/MainWindowInterface.h"
#include "OpenGLVisualizer.h"



// ****************************************************************************
// Constructor
// ****************************************************************************

COpenGLVisualizerControl::COpenGLVisualizerControl(WIDGET_HANDLE widget, int width, int height, COpenGLVisualizer *visualizer, CMainWindowInterface *main_window, CMainWindowEventInterface *event_callback)
: m_widget(widget), m_visualizer(visualizer), m_main_window(main_window), m_event_callback(event_callback), m_width(width), m_height(height), m_mouse_mode(0)
{
	ResetViewMatrix();
}


// ****************************************************************************
// Methods
// ****************************************************************************

void COpenGLVisualizerControl::SetViewMatrix(const Transformation3d &transformation)
{
	m_ViewMatrix = transformation;
	
	if (m_visualizer)
		m_visualizer->SetViewMatrix(m_ViewMatrix);
}

const Transformation3d &COpenGLVisualizerControl::GetViewMatrix()
{
	return m_ViewMatrix;
}

void COpenGLVisualizerControl::ResetViewMatrix()
{
	m_ViewMatrix.rotation = Math3d::unit_mat;
	m_ViewMatrix.translation = Math3d::zero_vec;
	
	if (m_visualizer)
		m_visualizer->SetViewMatrix(m_ViewMatrix);
}
	
// this two events are specific for the image widget
void COpenGLVisualizerControl::RectSelected(WIDGET_HANDLE widget, int x0, int y0, int x1, int y1)
{
	if (m_event_callback)
		m_event_callback->RectSelected(widget, x0, y0, x1, y1);
}

void COpenGLVisualizerControl::PointClicked(WIDGET_HANDLE widget, int x, int y)
{
	if (m_event_callback)
		m_event_callback->PointClicked(widget, x, y);
}

// the following events are only generated for image and GL widgets
void COpenGLVisualizerControl::MouseDown(WIDGET_HANDLE widget, int button, int x, int y)
{
	if (widget == m_widget)
	{
		int mod = 0;
		if (m_main_window)
			mod = m_main_window->GetModifierKeyState();
		
		if (button == IVT_LEFT_BUTTON)
		{
			if (mod == 0)
				m_mouse_mode = 1;
			else if (mod & IVT_SHIFT_KEY)
				m_mouse_mode = 3;
			else if (mod & IVT_CONTROL_KEY || mod & IVT_ALT_KEY)
				m_mouse_mode = 4;
		}
		else if (button == IVT_RIGHT_BUTTON)
		{
			m_mouse_mode = 2;
		}
		else if (button == IVT_MIDDLE_BUTTON)
		{
			m_mouse_mode = 3;
		}
		
		m_mouse_old_x = x;
		m_mouse_old_y = y;
		
		return;
	}
	
	if (m_event_callback)
		m_event_callback->MouseDown(widget, button, x, y);
}

void COpenGLVisualizerControl::MouseUp(WIDGET_HANDLE widget, int button, int x, int y)
{
	if (widget == m_widget)
	{
		m_mouse_mode = 0;
		
		m_mouse_old_x = x;
		m_mouse_old_y = y;
		
		return;
	}
	
	if (m_event_callback)
		m_event_callback->MouseUp(widget, button, x, y);
}

void COpenGLVisualizerControl::MouseMove(WIDGET_HANDLE widget, int x, int y)
{
	if (widget == m_widget && m_mouse_mode != 0)
	{
		m_mouse_old_x = x;
		m_mouse_old_y = y;
		
		return;
	}
	
	if (m_event_callback)
		m_event_callback->MouseMove(widget, x, y);
}

void COpenGLVisualizerControl::KeyDown(WIDGET_HANDLE widget, int key)
{
	if (m_event_callback)
		m_event_callback->KeyDown(widget, key);
}

void COpenGLVisualizerControl::KeyUp(WIDGET_HANDLE widget, int key)
{
	if (m_event_callback)
		m_event_callback->KeyUp(widget, key);
}

// this event is only emitted for buttons
void COpenGLVisualizerControl::ButtonPushed(WIDGET_HANDLE widget)
{
	if (m_event_callback)
		m_event_callback->ButtonPushed(widget);
}

// this event is generated by check boxes, sliders, text edits (value = -1)
// and combo boxes
void COpenGLVisualizerControl::ValueChanged(WIDGET_HANDLE widget, int value)
{
	if (m_event_callback)
		m_event_callback->ValueChanged(widget, value);
}

#define ROT_SPEED 0.1f
#define TRANS_SPEED 100.0f

void COpenGLVisualizerControl::Update()
{
	float fx = (float)(m_mouse_old_x - (m_width / 2)) / (float)(m_width / 2);
	float fy = (float)(m_mouse_old_y - (m_height / 2)) / (float)(m_height / 2);
	
	if (m_mouse_mode == 1)
	{
		Mat3d m1, m2;
		
		Math3d::SetRotationMatX(m1, ROT_SPEED * fy);
		Math3d::SetRotationMatY(m2, ROT_SPEED * -fx);
		
		Math3d::MulMatMat(m1, m2, m1);
		Math3d::MulMatMat(m1, m_ViewMatrix.rotation, m_ViewMatrix.rotation);
		
		if (m_visualizer)
			m_visualizer->SetViewMatrix(m_ViewMatrix);
	}
	else if (m_mouse_mode == 2)
	{
		m_ViewMatrix.translation.z += TRANS_SPEED * fy;
		
		if (m_visualizer)
			m_visualizer->SetViewMatrix(m_ViewMatrix);
	}
	else if (m_mouse_mode == 3)
	{
		m_ViewMatrix.translation.x += TRANS_SPEED * fx;
		m_ViewMatrix.translation.y += TRANS_SPEED * fy;
		
		if (m_visualizer)
			m_visualizer->SetViewMatrix(m_ViewMatrix);
	}
	else if (m_mouse_mode == 4)
	{
		Mat3d m1;
		
		Math3d::SetRotationMatZ(m1, ROT_SPEED * fy);
		
		Math3d::MulMatMat(m1, m_ViewMatrix.rotation, m_ViewMatrix.rotation);
		
		if (m_visualizer)
			m_visualizer->SetViewMatrix(m_ViewMatrix);
	}
}

#undef ROT_SPEED
#undef TRANS_SPEED
