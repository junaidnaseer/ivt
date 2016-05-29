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
// Filename:  FloatImage.h
// Author:    Jan Issac
// Date:      2011
// ****************************************************************************

#ifndef __FLOAT_IMAGE_H__
#define __FLOAT_IMAGE_H__


// ****************************************************************************
// CFloatImage
// ****************************************************************************

/*!
	\class CFloatImage
	\ingroup ImageRepresentations
	\brief Data structure for the representation of any image type (arbitrary number of channels) using the data type float.
	
	Images of type CFloatImage are capable to represent high dynamic range images, however, it cannot be directly loaded from or stored to files nor
	visualized directly in GUIs. Since CByteImages is the central image type, loading, storage and visualization will require a conversion from
	CByteFloat into CByteImage.
	The image data is represented as a linear array of pixels, always starting from the top left corner and stored row wise.
	
	In the case of gray scale/single channel images , the gray scale value of each pixel is encoded in one float (0.0: black, 255.0: white, nuances in between).
	In the case of 3-channel color images the color information of each pixel is encoded in three floats (first: red, second: green, third: blue).
*/
class CFloatImage
{
public:
	/*!
		\brief The default constructor.
	 
		The default constructor sets all member variables to zero, i.e. after construction no valid image is represented.
		
		This constructor is useful in case an image is to be loaded from a file at a later point, e.g. with the method LoadFromFile(const char *).
	*/
	CFloatImage();
	
	/*!
		\brief Constructor for creating an image of a specific size and type.
	 
		With this constructor, an instance of CFloatImage is created, given the desired image resolution and image type.
		The contents of the image are not initialized, i.e. are undefined after creation.
	 
		\param[in] nImageWidth The desired width of the image in pixels. Must be > 0.
		\param[in] nImageHeight The desired height of the image in pixels. Must be > 0.
		\param[in] nNumberOfChannels The desired number of channels of each pixel.
		\param[in] bHeaderOnly If set to false (default value), memory is allocated for the pixels. If set to true, no memory is allocated for the pixels.
			   The latter can be useful for assigning image data from other sources (see e.g. implementation of ImageAccessCV::LoadFromFile).
			   Note that if bHeaderOnly is set to true, the member variable CFloatImage::m_bOwnMemory is set to false so that memory assigned to the member variable CFloatImage::pixels is not freed
			   throughout re-initialization/destruction, i.e. freeing memory must be handled by the caller in this case.
	 */
	CFloatImage(int nImageWidth, int nImageHeight, int nNumberOfChannels, bool bHeaderOnly = false);
	
	/*!
		\brief Constructor for creating an image given a pointer to a CByteImage.
	 
		This constructor creates a new instance with the same properties as the image provided by the parameter 'pImage'.
		<b>Note that the contents of the image are not copied.</b> Use ImageProcessor::CopyImage(const CFloatImage*, CFloatImage*, const MyRegion*, bool) for copying image contents.
	 
		\param[in] pImage The template image.
		\param[in] bHeaderOnly If set to false (default value), memory is allocated for the pixels. If set to true, no memory is allocated for the pixels.
			   The latter can be useful for assigning image data from other sources (see e.g. implementation of ImageAccessCV::LoadFromFile).
			   Note that if bHeaderOnly is set to true, the member variable CFloatImage::m_bOwnMemory is set to false so that memory assigned to the member variable CFloatImage::pixels is not freed
			   throughout re-initialization/destruction, i.e. freeing memory must be handled by the caller in this case.
	*/
	CFloatImage(const CFloatImage *pImage, bool bHeaderOnly = false);

