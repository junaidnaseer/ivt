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
// Filename:  ImageProcessor.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************
// Changes:   28.09.2011, Jan Issac
//            * Added methods ConvertImage for conversion from CFloatImage
//              to CByteImage and vice versa
// ****************************************************************************

/** \defgroup ImageProcessing Image processing */


#ifndef _IMAGE_PROCESSOR_H_
#define _IMAGE_PROCESSOR_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Math/Math2d.h"
#include "Math/Math3d.h"
#include "Structs/Structs.h"
#include "Structs/ObjectDefinitions.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CFloatMatrix;
class CDoubleMatrix;
class CRGBColorModel;
class CByteImage;
class CShortImage;
class CIntImage;
class CFloatImage;
class CColorParameterSet;
struct Mat3d;



// ****************************************************************************
// ImageProcessor
// ****************************************************************************

/*!
	\namespace ImageProcessor
	\ingroup ImageProcessing
	\brief Central namespace offering various image processing routines and functions.
	
	The ImageProcessor namespace provides various image processing functions, mostly operating on
	instances of CByteImage, but also CShortImage and CIntImage, and conversion functions between these types.
	Among the offered functions are filtering (edge filters, smoothing filters), color segmentation,
	edge detection, corner detection, line/circle detection, histogram-based operators, morphological operators,
	arithmetic operators, logical operators, affine point operators, homography transformations, resize,
	thresholding and region growing.
*/
namespace ImageProcessor
{
	/*!
		\brief Converts a grayscale CByteImage to an RGB CByteImage image and vice versa.

		The RGB image can be of type CByteImage::eRGB24 or CByteImage::eRGB24Split.<br>
		If pInputImage->type and pOutputImage->type are equal, the input image is copied to the output image.
		
		The width and height of pInputImage and pOutputImage must match.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bFast If true, a fast bit shift implementation of g = (r + 2 * g + b + 2) / 4 is used, otherwise an approximation of 0.299 * r + 0.587 * g + 0.114 * b using integer arithmetics is used.
		@param pROI Describes the area containing the pixels which shall be converted. If pROI is 0, then the whole image is processed.
	*/
	bool ConvertImage(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bFast = false, const MyRegion *pROI = 0);

    /*!
        \brief Converts a CFloatImage to a CByteImage.

        Converts pInputImage to pOutputImage by casting the float values to unsigned char.

        The dimension and channel number of pInputImage must match the dimension channel number of pOutputImage.<br>

        @param pInputImage The input image.
        @param pOutputImage The output image.
        @param equalize If set to true, the input values are equalized.
    */
    bool ConvertImage(const CFloatImage *pInputImage, CByteImage *pOutputImage, bool equalize = false);

    /*!
        \brief Converts a CByteImage to a CFloatImage.

        Converts pInputImage to pOutputImage by casting the unsigned char values to float.

        The dimension and channel number of pInputImage must match the dimension channel number of pOutputImage.<br>

        @param pInputImage The input image.
        @param pOutputImage The output image.
    */
    bool ConvertImage(const CByteImage *pInputImage, CFloatImage *pOutputImage);

    /*!
        \brief Converts a CFloatMatrix to a grayscale CByteImage.

        Converts pInputImage to pOutputImage by linearly mapping the interval [min, max] to [0, 255], where min/max are the minimum/maximum value in pInputImage. This conversion can cause a loss of information.<br>
        If pInputImage is homogeneous, the values of pOutputImage are all set to zero.

        The column count of pInputImage must match the width of pOutputImage and the row count of pInputImage must match the height of pOutputImage.<br>
        pOutputImage must be of type CByteImage::eGrayScale.

        @param pInputImage The input image.
        @param pOutputImage The output image.
    */
    bool ConvertImage(const CFloatMatrix *pInputImage, CByteImage *pOutputImage);

