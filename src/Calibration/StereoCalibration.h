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
// Filename:  StereoCalibration.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


#ifndef _STEREO_CALIBRATION_H_
#define _STEREO_CALIBRATION_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Math/Math3d.h"


// ****************************************************************************
// Forward declarations
// ****************************************************************************

class CCalibration;
class CByteImage;
struct Vec2d;
struct Vec3d;
struct Mat3d;
struct PointPair2d;
struct PointPair3d;



// ****************************************************************************
// CStereoCalibration
// ****************************************************************************

/*!
	\ingroup Calibration StereoProcessing
	\brief Camera model and functions for a stereo camera system.
 
	Handles two instances of CCalibration to model a stereo camera system,
	computes the epipolar geometriy and performs stereo triangulation.
	
	The camera model that is used for modeling the two individual cameras, including the extrinsic camera parameters, is described in detail in the documentation of the class CCalibration.
	
	The instance can be initialized either by reading a stereo camera parameter file (LoadCameraParameters(const char*, bool) or
	by assigning two instances of CCalibration (SetSingleCalibrations(const CCalibration&, const CCalibration&, bool). It is possible to adapt the extrinsic parameters,
	so that the world coordinate system and the camera coordinate system of the left camera become equal.
	
	After initialization, a 3D point can be calculated, given a point correspondence in the left and right camera image (Calculate3DPoint(const Vec2d&, const Vec2d&, Vec3d&, bool, bool, PointPair3d*).
	Furthermore, access to the epipolar lines is offered.
*/
class CStereoCalibration
{
public:
	/*!
		\brief The default constructor.
		
		The default constructor initializes the stereo camera model with a standard camera setup with a baseline of 100 mm (no orientation difference,
		i.e. \f$R_{{\rm left}} = R_{{\rm right}} = I\f$ and \f${\bf t}{{\rm left}} = (0,\,0,\,0)^T,\ {\bf t}{{\rm right}} = (-100,\,0,\,0)^T\f$), where I denotes the 3x3 identity matrix..
		
		The intrinsic parameters of the cameras are equal and are listed in CCalibration::CCalibration().
	*/
	CStereoCalibration();

	/*!
		\brief The destructor.
	*/
	~CStereoCalibration();
	
	/*!
		\brief The copy constructor.
	*/
	CStereoCalibration(const CStereoCalibration &stereoCalibration);

	
	/*!
		\brief Initializes the stereo camera model, given an instance of CStereoCalibration.
		@param stereoCalibration The template instance.
	*/
	void Set(const CStereoCalibration &stereoCalibration);
	
	/*!
		\brief Initializes the stereo camera model, given two instances of CCalibration.
		
		@param leftCalibration The template instance for the left camera.
		@param rightCalibration The template instance for the right camera.
		@param bTransformLeftCameraToIdentity If set to true, then the extrinsic parameters will be adapted so that the extrinsic parameters of the left camera
			become the identiy mapping, i.e. \f$R_{{\rm left}} = I\f$ and \f${\bf t}{{\rm left}} = (0,\,0,\,0)^T\f$, where I denotes the 3x3 identity matrix.
			That means that the world coordinate systems and the camera coordinate system of the left camera become equal. If set to false, the extrinsic parameters
			are not changed. Note that the internal fundamental matrix for the epipolar geometry is not affected by the choice of this flag.
	*/
	void SetSingleCalibrations(const CCalibration &leftCalibration, const CCalibration &rightCalibration, bool bTransformLeftCameraToIdentity = false);
	
	/*!
		\brief Sets the distortion parameters of the distortion models of both cameras.
		
		@param[in] d1_left The first radial lens distortion parameter of the left camera.
		@param[in] d2_left The second radial lens distortion parameter of the left camera.
		@param[in] d3_left The first tangential lens distortion parameter of the left camera.
		@param[in] d4_left The second tangential lens distortion parameter of the left camera.
		@param[in] d1_right The first radial lens distortion parameter of the right camera.
		@param[in] d2_right The second radial lens distortion parameter of the right camera.
		@param[in] d3_right The first tangential lens distortion parameter of the right camera.
		@param[in] d4_right The second tangential lens distortion parameter of the right camera.
	*/
	void SetDistortionParameters(float d1_left, float d2_left, float d3_left, float d4_left, float d1_right, float d2_right, float d3_right, float d4_right);

