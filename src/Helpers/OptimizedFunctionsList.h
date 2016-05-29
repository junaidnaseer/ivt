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
// Filename:  OptimizedFunctionsList.h
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************
// Changes:   Pedram Azad: added new functions
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "Structs/Structs.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CByteImage;
class CShortImage;
class CIntImage;
struct Vec2d;



// ****************************************************************************
// Macros
// ****************************************************************************

// GaussianSmooth3x3 
DECLARE_OPTIMIZED_FUNCTION_2(GaussianSmooth3x3, const CByteImage *pInputImage, CByteImage *pOutputImage)

// GaussianSmooth3x3 
DECLARE_OPTIMIZED_FUNCTION_2(GaussianSmooth5x5, const CByteImage *pInputImage, CByteImage *pOutputImage)

// Prewitt filters
DECLARE_OPTIMIZED_FUNCTION_3(PrewittX, const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue)
DECLARE_OPTIMIZED_FUNCTION_3(PrewittY, const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue)
DECLARE_OPTIMIZED_FUNCTION_2(CalculateGradientImageSobel, const CByteImage *pInputImage, CByteImage *pOutputImage)

// Sobel filters
DECLARE_OPTIMIZED_FUNCTION_3(SobelX, const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue)
DECLARE_OPTIMIZED_FUNCTION_3(SobelY, const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue)
DECLARE_OPTIMIZED_FUNCTION_2(CalculateGradientImagePrewitt, const CByteImage *pInputImage, CByteImage *pOutputImage)

// Laplace filters
DECLARE_OPTIMIZED_FUNCTION_3(Laplace1, const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue)
DECLARE_OPTIMIZED_FUNCTION_3(Laplace2, const CByteImage *pInputImage, CShortImage *pOutputImage, bool bAbsoluteValue)

// Canny
DECLARE_OPTIMIZED_FUNCTION_4(Canny, const CByteImage *pInputImage, CByteImage *pOutputImage, int nLowThreshold, int nHighThreshold)
DECLARE_OPTIMIZED_FUNCTION_5(CannyList, const CByteImage *pInputImage, CVec2dArray &resultPoints, CVec2dArray &resultDirections, int nLowThreshold, int nHighThreshold)

// Morphological filters
DECLARE_OPTIMIZED_FUNCTION_2(Erode3x3, const CByteImage *pInputImage, CByteImage *pOutputImage)
DECLARE_OPTIMIZED_FUNCTION_2(Dilate3x3, const CByteImage *pInputImage, CByteImage *pOutputImage)

// Point operators
DECLARE_OPTIMIZED_FUNCTION_3(ThresholdBinarize, const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nThreshold)
DECLARE_OPTIMIZED_FUNCTION_3(ThresholdBinarizeInverse, const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nThreshold)
DECLARE_OPTIMIZED_FUNCTION_3(ThresholdFilter, const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nThreshold)
DECLARE_OPTIMIZED_FUNCTION_3(ThresholdFilterInverse, const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char nThreshold)
DECLARE_OPTIMIZED_FUNCTION_2(CalculateHSVImage, const CByteImage *pInputImage, CByteImage *pOutputImage)
DECLARE_OPTIMIZED_FUNCTION_4(ApplyAffinePointOperation, const CByteImage *pInputImage, CByteImage *pOutputImage, float a, float b)
DECLARE_OPTIMIZED_FUNCTION_3(Amplify, const CByteImage *pInputImage, CByteImage *pOutputImage, float fFactor)
DECLARE_OPTIMIZED_FUNCTION_2(Invert, const CByteImage *pInputImage, CByteImage *pOutputImage)
DECLARE_OPTIMIZED_FUNCTION_8(FilterHSV2, const CByteImage *pInputImage, CByteImage *pOutputImage, unsigned char min_hue, unsigned int max_hue, unsigned int min_sat, unsigned int max_sat, unsigned int min_v, unsigned int max_v)