	/*!
		\brief Converts a grayscale CByteImage to a CFloatMatrix.

		Converts pInputImage to pOutputImage by casting the unsigned char values to float.
		
		The width of pInputImage must match the column count of pOutputImage and the height of pInputImage must match the row count of pOutputImage.<br>
		pInputImage must be of type CByteImage::eGrayScale.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool ConvertImage(const CByteImage *pInputImage, CFloatMatrix *pOutputImage);
	
	/*!
		\brief Converts a CShortImage to a grayscale CByteImage.

		Converts pInputImage to pOutputImage by linearly mapping the interval [min, max] to [0, 255], where min/max are the minimum/maximum value in pInputImage. This conversion can cause a loss of information.<br>
		If pInputImage is homogeneous, the values of pOutputImage are all set to zero.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pOutputImage must be of type CByteImage::eGrayScale.

		@param pInputImage The input image.
		@param pOutputImage The output image.

	*/
	bool ConvertImage(const CShortImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\brief Converts a grayscale CByteImage to a CShortImage.

		Converts pInputImage to pOutputImage by casting the unsigned char values to short.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage must be of type CByteImage::eGrayScale.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool ConvertImage(const CByteImage *pInputImage, CShortImage *pOutputImage);
	
	/*!
		\brief Converts a CIntImage to a grayscale CByteImage.
	 
		Converts pInputImage to pOutputImage by linearly mapping the interval [min, max] to [0, 255], where min/max are the minimum/maximum value in pInputImage. This conversion can cause a loss of information.<br>
		If pInputImage is homogeneous, the values of pOutputImage are all set to zero.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pOutputImage must be of type CByteImage::eGrayScale.
	 
		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool ConvertImage(const CIntImage *pInputImage, CByteImage *pOutputImage);
	
	/*!
		\brief Converts a grayscale CByteImage to a CIntImage.
	 
		Converts pInputImage to pOutputImage by casting the unsigned char values to int.
	
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage must be of type CByteImage::eGrayScale.
	 
		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool ConvertImage(const CByteImage *pInputImage, CIntImage *pOutputImage);
	
	/*!
		\brief Converts a CFloatMatrix to a CDoubleMatrix.

		Converts pInputImage to pOutputImage by casting the float values to double.
		
		The row count and the column count of pInputImage and pOutputImage must match.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool ConvertMatrix(const CFloatMatrix *pInputImage, CDoubleMatrix *pOutputImage);
	
	/*!
		\brief Converts a CDoubleMatrix to a CFloatMatrix.

		Converts pInputImage to pOutputImage by casting the double values to float. This conversion can cause a loss of precision.
		
		The row count and the column count of pInputImage and pOutputImage must match.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool ConvertMatrix(const CDoubleMatrix *pInputImage, CFloatMatrix *pOutputImage);

	/*!
		\brief Copies one CByteImage to another.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param pROI Describes the area containing the pixels which shall be copied. If pROI is 0, then the full image is copied.
		@param bUseSameSize This parameter is only relevant if pROI is not 0. If bUseSameSize is true, the width and height of pInputImage and pOutputImage must match. Otherwise, the dimensions of pROI and pOutputImage must match, i.e. pROI->max_x - pROI->min_x + 1 = pOutputImage->width and pROI->max_y - pROI->min_y + 1 = pOutputImage->height.
	*/
	bool CopyImage(const CByteImage *pInputImage, CByteImage *pOutputImage, const MyRegion *pROI = 0, bool bUseSameSize = false);

	/*!
		\brief Copies one CShortImage to another.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param pROI Describes the area containing the pixels which shall be copied. If pROI is 0, then the full image is copied.
		@param bUseSameSize This parameter is only relevant if pROI is not 0. If bUseSameSize is true, the width and height of pInputImage and pOutputImage must match. Otherwise, the dimensions of pROI and pOutputImage must match, i.e. pROI->max_x - pROI->min_x + 1 = pOutputImage->width and pROI->max_y - pROI->min_y + 1 = pOutputImage->height.
	*/
	bool CopyImage(const CShortImage *pInputImage, CShortImage *pOutputImage, const MyRegion *pROI = 0, bool bUseSameSize = false);

	/*!
		\brief Copies one CFloatMatrix to another.

		The row count and the column count of pInputImage and pOutputImage must match.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool CopyMatrix(const CFloatMatrix *pInputImage, CFloatMatrix *pOutputImage);
	
	/*!
		\brief Copies one CDoubleMatrix to another.

		The row count and the column count of pInputImage and pOutputImage must match.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool CopyMatrix(const CDoubleMatrix *pInputImage, CDoubleMatrix *pOutputImage);

	/*!
		\brief Sets all values in a CByteImage to zero.

		@param pImage The image.
		@param pROI The region which contains the values which shall be set to zero. If pROI is 0, then all pixels of pImage are set to zero.
	*/
	void Zero(CByteImage *pImage, const MyRegion *pROI = 0);
	
	/*!
		\brief Sets all values in a CShortImage to zero.

		@param pImage The image.
	*/
	void Zero(CShortImage *pImage);
	
	/*!
		\brief Sets all values in a CIntImage to zero.

		@param pImage The image.
	*/
	void Zero(CIntImage *pImage);
	
	/*!
		\brief Sets all values in a CFloatMatrix to zero.

		@param pImage The image.
	*/
	void Zero(CFloatMatrix *pImage);

	/*!
		\brief Sets all values in a CDoubleMatrix to zero.

		@param pImage The image.
	*/
	void Zero(CDoubleMatrix *pImage);


	/*!
		\brief Flips the rows in a CByteImage vertically and writes the result to a CByteImage.
	 
		Flips the rows in pInputImage vertically, i.e. mirros the image with the center axis, and writes the result to pOutputImage.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be either both of type CByteImage::eGrayScale or both of type CByteImage::eRGB24.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool FlipY(const CByteImage *pInputImage, CByteImage *pOutputImage);

		
	/*!
		\brief Applies an average filter to a CByteImage and writes the result to a CByteImage.

		Applies an average filter (low pass filter) of size nMaskSize x nMaskSize to pInputImage and writes the result to pOutputImage.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
		
		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param nMaskSize Determines the side length of the window whose interior points are used for computing the average. Must be an odd integer greater than 2.
	*/
	bool AverageFilter(const CByteImage *pInputImage, CByteImage *pOutputImage, int nMaskSize = 3);

	/*!
		\brief Applies a Gaussian filter to a CByteImage and writes the result to a CFloatMatrix.
		
		Applies a Gaussian filter of size nKernelSize x nKernelSize to pInputImage and writes the result to pOutputImage.
		
		The width and height of pInputImage must match the row and column count of pOutputImage. The image size must be at least nKernelSize x nKernelSize.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param fVariance The variance \f$ \sigma^2 \f$ used for constructing the Gaussian kernel.
		@param nKernelSize Determines the side the length of the Gaussian kernel used for smoothing. It should meet the following criterion: \f$ {\rm nKernelSize} \geq \lfloor 2\sigma \rfloor \cdot 2 + 1 \f$ to achieve a meaningful approximation.
	*/
	bool GaussianSmooth(const CByteImage *pInputImage, CFloatMatrix *pOutputImage, float fVariance, int nKernelSize);
	
	/*!
		\brief Applies a Gaussian filter to a CFloatMatrix and writes the result to a CFloatMatrix.
		
		Applies a Gaussian filter of size nKernelSize x nKernelSize to pInputImage and writes the result to pOutputImage.
		
		The row and column count of pInputImage and pOutputImage must match. The image size must be at least nKernelSize x nKernelSize.

		pInputImage must be of type CByteImage::eGrayScale.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param fVariance The variance \f$ \sigma^2 \f$ used for constructing the Gaussian kernel.
		@param nKernelSize Determines the size of the Gaussian kernel used for smoothing. It should meet the following criterion: \f$ {\rm nKernelSize} \geq \lfloor 2\sigma \rfloor \cdot 2 + 1 \f$ to achieve a meaningful approximation.
	*/
	bool GaussianSmooth(const CFloatMatrix *pInputImage, CFloatMatrix *pOutputImage, float fVariance, int nKernelSize);

	/*!
		\brief Applies a Gaussian filter to a CByteImage and writes the result to a CByteImage.
	 
		Applies a Gaussian filter of size nKernelSize x nKernelSize to pInputImage and writes the result to pOutputImage.
		
		The width and height of pInputImage and pOutputImage must match. The image size must be at least nKernelSize x nKernelSize.

		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param fVariance The variance \f$ \sigma^2 \f$ used for constructing the Gaussian kernel.
		@param nKernelSize Determines the size of the Gaussian kernel used for smoothing. It should meet the following criterion: \f$ {\rm nKernelSize} \geq \lfloor 2\sigma \rfloor \cdot 2 + 1 \f$ to achieve a meaningful approximation.
	*/
	bool GaussianSmooth(const CByteImage *pInputImage, CByteImage *pOutputImage, float fVariance, int nKernelSize);

	/*!
		\brief Applies a 5x5 Gaussian filter to a CFloatMatrix and writes the result to a CFloatMatrix.
		
		This is an optimized implementation of GaussianSmooth(const CFloatMatrix *pInputImage, CFloatMatrix *pOutputImage, float fVariance, int nKernelSize) for the special case of a 5x5 kernel, i.e. nKernelSize = 5.

		The row and column count of pInputImage and pOutputImage must match. The image size must be at least 5 x 5.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param fVariance The variance \f$ \sigma^2 \f$ used for constructing the Gaussian kernel. According to the formula \f$ n \geq \lfloor 2\sigma \rfloor \cdot 2 + 1 \f$ (where n denotes the side length of the filter kernel, i.e. n = 5 for this function), it must be \f$ \sigma^2 < 1.5^2 \f$.
	*/
	bool GaussianSmooth5x5(const CFloatMatrix *pInputImage, CFloatMatrix *pOutputImage, float fVariance);

	/*!
		\brief Applies a 3x3 Gaussian filter to a CByteImage and writes the result to a CByteImage.

		Applies a 3x3 Gaussian low pass filter with \f$ \sigma \approx 0.85 \f$, i.e. variance = \f$ \sigma^2 \approx 0.85^2 \f$, to pInputImage and writes the result to pOutputImage. The used filter kernel is:
		\f$
		\frac{1}{16}\left(\begin{array}{ccc}
		1 & 2 & 1\\
		2 & 4 & 2\\
		1 & 2 & 1\\
		\end{array}\right)
		\f$.
		
		All calculations are performed with integer arithmetics.
		
		The width and height of pInputImage and pOutputImage must match. The image size must be at least 3 x 3.

		pInputImage and pOutputImage must be either both of type CByteImage::eGrayScale or both of type CByteImage::eRGB24.
		
		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool GaussianSmooth3x3(const CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\brief Applies a 5x5 Gaussian filter to a CByteImage and writes the result to a CByteImage.

		Applies a 5x5 Gaussian filter with \f$ \sigma \approx 0.9 \f$, i.e. variance = \f$ \sigma^2 \approx 0.9^2 \f$, to pInputImage and writes the result to pOutputImage. The used filter kernel is:
		\f$
		\frac{1}{256}\left(\begin{array}{ccccc}
		1 & 4 & 6 & 4 & 1\\
		4 & 16 & 24 & 16 & 4\\
		6 & 24 & 36 & 24 & 6\\
		4 & 16 & 24 & 16 & 4\\
		1 & 4 & 6 & 4 & 1
		\end{array}\right)
		\f$.
	 
		All calculations are performed with integer arithmetics.
		
		The width and height of pInputImage and pOutputImage must match. The image size must be at least 5 x 5.

		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool GaussianSmooth5x5(const CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\brief Applies a 1x3 highpass filter to a CFloatMatrix and write the result to a CFloatMatrix.
		
		This is a simple highpass filter computing gradients in x-direction and is thus suitable for the detection of vertical edges. The used filter kernel is:
		\f$
		\left(
		\begin{array}{ccc}
		1 & 0 & -1 \\
		\end{array}
		\right)
		\f$.
		
		The row and column count of pInputImage and pOutputImage must match.
		
		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool HighPassX3(const CFloatMatrix *pInputImage, CFloatMatrix *pOutputImage);

	/*!
		\brief Applies a 3x1 highpass filter to a CFloatMatrix and writes the result to a CFloatMatrix.
		
		This is a simple highpass filter computing gradients in y-direction and is thus suitable for the detection of horizontal edges. The used filter kernel is:
		\f$
		\left(
		\begin{array}{c}
		1 \\
		0 \\
		-1 \\
		\end{array}\right)
		\f$.
		
		The row and column count of pInputImage and pOutputImage must match.
		
		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool HighPassY3(const CFloatMatrix *pInputImage, CFloatMatrix *pOutputImage);

	/*!
		\brief Filters a CByteImage with the SobelX operator and writes the result to a CShortImage.
		
		The SobelX operator computes gradients in x-direction and is thus suitable for the detection of vertical edges. The used filter kernel is:
		\f$
		\left(\begin{array}{ccc}
		-1 & 0 & 1\\
		-2 & 0 & 2\\
		-1 & 0 & 1\\
		\end{array}\right)
		\f$.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage must be of type CByteImage::eGrayScale.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage, i.e. edge direction information is lost.
	*/
	bool SobelX(const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue = true);

	/*!
		\brief Filters a CByteImage with the SobelX operator and writes the result to a CByteImage.
		
		This function is a convenience function, creating an instance of CShortImage, calling SobelX(const CByteImage*, CShortImage*, bool), and then calling ConvertImage(const CShortImage*, CByteImage*).
		See the documentations of these functions for further information. Note that there is no runtime penalty if pInputImage and pOutputImage share the same memory area, since a temporary instance of
		CShortImage is used for storing the result of the convolution in any case.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage, i.e. edge direction information is lost.
	*/
	bool SobelX(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bAbsoluteValue = true);

	/*!
		\brief Filters a CByteImage with the SobelY operator and writes the result to a CShortImage.
		 
		The SobelY operator computes gradients in y-direction and is thus suitable for the detection of horizontal edges. The used filter kernel is:
		\f$
		\left(\begin{array}{ccc}
		-1 & -2 & -1\\
		 0 & 0 & 0\\
		 1 & 2 & 1\\
		\end{array}\right)
		\f$.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage must be of type CByteImage::eGrayScale.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage, i.e. edge direction information is lost.
	*/
	bool SobelY(const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue = true);

	/*!
		\brief Filters a CByteImage with the SobelY operator and writes the result to a CByteImage.
		
		This function is a convenience function, creating an instance of CShortImage, calling SobelY(const CByteImage*, CShortImage*, bool), and then calling ConvertImage(const CShortImage*, CByteImage*).
		See the documentations of these functions for further information. Note that there is no runtime penalty if pInputImage and pOutputImage share the same memory area, since a temporary instance of
		CShortImage is used for storing the result of the convolution in any case.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage, i.e. edge direction information is lost.
	*/
	bool SobelY(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bAbsoluteValue = true);

	/*!
		\brief Filters a CByteImage with the PrewittX operator and writes the result to a CShortImage.
		
		The PrewittX operator computes gradients in x-direction and is thus suitable for the detection of vertical edges. The used filter kernel is:
		\f$
		\left(\begin{array}{ccc}
		-1 & 0 & 1\\
		-1 & 0 & 1\\
		-1 & 0 & 1\\
		\end{array}\right)
		\f$.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage must be of type CByteImage::eGrayScale.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage, i.e. edge direction information is lost.
	*/
	bool PrewittX(const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue = true);
	
	/*!
		\brief Filters a CByteImage with the PrewittX operator and writes the result to a CByteImage.
	 
		This function is a convenience function, creating an instance of CShortImage, calling PrewittX(const CByteImage*, CShortImage*, bool), and then calling ConvertImage(const CShortImage*, CByteImage*).
		See the documentations of these functions for further information. Note that there is no runtime penalty if pInputImage and pOutputImage share the same memory area, since a temporary instance of
		CShortImage is used for storing the result of the convolution in any case.
	 
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
	 
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage, i.e. edge direction information is lost.
	*/
	bool PrewittX(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bAbsoluteValue = true);

	/*!
		\brief Filters a CByteImage with the PrewittY operator and writes the result to a CShortImage.
	 
		The PrewittY operator computes gradients in y-direction and is thus suitable for the detection of horizontal edges. The used filter kernel is:
		\f$
		\left(\begin{array}{ccc}
		-1 & -1 & -1\\
		0 & 0 & 0\\
		1 & 1 & 1\\
		\end{array}\right)
		\f$.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage must be of type CByteImage::eGrayScale.
	 
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage, i.e. edge direction information is lost.
	*/
	bool PrewittY(const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue = true);

	/*!
		\brief Filters a CByteImage with the PrewittY operator and writes the result to a CByteImage.
	 
		This function is a convenience function, creating an instance of CShortImage, calling PrewittY(const CByteImage*, CShortImage*, bool), and then calling ConvertImage(const CShortImage*, CByteImage*).
		See the documentations of these functions for further information. Note that there is no runtime penalty if pInputImage and pOutputImage share the same memory area, since a temporary instance of
		CShortImage is used for storing the result of the convolution in any case.
	 
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
	 
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage, i.e. edge direction information is lost.
	*/
	bool PrewittY(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bAbsoluteValue = true);

	/*!
		\brief Filters a CByteImage with the Laplace1 operator and writes the result to a CShortImage.
		
		The Laplace1 operator is an omnidirectional high pass filter (edge filter). It is an approximation of a second order derivative. The used filter kernel is:
		\f$
		\left(\begin{array}{ccc}
		0 & 1 & 0 \\
		1 & -4 & 1 \\
		0 & 1 & 0 \\
		\end{array}\right)
		\f$.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage must be of type CByteImage::eGrayScale.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage.
	*/
	bool Laplace1(const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue = true);

	/*!
		\brief Filters a CByteImage with the Laplace1 operator and writes the result to a CByteImage.
	 
		This function is a convenience function, creating an instance of CShortImage, calling Laplace1(const CByteImage*, CShortImage*, bool), and then calling ConvertImage(const CShortImage*, CByteImage*).
		See the documentations of these functions for further information. Note that there is no runtime penalty if pInputImage and pOutputImage share the same memory area, since a temporary instance of
		CShortImage is used for storing the result of the convolution in any case.
	 
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
	 
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage.
	*/
	bool Laplace1(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bAbsoluteValue = true);

	/*!
		\brief Filters a CByteImage with the Laplace2 operator and writes the result to a CShortImage.
	 
		The Laplace2 operator is an omnidirectional high pass filter (edge filter). It is an approximation of a second order derivative. The used filter kernel is:
		\f$
		\left(\begin{array}{ccc}
		1 & 1 & 1 \\
		1 & -8 & 1 \\
		1 & 1 & 1 \\
		\end{array}\right)
		\f$.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage must be of type CByteImage::eGrayScale.
	 
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage.
	*/
	bool Laplace2(const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue = true);

	/*!
		\brief Filters a CByteImage with the Laplace2 operator and writes the result to a CByteImage.
	 
		This function is a convenience function, creating an instance of CShortImage, calling Laplace2(const CByteImage*, CShortImage*, bool), and then calling ConvertImage(const CShortImage*, CByteImage*).
		See the documentations of these functions for further information. Note that there is no runtime penalty if pInputImage and pOutputImage share the same memory area, since a temporary instance of
		CShortImage is used for storing the result of the convolution in any case.
	 
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
	 
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage.
	*/
	bool Laplace2(const CByteImage *pInputImage, CByteImage *pOutputImage, bool bAbsoluteValue = true);

	/*!
		\brief Applies a user defined filter to a CByteImage and writes the result to a CByteImage.

		Applies a user defined filter of size nMaskSize x nMaskSize to pInputImage and writes the result to pOutputImage. Overflows are not handled.
		Use the functions GeneralFilter(const CByteImage*, CShortImage*, const int*, int, int, bool) or GeneralFilter(const CByteImage*, CFloatMatrix*, const int*, int, int, bool),
		if overflows can potentially occur for the desired kernel or for higher precision.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
		
		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param pKernel The filter kernel. The coefficients are expected to be stored row wise, starting from the top left corner.
		@param nMaskSize The side length of the filter kernel. Must be an odd integer greater than 2.
		@param nDivider Each value is divided by nDivider before it is written to pOutputImage. If nDivider = 1, then the division is omitted, thus saving computation time.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage.
	*/
	bool GeneralFilter(const CByteImage *pInputImage, CByteImage *pOutputImage, const int *pKernel, int nMaskSize, int nDivider = 1, bool bAbsoluteValue = false);

	/*!
		\brief Applies a user defined filter to a CByteImage and writes the result to a CShortImage.
		
		Applies a user defined filter of size nMaskSize x nMaskSize to pInputImage and writes the result to pOutputImage. Overflows are not handled.
		Use the function GeneralFilter(const CByteImage*, CFloatMatrix*, const int*, int, int, bool) for higher precision.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param pKernel The filter kernel. The coefficients are expected to be stored row wise, starting from the top left corner.
		@param nMaskSize The side length of the filter kernel. Must be an odd integer greater than 2.
		@param nDivider Each value is divided by nDivider before it is written to pOutputImage. If nDivider = 1, then the division is omitted, thus saving computation time.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage.
	*/
	bool GeneralFilter(const CByteImage *pInputImage, CShortImage *pOutputImage, const int *pKernel, int nMaskSize, int nDivider = 1, bool bAbsoluteValue = false);

	/*!
		\brief Applies a user defined filter to a CByteImage and writes the result to a CFloatMatrix.
	 
		Applies a user defined filter of size nMaskSize x nMaskSize to pInputImage and writes the result to pOutputImage.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
	 
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param pKernel The filter kernel. The coefficients are expected to be stored row wise, starting from the top left corner.
		@param nMaskSize The side length of the filter kernel. Must be an odd integer greater than 2.
		@param nDivider Each value is divided by nDivider before it is written to pOutputImage. If nDivider = 1, then the division is omitted, thus saving computation time.
		@param bAbsoluteValue If true, the absolute values are written to pOutputImage.
	 */
	bool GeneralFilter(const CByteImage *pInputImage, CFloatMatrix *pOutputImage, const int *pKernel, int nMaskSize, int nDivider = 1, bool bAbsoluteValue = false);

	/*!
		\brief Applies a combined Prewitt filter for both x- and y-direction to a CByteImage and stores the result in a CByteImage.

		Applies a PrewittX and a PrewittY operator for each pixel of pInputImage, resulting in the values gx and gy, and writes the result min(255, max(gx, gy)) to pOutputImage.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
		
		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool CalculateGradientImagePrewitt(const CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\brief Applies a combined Sobel filter for both x- and y-direction to a CByteImage and stores the result in a CByteImage.

		Applies a SobelX and a SobelY operator for each pixel of pInputImage, resulting in the values gx and gy, and writes the result min(255, max(gx, gy)) to pOutputImage.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be of type CByteImage::eGrayScale.
		
		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool CalculateGradientImageSobel(const CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\brief Applies a combined simple gradient filter for both x- and y-direction to a CByteImage and stores the result in a CByteImage.

		Applies a 1x3 gradient operator of the form (-1 0 1) and a 3x1 gradient operator of the form (1 0 -1)^T for each pixel of pInputImage,
		resulting in the values gx and gy, and writes the result min(255, max(gx, gy)) to pOutputImage.
		In the case of color images, each channel is processed independently.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be either both of type CByteImage::eGrayScale or both of type CByteImage::eRGB24.
		In the case of CByteImage::eRGB24, each channel is processed independently and for each pixel, the maximum over all channels is taken.
		
		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool CalculateGradientImage(const CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\brief Calculates a gradient image for a CByteImage and writes the result to a CByteImage, for the special case of binary images.
	 
		Calculates a gradient image for a binary image stored in pInputImage and writes the result to pOutputImage. This function implements
		an optimized version of \f$ I_g(x,y) = \max(|I(x+1,y) - I(x,y)|, |I(x,y+1)-I(x,y)|)\f$ for the special case of binary images.
		
		Note that the gradient locations are in general displaced with an offset of (-0.5, -0.5) pixels with respect to the true edge locations
		in the input image. If subpixel accuracy is needed, it is recommended to filter the image with a Gaussian filter first
		(e.g. GaussianSmooth5x5(const CByteImage*, CByteImage*)) and then apply a general edge filter (e.g. CalculateGradientImageSobel(const CByteImage*, CShortImage*))
		or an edge detector (e.g. Canny(const CByteImage*, CVec2dArray&, CVec2dArray&, int, int), which directly outputs edge points with subpixel accuracy).
		The width and height of pInputImage and pOutputImage must match.
		
		pInputImage and pOutputImage must be both of type CByteImage::eGrayScale and must contain binary images, i.e. contain only two different
		values: 0 and a non-zero value (usually 255 for visualization purposes).
		
		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.
	 
	 @param pInputImage The input image.
	 @param pOutputImage The output image.
	*/
	bool CalculateGradientImageBinary(const CByteImage *pInputImage, CByteImage *pOutputImage);


	/*!
		\brief Applies an affine point operation to a CByteImage and writes the result to a CByteImage.

		Applies an affine point operation with the formula a * x + b to pInputImage and writes the result to pOutputImage.<br>
		The resulting float value is rounded to an integer value v, and min(255, max(0, v)) is written to pOutputImage.
		
		The width, height and type of pInputImage and pOutputImage must match.<br>
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param a Coefficient for the affine point operation (factor).
		@param b Coefficient for the affine point operation (addend).
	*/
	bool ApplyAffinePointOperation(const CByteImage *pInputImage, CByteImage *pOutputImage, float a, float b);
	
	/*!
	 \brief Applies histogram equalization to all channels of a color CByteImage.
	 
	 Applies histogram equalization to all three channels of a color image independently.
	 
	 The width, height and type of pInputImage and pOutputImage must match.<br>
	 All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
	 As this is a point operation, there is no copy overhead if pInputImage and pOutputImage share the same memory area.
	 
	 @param pInputImage The input image. Must be a color image of type CByteImage::eRGB24 or CByteImage::eRGB24Split.
	 @param pOutputImage The output image. Must be of the same type as pInputImage.
	 */
	bool NormalizeColor(const CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\brief Performs histogram equalization for a CByteImage and writes the result to a CByteImage.

		Performs histogram equalization by computing the accumulated histogram \f$H_a\f$ for input image and using the following
		mapping function for writing pOutputImage: \f$I_{out}(x, y) = \frac{255}{n} \cdot H_a(I_{in}(x, y))\f$, where n denotes the number of pixels contained in pInputImage (resp. pOutputImage).

		The width and height of pInputImage and pOutputImage must match.<br>
		Both images must be of type CByteImage::eGrayScale.
	 
		As this is a point operation, there is no copy overhead if pInputImage and pOutputImage share the same memory area.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool HistogramEqualization(const CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\brief Performs histogram stretching on pInputImage and stores the result in pOutputImage.

		Linearly maps the interval [min, max] to [0, 255], where min/max are the two peaks of the histogram determined with the aid of a quantile analysis, assuming a bimodal histogram.<br>
		If pInputImage is homogeneous, the values of pOutputImage are all set to zero.

		The width and height of pInputImage and pOutputImage must match.<br>
		Both images must be of type CByteImage::eGrayScale.
	 
		As this is a point operation, there is no copy overhead if pInputImage and pOutputImage share the same memory area.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param p1 The quantile used for determining the position of the first peak in the histogram (minimum intensity).
		@param p2 The quantile used for determining the position of the second peak in the histogram (maximum intensity).
	*/
	bool HistogramStretching(const CByteImage *pInputImage, CByteImage *pOutputImage, float p1 = 0.1f, float p2 = 0.9f);

	/*!
		\brief Performs a spread operation on pInputImage and stores the result in pOutputImage.
	 
		Linearly maps the interval [min, max] to [0, 255], where min/max are the minimum/maximum value in pInputImage.<br>
		If pInputImage is homogeneous, the values of pOutputImage are all set to zero.

		The width and height of pInputImage and pOutputImage must match.<br>
		Both images must be of type CByteImage::eGrayScale.
	 
		As this is a point operation, there is no copy overhead if pInputImage and pOutputImage share the same memory area.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool Spread(const CByteImage *pInputImage, CByteImage *pOutputImage);
		
	
	/*!
		\brief Applies a homography to a CByteImage and writes the result to a CByteImage.

		The homography can generally be formulated using homogenous coordinates as follows:
		\f$ \left( \begin{array}{c} u' s \\ v' s \\ s \end{array} \right) = \left( \begin{array}{ccc} a_1 & a_2 & a_3 \\ a_4 & a_5 & a_6 \\ a_7 & a_8 & a_9 \end{array} \right) \cdot \left( \begin{array}{c} u \\ v \\ 1 \end{array} \right) \f$. \f$ a_9 = 1 \f$ is assumed without loss of generality because all real-valued multiples of the matrix define the same mapping.
		The mapping takes place as follows: for every (u, v) in the new image pOutputImage, the homography specified by the parameters \f$a_1, \ldots, a_8\f$ computes in the original image pInputImage.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be either both of type CByteImage::eGrayScale or both of type CByteImage::eRGB24.
	 
		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param A The 3x3 describing the homography matrix.
		@param bInterpolation Determines whether bilinear interpolation is used or not. 
	*/
	bool ApplyHomography(const CByteImage *pInputImage, CByteImage *pOutputImage, const Mat3d& A, bool bInterpolation = true);

	/*!
		\deprecated
		\brief Deprecated.

		This function is deprecated. It internally calls ApplyHomography(const CByteImage*, CByteImage*, const Mat3d&, bool) (A.r9 is set to 1.0f); please refer to the documentation of this function for more information.
	*/
	bool ApplyHomography(const CByteImage *pInputImage, CByteImage *pOutputImage, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, bool bInterpolation = true);

	/*!
		\brief Resizes a CByteImage and writes the result to a CByteImage.
	 
		The image stored in pInputImage is resized to the dimensions specified by pOutputImage. The result is written to pOutputImage.

		pInputImage and pOutputImage must be either both of type CByteImage::eGrayScale or both of type CByteImage::eRGB24.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param pROI Describes the area within pInputImage containing the pixels which shall be resized. If pROI is 0, then the whole image contained in pInputImage is resized.
		@param bInterpolation Determines whether bilinear interpolation is used or not.
	*/
	bool Resize(const CByteImage *pInputImage, CByteImage *pOutputImage, const MyRegion *pROI = 0, bool bInterpolation = true);

	/*!
		\brief Rotates pInputImage to the dimensions specified by pOutputImage and stores the result in pOutputImage.

		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be either both of type CByteImage::eGrayScale or both of type CByteImage::eRGB24.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param mx The x-coordinate of the rotation center.
		@param my The y-coordinate of the rotation center.
		@param theta The rotation angle, given in radians.
		@param bInterpolation Determines whether bilinear interpolation is used or not.
	*/
	bool Rotate(const CByteImage *pInputImage, CByteImage *pOutputImage, float mx, float my, float theta, bool bInterpolation = true);
	
	/*!
		\deprecated
		\brief Deprecated.

		This function is deprecated. It internally calls LinearAlgebra::DetermineHomography; please refer to the documentation of this function for more information.
	*/
	bool DetermineHomography(const Vec2d *pSourcePoints, const Vec2d *pTargetPoints, int nPoints, float &a1, float &a2, float &a3, float &a4, float &a5, float &a6, float &a7, float &a8);

	/*!
		\deprecated
		\brief Deprecated.

		This function is deprecated. It calls LinearAlgebra::DetermineAffineTransformation; please refer to the documentation of this function for more information.
	*/
	bool DetermineAffineTransformation(const Vec2d *pSourcePoints, const Vec2d *pTargetPoints, int nPoints, float &a1, float &a2, float &a3, float &a4, float &a5, float &a6);

	/*!
		\brief Calculates the saturation image for an RGB CByteImage and writes the result to a CByteImage.
		
		Computes the saturation image (S-channel of the HSV image) of pInputImage and writes the result to pOutputImage.

		The width and height of pInputImage and pOutputImage must match.

		@param pInputImage The input image. Must be of type CByteImage::eRGB24.
		@param pOutputImage The output image. Must be of type CByteImage::eGrayScale.
	*/
	bool CalculateSaturationImage(const CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\ingroup ColorProcessing
		\brief Performs color filtering with binarization for an RGB CByteImage, given a CRGBColorModel, and writes the result to a grayscale CByteImage.
		
		Calculates the color probability of each pixel in pInputImage based on the Mahalanobis distance and write the result to pOutputImage.

		Using the covariance matrix C the Mahalanobis distance is calculated by \f$ f_{+}(x) = exp(-\frac{1}{2}(x - \bar x)^T C^{-1} (x - \bar x)) \f$.
		pOutputImage is a binary image.

		The width and height of pInputImage and pOutputImage must match.
		
		@param pInputImage The input image. Must be of type CByteImage::eRGB24.
		@param pOutputImage The output image. Must be of type CByteImage::eGrayScale.
		@param pColorModel The color model.
		@param fThreshold The threshold value. All color probabilities > fThreshold are set to 255, all other to 0.
	*/
	bool FilterRGB(const CByteImage *pInputImage, CByteImage *pOutputImage, CRGBColorModel *pColorModel, float fThreshold);

	/*!
		\ingroup ColorProcessing
		\brief Computes the HSV image for a RGB CByteImage and writes the result to a CByteImage.
		
		Converts each pInputImage from RGB24 to HSV24 (Hue, Saturation, Value) and writes the result pOutputImage.

		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be both of type CByteImage::eRGB24.

		The pixels of pOutputImage are after completion encoded according to the HSV color model, although the type of the image is CByteImage::eRGB24.
		The values S (Saturation) and V (Value) have a possible range of [0, 255]. The value H (Hue) has a possible range of [0, 179].
	 
		As this is a point operation, there is no copy overhead if pInputImage and pOutputImage share the same memory area.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param pROI Describes the area containing the pixels which shall be converted. If pROI is 0, then the whole image is processed.
	*/
	bool CalculateHSVImage(const CByteImage *pInputImage, CByteImage *pOutputImage, const MyRegion *pROI = 0);
	
	/*!
		\ingroup ColorProcessing
		\brief Performs color filtering with binarization for an HSV CByteImage and writes the result to a grayscale CByteImage.

		The color segmentation is performed using fixed bounds (with the operator <= and >=) for each color channel, except of hue where a center value and a +- tolerance are specified.
	 
		The width and height of pInputImage and pOutputImage must match.

		@param pInputImage The input image. Must be of type CByteImage::eRGB24 and is assumed to be an HSV image (see also CalculateHSVImage(const CByteImage*, CByteImage*, const *pROI)).
		@param pOutputImage The output image. Must be of type CByteImage::eGrayScale.
		@param hue The hue center.
		@param tol_hue The hue tolerance.
		@param min_sat The minimum saturation.
		@param max_sat The maximum saturation.
		@param min_v The minimum value.
		@param max_v The maximum value.
		@param pROI Describes the area containing the pixels which shall be processed. If pROI is 0, then the whole image is processed.
	*/
	bool FilterHSV(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char hue, unsigned char tol_hue, unsigned char min_sat, unsigned char max_sat, unsigned char min_v, unsigned char max_v, const MyRegion *pROI = 0);

	/*!
		\ingroup ColorProcessing
		\brief Performs color filtering with binarization for an HSV CByteImage and writes the result to a grayscale CByteImage.

		The color segmentation is performed using fixed bounds (with the operator <= and >=) for each color channel.
	 
		The width and height of pInputImage and pOutputImage must match.

		@param pInputImage The input image. Must be of type CByteImage::eRGB24 and is assumed to be a HSV image (see also CalculateHSVImage(const CByteImage*, CByteImage*, const *pROI)).
		@param pOutputImage The output image. Must be of type CByteImage::eGrayScale.
		@param min_hue The minimum hue.
		@param max_hue The maximum hue.
		@param min_sat The minimum saturation.
		@param max_sat The maximum saturation.
		@param min_v The minimum value.
		@param max_v The maximum value.
		@param pROI Describes the area containing the pixels which shall be processed. If pROI is 0, then the whole image is processed.
	*/
	bool FilterHSV2(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char min_hue, unsigned char max_hue, unsigned char min_sat, unsigned char max_sat, unsigned char min_v, unsigned char max_v, const MyRegion *pROI = 0);

    /*!
        \ingroup ColorProcessing
        \brief Performs color filtering with binarization for a CByteImage and writes the result to a grayscale CByteImage.

        The color segmentation is performed using the FilterHSV method using the min/max values for the chosen color as defined in the ColorParameterSet.

        The width and height of pInputImage and pOutputImage must match.

        @param pInputImage The input image. Must be of type CByteImage::eRGB24 and be eithter in RGB oder HSV format.
        @param pOutputImage The output image. Must be of type CByteImage::eGrayScale.
        @param cColor The color that will be segmented.
        @param pColorParameterSet Contains color parameters, must contain the chosen color.
        @param bImageIsHSV Indicates whether the image is already in HSV format.
    */
    bool FilterColor(const CByteImage *pInputImage, CByteImage *pOutputImage, ObjectColor cColor, CColorParameterSet* pColorParameterSet, bool bImageIsHSV=true);


	/*!
		\brief Sets all pixels on a one pixel wide frame of a CByteImage to zero.

		The frame consists of the top and bottom row as well as the leftmost and rightmost column.

		@param pImage The target image.
	*/
	void ZeroFrame(CByteImage *pImage);

	/*!
		\brief Sets all pixels on a one pixel wide frame of a CShortImage to zero.

		The frame consists of the top and bottom row as well as the leftmost and rightmost column.

		@param pImage The target image.
	*/
	void ZeroFrame(CShortImage *pImage);
	
	/*!
		\brief Sets all pixels on a one pixel wide frame of a CIntImage to zero.
	 
		The frame consists of the top and bottom row as well as the leftmost and rightmost column.
	 
		@param pImage The target image.
	 */
	void ZeroFrame(CIntImage *pImage);	

	/*!
		\brief Copies all pixels on a one pixel wide frame from one CByteImage to CByteImage.

		The frame consists of the top and bottom row as well as the leftmost and rightmost column.
	 
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be either both of type CByteImage::eGrayScale or both of type CByteImage::eRGB24.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool CopyFrame(const CByteImage *pInputImage, CByteImage *pOutputImage);
	
	/*!
		\brief Sets all pixels on a one pixel wide outer frame in a CByteImage to the pixel values of the inner frame of a CByteImage.
	 
		This function is useful for post-processing of filter responses, in particular low pass filters (smoothing filters), which do not compute values for the outer frame.
		For high pass filters (edge filters) it is also meaningful to call Zero(CByteImage*).

		The outer frame consists of the top and bottom row as well as the leftmost and rightmost column.
		The inner frame consists of the second top most and second bottom most rows as well as the second leftmost and second rightmost columns.
	 
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be either both of type CByteImage::eGrayScale or both of type CByteImage::eRGB24.
	 
		pInputImage and pOutputImage can be pointers to the same image, with no runtime penalty.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool AdaptFrame(const CByteImage *pInputImage, CByteImage *pOutputImage);
	
	/*!
		\brief Calculates the inverted image of a CByteImage and writes the result to a CByteImage.
	 
		Inverting is accomplished by calculating 255 - value for each byte value.

		The width, height and type of pInputImage and pOutputImage must match.
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool Invert(const CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\brief Multiplies each byte value of a CByteImage with a floating point factor and writes the result to a CByteImage.
	 
		Overflows are handled by truncation.
		
		The width, height and type of pInputImage and pOutputImage must match.
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param fFactor The amplification factor.
	*/
	bool Amplify(const CByteImage *pInputImage, CByteImage *pOutputImage, float fFactor);
	
	/*!
		\brief Rotates a CByteImage by 180 degrees and writes the result to a CByteImage.
	 
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be either both of type CByteImage::eGrayScale or both of type CByteImage::eRGB24.

		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.

		@param pInputImage The input image.
		@param pOutputImage The output image.
	*/
	bool Rotate180Degrees(const CByteImage *pInputImage, CByteImage *pOutputImage);

	/*!
		\brief Applies the Canny edge detector to a CByteImage and writes the result to a CByteImage.
	 
		In pOutputImage, edge pixels will be encoded with the value 255, all other pixels with the value 0.
	 
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be both of type CByteImage::eGrayScale.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param nLowThreshold The low threshold for the Canny edge detector.
		@param nHighThreshold The high threshold for the Canny edge detector.
	*/
	bool Canny(const CByteImage *pInputImage, CByteImage *pOutputImage, int nLowThreshold, int nHighThreshold);

	/*!
		\brief Applies the Canny edge detector to a CByteImage and writes the edge pixels and directions to two instances of CVec2dArray.
	 
		This function computes edge pixels with subpixel accuracy as well as the edge direction for each edge pixel.

		@param pInputImage The input image. Must be of type CByteImage::eGrayScale.
		@param resultPoints Result list containing edge points with subpixel accuracy.
		@param resultDirections Result list containing gradient directions. Entries with the same index match between resultPoints and resultDirections.
		@param nLowThreshold The low threshold for the Canny edge detector.
		@param nHighThreshold The high threshold for the Canny edge detector.
	*/
	bool Canny(const CByteImage *pInputImage, CVec2dArray &resultPoints, CVec2dArray &resultDirections, int nLowThreshold, int nHighThreshold);

	/*!
		\brief Performs threshold binarization to a CByteImage and writes the result to a CByteImage.
	 
		Sets each pixel in pOutputImage to 255 if the corresponding pixel in pInputImage has a value greater than or equal to nThreshold and to 0 otherwise.

		The width and height of pInputImage and pOutputImage must match.
		pInputImage and pOutputImage must be both of type CByteImage::eGrayScale.
	 
		As this is a point operation, there is no copy overhead if pInputImage and pOutputImage share the same memory area.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param nThreshold The threshold value.
	*/
	bool ThresholdBinarize(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nThreshold);

	/*!
		\brief Performs inverse threshold binarization to a CByteImage and writes the result to a CByteImage.
	 
		Sets each pixel in pOutputImage to 255 if the corresponding pixel in pInputImage has a value less than or equal to nThreshold and to 0 otherwise.

		The width and height of pInputImage and pOutputImage must match.
		pInputImage and pOutputImage must be both of type CByteImage::eGrayScale.
	 
		As this is a point operation, there is no copy overhead if pInputImage and pOutputImage share the same memory area.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param nThreshold The threshold value.
	*/
	bool ThresholdBinarizeInverse(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nThreshold);

	/*!
		\brief Performs threshold binarization to a CByteImage and writes the result to a CByteImage.
	 
		Sets each pixel in pOutputImage to 255 if the corresponding pixel in pInputImage has a value greater than or equal to nMinThreshold and smaller than or equal nMaxThreshold, and to 0 otherwise.

		The width and height of pInputImage and pOutputImage must match.
		pInputImage and pOutputImage must be both of type CByteImage::eGrayScale.
	 
		As this is a point operation, there is no copy overhead if pInputImage and pOutputImage share the same memory area.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param nThreshold The threshold value.
	*/
	bool ThresholdBinarize(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nMinThreshold, unsigned char nMaxThreshold);

	/*!
		\brief Performs threhold filtering to a CByteImage and writes the result to a CByteImage.
	 
		Copies each pixel in pOutputImage if the corresponding pixel in pInputImage has a value greater than or equal to nThreshold and sets it to 0 otherwise.

		The width and height of pInputImage and pOutputImage must match.
		pInputImage and pOutputImage must be both of type CByteImage::eGrayScale.
	 
		As this is a point operation, there is no copy overhead if pInputImage and pOutputImage share the same memory area.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param nThreshold The threshold value.
	*/
	bool ThresholdFilter(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nThreshold);

	/*!
		\brief Performs inverse threhold filtering to a CByteImage and writes the result to a CByteImage.
	 
		Copies each pixel in pOutputImage if the corresponding pixel in pInputImage has a value less than or equal to nThreshold and sets it to 0 otherwise.

		The width and height of pInputImage and pOutputImage must match.
		pInputImage and pOutputImage must be both of type CByteImage::eGrayScale.
	 
		As this is a point operation, there is no copy overhead if pInputImage and pOutputImage share the same memory area.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param nThreshold The threshold value.
	*/
	bool ThresholdFilterInverse(const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nThreshold);

	/*!
		\brief Calculates the Harris cornerness measure for every pixel in a CByteImage and writes the results to a CIntImage.

		The cornerness value is defined as \f$ c = det(C) - \kappa \cdot {trace(C)}^2 \f$ where C is the covariance matrix for a given pixel location and observation windows and \f$ \kappa = 0.04 \f$.
		As observation window a 3x3 windows is used.
	 
		The width and height of pInputImage and pOutputImage must match.
		pInputImage must be of type CByteImage::eGrayScale.

		@param pInputImage The input image. Must be of type CByteImage::eGrayScale.
		@param pOutputImage The output image.
	*/
	bool CalculateHarrisMap(const CByteImage *pInputImage, CIntImage *pOutputImage);

	/*!
		\brief Computes interest points within a CByteImage by applying the Harris corner detector.
		
		Uses the function CalculateHarrisMap(const CByteImage*, CIntImage*) to compute a cornerness map. nMaxPoints maxima are searched, also applying a non maximum suppression, and sorting the candiates in decreasing order.
		
		The interest points must fulfill the quality constraint specified by fQualityLevel. Two interest points must have a distance of at least fMinDistance.
	 
		All interest points are computed with pixel accuracy.

		@param pInputImage The input image. Must be of type CByteImage::eGrayScale.
		@param pInterestPoints The memory area for the interest points to be computed. Must provide space for at least nMaxPoints instances of Vec2d.
		@param nMaxPoints The maximum number of interest points to determine.
		@param fQualityLevel The algorithm determines the interest point with the maximum cornerness value first. fQualityLevel is multiplied with the maximum cornerness value, yielding a minimum quality constraint.
		@param fMinDistance The minimum Euclidean distance two computed interest points must have.
		\return the number of interest points found. If an error occurs -1 is returned.
	*/
	int CalculateHarrisInterestPoints(const CByteImage *pInputImage, Vec2d *pInterestPoints, int nMaxPoints, float fQualityLevel = 0.01f, float fMinDistance = 5.0f);
	

	/*!
		\brief Applies the bitwise operator AND to two instance of CByteImage and writes the result to a CByteImage.

		This function implements the following point operation: pOutputImage->pixels[i] = pInputImage1->pixels[i] & pInputImage2->pixels[i].
	 
		The width, height and type of pInputImage1, pInputImage2 and pOutputImage must match.<br>
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
		
		As this is a point operation, there is no copy overhead if pInputImage1, pInputImage2, or pOutputImage share the same memory area.

		@param pInputImage1 The first input image.
		@param pInputImage2 The second input image.
		@param pOutputImage The output image.
	*/
	bool And(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage);

	/*!
		\brief Applies the bitwise operator XOR to two instance of CByteImage and writes the result to a CByteImage.
		
		This function implements the following point operation: pOutputImage->pixels[i] = pInputImage1->pixels[i] ^ pInputImage2->pixels[i].
		
		The width, height and type of pInputImage1, pInputImage2 and pOutputImage must match.<br>
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
		
		As this is a point operation, there is no copy overhead if pInputImage1, pInputImage2, or pOutputImage share the same memory area.
		
		@param pInputImage1 The first input image.
		@param pInputImage2 The second input image.
		@param pOutputImage The output image.
	*/
	bool Xor(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage);

	/*!
		\brief Applies the bitwise operator OR to two instance of CByteImage and writes the result to a CByteImage.
		
		This function implements the following point operation: pOutputImage->pixels[i] = pInputImage1->pixels[i] | pInputImage2->pixels[i].
		
		The width, height and type of pInputImage1, pInputImage2 and pOutputImage must match.<br>
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
		
		As this is a point operation, there is no copy overhead if pInputImage1, pInputImage2, or pOutputImage share the same memory area.

		@param pInputImage1 The first input image.
		@param pInputImage2 The second input image.
		@param pOutputImage The output image.
	*/
	bool Or(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage);


	/*!
		\brief Applies a morphological dilate operation to a binary CByteImage and writes the result to a binary CByteImage.
		
		A morphological dilate operation with a squared structure element is applied. The input image is assumed to encode background with the value 0 and foreground with 255. The output image will be encoded in the same way.

		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be both of type CByteImage::eGrayScale.
	 
		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.

		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param nMaskSize Determines the size of the structure element. A squared structure element of size nMaskSize x nMaskSize is used. nMaskSize must be an uneven number greater or equal 3.
		@param pROI Describes the area containing the pixels on which the operation shall be executed. If pROI is 0, then the whole image is processed.
	*/
	bool Dilate(const CByteImage *pInputImage, CByteImage *pOutputImage, int nMaskSize = 3, const MyRegion *pROI = 0);

	/*!
		\brief Applies a morphological erode operation to a binary CByteImage and writes the result to a binary CByteImage.
		
		A morphological erode operation with a squared structure element is applied. The input image is assumed to encode background with the value 0 and foreground with 255. The output image will be encoded in the same way.
		
		The width and height of pInputImage and pOutputImage must match.<br>
		pInputImage and pOutputImage must be both of type CByteImage::eGrayScale.
		
		If pInputImage and pOutputImage share the same memory area, the input image is copied to a temporary image first.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param nMaskSize Determines the size of the structure element. A squared structure element of size nMaskSize x nMaskSize is used. nMaskSize must be an uneven number greater or equal 3.
		@param pROI Describes the area containing the pixels on which the operation shall be executed. If pROI is 0, then the whole image is processed.
	*/
	bool Erode(const CByteImage *pInputImage, CByteImage *pOutputImage, int nMaskSize = 3, const MyRegion *pROI = 0);

	/*!
		\brief Performs a region growing on a binary CByteImage on the basis of one seed point and stores the computed region in a MyRegion.
	 
		The region growing algorithm segments the set of connected foreground pixels (4-connectivity) around a given seed point.
	 
		The input image is assumed to encode background with the value 0 and foreground with the value 255.

		@param pInputImage The input image. Must be a binary image of type CByteImage::eGrayScale.
		@param resultRegion The region entry, in which the information about the region to be detected will be stored.
		@param x The x-coordinate of the seed point. Must lie within the bounds of pInputImage, i.e. >= 0 and < pInputImage->width.
		@param y The y-coordinate of the seed point. Must lie within the bounds of pInputImage, i.e. >= 0 and < pInputImage->height.
		@param nMinimumPointsPerRegion Specifies the minimum number of pixels the region must contain. If the region contains less pixels, the function returns 0. The default value nMinimumPointsPerRegion = 0 means that no lower bound is checked.
		@param nMaximumPointsPerRegion Specifies the maximum number of pixels the region may contain. If the region contains more pixels, the function returns 0. The default value nMaximumPointsPerRegion = 0 means that no upper bound is checked.
		@param bCalculateBoundingBox Calculate bounding box (members min_x, min_y, max_x, max_y, ratio of MyRegion) or not. Setting bCalculateBoundingBox to false saves computation time.
		@param bStorePixels Store pixels belonging to the region in the member MyRegion::pPixels. Memory is handled automatically (allocation/deletion). Setting bStorePixels to false saves computation time.
	 
		\return the number of pixels in resultRegion. If an error occurs, -1 is returned. If the segmented region does not fulfill the size constraints, 0 is returned. After a successful call, the return value is at least 1 (for the seed point).
	*/
	int RegionGrowing(const CByteImage *pInputImage, MyRegion &resultRegion, int x, int y, int nMinimumPointsPerRegion = 0, int nMaximumPointsPerRegion = 0, bool bCalculateBoundingBox = true, bool bStorePixels = false);

	/*!
		\brief Performs region growing on a binary CByteImage, segmenting all regions in the image.
	 
		It is recommended to use the function FindRegions(const CByteImage*, CRegionArray&, int, int, bool, bool) instead. The only difference is the type of the parameter regionList, which is a std::vector<MyRegion> here
		and a CDynamicArrayTemplate<MyRegion> in the other case, which is more efficient.
	 
		This function chooses every foreground pixel as seed point and performs a region growing (4-connectivity) by calling the function RegionGrowing(const CByteImage*, MyRegion&, int, int, int, int, bool, bool).
		Already visited pixels are marked so that only not unvisited pixels can serve as seed points.
	 
		The input image is assumed to encode background with the value 0 and foreground with the value 255.

		@param pImage The input image. Must be a binary image of type CByteImage::eGrayScale.
		@param regionList The list of regions. RegionList is a typedef for std::vector<MyRegion>.
		@param nMinimumPointsPerRegion Specifies the minimum number of pixels the region must contain. The default value nMinimumPointsPerRegion = 0 means that no lower bound is checked.
		@param nMaximumPointsPerRegion Specifies the maximum number of pixels the region may contain. The default value nMaximumPointsPerRegion = 0 means that no upper bound is checked.
		@param bCalculateBoundingBox Calculate bounding box (members min_x, min_y, max_x, max_y, ratio of MyRegion) or not. Setting bCalculateBoundingBox to false saves computation time.
		@param bStorePixels Store pixels belonging to the region in the member MyRegion::pPixels. Memory is handled automatically (allocation/deletion). Setting bStorePixels to false saves computation time.
	*/
	bool FindRegions(const CByteImage *pImage, RegionList &regionList, int nMinimumPointsPerRegion = 0, int nMaximumPointsPerRegion = 0, bool bCalculateBoundingBox = true, bool bStorePixels = false);

	/*!
		\brief Performs region growing on a binary CByteImage, segmenting all regions in the image.
	 	 
		This function chooses every foreground pixel as seed point and performs a region growing (4-connectivity) by calling the function RegionGrowing(const CByteImage*, MyRegion&, int, int, int, int, bool, bool).
		Already visited pixels are marked so that only not unvisited pixels can serve as seed points.
	 
		The input image is assumed to encode background with the value 0 and foreground with the value 255.
	 
		@param pImage The input image. Must be a binary image of type CByteImage::eGrayScale.
		@param regionList The list of regions. CRegionArray is a typedef for CDynamicArrayTemplate<MyRegion>.
		@param nMinimumPointsPerRegion Specifies the minimum number of pixels the region must contain. The default value nMinimumPointsPerRegion = 0 means that no lower bound is checked.
		@param nMaximumPointsPerRegion Specifies the maximum number of pixels the region may contain. The default value nMaximumPointsPerRegion = 0 means that no upper bound is checked.
		@param bCalculateBoundingBox Calculate bounding box (members min_x, min_y, max_x, max_y, ratio of MyRegion) or not. Setting bCalculateBoundingBox to false saves computation time.
		@param bStorePixels Store pixels belonging to the region in the member MyRegion::pPixels. Memory is handled automatically (allocation/deletion). Setting bStorePixels to false saves computation time.
	*/
	bool FindRegions(const CByteImage *pImage, CRegionArray &regionList, int nMinimumPointsPerRegion = 0, int nMaximumPointsPerRegion = 0, bool bCalculateBoundingBox = true, bool bStorePixels = false);

	/*!
		\brief Performs the Hough transform for straight lines on a CByteImage.
	 
		This function implements the standard form of the Hough transform, which does not incorporate the edge directions in the voting algorithm.
		For more efficient and more precise line detection it is recommended to use the function HoughTransformLines(const CVec2dArray&, const CVec2dArray&, int, int, int, int, CVec2dArray&, CIntArray&, CByteImage*).
	 
		As input a binary image is assumed in pImage, where background pixels are encoded with the value 0 and foreground pixels are encoded with non-zero values (usually 255).
		It is recommended to use the function Canny(const CByteImage*, CByteImage*, int, int) to produce the input image for this function.

		Lines are represented with by polar coordinates: \f$ r = u \cdot cos(\theta) + v \cdot sin(\theta) \f$, where (u, v) denote image coordinates, \f$\theta\f$ is the angle of the normal vector and r is the distance of the line to the origin.

		@param pImage The input image. Must be a binary image of type CByteImage::eGrayScale.
		@param pVisualizationImage If pVisualizationImage is not NULL, the recognized lines will be drawn in this image.
		@param resultLines The list of recognized lines represented by tuples \f$ (\theta, r) \f$ (\f$\theta\f$ = Vec2d::x, \f$r\f$ = Vec2d::y) according to the polar coordinate form explained above.
			   The function PrimitivesDrawer::DrawLinePolar(CByteImage*, float, float, int, int, int) can be used for drawing lines specified by these parameters.
		@param nLinesToExtract At most nLinesToExtract lines are extracted. The greater this value, the greater is the computational effort.
		@param nMinHits The minimum number of votes a line needs to receive before being recognized as a line.
	*/
	bool HoughTransformLines(const CByteImage *pImage, CByteImage *pVisualizationImage, Vec2dList &resultLines, int nLinesToExtract, int nMinHits = 1);

	/*!
		\brief Performs the Hough transform for circles on a CByteImage.
	 
		This function implements the standard form of the Hough transform, which does not incorporate the edge directions in the voting algorithm.
		For more efficient and more precise circle detection it is recommended to use the function HoughTransformCircles(const CVec2dArray&, const CVec2dArray&, int, int, int, int, int, int, CVec3dArray&, CIntArray&, CByteImage*).
	 
		As input a binary image is assumed in pImage, where background pixels are encoded with the value 0 and foreground pixels are encoded with non-zero values (usually 255).
		It is recommended to use the function Canny(const CByteImage*, CByteImage*, int, int) to produce the input image for this function.

		Circles are represented by the equations \f$ u = u_m + r \cdot cos(\theta) \f$ and \f$ v = v_m + r \cdot sin(\theta) \f$.

		@param pImage The input image.  Must be a binary image of type CByteImage::eGrayScale.
		@param pVisualizationImage If pVisualizationImage is not NULL, the recognized circles will be drawn in this image.
		@param resultCircles The list of recognized circles in pImage represented by tuples \f$(u_m, v_m, r)\f$ (\f$u_m\f$ = Vec3d::x, \f$v_m\f$ = Vec3d::y, \f$r\f$ = Vec3d::z) according to the equations explained above.
			   The function PrimitivesDrawer::DrawCircle(CByteImage*, float, float, float, int, int, int, int, bool) can be used for drawing circles specified by these parameters.
		@param rmin The minimum radius. The smaller this value, the greater is the computational effort.
		@param rmax The maximum radius. The greater this value, the greater is the computatoinal effort.
		@param nCirclesToExtract At most nCirclesToExtract circles are extracted. The greater this value, the greater is the computational effort.
		@param nMinHits The minimum number of votes a circle needs to receive before being recognized as a circle.
	*/
	bool HoughTransformCircles(const CByteImage *pImage, CByteImage *pVisualizationImage, Vec3dList	&resultCircles, int rmin, int rmax, int nCirclesToExtract, int nMinHits = 1);
	
	/*!
		\brief Performs the Hough transform for straight lines, given a list of edge points and directions.
	 
		This function implements an improved form of the Hough transform, which also incorporates the edge directions in the voting algorithm.
		The standard version of the Hough transform for straight lines is implemented in HoughTransformLines(const CByteImage*, CByteImage*, Vec2dList&, int, int).
	 
		As input lists with edge points and edge directions are required.
		It is recommended to use the function Canny(const CByteImage*, CVec2dArray&, CVec2dArray&, int, int) to produce these input lists for this function.
	 
		Lines are represented with the aid of polar coordinates: \f$ r = u \cdot cos(\theta) + v \cdot sin(\theta) \f$.

		It is recommended to use the function ImageProcessor::HoughTransformLines(const CVec2dArray&, const CVec2dArray&, int, int, int, int, CStraightLine2dArray&, CIntArray&, CByteImage*).
	 
		@param edgePoints The list containing the edge points.
		@param edgeDirections The list containing the edge directions. Must be of same size as the parameter edgePoints and entries with same index must correspond.
		@param width The width of the image the edge points/directions were computed for.
		@param height The height of the image the edge points/directions were computed for.
		@param nLinesToExtract At most nLinesToExtract lines are extracted. The greater this value, the greater is the computational effort.
		@param nMinHits The minimum number of votes a line needs to receive before being recognized as a line.
		@param resultLines The list of recognized lines, represented by tuples \f$ (\theta, r) \f$ (\f$\theta\f$ = Vec2d::x, \f$r\f$ = Vec2d::y) according to the polar coordinate form explained above.
			   The function PrimitivesDrawer::DrawLinePolar(CByteImage*, float, float, int, int, int) can be used for drawing lines specified by these parameters.
		@param resultHits The list containing the number of hits for corresponding entries in resultLines.
		@param pVisualizationImage If pVisualizationImage is not NULL, the recognized lines will be drawn in here.
	*/
	void HoughTransformLines(const CVec2dArray &edgePoints, const CVec2dArray &edgeDirections, int width, int height, int nLinesToExtract, int nMinHits, CVec2dArray &resultLines, CIntArray &resultHits, CByteImage *pVisualizationImage = 0);
	
	/*!
		\brief Performs the Hough transform for straight lines, given a list of edge points and directions.
	 
		This function implements an improved form of the Hough transform, which also incorporates the edge directions in the voting algorithm.
		The standard version of the Hough transform for straight lines is implemented in HoughTransformLines(const CByteImage*, CByteImage*, Vec2dList&, int, int).
	 
		As input lists with edge points and edge directions are required.
		It is recommended to use the function Canny(const CByteImage*, CVec2dArray&, CVec2dArray&, int, int) to produce these input lists for this function.
	 
		Lines are represented in terms of the struct StraightLine2d.
	 
		@param edgePoints The list containing the edge points.
		@param edgeDirections The list containing the edge directions. Must be of same size as the parameter edgePoints and entries with same index must correspond.
		@param width The width of the image the edge points/directions were computed for.
		@param height The height of the image the edge points/directions were computed for.
		@param nLinesToExtract At most nLinesToExtract lines are extracted. The greater this value, the greater is the computational effort.
		@param nMinHits The minimum number of votes a line needs to receive before being recognized as a line.
		@param resultLines The list of recognized lines, represented by instances of the struct StraightLine2d.
			   The function PrimitivesDrawer::DrawLine(CByteImage*, const StraightLine2d&, int, int, int) can be used for drawing lines specified by this struct.
		@param resultHits The list containing the number of hits for corresponding entries in resultLines.
		@param pVisualizationImage If pVisualizationImage is not NULL, the recognized lines will be drawn in here.
	*/
	void HoughTransformLines(const CVec2dArray &edgePoints, const CVec2dArray &edgeDirections, int width, int height, int nLinesToExtract, int nMinHits, CStraightLine2dArray &resultLines, CIntArray &resultHits, CByteImage *pVisualizationImage = 0);
	
	/*!
		\brief Performs the Hough transform for circles, given a list of edge points and directions.
	 
		This function implements an improved form of the Hough transform, which also incorporates the edge directions in the voting algorithm.
		The standard version of the Hough transform for straight lines is implemented in HoughTransformCircles(const CByteImage*, CByteImage*, Vec3dList&, int, int, int, int).
	 
		As input lists with edge points and edge directions are required.
		It is recommended to use the function Canny(const CByteImage*, CVec2dArray&, CVec2dArray&, int, int) to produce these input lists for this function.
	 
		Circles are represented by the equations \f$ u = u_m + r \cdot cos(\theta) \f$ and \f$ v = v_m + r \cdot sin(\theta) \f$.

		It is recommended to use the function ImageProcessor::HoughTransformLines(const CVec2dArray&, const CVec2dArray&, int, int, int, int, CCircle2dArray&, CIntArray&, CByteImage*).
	 
		@param edgePoints The list containing the edge points.
		@param edgeDirections The list containing the edge directions. Must be of same size as the parameter edgePoints and entries with same index must correspond.
		@param width The width of the image the edge points/directions were computed for.
		@param height The height of the image the edge points/directions were computed for.
		@param rmin The minimum radius. The smaller this value, the greater is the computational effort.
		@param rmax The maximum radius. The greater this value, the greater is the computatoinal effort.
		@param nCirclesToExtract At most nCirclesToExtract circles are extracted. The greater this value, the greater is the computational effort.
		@param nMinHits The minimum number of votes a circle needs to receive before being recognized as a circle.
		@param resultCircles The list of recognized circles, represented by tuples \f$ (u_m, v_m, r) \f$ (\f$u_m\f$ = Vec3d::x, \f$v_m\f$ = Vec3d::y, \f$r\f$ = Vec3d::z) according to the equations explained above.
			   The function PrimitivesDrawer::DrawCircle(CByteImage*, float, float, float, int, int, int, int, bool) can be used for drawing circles specified by these parameters.
		@param resultHits The list containing the number of hits for corresponding entries in resultCircles.
		@param pVisualizationImage If pVisualizationImage is not NULL, the recognized circles will be drawn in here.
	*/
	bool HoughTransformCircles(const CVec2dArray &edgePoints, const CVec2dArray &edgeDirections, int width, int height, int rmin, int rmax, int nCirclesToExtract, int nMinHits, CVec3dArray &resultCircles, CIntArray &resultHits, CByteImage *pVisualizationImage = 0);

	/*!
		\brief Performs the Hough transform for circles, given a list of edge points and directions.
	 
		This function implements an improved form of the Hough transform, which also incorporates the edge directions in the voting algorithm.
		The standard version of the Hough transform for straight lines is implemented in HoughTransformCircles(const CByteImage*, CByteImage*, Vec3dList&, int, int, int, int).
	 
		As input lists with edge points and edge directions are required.
		It is recommended to use the function Canny(const CByteImage*, CVec2dArray&, CVec2dArray&, int, int) to produce these input lists for this function.
	 
		Circles are represented in terms of the struct Circle2d.
	 
		@param edgePoints The list containing the edge points.
		@param edgeDirections The list containing the edge directions. Must be of same size as the parameter edgePoints and entries with same index must correspond.
		@param width The width of the image the edge points/directions were computed for.
		@param height The height of the image the edge points/directions were computed for.
		@param rmin The minimum radius. The smaller this value, the greater is the computational effort.
		@param rmax The maximum radius. The greater this value, the greater is the computatoinal effort.
		@param nCirclesToExtract At most nCirclesToExtract circles are extracted. The greater this value, the greater is the computational effort.
		@param nMinHits The minimum number of votes a circle needs to receive before being recognized as a circle.
		@param resultCircles The list of recognized circles, represented by instances of the struct Circle2d.
			   The function PrimitivesDrawer::DrawLine(CByteImage*, const CCircle2d&, int, int, int, int bool) can be used for drawing circles specified by this struct.
		@param resultHits The list containing the number of hits for corresponding entries in resultCircles.
		@param pVisualizationImage If pVisualizationImage is not NULL, the recognized circles will be drawn in here.
	*/
	bool HoughTransformCircles(const CVec2dArray &edgePoints, const CVec2dArray &edgeDirections, int width, int height, int rmin, int rmax, int nCirclesToExtract, int nMinHits, CCircle2dArray &resultCircles, CIntArray &resultHits, CByteImage *pVisualizationImage = 0);


	/*!
		\brief Performs threshold binarization to a CFloatMatrix and writes the result to a CFloatMatrix.
		
		Sets each pixel in pOutputImage to 255.0f if the corresponding pixel in pInputImage has a value greater than or equal to fThreshold and to 0.0f otherwise.
		
		The number of columns and rows of pInputImage and pOutputImage must match.
		
		As this is a point operation, there is no copy overhead if pInputImage and pOutputImage share the same memory area.
		
		@param pInputImage The input image.
		@param pOutputImage The output image.
		@param fThreshold The threshold value.
	*/
	bool ThresholdBinarize(const CFloatMatrix *pInputImage, CFloatMatrix *pOutputImage, float fThreshold);
	
	/*!
		\brief Calculates the integral image of a grayscale CByteImage and writes the result to a CIntImage.
	 
		In the integral image, each value is the sum of all values in pInputImage left and above of it, i.e.
		\f$ {\rm IntegralImage}_{x, y} = \sum_{i=0}^x \sum_{j=0}^y I_{i, j} \f$.
	 
		If the input image size is so large that overflows can occur, CalculateBinarizedSummedAreaTable(const CByteImage*, CIntImage*) should be caled.
		For example, the maximum value for a 1024x768 image is 1024 * 768 * 255 = 200540160 < 2147483647 = INT_MAX, which is ok. Overflows will occur approximately for images with more than 8 megapixels.
	 
		@param pInputImage The input image. Must be of type CByteImage::eGrayScale:
		@param pIntegralImage The output image.
	*/
	bool CalculateIntegralImage(const CByteImage *pInputImage, CIntImage *pIntegralImage);
	
	/*!
		\brief Calculates the summed area table of a grayscale CByteImage and writes the result to a CIntImage.
		
		This function performs the same as CalculateIntegralImage - two different names are available only to simplify searching for the functionality.
		
		In the summed area table each value is the sum of all values in pInputImage left and above of it, i.e.
		\f$ {\rm SummedAreaTable}_{x, y} = \sum_{i=0}^x \sum_{j=0}^y I_{i, j} \f$.
	 
		If the input image size is so large that overflows can occur, CalculateBinarizedSummedAreaTable(const CByteImage*, CIntImage*) should be caled.
		For example, the maximum value for a 1024x768 image is 1024 * 768 * 255 = 200540160 < 2147483647 = INT_MAX, which is ok. Overflows will occur approximately for images with more than 8 megapixels.
		
		@param pInputImage The input image. Must be of type CByteImage::eGrayScale.
		@param pSummedAreaTable The output image.
	*/
	bool CalculateSummedAreaTable(const CByteImage *pInputImage, CIntImage *pSummedAreaTable);
	
	/*!
		\brief Efficiently computes the sum of all pixel values in a rectangular area using integral image lookups.
	 
		This function efficiently computes \f$ \sum_{i=min_x}^{max_x}\sum_{j=min_y}^{max_y} I_{i, j} \f$ using four memory lookups, where \f$I\f$ denotes the original image the integral image was computed for.
		The integral image for a given grayscale CByteImage can be computed by calling the function CalculateIntegralImage(const CByteImage*, CIntImage*).
	 
		This function performs checking of bounds. For maximum speed, GetAreaSumNoChecks(const pIntegralImage*, int, int, int, int) is recommended, which, however, does not perform any bounds checking and requires min_x >= 1 and min_y >= 1.
	 
		@param pIntegralImage The integral image.
		@param min_x The x-coordinate of the upper left corner of the rectangular region. Must be >= 0.
		@param min_y The y-coordinate of the upper left corner of the rectangular region. Must be >= 0.
		@param max_x The x-coordinate of the lower right corner of the rectangular region. Must be < pInputImage->width.
		@param max_y The y-coordinate of the lower right corner of the rectangular region. Must be < pInputImage->height.
	*/
	int GetAreaSum(const CIntImage *pIntegralImage, int min_x, int min_y, int max_x, int max_y);
	
	/*!
		\brief Efficiently computes the sum of all pixel values in a rectangular area using integral image lookups.
		
		This function computes \f$ \sum_{i=min_x}^{max_x}\sum_{j=min_y}^{max_y} I_{i, j} \f$ using an precomputed integral image.
	 
		This functions does not perform any bounds checks and requires min_x >= 1 and min_y >= 1. If min_x = 0 and min_y = 0 are relevant or bounds checking is required, the function GetAreaSum(const pIntegralImage*, int, int, int, int) should be used, which is, however, slightly slower.
		
		@param pIntegralImage The integral image.
		@param min_x The x-coordinate of the upper left corner of the rectangular region. Must be >= 1.
		@param min_y The y-coordinate of the upper left corner of the rectangular region. Must be >= 1.
		@param max_x The x-coordinate of the lower right corner of the rectangular region. Must be < pInputImage->width.
		@param max_y The y-coordinate of the lower right corner of the rectangular region. Must be < pInputImage->height.
	*/
	inline int GetAreaSumNoChecks(const CIntImage *pIntegralImage, int min_x, int min_y, int max_x, int max_y);
	
	/*!
		\brief Efficiently computes the sum of all pixel values in a rectangular area using integral image lookups.
		
		This is a convenience function, calling GetAreaSum(const CIntImage*, int, int, int, int).
	 
		@param pIntegralImage The integral image.
		@param pRegion The region entry that specifies the region of interest.
	*/
	int GetAreaSum(const CIntImage *pIntegralImage, const MyRegion *pRegion);
	
	/*!
		\brief Calculates the binarized summed area table of a grayscale CByteImage and writes the result to a CIntImage.

		In the binarized summed area table, each value is the count of all non-zero values in pInputImage left and above of it, i.e.
		\f$ {\rm BinarizedSummedAreaTable}_{x, y} = \sum_{i=0}^x \sum_{j=0}^y min(1, I_{i, j}) \f$.
	 
		This function is useful if the image size is so large that overflows can occur when applying CalculateIntegralImage(const CByteImage*, CIntImage*) (resp. CalculateSummedAreaTable(const CByteImage*, CIntImage*)).
		For example, the maximum value for a 1024x768 image is 1024 * 768 * 255 = 200540160 < 2147483647 = INT_MAX, which is ok. Overflows will occur approximately for images with more than 8 megapixels.

		@param pInputImage The input image. Must be of type CByteImage::eGrayScale.
		@param pSummedAreaTable The output image.
	*/
	bool CalculateBinarizedSummedAreaTable(const CByteImage *pInputImage, CIntImage *pSummedAreaTable);
	
	/*!
		\brief Reconstructs the original image, given a summed area table, and writes the result to a grayscale CByteImage.
	 
		The original image pSummedAreaTable was computed for (applying CalculateSummedAreaTable(const CByteImage*, CIntImage*) or CalculateIntegralImage(const CByteImage*, CIntImage*)) is reconstructed.

		@param pSummedAreaTable The summed area table (resp. integral image).
		@param pOutputImage The result image (reconstructed image). Must be of type CByteImage::eGrayScale.
	*/
	bool CalculateReverseSummedAreaTable(const CIntImage *pSummedAreaTable, CByteImage *pOutputImage);


	/*!
		\brief The four possible variants for Bayer pattern conversion.
	 
		This enum is used for specifying the variant of Bayer pattern to be processed in ConvertBayerPattern(const CByteImage*, CByteImage*, BayerPatternType). In eBayerXY, XY describes the type of the first two pixels of the Bayer pattern in the top left corner of the image.
	 
		The simplest approach is to find out which is the correct one for a specific camera is to simply try all four parameters and check in the computed RGB color image, if the colors are correct (in particular red and blue).
	*/
	enum BayerPatternType
	{
		eBayerRG,
		eBayerGR,
		eBayerBG,
		eBayerGB
	};

	/*!
		\brief Converts an 8 bit Bayer pattern CByteImage to an RGB24 color CByteImage.
	 
		This function computes the RGB24 color image for a given 8 bit grayscale image containing the raw data of a Bayer pattern.

		@param pInputImage The input image. Must be of type CByteImage::eGrayScale.
		@param pOutputImage The output image. Must be of type CByteImage::eRGB24.
		@param type The type of Bayer pattern assumed for processing. See BayerPatternType for further information.
	*/
	bool ConvertBayerPattern(const CByteImage *pInputImage, CByteImage *pOutputImage, BayerPatternType type);


	/*!
		\brief Sets each pixel in a CByteImage to the sum of the corresponding pixels in two instances of CByteImage.

		This function implements the following point operation: pOutputImage->pixels[i] = pInputImage1->pixels[i] + pInputImage2->pixels[i].
		
		No saturation is performed, i.e. overflows are not handled. For instance, 200 + 56 results in 0, 200 + 57 results in 1, and so on.
	 
		The width, height and type of pInputImage1, pInputImage2 and pOutputImage must match.<br>
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		As this is a point operation, there is no copy overhead if pInputImage1, pInputImage2, or pOutputImage share the same memory area.

		@param pInputImage1 The first input image.
		@param pInputImage2 The second input image.
		@param pOutputImage The output image.
	*/
	bool Add(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage);
	
	/*!
		\brief Sets each pixel in a CByteImage to the sum of the corresponding pixels in two instances of CByteImage, handling overflows with saturation.
	 
		This function implements the following point operation: pOutputImage->pixels[i] = MY_MAX((unsigned int) pInputImage1->pixels[i] + (unsigned int) pInputImage2->pixels[i], 255).
	 
		Saturation is performed, i.e. overflows are handled with truncation. For instance, 200 + 56 results in 255, 200 + 57 results in 255, and so on.
	 
		The width, height and type of pInputImage1, pInputImage2 and pOutputImage must match.<br>
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		As this is a point operation, there is no copy overhead if pInputImage1, pInputImage2, or pOutputImage share the same memory area.
	 
		@param pInputImage1 The first input image.
		@param pInputImage2 The second input image.
		@param pOutputImage The output image.
	*/
	bool AddWithSaturation(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage);

	/*!
		\brief Sets each pixel in a CByteImage to the difference between the corresponding pixels in two instances of CByteImage.
	 
		This function implements the following point operation: pOutputImage->pixels[i] = pInputImage1->pixels[i] - pInputImage2->pixels[i].
	 
		No saturation is performed, i.e. overflows are not handled. For instance, 200 - 201 results in 255, 200 - 202 results in 254, and so on.
		
		The width, height and type of pInputImage1, pInputImage2 and pOutputImage must match.<br>
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
		
		As this is a point operation, there is no copy overhead if pInputImage1, pInputImage2, or pOutputImage share the same memory area.
	 
		@param pInputImage1 The first input image.
		@param pInputImage2 The second input image.
		@param pOutputImage The output image.
	*/
	bool Subtract(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage);	
	
	/*!
		\brief Sets each pixel in a CByteImage to the difference between the corresponding pixels in two instances of CByteImage, handling overflows with saturation.
	 
		This function implements the following point operation: pOutputImage->pixels[i] = MY_MAX((int) pInputImage1->pixels[i] + (int) pInputImage2->pixels[i], 0).
		
		Saturation is performed, i.e. overflows are handled with truncation. For instance, 200 - 201 results in 0, 200 - 202 results in 0, and so on.
		
		The width, height and type of pInputImage1, pInputImage2 and pOutputImage must match.<br>
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
		
		As this is a point operation, there is no copy overhead if pInputImage1, pInputImage2, or pOutputImage share the same memory area.
		
		@param pInputImage1 The first input image.
		@param pInputImage2 The second input image.
		@param pOutputImage The output image.
	*/
	bool SubtractWithSaturation(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage);
	
	/*!
		\brief Sets each pixel in a CByteImage to the absolute value of the difference between the corresponding pixels in two instances of CByteImage.
	 
		This function implements the following point operation: pOutputImage->pixels[i] = abs((int) pInputImage1->pixels[i] - (int) pInputImage2->pixels[i]).
	 
		Overflows cannot occur.
	 
		The width, height and type of pInputImage1, pInputImage2 and pOutputImage must match.<br>
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		As this is a point operation, there is no copy overhead if pInputImage1, pInputImage2, or pOutputImage share the same memory area.
	 
		@param pInputImage1 The first input image.
		@param pInputImage2 The second input image.
		@param pOutputImage The output image.
	*/
	bool AbsoluteDifference(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage);
	
	/*!
		\brief Sets each pixel in a CByteImage to the average of the corresponding pixels in two instances of CByteImage.
	 
		This function implements the following point operation: pOutputImage->pixels[i] = (pInputImage1->pixels[i] + pInputImage2->pixels[i] + 1) >> 1.
	 
		Overflows cannot occur.
	 
		The width, height and type of pInputImage1, pInputImage2 and pOutputImage must match.<br>
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		As this is a point operation, there is no copy overhead if pInputImage1, pInputImage2, or pOutputImage share the same memory area.
	 
		@param pInputImage1 The first input image.
		@param pInputImage2 The second input image.
		@param pOutputImage The output image.
	*/
	bool Average(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage);

	/*!
		\brief Sets each pixel in a CByteImage to the minimum of the corresponding pixels in two instances of CByteImage.
		
		This function implements the following point operation: pOutputImage->pixels[i] = MY_MIN(pInputImage1->pixels[i], pInputImage2->pixels[i]).
		
		The width, height and type of pInputImage1, pInputImage2 and pOutputImage must match.<br>
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
		
		As this is a point operation, there is no copy overhead if pInputImage1, pInputImage2, or pOutputImage share the same memory area.
		
		@param pInputImage1 The first input image.
		@param pInputImage2 The second input image.
		@param pOutputImage The output image.
	*/
	bool Min(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage);

	/*!
		\brief Sets each pixel in a CByteImage to the maximum of the corresponding pixels in two instances of CByteImage.
	 
		This function implements the following point operation: pOutputImage->pixels[i] = MY_MAX(pInputImage1->pixels[i], pInputImage2->pixels[i]).
	 
		The width, height and type of pInputImage1, pInputImage2 and pOutputImage must match.<br>
		All types of CByteImage are supported (CByteImage::eGrayScale, CByteImage::eRGB24, CByteImage::eRGB24Split).
	 
		As this is a point operation, there is no copy overhead if pInputImage1, pInputImage2, or pOutputImage share the same memory area.
	 
		@param pInputImage1 The first input image.
		@param pInputImage2 The second input image.
		@param pOutputImage The output image.
	 */
	bool Max(const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage);
	
	
	/*!
		\brief Returns the maximum value within a grayscale CByteImage.

		@param pImage The input image. Must be of type CByteImage::eGrayScale.
	*/
	unsigned char MaxValue(const CByteImage *pImage);

	/*!
		\brief Returns the maximum value within a CShortImage.

		@param pImage The input image.
	*/
	short MaxValue(const CShortImage *pImage);

	/*!
		\brief Returns the maximum value within a CIntImage.

		@param pImage The input image.
	*/
	int MaxValue(const CIntImage *pImage);
	
	/*!
		\brief Returns the minimum value within a grayscale CByteImage.
	 
		@param pImage The input image.
	*/
	unsigned char MinValue(const CByteImage *pImage);
	
	/*!
		\brief Returns the minimum value within a CShortImage.
	 
		@param pImage The input image.
	*/
	short MinValue(const CShortImage *pImage);
	
	/*!
		\brief Returns the minimum value within a CIntImage.
	 
		@param pImage The input image.
	*/
	int MinValue(const CIntImage *pImage);
	
	/*!
		\brief Computes the minimum and maximum value within a grayscale CByteImage.
	
		@param pImage The input image. Must be of type CByteImage::eGrayScale.
		@param min The result parameter containing the minimum value after execution.
		@param max The result parameter containing the maximum value after execution.
	*/
	bool MinMaxValue(const CByteImage *pImage, unsigned char &min, unsigned char &max);
	
	/*!
		\brief Computes the minimum and maximum value within a CShortImage.
	 
		@param pImage The input image.
		@param min The result parameter containing the minimum value after execution.
		@param max The result parameter containing the maximum value after execution.
	*/
	void MinMaxValue(const CShortImage *pImage, short &min, short &max);
	
	/*!
		\brief Computes the minimum and maximum value within a CIntImage.
	 
		@param pImage The input image.
		@param min The result parameter containing the minimum value after execution.
		@param max The result parameter containing the maximum value after execution.
	*/
	void MinMaxValue(const CIntImage *pImage, int &min, int &max);

	/*!
		\brief Returns the sum of all pixel values of a grayscale CByteImage.

		@param pImage The input image. Must be of type CByteImage::eGrayScale.
	*/
	unsigned int PixelSum(const CByteImage *pImage);
}



#endif /* _IMAGE_PROCESSOR_H_ */
