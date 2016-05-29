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
// Filename:  Rectification.h
// Author:    Pedram Azad
// Date:      04.10.2008
// ****************************************************************************


#ifndef _RECTIFICATION_H_
#define _RECTIFICATION_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Image/ImageMapper.h"
#include "Math/Math3d.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;
class CCalibration;
class CStereoCalibration;
struct Vec2d;



// ****************************************************************************
// CRectification
// ****************************************************************************

/*!
	\ingroup StereoProcessing Calibration
	\brief Performing rectification of a stereo image pair.
*/
class CRectification
{
public:
	// constructor
	CRectification(bool bInterpolate = true, bool bUndistort = true);

	// destructor
	~CRectification();


	// public methods

	// initialize by reading a camera parameter file
	bool Init(const char *pCameraParameterFileName);

	// initialize by setting a calibration object
	void Init(const CStereoCalibration *pStereoCalibration);
	// use this method for re-calculating the maps (not needed for static calibrations)
	void UpdateMaps();

	void Rectify(const CByteImage * const *ppInputImages, CByteImage **ppOutputImages);
	
	
private:
	class CRectificationMapper : public CImageMapper
	{
	public:
		CRectificationMapper(bool bInterpolate, bool bUndistort) : CImageMapper(bInterpolate)
		{
			m_bUndistort = bUndistort;
		}

		void Init(const Mat3d &homography, const CCalibration *pCalibration);

	private:
		void ComputeOriginalCoordinates(const Vec2d &newCoordinates, Vec2d &originalCoordinates);

		const CCalibration *m_pCalibration;
		Mat3d m_homography;

		bool m_bUndistort;
	};


	// private attributes
	CStereoCalibration *m_pStereoCalibration;
	const CStereoCalibration *m_pUsedStereoCalibration;
	
	CRectificationMapper *m_pRectificationMapperLeft;
	CRectificationMapper *m_pRectificationMapperRight;
};



#endif /* _RECTIFICATION_H_ */
