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
// Filename:  ExtrinsicParameterCalculatorCV.cpp
// Author:    Pedram Azad
// Date:      27.03.2007
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ExtrinsicParameterCalculatorCV.h"
#include "Image/PrimitivesDrawerCV.h"
#include "Image/PrimitivesDrawer.h"
#include "Image/ImageProcessor.h"
#include "Image/IplImageAdaptor.h"
#include "Image/ByteImage.h"
#include "Calibration/Calibration.h"
#include "Math/Math3d.h"
#include "Math/Math2d.h"
#include "Helpers/helpers.h"

#include <stdio.h>
#include <cv.h>



// ****************************************************************************
// Functions
// ****************************************************************************

bool ExtrinsicParameterCalculatorCV::GetPointsAndTranslationAndRotation(const CCalibration *pCalibration,
		const CByteImage *pImage, int nColumns, int nRows, float fSquareSize, // input
		Vec2d *pPoints, Mat3d &rotation, Vec3d &translation) // output
{
	CByteImage gray_image(pImage->width, pImage->height, CByteImage::eGrayScale);
	CByteImage temp_image(&gray_image);
	
	ImageProcessor::ConvertImage(pImage, &gray_image);

	IplImage *pIplImage = IplImageAdaptor::Adapt(&gray_image);
	IplImage *pIplTempImage = IplImageAdaptor::Adapt(&temp_image);
	CvMemStorage *pStorage = cvCreateMemStorage();

	CvPoint2D32f *pIplPoints = new CvPoint2D32f[nColumns * nRows];

	int nPoints = 0;

	if (cvFindChessBoardCornerGuesses(pIplImage, pIplTempImage, pStorage, 
		cvSize(nColumns - 1, nRows - 1), pIplPoints, &nPoints) == 0 ||
		nPoints != (nRows - 1) * (nColumns - 1))
	{
		// free memory
		cvReleaseImageHeader(&pIplImage);
		cvReleaseImageHeader(&pIplTempImage);
		cvReleaseMemStorage(&pStorage);
		delete [] pIplPoints;
		return false;
	}

	cvFindCornerSubPix(pIplImage, pIplPoints, nPoints,
							cvSize(5, 5), cvSize(-1, -1),
							cvTermCriteria(CV_TERMCRIT_ITER |CV_TERMCRIT_EPS, 10, 0.1));

	const CCalibration::CCameraParameters &cameraParameters = pCalibration->GetCameraParameters();

	float distortion[] = { (float) cameraParameters.distortion[0], (float) cameraParameters.distortion[1], (float) cameraParameters.distortion[2], (float) cameraParameters.distortion[3] };
	float focalLength[] = { (float) cameraParameters.focalLength.x, (float) cameraParameters.focalLength.y };
	CvPoint2D32f principalPoint;
	float rotationVector[3];
	float translationVector[3];

	principalPoint.x = (float) cameraParameters.principalPoint.x;
	principalPoint.y = (float) cameraParameters.principalPoint.y;

	CvPoint3D32f *pIplObjectPoints = (CvPoint3D32f *) malloc(nPoints * sizeof(CvPoint3D32f));

	int i = 0;
	for (int y = 0; y < nRows - 1; y++)
		for (int x = 0; x < nColumns - 1; x++, i++)
		{
			pIplObjectPoints[i].x = fSquareSize * x;
			pIplObjectPoints[i].y = fSquareSize * y;
			pIplObjectPoints[i].z = 0;
		}

	cvFindExtrinsicCameraParams(nPoints,
                                cvSize(pImage->width, pImage->height),
                                pIplPoints,
								pIplObjectPoints,
                                focalLength,
								principalPoint,
								distortion,
								rotationVector,
								translationVector);

	float rotationMatrix[9];

	CvMat rmat = cvMat(3, 3, CV_32FC1, rotationMatrix);
	CvMat rvec = cvMat(3, 1, CV_32FC1, rotationVector);
	cvRodrigues(&rmat, &rvec, 0, CV_RODRIGUES_V2M);

	// result:

	// translation
	Math3d::SetVec(translation, translationVector[0], translationVector[1], translationVector[2]);

	// rotation
	rotation.r1 = rotationMatrix[0];
	rotation.r2 = rotationMatrix[1];
	rotation.r3 = rotationMatrix[2];
	rotation.r4 = rotationMatrix[3];
	rotation.r5 = rotationMatrix[4];
	rotation.r6 = rotationMatrix[5];
	rotation.r7 = rotationMatrix[6];
	rotation.r8 = rotationMatrix[7];
	rotation.r9 = rotationMatrix[8];

	// corner points
	for (i = 0; i < nPoints; i++)
		Math2d::SetVec(pPoints[i], pIplPoints[i].x, pIplPoints[i].y);

	// free memory
	cvReleaseImageHeader(&pIplImage);
	cvReleaseImageHeader(&pIplTempImage);
	cvReleaseMemStorage(&pStorage);
	free(pIplObjectPoints);
	delete [] pIplPoints;

	return true;
}


void ExtrinsicParameterCalculatorCV::DrawExtrinsic(CByteImage *pResultImage, const CCalibration *pCalibration, const Vec2d *pPoints, int nPoints, float fSquareSize)
{
	Vec3d worldPoint0 = { 0, 0, 0 };
	Vec3d worldPointX = { 2 * fSquareSize, 0, 0 };
	Vec3d worldPointY = { 0, 2 * fSquareSize, 0 };
	Vec2d imagePoint0, imagePointX, imagePointY;
	pCalibration->WorldToImageCoordinates(worldPoint0, imagePoint0);
	pCalibration->WorldToImageCoordinates(worldPointX, imagePointX);
	pCalibration->WorldToImageCoordinates(worldPointY, imagePointY);
	
	PrimitivesDrawer::DrawLine(pResultImage, imagePoint0, imagePointX, 0, 0, 0, 2);
	PrimitivesDrawer::DrawLine(pResultImage, imagePoint0, imagePointY, 0, 0, 0, 2);
	PrimitivesDrawer::DrawCircle(pResultImage, imagePoint0, 5, 0, 0, 0, -1);
	PrimitivesDrawerCV::PutText(pResultImage, "x", imagePointX.x, imagePointX.y, 0.75, 0.75, 0, 0, 0, 2);
	PrimitivesDrawerCV::PutText(pResultImage, "y", imagePointY.x, imagePointY.y, 0.75, 0.75, 0, 0, 0, 2);
	
	if (pPoints && nPoints > 0)
	{
		Vec2d last_point;
		Math2d::SetVec(last_point, pPoints[0]);

		for (int i = 0; i < nPoints; i++)
		{
			int r, g, b;
			hsv2rgb(int(float(i) / (nPoints - 1) * 240), 255, 255, r, g, b);
			PrimitivesDrawer::DrawCircle(pResultImage, pPoints[i], 3, r, g, b, -1);
			PrimitivesDrawer::DrawLine(pResultImage, last_point, pPoints[i], r, g, b, 1);
			Math2d::SetVec(last_point, pPoints[i]);
		}
	}
}
