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
// Filename:  ObjectPose.h
// Author:    Moritz Ritter
// Date:      08.04.2008
// ****************************************************************************


#ifndef _OBJECT_POSE_H_
#define _OBJECT_POSE_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Math/Math3d.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CCalibration;
struct Vec2d;



// ****************************************************************************
// CObjectPose
// ****************************************************************************

/*!
	\ingroup Tracking
	\brief Implementation of an alternative to the POSIT algorithm.
 
	This class implements the approach presented in (C.-P. Lu, G. D. Hager and E. Mjolsness, "Fast and Globally Convergent Pose Estimation from Video Images", 2000).
 
	This algorithm also succeeds in the case of planar points sets (objects).
*/
class CObjectPose
{

public:
	// constructor
	CObjectPose(const Vec3d *pObjectPoints, int nPoints);

	// destructor
	~CObjectPose();


	// public methods
	bool EstimatePose(const Vec2d *pImagePoints,
		Mat3d &rotationMatrix, Vec3d &translationVector,
		const CCalibration *pCalibration, int nMaxIterations = 350);


private:
	void AbsKernel(Mat3d &R, Vec3d &T, float &error);
	Vec3d tEstimate(Mat3d &R);

	Vec3d *m_pP;
	int m_nPoints;
	Vec3d m_tPbar;

	Vec3d *m_pQ;

	Mat3d *m_pF;
	Mat3d m_tFactor;
};



#endif /* _OBJECT_POSE_H_ */
