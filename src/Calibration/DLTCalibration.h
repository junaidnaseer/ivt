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
// Filename:  DLTCalibration.h
// Author:    Pedram Azad
// Date:      04.12.2009
// ****************************************************************************


#ifndef _DLT_CALIBRATION_H_
#define _DLT_CALIBRATION_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Math/Math2d.h"
#include "Math/Math3d.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CCalibration;



// ****************************************************************************
// CDLTCalibration
// ****************************************************************************

class CDLTCalibration
{
public:
	// enums
	enum DistortionType
	{
		eNoDistortion,
		eRadialDistortion,
		eRadialAndTangentialDistortion
	};

	// structs
	struct PairElement
	{
		Vec3d worldPoint;
		Vec2d imagePoint;
	};
	
	// constructor
	CDLTCalibration();
	
	// destructor
	~CDLTCalibration();


	// public methods
	// Parameter: nIterations (recommendations)
	// nCalculateDistortionParameters = 0: nIterations not used
	// nCalculateDistortionParameters = 1: nIterations > 500
	// nCalculateDistortionParameters = 2: nIterations > 1000
	float Calibrate(const PairElement *pPairElements, int nPairElements, CCalibration &resultCalibration, DistortionType eCalculateDistortionParameters = eNoDistortion, int nIterations = 1000);

	// camera transformations (2D <=> 3D)
	void GetImageCoordinatesDLT(const Vec3d &worldPoint, Vec2d &imagePoint);
	
	float CheckCalibration(const CCalibration &calibration);
	
	

private:
	// private methods
	void CalculateDLT(const CCalibration &calibration, bool bFirstCall);
	void ExtractFromDLT(CCalibration &calibration);
	void CalculateRadialLensDistortion(CCalibration &calibration);
	void CalculateRadialAndTangentialLensDistortion(CCalibration &calibration);
	float CheckDLT();
	
	
	// private attributes
	float L1, L2, L3, L4, L5, L6, L7, L8, L9, L10, L11;

	const PairElement *m_pPairElements;
	int m_nPairElements;
};



#endif /* _DLT_CALIBRATION_H_ */
