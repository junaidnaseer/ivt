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
// Filename:  Vecd.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


#ifndef _VEC_D_H_
#define _VEC_D_H_



// ****************************************************************************
// CVecd
// ****************************************************************************

/** \ingroup Math
 *  \brief Data structure and operations for calculating with vectors of arbitrary dimension
 */
class CVecd
{
public:
	// constructors
	CVecd();
	CVecd(int nLength);
	CVecd(double dX1, double dX2);
	CVecd(const CVecd &v);

	// destructor
	~CVecd();


	// operators
	CVecd& operator= (const CVecd &v);
	CVecd operator+ (const CVecd &v);
	CVecd operator- (const CVecd &v);
	double operator* (const CVecd &v);
	double& operator[] (const int n) const;
	double Length();

	// methods
	void SetSize(int nSize);

	// member access
	int GetSize() const { return m_nSize; }


private:
	// private attributes
	double *m_pElements;
	int m_nSize;
};



#endif /* _VEC_D_H_ */
