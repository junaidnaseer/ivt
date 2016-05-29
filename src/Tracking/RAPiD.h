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
// Filename:  RAPiD.h
// Author:    Pedram Azad
// Date:      25.01.2008
// ****************************************************************************


#ifndef __RAPID_H__
#define __RAPID_H__


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/RigidObjectTrackingInterface.h"
#include "DataStructures/DynamicArray.h"
#include "Math/Math2d.h"
#include "Math/Math3d.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CCalibration;
class CByteImage;



// ****************************************************************************
// CRAPiD
// ****************************************************************************

/*!
	\ingroup Tracking
	\brief Implementation of the RAPiD algorithm.
 
	The RAPiD algorithm has been first published in (C.G. Harris and C. Stennett, "3D object tracking at video rate - RAPiD", 1990).
*/
class CRAPiD : public CRigidObjectTrackingInterface
{
public:
	// public class definitions
	class CRAPiDElement : public CDynamicArrayElement
	{
	public:
		Vec3d p;
		Vec2d n;
		float l;
	};

	// constructor
	CRAPiD();

	// desturctor
	~CRAPiD();
	
	
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

	// public static methods
	static bool RAPiD(CDynamicArray &elementList, const CCalibration *pCalibration, Mat3d &rotation, Vec3d &translation);


private:
	// private attributes
	const CCalibration *m_pCalibration;
	
	int m_nPixelsDelta;
	int m_nPixelsSearchDistance;
};



#endif /* __RAPID_H__ */
