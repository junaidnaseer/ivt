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
// Filename:  OpenInventorVisualizer.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "OpenInventorVisualizer.h"

#include "Math/Math3d.h"

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoCoordinate3.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

COpenInventorVisualizer::COpenInventorVisualizer(int argc, char **args)
{
	// init SoQt library (initializes QT and Coin)
    m_pMainWidget = SoQt::init(argc, args, args[0]);
	
	m_pRoot = new SoSeparator();
	m_pPointSet = new SoPointSet();
	m_pCoordinate3 = new SoCoordinate3();
	
	m_pRoot->addChild(m_pCoordinate3);
	m_pRoot->addChild(m_pPointSet);
	
	m_nCounter = 0;
}

COpenInventorVisualizer::~COpenInventorVisualizer()
{
}


// ****************************************************************************
// Methods
// ****************************************************************************

void COpenInventorVisualizer::Clear()
{
	// TODO: empty m_pCoordinate3
	m_nCounter = 0;
}

void COpenInventorVisualizer::AddPoint(const Vec3d &point)
{
	m_pCoordinate3->point.set1Value(m_nCounter++, (float) point.x, (float) point.y, (float) point.z);
}

void COpenInventorVisualizer::Render()
{
	// create root node
	m_pRoot->ref();
  
	// create viewer
	SoQtExaminerViewer *pViewer = new SoQtExaminerViewer(m_pMainWidget);
	pViewer->setSceneGraph(m_pRoot);
	pViewer->show();
	
	// pop up the main window and start main loop
	SoQt::show(m_pMainWidget);
	SoQt::mainLoop();

	// free memory
	m_pRoot->unref();
	
	//delete pViewer;
}
