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
// Filename:  BitmapSequenceCapture.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _BITMAP_SEQUENCE_CAPTURE_H_
#define _BITMAP_SEQUENCE_CAPTURE_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Interfaces/VideoCaptureInterface.h"
#include <string>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;



// ****************************************************************************
// CBitmapSequenceCapture
// ****************************************************************************

class CBitmapSequenceCapture : public CVideoCaptureInterface
{
public:
	// constructor
	CBitmapSequenceCapture(const char *pFilePathBase, const char *pSecondFilePathBase = NULL);

	// destructor
	~CBitmapSequenceCapture();


	// public methods
	bool OpenCamera();
	void CloseCamera();
	bool CaptureImage(CByteImage **ppImages);
	
	int GetWidth();
	int GetHeight();
	CByteImage::ImageType GetType();
	int GetNumberOfCameras() { return m_bStereo ? 2 : 1; }

	//! Set the image counter to the first image
	/*! Set the image counter to the first image.
	 *  \sa n, nFirstImage
	 */
	void Rewind();


private:
	// private attributes
	CByteImage *m_pLeftImage;
	CByteImage *m_pRightImage;

	std::string m_sFirstFilePathLeft;
	std::string m_sFirstFilePathRight;
	std::string m_sFilePathBaseLeft;
	std::string m_sFilePathBaseRight;

	int m_nFiguresLeft;
	int m_nFiguresRight;

	bool m_bStereo;
	bool m_bOK;
	int n;
	//! Number of the First Image
	int nFirstImage;

	std::string m_sFileExtention;
};



#endif /* _BITMAP_SEQUENCE_CAPTURE_H_ */
