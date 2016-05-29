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
// Filename:  FeatureEntry.h
// Author:    Pedram Azad
// Date:      2006
// ****************************************************************************

/** \defgroup FeatureRepresentations Representations of local features */


#ifndef _FEATURE_ENTRY_H_
#define _FEATURE_ENTRY_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "DataStructures/DynamicArray.h"
#include "Helpers/helpers.h"
#include "Math/Math2d.h"
#include "Math/Math3d.h"

#include <stdio.h>
#include <string.h>



// ****************************************************************************
// CFeatureEntry
// ****************************************************************************

/*!
	\ingroup FeatureRepresentations
	\brief Base class for the representation of local features.
*/
class CFeatureEntry : public CDynamicArrayElement
{
public:
	// enums
	enum eFeatureType { tSIFT, tMSER, tPatch, tCCH, tLCCH, tNLCCH };


	// constructors
	CFeatureEntry(int nSize, float x, float y, float angle, float scale, Vec3d point3d = Math3d::zero_vec) : CDynamicArrayElement()
	{
		m_nSize = nSize;

		Math2d::SetVec(point, x, y);
		Math3d::SetVec(this->point3d, point3d);
		this->angle = angle;
		this->scale = scale;
		
		if (nSize)
			m_pFeature = new float[nSize];
		else
			m_pFeature = 0;
	}

	CFeatureEntry(const float *pFeature, int nSize, float x, float y, float angle, float scale, Vec3d point3d = Math3d::zero_vec) : CDynamicArrayElement()
	{
		m_nSize = nSize;

		Math2d::SetVec(point, x, y);
		Math3d::SetVec(this->point3d, point3d);
		this->angle = angle;
		this->scale = scale;

		m_pFeature = new float[nSize];
		memcpy(m_pFeature, pFeature, nSize * sizeof(float));
	}

	CFeatureEntry(const CFeatureEntry &featureEntry)
	{
		Math2d::SetVec(point, featureEntry.point);
		Math3d::SetVec(point3d, featureEntry.point3d);
		angle = featureEntry.angle;
		scale = featureEntry.scale;
		m_nSize = featureEntry.m_nSize;

		m_pFeature = new float[m_nSize];
		memcpy(m_pFeature, featureEntry.m_pFeature, m_nSize * sizeof(float));
	}

	// destructor
	~CFeatureEntry()
	{
		if (m_pFeature)
			delete [] m_pFeature;
	}


	// public methods
	virtual bool ReadFromFileOld(FILE *pFile)
	{
		if (fread(&m_nSize, sizeof(int), 1, pFile) != 1) return false;
		#ifdef IVT_BIG_ENDIAN
		m_nSize = invert_byte_order_long(m_nSize);
		#endif

		if (m_pFeature) delete [] m_pFeature;
		m_pFeature = new float[m_nSize];

		float u, v;

		if (fread(m_pFeature, m_nSize * sizeof(float), 1, pFile) != 1) return false;
		if (fread(&u, sizeof(float), 1, pFile) != 1) return false;
		if (fread(&v, sizeof(float), 1, pFile) != 1) return false;
		if (fread(&angle, sizeof(float), 1, pFile) != 1) return false;
		if (fread(&scale, sizeof(float), 1, pFile) != 1) return false;
		
		#ifdef IVT_BIG_ENDIAN
		for (int i = 0; i < m_nSize; i++)
			m_pFeature[i] = invert_byte_order_float(m_pFeature[i]);
		u = invert_byte_order_float(u);
		v = invert_byte_order_float(v);
		angle = invert_byte_order_float(angle);
		scale = invert_byte_order_float(scale);
		#endif

		Math2d::SetVec(point, u, v);

		return true;
	}