	/*!
		\brief Sets the extrinsic parameters of both cameras.
		
		The member variables CCalibration::m_rotation_inverse and CCalibration::m_translation_inverse for both cameras are updated automatically.
		
		@param[in] left_rotation The rotation matrix for the left camera as part of the extrinsic camera parameters.
		@param[in] left_translation The translation vector for the left camera as part of the extrinsic camera parameters.
		@param[in] right_rotation The rotation matrix for the right camera as part of the extrinsic camera parameters.
		@param[in] right_translation The translation vector for the right camera as part of the extrinsic camera parameters.
		@param bTransformLeftCameraToIdentity If set to true, then the extrinsic parameters will be adapted so that the extrinsic parameters of the left camera
			become the identiy mapping, i.e. \f$R_{{\rm left}} = I\f$ and \f${\bf t}{{\rm left}} = (0,\,0,\,0)^T\f$, where I denotes the 3x3 identity matrix.
			That means that the world coordinate systems and the camera coordinate system of the left camera become equal. If set to false, the extrinsic parameters
			are not changed. Note that the internal fundamental matrix for the epipolar geometry is not affected by the choice of this flag.
	*/
	void SetExtrinsicParameters(const Mat3d& left_rotation, const Vec3d& left_translation, const Mat3d& right_rotation, const Vec3d& right_translation, bool bTransformLeftCameraToIdentity = false);
	
	/*!
		\brief Initializes the stereo camera model, given a file path to a stereo camera parameter file.
		
		The format of the file is the same as the one used in OpenCV 1.0. It is...
		
		Camera calibration can be performed with the application found in IVT/examples/CalibrationApp (resp. IVT/win32/CalibrationApp). See also the class CDLTCalibration.
		
		@param[in] pCameraParameterFileName The file path to the camera parameter file to be loaded.
		@param bTransformLeftCameraToIdentity If set to true, then the extrinsic parameters will be adapted so that the extrinsic parameters of the left camera
			become the identiy mapping, i.e. \f$R_{{\rm left}} = I\f$ and \f${\bf t}{{\rm left}} = (0,\,0,\,0)^T\f$, where I denotes the 3x3 identity matrix.
			That means that the world coordinate systems and the camera coordinate system of the left camera become equal. If set to false, the extrinsic parameters
			are not changed. Note that the internal fundamental matrix for the epipolar geometry is not affected by the choice of this flag.
	*/
	bool LoadCameraParameters(const char *pCameraParameterFileName, bool bTransformLeftCameraToIdentity = true);
		
	/*!
		\brief Writes the parameters of the camera model to camera parameter file.
		
		The format of the file is the same as the one used in OpenCV 1.0. The file format is described in LoadCameraParameters(const char*, bool).
		
		In the following specification of the file format, the first row of each double row contains the parameters of the first, i.e. left, camera,
		and the second row the parameters of the right camera, respectively. The parameter \f$n \in \{1,\,2\}\f$ denotes the number of cameras.
		
		n
		
		w h fx 0 cx 0 fy cy 0 0 1 d1 d2 d3 d4 r1 r2 r3 r4 r5 r6 r7 r8 r9 t1 t2 t3<br>
		w h fx 0 cx 0 fy cy 0 0 1 d1 d2 d3 d4 r1 r2 r3 r4 r5 r6 r7 r8 r9 t1 t2 t3<br>
		
		u1 v1 u2 v2 u3 v3 u4 v4<br>
		u1 v1 u2 v2 u3 v3 u4 v4<br>
		
		a1 a2 a3 a4 a5 a6 a7 a8 a9<br>
		a1 a2 a3 a4 a5 a6 a7 a8 a9<br>
		
		For each camera, the parameters w and h denote the width and height, respectively.
		The next nine parameters define the calibration matrix (see CCalibration::GetCalibrationMatrix).
		The parameters d1-d4 denote the distortion parameters of the distortion model,
		the extrinsic parameters r1-r9 define the rotation matrix R and t1-t3 the translation vector t (see camera model description in the documentation of the class CCalibration).
		These parameters of both cameras fully describe the epipolar geometry.
		
		The parameters (u1, v1), ..., (u4, v4) define the corner points of the destination quadrangle after rectification. These parameters are not necessary (and redundant) and are therefore not read by the IVT.
		
		Finally, the rectification parameters a1-a9 specify the 3x3 homography matrix A. This homography computes the image coordinates in the original image for given image coordinates in the rectified images.
		It can thus be directly used to compute a lookup table for the mapping, which is implemented efficiently by the class CRectification.
		
		@param[in] pCameraParameterFileName The file path to the target camera parameter file.
		@return true on success and false on failure.
	*/
	bool SaveCameraParameters(const char *pCameraParameterFileName) const;

	
	/*! \brief Access to the instance of CCalibration for the camera model of the left camera. */
	const CCalibration* GetLeftCalibration() const { return m_pLeftCalibration; }
	
