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
// Filename:  ImageAccessCV.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _IMAGE_ACCESS_CV_H_
#define _IMAGE_ACCESS_CV_H_


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;



// ****************************************************************************
// ImageAccessCV
// ****************************************************************************

/*!
	\brief Loading of images using HighGUI from OpenCV.
	
	The funtions offered here are useful in case access to JPEG or PNG files is needed.
	In all other cases (BMP, PPM, PGM), it is recommended to use CByteImage::LoadFromFile(const char *) instead.
*/
namespace ImageAccessCV
{
	/*!
		\brief Loads an image from a file.
		
		If an image is already loaded, reinitialization is performed automatically.
	 
		Use the file endings .JPG, .JPEG, .jpg or .jpeg for JPEG files, .PNG or .png for PNG files, and .TIFF or .tiff for TIFF files.
	 
		For BMP, PPM and PGM files, it is recommended to use CByteImage::LoadFromFile(const char *) instead.
	 
		\param[in, out] pImage The target instance. pImage must contain an allocated instance of CByteImage, but does not need to contain a valid image.
		\param[in] pFilePath The path to the image file to be loaded.
		\return true on success and false on failure.
	*/
	bool LoadFromFile(CByteImage *pImage, const char *pFilePath);
	
	/*!
		\brief Saves an image to a file.
	 
		If an image is already loaded, reinitialization is performed automatically.
	 
		Use the file endings .JPG, .JPEG, .jpg or .jpeg for JPEG files, .PNG or .png for PNG files, and .TIFF or .tiff for TIFF files.
	 
		For BMP, PPM and PGM files, it is recommended to use CByteImage::LoadFromFile(const char *) instead.
	 
		\param[in, out] pImage The instance of the image to be saved.
		\param[in] pFilePath The path to the destination file.
		\return true on success and false on failure.
	*/
	bool SaveToFile(const CByteImage *pImage, const char *pFilePath);
}



#endif /* _IMAGE_ACCESS_CV_H_ */
