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
// Filename:  ICP.h
// Author:    Pedram Azad
// Date:      06.09.2003
// ****************************************************************************


#ifndef __I_C_P_H__
#define __I_C_P_H__


// ****************************************************************************
// Forward declarations
// ****************************************************************************

struct Vec3d;
struct Mat3d;



// ****************************************************************************
// CICP
// ****************************************************************************

/*!
	\ingroup Tracking
	\brief Implementation of the Iterative Closest Point (ICP) algorithm.
 
	The ICP algorithm has been first published in (P.J. Besl and N.D. McKay, "A Method for Registration of 3-D Shapes", 1992).
 
	Currently only the core 3D-3D point correspondence optimization function from (B.K.P. Horn, "Closed-form Solution of Absolute Orientation using Unit Quaternions", 1987) is implemented.
*/
class CICP
{
public:
	// public methods
	static bool CalculateOptimalTransformation(const Vec3d *pSourcePoints, const Vec3d *pTargetPoints, int nPoints, Mat3d &rotation, Vec3d &translation);	
};



#endif /* __I_C_P_H__ */
