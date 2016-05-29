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
// Filename:  ImageProcessorCV.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ImageProcessorCV.h"

#include "IplImageAdaptor.h"
#include "ByteImage.h"
#include "Math/Math2d.h"

#include <cv.h>
#include <stdio.h>



// ****************************************************************************
// Functions
// ****************************************************************************

void ImageProcessorCV::FlipY(CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type)
		return;

	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);
	
	cvFlip(pIplInputImage, pIplOutputImage);
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

void ImageProcessorCV::GaussianSmooth3x3(CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type)
		return;

	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);
	
	cvSmooth(pIplInputImage, pIplOutputImage, CV_GAUSSIAN, 3, 3);
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

void ImageProcessorCV::GaussianSmooth5x5(CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type)
		return;

	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);
	
	cvSmooth(pIplInputImage, pIplOutputImage, CV_GAUSSIAN, 5, 5);
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

void ImageProcessorCV::BilateralSmooth(CByteImage *pInputImage, CByteImage *pOutputImage, int param1, int param2)
{
       if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type)
		return;

	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);
	
	cvSmooth(pIplInputImage, pIplOutputImage, CV_BILATERAL, param1, param2);
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);

}

void ImageProcessorCV::Laplacian3x3(CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type)
		return;

	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);
	IplImage *pIplTempImage = cvCreateImage(cvSize(pInputImage->width, pInputImage->height), IPL_DEPTH_16S, 1);
	
	cvLaplace(pIplInputImage, pIplTempImage, 1);
	cvConvertScaleAbs(pIplTempImage, pIplOutputImage);
	
	cvReleaseImage(&pIplTempImage);
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

void ImageProcessorCV::Laplacian5x5(CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
		return;

	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);
	IplImage *pIplTempImage = cvCreateImage(cvSize(pInputImage->width, pInputImage->height), IPL_DEPTH_16S, 1);
	
	cvLaplace(pIplInputImage, pIplTempImage, 5);
	cvConvertScaleAbs(pIplTempImage, pIplOutputImage);
	
	cvReleaseImage(&pIplTempImage);
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

void ImageProcessorCV::Resize(const CByteImage *pInputImage, CByteImage *pOutputImage, int x, int y, int width, int height)
{
	if (pInputImage->type != pOutputImage->type)
		return;

	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);
	
	if (x != -1)
		cvSetImageROI(pIplInputImage, cvRect(x, y, width, height));
		
	cvResize(pIplInputImage, pIplOutputImage);
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

void ImageProcessorCV::CalculateGradientImage(CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pOutputImage->type != CByteImage::eGrayScale)
		return;

	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);

	if (pInputImage->type == CByteImage::eGrayScale)
	{
		IplImage *diff = cvCreateImage(cvSize(pInputImage->width, pInputImage->height), IPL_DEPTH_16S, 1);
		IplImage *abs = cvCreateImage(cvSize(pInputImage->width, pInputImage->height), IPL_DEPTH_8U, 1);
		
		cvSmooth(pIplInputImage, abs, CV_GAUSSIAN, 3, 3);
		cvSobel(abs, diff, 1, 0, 3);
		cvConvertScaleAbs(diff, pIplOutputImage);
		cvSobel(abs, diff, 0, 1, 3);
		cvConvertScaleAbs(diff, abs);
		cvAdd(abs, pIplOutputImage, pIplOutputImage, 0);
		
		cvReleaseImage(&diff);
		cvReleaseImage(&abs);
	}
	else if (pInputImage->type == CByteImage::eRGB24)
	{
		//	Determine Gradient Image by Irina Wchter
		//	instead of normal norm sqrt(x*x +y*y) use |x|+|y| because it is much faster
		IplImage *singleChannel0 = cvCreateImage(cvSize(pInputImage->width,pInputImage->height), IPL_DEPTH_8U, 1);
		IplImage *singleChannel1 = cvCreateImage(cvSize(pInputImage->width,pInputImage->height), IPL_DEPTH_8U, 1);
		IplImage *singleChannel2 = cvCreateImage(cvSize(pInputImage->width,pInputImage->height), IPL_DEPTH_8U, 1);
		IplImage *diff = cvCreateImage(cvSize(pInputImage->width, pInputImage->height), IPL_DEPTH_16S, 1);
		IplImage *abs = cvCreateImage(cvSize(pInputImage->width, pInputImage->height), IPL_DEPTH_8U, 1);
		
		cvCvtPixToPlane(pIplInputImage, singleChannel0, singleChannel1, singleChannel2, NULL);
	
		cvSmooth(singleChannel0, singleChannel0, CV_GAUSSIAN, 3, 3);
		cvSobel(singleChannel0, diff, 1, 0, 3);
		cvConvertScaleAbs(diff, abs);
		cvSobel(singleChannel0, diff, 0, 1, 3);
		cvConvertScaleAbs(diff, singleChannel0);
		cvAdd(abs, singleChannel0, pIplOutputImage, 0);
	
		cvSmooth(singleChannel1, singleChannel1, CV_GAUSSIAN, 3, 3);
		cvSobel(singleChannel1, diff, 1, 0, 3);
		cvConvertScaleAbs(diff, abs);
		cvSobel(singleChannel1, diff, 0, 1, 3);
		cvConvertScaleAbs(diff, singleChannel1);
		cvAdd(abs, singleChannel1, singleChannel1, 0);
		cvMax(pIplOutputImage, singleChannel1, pIplOutputImage);
	
		cvSmooth(singleChannel2, singleChannel2, CV_GAUSSIAN, 3, 3);
		cvSobel(singleChannel2, diff, 1, 0, 3);
		cvConvertScaleAbs(diff, abs);
		cvSobel(singleChannel2, diff, 0, 1, 3);
		cvConvertScaleAbs(diff, singleChannel2);
		cvAdd(abs, singleChannel2, singleChannel2, 0);
		cvMax(pIplOutputImage, singleChannel2, pIplOutputImage);
	
		cvReleaseImage(&singleChannel0);
		cvReleaseImage(&singleChannel1);
		cvReleaseImage(&singleChannel2);
		cvReleaseImage(&diff);
		cvReleaseImage(&abs);
	}
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