	/*!
		\brief Copy constructor.
	 
		This copy constructor creates a new instance with the same properties as the image provided by the parameter 'image'.
		<b>Note that the contents of the image are not copied.</b> Use ImageProcessor::CopyImage(const CFloatImage*, CFloatImage*, const MyRegion*, bool) for copying image contents.
	 
		\param[in] image The template image.
		\param[in] bHeaderOnly If set to false (default value), memory is allocated for the pixels. If set to true, no memory is allocated for the pixels.
			   The latter can be useful for assigning image data from other sources (see e.g. implementation of ImageAccessCV::LoadFromFile).
			   Note that if bHeaderOnly is set to true, the member variable CFloatImage::m_bOwnMemory is set to false so that memory assigned to the member variable CFloatImage::pixels is not freed
			   throughout re-initialization/destruction, i.e. freeing memory must be handled by the caller in this case.
	*/
	CFloatImage(const CFloatImage &image, bool bHeaderOnly = false);
	
	/*!
		\brief The destructor.
	*/
	~CFloatImage();
	

	// public methods

	/*!
		\brief Checks whether two images are compatible or not.
		
		\param[in] pImage Pointer to the image to be checked compatibility with.
		\return true if the attributes width, height and type are equal, otherwise returns false.
	*/
	bool IsCompatible(const CFloatImage *pImage) const;
	
	/*!
		\brief Loads an image from a file.
		
		Loads an image from a BMP, PGM, or PPM file.
		The image type is recognized by the file name ending (.bmp, .pgm, .ppm or .BMP, .PGM, .PPM).
	 
		If an image is already loaded, reinitialization is performed automatically.
		
		\param[in] pFileName The path to the image file to be loaded.
		\return true on success and false on failure.
	*/
	bool LoadFromFile(const char *pFileName);

	/*!
		\brief Saves an image to a file.
	 
		Saves the current image to a BMP, PGM, or PPM file.
		The desired image type is recognized by the file name ending (.bmp, .pgm, .ppm or .BMP, .PGM, .PPM).
		
		<b>Be careful to provide the correct file ending for PGM/PPM files</b>:
		<b>.PGM</b> for <i>grayscale</i> images or <b>.PPM</b> for <i>RGB</i> images.
		
		\param[in] pFileName The path to the destination file.
		\return true on success and false on failure.
	*/
	bool SaveToFile(const char *pFileName) const;



private:
	// private methods
	void FreeMemory();

public:
	// public attributes - not clean OOP design but easy access

	/*!
		\brief The width of the image in pixels.
		
		Padding is not implemented (on purpose).
	 
		This variable should only be read. It should only be modified by external image loaders (e.g. ImageAccessCV::LoadFromFile).
	*/
	int width;

	/*!
		\brief The height of the image in pixels.
	 
		This variable should only be read. It should only be modified by external image loaders (e.g. ImageAccessCV::LoadFromFile).
	*/
	int height;

	/*!
		\brief The number of bytes used for encoding one pixel.
		
		This information is redundant with the number of channels.
	 
		This variable should only be read. It should only be modified by external image loaders (e.g. ImageAccessCV::LoadFromFile).
	*/
	int bytesPerPixel;

	/*!
		\brief The pointer to the the pixels.
		
		This is the pointer pointing to the memory area containing the pixels of the image, starting from the top left corner
		and stored row wise (see above for type dependent information).
	 
		This variable should only be read. It should only be modified by external image loaders (e.g. ImageAccessCV::LoadFromFile).
	*/
	float *pixels;

	/*!
		\brief Number of channels per pixel.

		The number of channels per pixel of the image specifies its encoding.

		This variable should only be read. It should only be modified by external image loaders (e.g. ImageAccessCV::LoadFromFile).
	*/
	int numberOfChannels;

	/*!
		\brief Flag signaling if memory is to be freed or not.
		
		This flag signals whether the image memory must be freed throughut re-initiallization/destruction or not.
	 
		This flag is usually for internal use only. It should only be modified by external image loaders (e.g. ImageAccessCV::LoadFromFile).
	*/
	bool m_bOwnMemory;
};



#endif /* __FLOAT_IMAGE_H__ */
