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
// Filename:  StereoMatcher.h
// Author:    Pedram Azad
// Date:      12.03.2007
// ****************************************************************************


#ifndef _STEREO_MATCHER_H_
#define _STEREO_MATCHER_H_


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CStereoCalibration;
class CByteImage;
struct Vec2d;
struct Vec3d;



// ****************************************************************************
// CStereoMatcher
// ****************************************************************************

/*!
	\ingroup StereoProcessing
	\brief Class for computing matches for individual points in stereo image pairs.

	This class allows to search for a correspondence in the right image, given a point in the left image of stereo image pair.
	As correlation function a Zero-Mean Normalized Cross Correlation (ZNCC) is used. The ZNCC is invariant to
	constant additive and constant multiplicative illumination variations.

	The search for the correspondences is performed along the epipolar line. The images must not be rectified
	(or if rectified, the camera parameters must be updated to be valid for the rectified images).
	In any case it is recommended to provide the original images - not rectified images.

	As result, the corresponence in the right image is computed with subpixel accuracy as well as
	the 3D point in the world coordinate system by performing stereo triangulation.

	Images must be of type CByteImage::eGrayScale.
*/
class CStereoMatcher
{
public:
	/*!
		\brief The default constructor (and only constructor).

		Initialization is performed by calling one of the methods LoadCameraParameters(const char*) or InitCameraParameters(CStereoCalibration*, bool).
	*/
	CStereoMatcher();
	
	/*!
		\brief The destructor.
	*/
	~CStereoMatcher();
	
	
	/*!
		\brief Initializes the internally used camera model, given a file path to a camera parameter file.
	 
		The file must contain the parameters for the used stereo camera system.
		Details on the file format and camera calibration can be found in CStereoCalibration::LoadCameraParameters.

		The camera parameters are needed for knowledge of the epipolar geometry.
	 
		@param[in] pCameraParameterFileName The file path to the camera parameter file to be loaded.
		@return true on success and false on failure.
	*/
	bool LoadCameraParameters(const char *pCameraParameterFileName);

	/*!
		\brief Initializes the internally used camera model, given an instance of CStereoCalibration.
	 
		The camera parameters are needed for knowledge of the epipolar geometry.
	 
		@param[in] pStereoCalibration The pointer to the instance of CStereoCalibration.
		@param[in] bCloneCalibration If set to true, an internal instance is created and pStereoCalibration serves as the template instance.
				If set to false simply the pointer is stored. In any case, deletion of the provided instance pStereoCalibration is not handled by CStereoMatcher.
	*/
	void InitCameraParameters(CStereoCalibration *pStereoCalibration, bool bCloneCalibration);
	
	/*!
		\brief Computes a disparity estimated for a given z-distance.
	 
		The z-distance is defined as the z-coordinate in the camera coordinate system of the left camera.
		The disparity estimate is computed as the disparity of the point (0, 0, z)^T in the camera coordinate system of the left camera.

		Before application of this method, the instance must have been initialized by calling
		one of the methods LoadCameraParameters(const char*) or InitCameraParameters(CStereoCalibration*, bool).
	 
		@param[in] z The z-distance to compute the disparity estimate for.
		@return the disparity estimate in pixels.
	*/
	int GetDisparityEstimate(const float z);