void ImageProcessorCV::CalculateGradientImageHSV(CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eRGB24 || pOutputImage->type != CByteImage::eGrayScale)
		return;

	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);

	// Determine Gradient Image by Irina Wchter
	// instead of normal norm sqrt(x*x +y*y) use |x|+|y| because it is much faster
	IplImage *singleChannel0 = cvCreateImage(cvSize(pInputImage->width,pInputImage->height), IPL_DEPTH_8U, 1);
	IplImage *singleChannel1 = cvCreateImage(cvSize(pInputImage->width,pInputImage->height), IPL_DEPTH_8U, 1);
	IplImage *singleChannel2 = cvCreateImage(cvSize(pInputImage->width,pInputImage->height), IPL_DEPTH_8U, 1);
	IplImage *diff = cvCreateImage(cvSize(pInputImage->width, pInputImage->height), IPL_DEPTH_16S, 1);
	IplImage *abs = cvCreateImage(cvSize(pInputImage->width, pInputImage->height), IPL_DEPTH_8U, 1);
		
	cvCvtPixToPlane(pIplInputImage, singleChannel0, singleChannel1, singleChannel2, NULL);
	
	// calculate gradients on S-channel
	//cvSmooth(singleChannel1, singleChannel1, CV_GAUSSIAN, 3, 3);
	cvSobel(singleChannel1, diff, 1, 0, 3);
	cvConvertScaleAbs(diff, abs);
	cvSobel(singleChannel1, diff, 0, 1, 3);
	cvConvertScaleAbs(diff, pIplOutputImage);
	cvAdd(abs, pIplOutputImage, pIplOutputImage, 0);
	
	// threshold S-channel for creating a maskfor gradients of H-channel
	cvThreshold(singleChannel1, singleChannel1, 60, 255, CV_THRESH_BINARY);
	cvDilate(singleChannel1, singleChannel1);
	
	// calculate gradients on H-channel
	//cvSmooth(singleChannel0, singleChannel0, CV_GAUSSIAN, 3, 3);
	cvSobel(singleChannel0, diff, 1, 0, 3);
	cvConvertScaleAbs(diff, abs);
	cvSobel(singleChannel0, diff, 0, 1, 3);
	cvConvertScaleAbs(diff, singleChannel0);
	cvAdd(abs, singleChannel0, singleChannel0, 0);
	
	// filter gradients of H-channel with mask
	cvAnd(singleChannel0, singleChannel1, singleChannel0);
	
	// combine to gradient images
	cvMax(pIplOutputImage, singleChannel0, pIplOutputImage);
	
	// free memory
	cvReleaseImage(&singleChannel0);
	cvReleaseImage(&singleChannel1);
	cvReleaseImage(&singleChannel2);
	cvReleaseImage(&diff);
	cvReleaseImage(&abs);
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

void ImageProcessorCV::Canny(CByteImage *pInputImage, CByteImage *pOutputImage, int nLowThreshold, int nHighThreshold)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
		return;

	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);
	
	cvCanny(pIplInputImage, pIplOutputImage, nLowThreshold, nHighThreshold);
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

void ImageProcessorCV::Dilate(CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
		return;
		
	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);
	
	cvDilate(pIplInputImage, pIplOutputImage);
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

void ImageProcessorCV::Erode(CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eGrayScale)
		return;
		
	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);
	
	cvErode(pIplInputImage, pIplOutputImage);
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

