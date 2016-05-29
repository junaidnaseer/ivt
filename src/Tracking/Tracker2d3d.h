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
// Filename:  Tracker2d3d.h
// Author:    Pedram Azad
// Date:      25.01.2008
// ****************************************************************************


#ifndef _TRACKER_2D_3D_
#define _TRACKER_2D_3D_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/RigidObjectTrackingInterface.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CCalibration;
class CByteImage;
struct Mat3d;
struct Vec3d;



// ****************************************************************************
// CTracker2d3d
// ****************************************************************************

/*!
	\ingroup Tracking
	\brief Very basic implementation of the 2D-3D model-based tracking algorithm.
 
	This class offers a very basic implementation of the approach proposed in
	(E. Marchand, P. Bouthemy, F. Chaumette and V. Moreau, "Robust Real-Time Visual Tracking using a 2D-3D Model-based Approach", 1999).
*/
class CTracker2d3d : public CRigidObjectTrackingInterface
{
public:
	// constructor
	CTracker2d3d();

	// desturctor
	~CTracker2d3d();
	
	
	// public methods
	void SetParameters(int nPixelsDelta, int nPixelsSearchDistance)
	{
		m_nPixelsDelta = nPixelsDelta;
		m_nPixelsSearchDistance = nPixelsSearchDistance;
	}

	// public virtual methods
	void Init(const CCalibration *pCalibration);
	bool Track(const CByteImage *pEdgeImage, Vec3d *pOutlinePoints, int nOutlinePoints,
				Mat3d &rotation, Vec3d &translation);

private:
	// private attributes
	const CCalibration *m_pCalibration;
	
	int m_nPixelsDelta;
	int m_nPixelsSearchDistance;
};



#endif /* _TRACKER_2D_3D_ */
