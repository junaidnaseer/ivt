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
// Filename:  ImageMapper.h
// Author:    Pedram Azad
// Date:      04.10.2008
// ****************************************************************************

#ifndef _IMAGE_MAPPER_H_
#define _IMAGE_MAPPER_H_


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;
struct Vec2d;



// ****************************************************************************
// CImageMapper
// ****************************************************************************

/*!
	\ingroup ImageProcessing
	\brief Class for efficient application of arbitrary 2D-2D transformations to whole images.

	This class performs arbitrary 2D-2D transformations on instances of CByteImage.
	The 2D-2D transformation of interest must be specified by the inheriting class by implementing only (pure) virtual method:
	ComputeOriginalCoordinates(const Vec2d&, Vec2d&).

	For initialization the method ComputeMap(int, int) must be called. Throughout initialization,
	an internal lookup table is built, including pre-computed coefficients for bilinear interpolation if the interpolation
	flag was set to true in the constructor CImageMapper(bool).

	Bilinear interpolation has been optimized with integer arithmetics.

	After initialization, the mapping is performed by calling the method PerformMapping(const CByteImage*, CByteImage*).

	Images of type CByteImage::eGrayScale and CByteImage::eRGB24 are supported.
*/
class CImageMapper
{
public:
	/*!
		\brief The only constructor of this class.

		Initialization is performed by calling the method ComputeMap(int, int).

		@param[in] bInterpolate If set to true (default value) coefficients for bilinear interpolation are pre-computed
				throughout the initialization process and bilinear interpolation is performed when calling the method
				PerformMapping(const CByteImage*, CByteImage*). Bilinear interpolation has been optimized with integer arithmetics.
				If set to false, no interpolation takes place.
	*/
	CImageMapper(bool bInterpolate = true);

	/*!
		\brief The destructor.

		Does not need to be implemented by the inheriting class.
	*/
	virtual ~CImageMapper();


	/*!
		\brief This method initializes the instance for mapping of images of a specific size.
		
		This method must be called prior to use of PerformMapping(const CByteImage*, CByteIamge*).
		an internal lookup table is built, including pre-computed coefficients for bilinear interpolation
		if the interpolation flag was set to true in the constructor CImageMapper(bool).

		@param[in] width The width of the images to be mapped in pixels.
		@param[in] height The height of the images to be mapped in pixels.
	*/
	void ComputeMap(int width, int height);

	/*!
		\brief This method performs the mapping.

		Before application of this method, the instance must have been initialized by calling the method ComputeMap(int, int).

		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be either both of type CByteImage::eGrayScale or both of type CByteImage::eRGB24.

		@param[in] pInputImage The input image.
		@param[out] pOutputImage The output image.
	*/
	void PerformMapping(const CByteImage *pInputImage, CByteImage *pOutputImage);
	
	
private:
	struct MapCoordinates
	{
		int f00, f01, f10, f11;
	};

	// pure virtual method
	virtual void ComputeOriginalCoordinates(const Vec2d &newCoordinates, Vec2d &originalCoordinates) = 0;
	

	// private attributes
	int width, height; // image size

	// precomputed maps
	int *m_pOffsetMap;
	MapCoordinates *m_pCoordinateMap;

	// flags
	bool m_bInterpolate;
	bool m_bMapComputed;
};



#endif /* _IMAGE_MAPPER_H_ */