	/*! \brief Access to the instance of CCalibration for the camera model of the right camera. */
	const CCalibration* GetRightCalibration() const { return m_pRightCalibration; }

	
	/*!
		\brief Computes a 3D point, given a point correspondence in both images, by performing stereo triangulation.
		
		@param[in] cameraPointLeft The 2D point in the left image.
		@param[in] cameraPointRight The 2D point in the right image.
		@param[out] worldPoint The result 3D point in the world coordinate system.
		@param[in] bInputImagesAreRectified If set to true, cameraPointLeft and cameraPointRight are transformed to the corresponding 2D points in the original images,
			so that the camera models can be applied. If set to false, cameraPointLeft and cameraPointRight must have been computed on non-rectified images.
		@param[in] bUseDistortionParameters If set to true, the (inverse) distortion model will be applied before linear back projection, i.e. cameraPointLeft and cameraPointRight
			must contain distorted image coordinates. If set to false, the distortion model is skipped, i.e. cameraPointLeft and cameraPointRight must contain undistorted image coordinates.
			In other words: If the images in which the coordinates cameraPointLeft and cameraPoint right were calculate were undistorted beforehand then set this parameter to false, otherwise to true.
		@param[out] pConnectionLine Optional output parameter for receiving the two endpoints of the shortest connection line between the two re-projected straight lines.
			The parameter worldPoint contains the center of this connection line.
	*/
	void Calculate3DPoint(const Vec2d &cameraPointLeft, const Vec2d &cameraPointRight, Vec3d &worldPoint, bool bInputImagesAreRectified, bool bUseDistortionParameters = true, PointPair3d *pConnectionLine = 0);
	
	/*!
		\brief Given an image point in the right image, computes the corresponding epipolar line in the left image.
		
		The representation of the epipolar line \f${\bf l} = (l_1,\,l_2\,l_3)^T\f$ is defined as:
		
		\f$l_1 \cdot u + l_2 \cdot v + l3 = 0\f$.
		
		All image points (u, v) lying on the epipolar line satisfy this equation. Note that \f$(l_1,\,l_2)^T\f$ is the normal vector the line.
		
		@param[in] pointInRightImage The 2D point in the right image.
		@param[out] l A 3D vector representing the result epipolar line. It is l1 = l.x, l2 = l.y, l3 = l.z.
	*/
	void CalculateEpipolarLineInLeftImage(const Vec2d &pointInRightImage, Vec3d &l);
	
	/*!
		\brief Given an image point in the right image, computes the corresponding epipolar line in the left image.
		
		The epipolar line is represented by the two end points of the intersection with the image.
		
		@param[in] pointInRightImage The 2D point in the right image.
		@param[out] epipolarLine Result parameter that contains the two end points.
	*/
	void CalculateEpipolarLineInLeftImage(const Vec2d &pointInRightImage, PointPair2d &epipolarLine);
	
	/*!
		\deprecated
		\brief Deprecated.
		
		This function is deprecated. Use CalculateEpipolarLineInLeftImage(const Vec2d&, Vec3d&) or CalculateEpipolarLineInLeftImage(const Vec2d&, PointPair2d&) instead.
		
		Models the line as \f$v = m \cdot u + c\f$. This representation fails for vertical lines.
	*/
	void CalculateEpipolarLineInLeftImage(const Vec2d &pointInRightImage, float &m, float &c);
	
	/*!
		\brief Given an image point in the left image, computes the corresponding epipolar line in the right image.
		
		The representation of the epipolar line \f${\bf l} = (l_1,\,l_2\,l_3)^T\f$ is defined as:
		
		\f$l_1 \cdot u + l_2 \cdot v + l3 = 0\f$.
		
		All image points (u, v) lying on the epipolar line satisfy this equation. Note that \f$(l_1,\,l_2)^T\f$ is the normal vector the line.
		
		@param[in] pointInLeftImage The 2D point in the left image.
		@param[out] l A 3D vector representing the result epipolar line. It is l1 = l.x, l2 = l.y, l3 = l.z.
	*/
	void CalculateEpipolarLineInRightImage(const Vec2d &pointInLeftImage, Vec3d &l);
	
	/*!
		\brief Given an image point in the left image, computes the corresponding epipolar line in the right image.
		
		The epipolar line is represented by the two end points of the intersection with the image.
		
		@param[in] pointInLeftImage The 2D point in the right image.
		@param[out] epipolarLine Result parameter that contains the two end points.
	*/
	void CalculateEpipolarLineInRightImage(const Vec2d &pointInLeftImage, PointPair2d &epipolarLine);
	