// Logical operators
DECLARE_OPTIMIZED_FUNCTION_3(Or, const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
DECLARE_OPTIMIZED_FUNCTION_3(Xor, const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
DECLARE_OPTIMIZED_FUNCTION_3(And, const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)

// Arithmetic operators
DECLARE_OPTIMIZED_FUNCTION_3(Add, const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
DECLARE_OPTIMIZED_FUNCTION_3(AddWithSaturation, const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
DECLARE_OPTIMIZED_FUNCTION_3(Subtract, const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
DECLARE_OPTIMIZED_FUNCTION_3(SubtractWithSaturation, const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
DECLARE_OPTIMIZED_FUNCTION_3(AbsoluteDifference, const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
DECLARE_OPTIMIZED_FUNCTION_3(Average, const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
DECLARE_OPTIMIZED_FUNCTION_3(Max, const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)
DECLARE_OPTIMIZED_FUNCTION_3(Min, const CByteImage *pInputImage1, const CByteImage *pInputImage2, CByteImage *pOutputImage)

// Min / Max
DECLARE_OPTIMIZED_FUNCTION_2(MaxValue, const CByteImage *pInputImage, unsigned char &max)
DECLARE_OPTIMIZED_FUNCTION_2(MaxValue_Short, const CShortImage *pInputImage, short &max)
DECLARE_OPTIMIZED_FUNCTION_2(MaxValue_Int, const CIntImage *pInputImage, int &max)
DECLARE_OPTIMIZED_FUNCTION_2(MinValue, const CByteImage *pInputImage, unsigned char &min)
DECLARE_OPTIMIZED_FUNCTION_2(MinValue_Short, const CShortImage *pInputImage, short &min)
DECLARE_OPTIMIZED_FUNCTION_2(MinValue_Int, const CIntImage *pInputImage, int &min)
DECLARE_OPTIMIZED_FUNCTION_3(MinMaxValue, const CByteImage *pInputImage, unsigned char &min, unsigned char &max)
DECLARE_OPTIMIZED_FUNCTION_3(MinMaxValue_Short, const CShortImage *pInputImage, short &min, short &max)
DECLARE_OPTIMIZED_FUNCTION_3(MinMaxValue_Int, const CIntImage *pInputImage, int &min, int &max)

// PixelSum
DECLARE_OPTIMIZED_FUNCTION_2(PixelSum, const CByteImage *pImage, unsigned int &resultSum)

// Conversion (grayscale, RGB24 (split))
DECLARE_OPTIMIZED_FUNCTION_3(ConvertImage, const CByteImage *pInputImage, CByteImage *pOutputImage, bool bFast)

// Harris
DECLARE_OPTIMIZED_FUNCTION_5_RET(CalculateHarrisInterestPoints, const CByteImage *pInputImage, Vec2d *pInterestPoints, int nMaxPoints, float fQualityLevel, float fMinDistance)

// Nearest Neighbor
DECLARE_OPTIMIZED_FUNCTION_3(NearestNeighbor_TrainGPU, const float *pData, int nDimension, int nDataSets)
DECLARE_OPTIMIZED_FUNCTION_4(NearestNeighbor_ClassifyGPU, const float *pQuery, int nDimension, int &nResult, float &fResultError)
DECLARE_OPTIMIZED_FUNCTION_5(NearestNeighbor_ClassifyBundleGPU, const float *pQueries, int nDimension, int nQueries, int *pResults, float *pResultErrors)
DECLARE_OPTIMIZED_FUNCTION_0(NearestNeighbor_CleanupGPU)

// optimized function for the StereoVision class
//DECLARE_OPTIMIZED_FUNCTION_8(StereoVision, const CByteImage *pLeftImage, const CByteImage *pRightImage, CByteImage *pDepthImage, int nWindowSize, int d1, int d2, int d_step, int nErrorThreshold)
