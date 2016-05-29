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
// Filename:  ShortImage.h
// Author:    Pedram Azad
// Date:      09.01.2007
// ****************************************************************************


#ifndef __SHORT_IMAGE_H__
#define __SHORT_IMAGE_H__


// ****************************************************************************
// CShortImage
// ****************************************************************************

/*!
	\ingroup ImageRepresentations
	\brief Data structure for the representation of single channel images of the data type signed short.
*/
class CShortImage
{
public:
	// constructors
	CShortImage();
	CShortImage(int nImageWidth, int nImageHeight, bool bHeaderOnly = false);

	// copy constructors (will copy header and allocate memory)
	CShortImage(const CShortImage *pImage, bool bHeaderOnly = false);
	CShortImage(const CShortImage &image, bool bHeaderOnly = false);

	// destructor
	~CShortImage();


	// public attributes - not clean OOP design but easy access
	int width;
	int height;
	short *pixels;


private:
	// private methods
	void FreeMemory();

	// private attributes - only used internally
	bool m_bOwnMemory;
};



#endif /* __SHORT_IMAGE_H__ */
