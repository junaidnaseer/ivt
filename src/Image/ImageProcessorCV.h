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
// Filename:  ImageProcessorCV.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _IMAGE_PROCESSOR_CV_H_
#define _IMAGE_PROCESSOR_CV_H_


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;
struct Vec2d;



// ****************************************************************************
// ImageProcessorCV
// ****************************************************************************

/*!
	\brief Deprecated.

	Deprecated. Use ImageProcessor instead.
*/
namespace ImageProcessorCV
{
	enum BayerPatternType
	{
		eBG2BGR,
		eGB2BGR,
		eRG2BGR,
		eGR2BGR,
		eBG2RGB,
		eGB2RGB,
		eRG2RGB,
		eGR2RGB
	};

	/*!
		\brief Image processing functions using OpenCV.

		Use ImageProcessor::ConvertImage(const CByteImage*, CByteImage*) instead.
	*/
	void ConvertImage(CByteImage *pInputImage, CByteImage *pOutputImage);
	
	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::CalculateGradientImagePrewitt or ImageProcessor::CalculateGradientImageSobel
		for images of type CByteImage::eGrayScale and ImageProcessor::CalculateGradientImage for images
		of type CByteImage::eRGB24 instead.
	*/
	void CalculateGradientImage(CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\deprecated
		\brief Deprecated.

		It is recommended not to compute gradients on HSV images.

		Use ImageProcessor::CalculateGradientImagePrewitt or ImageProcessor::CalculateGradientImageSobel
		for images of type CByteImage::eGrayScale and ImageProcessor::CalculateGradientImage for images
		of type CByteImage::eRGB24 instead.
	*/
	void CalculateGradientImageHSV(CByteImage *pInputImage, CByteImage *pOutputImage);
	
	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::FlipY instead.
	*/
	void FlipY(CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::GaussianSmooth3x3 instead.
	*/
	void GaussianSmooth3x3(CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::GaussianSmooth5x5 instead.
	*/
	void GaussianSmooth5x5(CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::Resize instead.
	*/
	void Resize(const CByteImage *pInputImage, CByteImage *pOutputImage, int x = -1, int y = -1, int width = -1, int height = -1);

	/*!
		\brief Calls cvSmooth with the parameter CV_BILATERAL.

		Performs a 3x3 bilateral smoothing filter.

		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be either both of type CByteImage::eGrayScale or both of type CByteImage::eRGB24.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param param1 The first parameter of cvSmooth when using CV_BILATERAL (color sigma).
		@param param2 The second parameter of cvSmooth when using CV_BILATERAL (space sigma).
	*/
	void BilateralSmooth(CByteImage *pInputImage, CByteImage *pOutputImage, int param1, int param2);
	
	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::Laplace1 or ImageProcessor::Laplace2 instead.
	*/
	void Laplacian3x3(CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\brief Calls cvLaplace with size 5x5.

		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be both of type CByteImage::eGrayScale.

		@param pInputImage: The input image.
		@param pOutputImage: The output image.
	*/
	void Laplacian5x5(CByteImage *pInputImage, CByteImage *pOutputImage);
	
	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::CalculateHSVImage instead.
	*/
	void CalculateHSVImage(CByteImage *pInputImage, CByteImage *pOutputImage);
	
	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::Canny(const CByteImage*, CByteImage*, int, int) instead.
	*/
	void Canny(CByteImage *pInputImage, CByteImage *pOutputImage, int nLowThreshold, int nHighThreshold);

	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::Dilate instead.
	*/
	void Dilate(CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::Erode instead.
	*/
	void Erode(CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::Average instead.
	*/
	double Mean(CByteImage *pImage1, CByteImage *pImage2);

	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::CalculateHarrisInterestPoints instead.
	*/
	int CalculateFeaturePoints(const CByteImage *pImage, Vec2d *pFeaturePoints, int nMaxPoints, float fQualityLevel = 0.01f, float fMinDistance = 5.0f, bool bUseHarris = false);
	
	/*!
		\deprecated
		\brief Deprecated.

		Use ImageProcessor::ConvertBayerPattern instead.
	*/
	void ConvertBayerPattern(CByteImage *pInputImage, CByteImage *pOutputImage, BayerPatternType type = eRG2BGR);
}



#endif /* _IMAGE_PROCESSOR_CV_H_ */