double ImageProcessorCV::Mean(CByteImage *pImage1, CByteImage *pImage2)
{
	if (pImage1->width != pImage2->width || pImage1->height != pImage2->height ||
		pImage1->type != pImage2->type || pImage1->type != CByteImage::eGrayScale)
		return -1;
		
	IplImage *pIplImage1 = IplImageAdaptor::Adapt(pImage1);
	IplImage *pIplImage2 = IplImageAdaptor::Adapt(pImage2);
	
	double dRet = cvMean(pIplImage1, pIplImage2);
	
	cvReleaseImageHeader(&pIplImage1);
	cvReleaseImageHeader(&pIplImage2);
	
	return dRet;
}

void ImageProcessorCV::CalculateHSVImage(CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != pOutputImage->type || pInputImage->type != CByteImage::eRGB24)
	{
		printf("error: input and output image do not match for ImageProcessorCV::CalculateHSVImage\n");
		return;
	}
		
	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);
	
	cvCvtColor(pIplInputImage, pIplOutputImage, CV_RGB2HSV);
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

void ImageProcessorCV::ConvertBayerPattern(CByteImage *pInputImage, CByteImage *pOutputImage, BayerPatternType type)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type != CByteImage::eGrayScale || pOutputImage->type != CByteImage::eRGB24)
	{
		printf("error: input and output image do not match for ImageProcessorCV::ConvertBayerPattern\n");
		return;
	}
		
	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);

	switch (type)
	{
		case eBG2BGR: cvCvtColor(pIplInputImage, pIplOutputImage, CV_BayerBG2BGR); break;
		case eGB2BGR: cvCvtColor(pIplInputImage, pIplOutputImage, CV_BayerGB2BGR); break;
		case eRG2BGR: cvCvtColor(pIplInputImage, pIplOutputImage, CV_BayerRG2BGR); break;
		case eGR2BGR: cvCvtColor(pIplInputImage, pIplOutputImage, CV_BayerGR2BGR); break;
		case eBG2RGB: cvCvtColor(pIplInputImage, pIplOutputImage, CV_BayerBG2RGB); break;
		case eGB2RGB: cvCvtColor(pIplInputImage, pIplOutputImage, CV_BayerGB2RGB); break;
		case eRG2RGB: cvCvtColor(pIplInputImage, pIplOutputImage, CV_BayerRG2RGB); break;
		case eGR2RGB: cvCvtColor(pIplInputImage, pIplOutputImage, CV_BayerGR2RGB); break;
	}
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}

int ImageProcessorCV::CalculateFeaturePoints(const CByteImage *pImage, Vec2d *pFeaturePoints, int nMaxPoints, float fQualityLevel, float fMinDistance, bool bUseHarris)
{
	if (pImage->type != CByteImage::eGrayScale)
	{
		printf("error: input image is not grayscale in ImageProcessorCV::CalculateFeaturePoints\n");
		return -1;
	}

	IplImage *pIplImage = IplImageAdaptor::Adapt(pImage);
	IplImage *pEigenvalueImage = cvCreateImage(cvSize(pImage->width, pImage->height), IPL_DEPTH_32F, 1);
	IplImage *pTempImage = cvCreateImage(cvSize(pImage->width, pImage->height), IPL_DEPTH_32F, 1);
	
	CvPoint2D32f *pCorners = new CvPoint2D32f[nMaxPoints];
	int nCorners = nMaxPoints;
	
	if (bUseHarris)
		cvGoodFeaturesToTrack(pIplImage, pEigenvalueImage, pTempImage, pCorners, &nCorners, fQualityLevel, fMinDistance, 0, 3, 1, 0.04);
	else
		cvGoodFeaturesToTrack(pIplImage, pEigenvalueImage, pTempImage, pCorners, &nCorners, fQualityLevel, fMinDistance, 0, 3);
	
	for (int i = 0; i < nCorners; i++)
	{
		pFeaturePoints[i].x = pCorners[i].x;
		pFeaturePoints[i].y = pCorners[i].y;
	}
	
	delete [] pCorners;
	
	cvReleaseImageHeader(&pIplImage);
	cvReleaseImage(&pTempImage);
	cvReleaseImage(&pEigenvalueImage);
	
	return nCorners;
}

void ImageProcessorCV::ConvertImage(CByteImage *pInputImage, CByteImage *pOutputImage)
{
	if (pInputImage->width != pOutputImage->width || pInputImage->height != pOutputImage->height ||
		pInputImage->type == pOutputImage->type)
	{
		printf("error: input and output image do not match for ImageProcessorCV::ConvertImage\n");
		return;
	}

	IplImage *pIplInputImage = IplImageAdaptor::Adapt(pInputImage);
	IplImage *pIplOutputImage = IplImageAdaptor::Adapt(pOutputImage);
	
	if (pInputImage->type == CByteImage::eRGB24)
		cvCvtColor(pIplInputImage, pIplOutputImage, CV_RGB2GRAY);
	else
		cvCvtColor(pIplInputImage, pIplOutputImage, CV_GRAY2RGB);
	
	cvReleaseImageHeader(&pIplInputImage);
	cvReleaseImageHeader(&pIplOutputImage);
}