	/*!
		\brief Computes a disparity estimated for a given z-distance using the Zero-Mean Normalized Cross Correlation (ZNCC).
	 
		The z-distance is defined as the z-coordinate in the camera coordinate system of the left camera.
		The disparity estimate is computed as the disparity of the point (0, 0, z)^T in the camera coordinate system of the left camera.

		The width and height of pInputImage and pOutputImage must match.
		pInputImage and pOutputImage must be both of type CByteImage::eGrayScale.

		Before application of this method, the instance must have been initialized by calling
		one of the methods LoadCameraParameters(const char*) or InitCameraParameters(CStereoCalibration*, bool).
	 
		@param[in] pLeftImage The left camera image. Must be of type CByteImage::eGrayScale.
		@param[in] pRightImage The right camera image. Must be of type CByteImage::eGrayScale.
		@param[in] x The x-coordinate of the point in pLeftImage to search a correspondence for in pRightImage.
		@param[in] y The y-coordinate of the point in pLeftImage to search a correspondence for in pRightImage.
		@param[in] nWindowSize Specifies the size of the correlation window, which is nWindowSize x nWindowSize.
		@param[in] d1 The minimum disparity, defining the starting point of the search. Must be >= 0.
		@param[in] d2 The maximum disparity, defining the end point of the search. Must be > d1.
		@param[out] result The computed correspondence in the pRightImage, with subpixel accuracy.
		@param[out] result_3d The computed 3D point, expressed in the world coordinate system.
		@param[in] fThreshold The threshold for the Zero Mean-Normalized Cross Correlation (ZNCC). As the ZNCC is a similarity measure (not an error measure)
				the greater the value the greater is the similarity. The maximum value that can be achieved is 1. fThreshold = 0.0f practically accepts
				all correspondences, fThreshold = 0.5f can be considered as a relatively non-restrictive threshold, fThrehold = 0.8f can be considered
				as a relatively restrictive threshold. The choice of this value depends on the quality of the images and the application.
		@param[in] bInputImagesAreUndistorted If set to false (default value) the input coordinates are undistorted before performing the search.
				If set to true, it is assumed that the images are already undistorted, thus not undistorting the coordinates.<br>
				<b>As the epipolar geometry is only valid for the linear mapping, it is strictly recommended to undistort the images, if the lens distortions have an observable effect.</b>
		@return the disparity >= 0 in case of success. Returns -1 if the images (equal size, type) do not meet the conditions. Returns -2 if no match could be found.
	*/
	int Match(const CByteImage *pLeftImage, const CByteImage *pRightImage, int x, int y, int nWindowSize, int d1, int d2, Vec2d &result, Vec3d &result_3d, float fThreshold, bool bInputImagesAreUndistorted = false);

	/*!
		\brief Computes a disparity estimated for a given z-distance using the Zero-Mean Sum of Absolute Differences (ZSAD).
	 
		The z-distance is defined as the z-coordinate in the camera coordinate system of the left camera.
		The disparity estimate is computed as the disparity of the point (0, 0, z)^T in the camera coordinate system of the left camera.

		The width and height of pInputImage and pOutputImage must match.
		pInputImage and pOutputImage must be both of type CByteImage::eGrayScale.

		Before application of this method, the instance must have been initialized by calling
		one of the methods LoadCameraParameters(const char*) or InitCameraParameters(CStereoCalibration*, bool).
	 
		@param[in] pLeftImage The left camera image. Must be of type CByteImage::eGrayScale.
		@param[in] pRightImage The right camera image. Must be of type CByteImage::eGrayScale.
		@param[in] x The x-coordinate of the point in pLeftImage to search a correspondence for in pRightImage.
		@param[in] y The y-coordinate of the point in pLeftImage to search a correspondence for in pRightImage.
		@param[in] nWindowSize Specifies the size of the correlation window, which is nWindowSize x nWindowSize.
		@param[in] d1 The minimum disparity, defining the starting point of the search. Must be >= 0.
		@param[in] d2 The maximum disparity, defining the end point of the search. Must be > d1.
		@param[out] result The computed correspondence in the pRightImage, with subpixel accuracy.
		@param[out] result_3d The computed 3D point, expressed in the world coordinate system.
		@param[in] fThreshold The threshold for the Zero-Mean Sum of Absolute Differences (ZSAD). As the ZSAD is an error measure,
				the smaller the value the greater is the similarity. The minimum value that can be achieved is 0. fThreshold = 255.0f accepts
				all correspondences. The choice of this value depends on the quality of the images and the application.
		@param[in] bInputImagesAreUndistorted If set to false (default value) the input coordinates are undistorted before performing the search.
				If set to true, it is assumed that the images are already undistorted, thus not undistorting the coordinates.<br>
				<b>As the epipolar geometry is only valid for the linear mapping, it is strictly recommended to undistort the images, if the lens distortions have an observable effect.</b>
		@return the disparity >= 0 in case of success. Returns -1 if the images (equal size, type) do not meet the conditions. Returns -2 if no match could be found.
	*/
	int MatchZSAD(const CByteImage *pLeftImage, const CByteImage *pRightImage, int x, int y, int nWindowSize, int d1, int d2, Vec2d &result, Vec3d &result_3d, float fThreshold, bool bInputImagesAreUndistorted = false);
	
	
private:
	// private methods
	int SingleZNCC(const CByteImage *pInputImage1, const CByteImage *pInputImage2, int x, int y, int nWindowSize, int d1, int d2, float *values);
	int SingleZSAD(const CByteImage *pInputImage1, const CByteImage *pInputImage2, int x, int y, int nWindowSize, int d1, int d2, float *values);
	
	// private attributes
	CStereoCalibration *m_pStereoCalibration;
	bool m_bOwnStereoCalibrationObject;
};



#endif /* _STEREO_MATCHER_H_ */