	virtual bool ReadFromFile(FILE *pFile)
	{
		if (fread(&m_nSize, sizeof(int), 1, pFile) != 1) return false;
		#ifdef IVT_BIG_ENDIAN
		m_nSize = invert_byte_order_long(m_nSize);
		#endif

		if (m_pFeature) delete [] m_pFeature;
		m_pFeature = new float[m_nSize];

		float u, v, x, y, z;

		if (fread(m_pFeature, m_nSize * sizeof(float), 1, pFile) != 1) return false;
		if (fread(&u, sizeof(float), 1, pFile) != 1) return false;
		if (fread(&v, sizeof(float), 1, pFile) != 1) return false;
		if (fread(&x, sizeof(float), 1, pFile) != 1) return false;
		if (fread(&y, sizeof(float), 1, pFile) != 1) return false;
		if (fread(&z, sizeof(float), 1, pFile) != 1) return false;
		if (fread(&angle, sizeof(float), 1, pFile) != 1) return false;
		if (fread(&scale, sizeof(float), 1, pFile) != 1) return false;
		
		#ifdef IVT_BIG_ENDIAN
		for (int i = 0; i < m_nSize; i++)
			m_pFeature[i] = invert_byte_order_float(m_pFeature[i]);
		u = invert_byte_order_float(u);
		v = invert_byte_order_float(v);
		x = invert_byte_order_float(x);
		y = invert_byte_order_float(y);
		z = invert_byte_order_float(z);
		angle = invert_byte_order_float(angle);
		scale = invert_byte_order_float(scale);
		#endif

		Math2d::SetVec(point, u, v);
		Math3d::SetVec(point3d, x, y, z);

		return true;
	}

	virtual bool WriteToFile(FILE *pFile) const
	{
		float u = (float) point.x;
		float v = (float) point.y;
		float x = (float) point3d.x;
		float y = (float) point3d.y;
		float z = (float) point3d.z;

		#ifdef IVT_BIG_ENDIAN
		const int size = invert_byte_order_long(m_nSize);
		for (int i = 0; i < m_nSize; i++)
			m_pFeature[i] = invert_byte_order_float(m_pFeature[i]);
		u = invert_byte_order_float(u);
		v = invert_byte_order_float(v);
		x = invert_byte_order_float(x);
		y = invert_byte_order_float(y);
		z = invert_byte_order_float(z);
		float angle_ = invert_byte_order_float(angle);
		float scale_ = invert_byte_order_float(scale);
		#else
		const int size = m_nSize;
		#endif
		
		if (fwrite(&size, sizeof(int), 1, pFile) != 1) return false;
		if (fwrite(m_pFeature, m_nSize * sizeof(float), 1, pFile) != 1) return false;
		if (fwrite(&u, sizeof(float), 1, pFile) != 1) return false;
		if (fwrite(&v, sizeof(float), 1, pFile) != 1) return false;
		if (fwrite(&x, sizeof(float), 1, pFile) != 1) return false;
		if (fwrite(&y, sizeof(float), 1, pFile) != 1) return false;
		if (fwrite(&z, sizeof(float), 1, pFile) != 1) return false;
		
		#ifdef IVT_BIG_ENDIAN
		if (fwrite(&angle_, sizeof(float), 1, pFile) != 1) return false;
		if (fwrite(&scale_, sizeof(float), 1, pFile) != 1) return false;
		for (int i = 0; i < m_nSize; i++)
			m_pFeature[i] = invert_byte_order_float(m_pFeature[i]);
		#else
		if (fwrite(&angle, sizeof(float), 1, pFile) != 1) return false;
		if (fwrite(&scale, sizeof(float), 1, pFile) != 1) return false;
		#endif

		return true;
	}

	// other public methods
	int GetSize() const { return m_nSize; }
	
	virtual int GetSizeOnDisk() const
	{
		return sizeof(int) + m_nSize * sizeof(float) + 2 * sizeof(float) + 3 * sizeof(float) + 2 * sizeof(float);
	}

	// pure virtual methods
	virtual eFeatureType GetType() const = 0;
	virtual CFeatureEntry* Clone() const = 0;
	virtual float Error(const CDynamicArrayElement *pElement) const = 0;
	


protected:
	// protected attribute
	int m_nSize;
	
public:
	float *m_pFeature;
	Vec2d point;
	Vec3d point3d;
	float angle;
	float scale;
};



#endif /* _FEATURE_ENTRY_H_ */
