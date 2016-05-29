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
// Filename:  FeatureSet.cpp
// Copyright: Keyetech UG (haftungsbeschraenkt)
// Author:    Pedram Azad
// Date:      10.02.2010
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "FeatureSet.h"
#include "FeatureEntry.h"
#include "Helpers/helpers.h"

#include "Features/SIFTFeatures/SIFTFeatureEntry.h"

#include <stdio.h>



#define HEADER_FEATURE_SET	"FEATURESET"



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CFeatureSet::CFeatureSet() : m_featureArray(true, 1000), m_contourPointArray(10)
{
}

CFeatureSet::~CFeatureSet()
{
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CFeatureSet::SetName(const char *pName)
{
	m_sName = "";
	m_sName += pName;
}

void CFeatureSet::AddFeature(const CFeatureEntry *pFeature)
{
	m_featureArray.AddElement(pFeature->Clone());
};

void CFeatureSet::AddFeatureWithoutCloning(CFeatureEntry *pFeature)
{
	m_featureArray.AddElement(pFeature);
};

void CFeatureSet::ClearFeatureList()
{
	m_featureArray.Clear();
}

void CFeatureSet::AddContourPoint(const Vec2d &point)
{
	ContourPoint element;
	Math2d::SetVec(element.point, point);
	Math3d::SetVec(element.point3d, Math3d::zero_vec);
	element.bHas3dPoint = false;

	m_contourPointArray.AddElement(element);
}

void CFeatureSet::AddContourPoint(const Vec2d &point, const Vec3d &point3d)
{
	ContourPoint element;
	Math2d::SetVec(element.point, point);
	Math3d::SetVec(element.point3d, point3d);
	element.bHas3dPoint = true;

	m_contourPointArray.AddElement(element);
}

void CFeatureSet::ClearContourPointList()
{
	m_contourPointArray.Clear();
}


bool CFeatureSet::SaveToFile(const char *pFileName) const
{
	FILE *f = fopen(pFileName, "wb");
	if (!f)
		return false;
	
	if (fwrite(HEADER_FEATURE_SET, sizeof(HEADER_FEATURE_SET) - 1, 1, f) != 1)
		return false;

	int i, temp;

	// name
	#ifdef IVT_BIG_ENDIAN
	temp = invert_byte_order_int(m_sName.length());
	#else
	temp = (int) m_sName.length();
	#endif

	if (fwrite(&temp, sizeof(int), 1, f) != 1)
		return false;

	if (m_sName.length() > 0)
	{
		if (fwrite(m_sName.c_str(), m_sName.length(), 1, f) != 1)
			return false;
	}

	// contour points
	const int nContourPoints = m_contourPointArray.GetSize();
	
	#ifdef IVT_BIG_ENDIAN
	temp = invert_byte_order_int(nContourPoints);
	#else
	temp = nContourPoints;
	#endif
	
	if (fwrite(&temp, sizeof(int), 1, f) != 1)
		return false;
	
	for (i = 0; i < nContourPoints; i++)
	{
		const ContourPoint &point = m_contourPointArray[i];
		
		#ifdef IVT_BIG_ENDIAN
		const float u = invert_byte_order_float(point.point.x);
		const float v = invert_byte_order_float(point.point.y);
		const float x = invert_byte_order_float(point.point3d.x);
		const float y = invert_byte_order_float(point.point3d.y);
		const float z = invert_byte_order_float(point.point3d.z);
		const int nHas3dPoint = invert_byte_order_int(int(point.bHas3dPoint));
		#else
		const float u = point.point.x;
		const float v = point.point.y;
		const float x = point.point3d.x;
		const float y = point.point3d.y;
		const float z = point.point3d.z;
		const int nHas3dPoint = int(point.bHas3dPoint);
		#endif
		
		if (fwrite(&u, sizeof(float), 1, f) != 1)
			return false;
		
		if (fwrite(&v, sizeof(float), 1, f) != 1)
			return false;

		if (fwrite(&x, sizeof(float), 1, f) != 1)
			return false;

		if (fwrite(&y, sizeof(float), 1, f) != 1)
			return false;

		if (fwrite(&z, sizeof(float), 1, f) != 1)
			return false;

		if (fwrite(&nHas3dPoint, sizeof(int), 1, f) != 1)
			return false;
	}
	
	// features
	const int nFeatures = m_featureArray.GetSize();
	
	#ifdef IVT_BIG_ENDIAN
	temp = invert_byte_order_int(nFeatures);
	#else
	temp = nFeatures;
	#endif
	
	if (fwrite(&temp, sizeof(int), 1, f) != 1)
		return false;
	
	for (i = 0; i < nFeatures; i++)
	{
		const CFeatureEntry *pFeatureEntry = m_featureArray[i];

		#ifdef IVT_BIG_ENDIAN
		int type = invert_byte_order_int(pFeatureEntry->GetType());
		#else
		int type = pFeatureEntry->GetType();
		#endif

		if (fwrite(&type, sizeof(int), 1, f) != 1) 
			return false;

		if (!pFeatureEntry->WriteToFile(f))
			return false;
	}

	fclose(f);

	return true;
}

bool CFeatureSet::LoadFromFile(const char *pFileName)
{
	m_featureArray.Clear();
	m_contourPointArray.Clear();
	
	FILE *f = fopen(pFileName, "rb");
	if (!f)
		return false;
	
	char buffer[sizeof(HEADER_FEATURE_SET)];
	if (fread(buffer, sizeof(HEADER_FEATURE_SET) - 1, 1, f) != 1)
		return false;
	buffer[sizeof(HEADER_FEATURE_SET) - 1] = '\0';
	if (strcmp(buffer, HEADER_FEATURE_SET) != 0)
	{
		fclose(f);
		return false;
	}

	// name
	int nStringLength;
	if (fread(&nStringLength, sizeof(int), 1, f) != 1)
		return false;

	#ifdef IVT_BIG_ENDIAN
	nStringLength = invert_byte_order_int(nStringLength);
	#endif

	if (nStringLength < 0 || nStringLength > 4096)
		return false;

	m_sName = "";
	
	if (nStringLength > 0)
	{
		// allocate memory
		char *pBuffer = new char[nStringLength + 1];
		
		// read name from file
		if (fread(pBuffer, nStringLength, 1, f) != 1)
			return false;
		
		// set termination
		pBuffer[nStringLength] = '\0';
		
		// update name
		m_sName += pBuffer;
		
		// free memory
		delete [] pBuffer;
	}
	
	int i;
	
	// contour points
	int nContourPoints;
	if (fread(&nContourPoints, sizeof(int), 1, f) != 1)
	{
		fclose(f);
		return false;
	}
	
	#ifdef IVT_BIG_ENDIAN
	nContourPoints = invert_byte_order_int(nContourPoints);
	#endif

	if (nContourPoints < 0)
		return false;
	
	for (i = 0; i < nContourPoints; i++)
	{
		float u, v, x, y, z;
		int nHas3dPoint;

		if (fread(&u, sizeof(float), 1, f) != 1)
		{
			fclose(f);
			return false;
		}
		
		if (fread(&v, sizeof(float), 1, f) != 1)
		{
			fclose(f);
			return false;
		}

		if (fread(&x, sizeof(float), 1, f) != 1)
		{
			fclose(f);
			return false;
		}

		if (fread(&y, sizeof(float), 1, f) != 1)
		{
			fclose(f);
			return false;
		}

		if (fread(&z, sizeof(float), 1, f) != 1)
		{
			fclose(f);
			return false;
		}

		if (fread(&nHas3dPoint, sizeof(int), 1, f) != 1)
		{
			fclose(f);
			return false;
		}

		#ifdef IVT_BIG_ENDIAN
		u = invert_byte_order_float(u);
		v = invert_byte_order_float(v);
		x = invert_byte_order_float(x);
		y = invert_byte_order_float(y);
		z = invert_byte_order_float(z);
		nHas3dPoint = invert_byte_order_float(nHas3dPoint);
		#endif

		ContourPoint point;
		Math2d::SetVec(point.point, u, v);
		Math3d::SetVec(point.point3d, x, y, z);
		point.bHas3dPoint = nHas3dPoint ? true : false;
			
		m_contourPointArray.AddElement(point);
	}
	
	// features
	int nFeatures;
	if (fread(&nFeatures, sizeof(int), 1, f) != 1)
	{
		fclose(f);
		return false;
	}

	if (nFeatures < 0)
		return false;
	
	#ifdef IVT_BIG_ENDIAN
	nFeatures = invert_byte_order_int(nFeatures);
	#endif
	
	for (i = 0; i < nFeatures; i++)
	{
		int type;

		if (fread(&type, sizeof(int), 1, f) != 1)
		{
			fclose(f);
			return false;
		}

		#ifdef IVT_BIG_ENDIAN
		type = invert_byte_order_int(type);
		#endif

		CFeatureEntry *pFeatureEntry;

		switch (type) 
		{
			case CFeatureEntry::tSIFT:
				pFeatureEntry = new CSIFTFeatureEntry();
			break;

			default:
				fclose(f);
				printf("error: type %i in file '%s' is note supported\n", type, pFileName);
				return false;
		}

		if (!pFeatureEntry->ReadFromFile(f)) 
		{
			printf("error: ReadFromFile\n");
			fclose(f);
			return false;
		}

		m_featureArray.AddElement(pFeatureEntry);
	}

	fclose(f);

	return true;
}
