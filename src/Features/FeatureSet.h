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
// Filename:  FeatureSet.h
// Copyright: Keyetech UG (haftungsbeschraenkt)
// Author:    Pedram Azad
// Date:      10.02.2010
// ****************************************************************************


#ifndef __FEATURE_SET_H__
#define __FEATURE_SET_H__


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "DataStructures/DynamicArrayTemplatePointer.h"
#include "Math/Math2d.h"
#include "Math/Math3d.h"
#include <string>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CFeatureEntry;



// ****************************************************************************
// CFeatureSet
// ****************************************************************************

class CFeatureSet
{
public:
	// constructor
	CFeatureSet();

	// CFeatureSet
	~CFeatureSet();

	// structs
	struct ContourPoint
	{
		Vec2d point;
		Vec3d point3d;
		bool bHas3dPoint;
	};

	// public methods
	bool SaveToFile(const char *pFileName) const;
	bool LoadFromFile(const char *pFileName);

	void SetName(const char *pName);
	const char* GetName() const { return m_sName.c_str(); }
	
	void AddFeature(const CFeatureEntry *pFeature);
	void AddFeatureWithoutCloning(CFeatureEntry *pFeature);
	void ClearFeatureList();
	const CDynamicArrayTemplatePointer<CFeatureEntry>& GetFeatureList() const { return m_featureArray; }
	
	void AddContourPoint(const Vec2d &point);
	void AddContourPoint(const Vec2d &point, const Vec3d &point3d);
	void ClearContourPointList();
	const CDynamicArrayTemplate<ContourPoint>& GetContourPointList() const { return m_contourPointArray; }


private:
	// private attribute
	CDynamicArrayTemplatePointer<CFeatureEntry> m_featureArray;
	CDynamicArrayTemplate<ContourPoint> m_contourPointArray;

	std::string m_sName;
};



#endif /* __FEATURE_SET_H__ */
