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
// Filename:  ObjectDefinitions.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _OBJECT_DEFINITIONS_H_
#define _OBJECT_DEFINITIONS_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "DataStructures/DynamicArrayTemplate.h"
#include "Structs/Structs.h"
#include "Math/Math3d.h"

#include <string>



// ****************************************************************************
// Enums
// ****************************************************************************

enum ObjectColor
{
    eNone,
    eBlue,
    eBlue2,
    eBlue3,
    eColored,
    eGreen,
    eGreen2,
    eGreen3,
	eOrange,
	eOrange2,
	eOrange3,
	eRed,
	eRed2,
    eRed3,
	eSkin,
    eWhite,
    eYellow,
    eYellow2,
    eYellow3,
    eNumberOfColors
};

enum ObjectType
{
	eCompactObject,
	eTexturedObject,
	eHead,
	eLeftHand,
	eRightHand,
	eUnknownObject
};


// ****************************************************************************
// Structs
// ****************************************************************************

struct Object2DEntry
{
	Object2DEntry()
	{
		id = -1;
		type = eUnknownObject;
		color = eNone;
		data = 0;
		reserved = 0;
	}

	Object2DEntry(const Object2DEntry &entry)
	{
		id = entry.id;
		region = entry.region;
		type = entry.type;
		color = entry.color;
		sName = "";
		sName += entry.sName;
		data = entry.data;
		reserved = entry.reserved;
	}

	int id; // unique id
	MyRegion region;
	ObjectType type;
	ObjectColor color;
	std::string sName;
	void *data;
	int reserved;
};

struct	Object3DEntry
{
    Object3DEntry()
	{
		region_id_left = -1;
		region_id_right = -1;
		type = eUnknownObject;
		color = eNone;
		data = 0;
		class_id = -1;
		quality = 0.0f;
		quality2 = 0.0f;
        localizationValid = false;
        
		Math3d::SetVec(world_point, Math3d::zero_vec);
		Math3d::SetVec(orientation, Math3d::zero_vec);
		Math3d::SetVec(pose.translation, Math3d::zero_vec);
		Math3d::SetMat(pose.rotation, Math3d::unit_mat);
	}
	
	Object3DEntry(const Object3DEntry &entry)
	{
		region_left = entry.region_left;
		region_right = entry.region_right;
		region_id_left = entry.region_id_left;
		region_id_right = entry.region_id_right;
		type = entry.type;
		color = entry.color;
		Math3d::SetTransformation(pose, entry.pose);
		Math3d::SetVec(world_point, entry.world_point);
		Math3d::SetVec(orientation, entry.orientation);
		sName = "";
		sName += entry.sName;
		sOivFilePath = "";
		sOivFilePath += entry.sOivFilePath;
		data = entry.data;
		class_id = entry.class_id;
		quality = entry.quality;
		quality2 = entry.quality2;
        localizationValid = entry.localizationValid;
	}
	
	MyRegion region_left, region_right;
	int region_id_left, region_id_right;
	ObjectType type;
	ObjectColor color;
	Transformation3d pose;
	Vec3d world_point;
	Vec3d orientation;
	std::string sName;
	std::string sOivFilePath;
	void *data;
	int class_id;
	float quality, quality2;
    bool localizationValid;
};

// ****************************************************************************
// Typedefs
// ****************************************************************************

typedef std::vector<Object2DEntry> Object2DList;
typedef std::vector<Object3DEntry> Object3DList;
typedef CDynamicArrayTemplate<Object2DEntry> CObject2DArray;
typedef CDynamicArrayTemplate<Object3DEntry> CObject3DArray;



#endif /* _OBJECT_DEFINITIONS_H_ */
