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
// Filename:  IplImageAdaptor.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


#ifndef _IPL_IMAGE_ADAPTOR_H_
#define _IPL_IMAGE_ADAPTOR_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include <cv.h>


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;



// ****************************************************************************
// IplImageAdaptor
// ****************************************************************************

/*!
	\ingroup ImageRepresentations
	\brief Conversion between CByteImage (IVT) and IplImage (OpenCV).
*/
namespace IplImageAdaptor
{
	/*!
		\brief Converts a CByteImage to an IplImage.

		@param pImage The input image.
		@param bAllocateMemory If set to false (default value) the pointer pImage->pixels is assigned to the returned instance of IplImage.
			In this case, cvReleaseImageHeader must be called for deletion. If set to true the returned instance of IplImage
			allocates its own memory and the image contents are copied. In this case, cvReleaseImage must be called for deletion.
		@return the created instance of IplImage. Do not forget to delete the instance after use (see documentation of the parameter bAllocateMemory for details).
	*/
	IplImage* Adapt(const CByteImage *pImage, bool bAllocateMemory = false);

	/*!
		\brief Converts an IplImage to a CByteImage.

		@param pIplImage The input image.
		@param bAllocateMemory If set to false (default value) the pointer pIplImage->pixels is assigned to the returned instance of CByteImage.
			If set to true the returned instance of CByteImage allocates its own memory and the image contents are copied.
		@return the created instance of CByteImage. Do not forget to delete the instance after use (deletion of CByteImage::pixels is done automatically in the destructor if necessary).
	*/
	CByteImage* Adapt(const IplImage *pIplImage, bool bAllocateMemory = false);
}



#endif /* _IPL_IMAGE_ADAPTOR_H_ */