	/*!
		\deprecated
		\brief Deprecated.
		
		This function is deprecated. Use CalculateEpipolarLineInRightImage(const Vec2d&, Vec3d&) or CalculateEpipolarLineInRightImage(const Vec2d&, PointPair2d&) instead.
		
		Models the line as \f$v = m \cdot u + c\f$. This representation fails for vertical lines.
	*/
	void CalculateEpipolarLineInRightImage(const Vec2d &pointInLeftImage, float &m, float &c);
	
	
	/*!
		\brief Given a point correspondence, computes the distance from the epipolar line in the left image.
		
		Given pointInRightImage, computes the corresponding epipolar in the left image and the computes the distance of pointInLeftImage to it.
		
		@param[in] pointInLeftImage The 2D point in the left image.
		@param[in] pointInRightImage The 2D point in the right image.
		@return the computed distance.
	*/
	float CalculateEpipolarLineInLeftImageDistance(const Vec2d &pointInLeftImage, const Vec2d &pointInRightImage);
	
	/*!
		\brief Given a point correspondence, computes the distance from the epipolar line in the right image.
		
		Given pointInLeftImage, computes the corresponding epipolar in the right image and the computes the distance of pointInRightImage to it.
		
		@param[in] pointInLeftImage The 2D point in the left image.
		@param[in] pointInRightImage The 2D point in the right image.
		@return the computed distance.
	*/
	float CalculateEpipolarLineInRightImageDistance(const Vec2d &pointInLeftImage, const Vec2d &pointInRightImage);
	
	/*!
		\brief Sets up the projection matrix P for the rectified images.
	 
		The projection matrix P for an image that was rectified according to the homography H is a 3x4 matrix defined as:
	 
		\f$P = \left(H \cdot K \cdot R | H \cdot K \cdot {\bf t}\right)\f$, where K denotes the calibration matrix (see CCalibration::GetCalibrationMatrix), R and t are the extrinsic parameters, 
		and H is the homography that maps undistorted image coordinates to rectified image coordinates.
	 
		Note that in the case of the left camera H is the inverse of CStereoCalibration::rectificationHomographyLeft, and in the case of the right camera H is the inverse of CStereoCalibration::rectificationHomographyRight.
	 
		The camera model is described in the general information about the class CCalibration.
	 
		@param[out] P1Left The left 3x3 part of the projection matrix for the left camera, i.e. P1Left = H_left K_left R_left.
		@param[out] p2Left The right 3x1 part of the projection matrix for the right camera, i.e. p2Left = H_left K_left t_left.
		@param[out] P1Right The left 3x3 part of the projection matrix for the right camera, i.e. P1Right = H_right K_right R_right.
		@param[out] p2Right The right 3x1 part of the projection matrix for the right camera, i.e. p2Right = H_right K_right t_right.
	*/
	void GetProjectionMatricesForRectifiedImages(Mat3d &P1Left, Vec3d &p2Left, Mat3d &P1Right, Vec3d &p2Right) const;
	
	
	// public attributes
	/*! \brief The width of the images of the stereo camera system in pixels. */
	int width;
	/*! \brief The height of the images of the stereo camera system in pixels. */
	int height;
	
	/*!
		\brief The homography for the rectification mapping of the right image.
		
		Given a point in the rectified image, applying this homography computes the corresponding point in the original image (see also Math2d::ApplyHomography).
		
		This member variable is only valid if the method LoadCameraParameters(const char*, bool) was used for initialization.
		
		Use StereoCalibrationCV::CalculateRectificationHomographies if any other method was used for initialization and this member is needed (mainly if the class CRectification is to be used).
	*/
	Mat3d rectificationHomographyLeft;
	
	/*!
		\brief The homography for the rectification mapping of the right image.
		
		Given a point in the rectified image, applying this homography computes the corresponding point in the original image (see also Math2d::ApplyHomography).
		
		This member variable is only valid if the method LoadCameraParameters(const char*, bool) was used for initialization.
		
		Use StereoCalibrationCV::CalculateRectificationHomographies if any other method was used for initialization and this member is needed (mainly if the class CRectification is to be used).
	*/
	Mat3d rectificationHomographyRight;
		

private:
	// private methods
	void TransformLeftCameraToIdentity();
	void CalculateFundamentalMatrix();

	// private attributes
	CCalibration *m_pLeftCalibration;
	CCalibration *m_pRightCalibration;
	
	Mat3d F, FT; // F = fundamental matrix, FT = transposed fundamental matrix
};



#endif /* _STEREO_CALIBRATION_H_ */
