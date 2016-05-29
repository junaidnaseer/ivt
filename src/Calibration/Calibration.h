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
// Filename:  Calibration.h
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************

/** \defgroup Calibration Camera Calibration */


#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include "Math/Math2d.h"
#include "Math/Math3d.h"



// ****************************************************************************
// CCalibration
// ****************************************************************************

/*!
	\ingroup Calibration
	\brief Camera model parameters and functions for a single camera.
	
	The linear projection is modeled as a central perspective projection with the parameters fx, fy and cx, cy.
	
	\f$\left(\begin{array}{c}
	u\\
	v\\
	\end{array}\right)=
	\left(\begin{array}{c}
	c_x\\
	c_y\\
	\end{array}\right)+\frac{1}{z_c}
	\left(\begin{array}{c}
	f_x\,x_c\\
	f_y\,y_c\\
	\end{array}\right)\f$
	
	where u, v denote image coordinates, fx, fy the focal lengths, cx, cy the principal point, and xc, yc, zc denote camera coordinates.
	
	The relationship between the world coordinate system and the camera coordinate system is modeled as a rigid body coordinate transformation from the world coordinate system into the camera coordinate system:
	
	\f${\bf x}_c = R \cdot {\bf x}_w + {\bf t}\f$
	
	where \f${\bf x}_w\f$ denotes world coordinates, \f${\bf x}_c\f$ denotes camera coordinates, R is a 3x3 rotation matrix and t is a translation vector (R, t are the extrinsic camera parameters).
	
	The distortion model is composed of radial lens distortion (parameters d1, d2) and tangential lens distortion (parameters d3, d4). It is modeled as the computation of distorted image coordinates (ud, vd), given undistorted image coordinates (u, v):
 
	\f$x_n = \frac{u-c_x}{f_x}\f$
	\f$y_n = \frac{v-c_y}{f_y}\f$
	
	\f$r = \sqrt{x_n^2 + y_n^2}\f$
	
	\f$\left(\begin{array}{c}
	x_d\\
	y_d\\
	\end{array}\right)
	=(1+d_1r^2+d_2r^4)
	\left(\begin{array}{c}
	x_n\\
	y_n\\
	\end{array}\right)+
	\left(\begin{array}{c}
	d_3(2x_ny_n)+d_4(r^2+2x_n^2)\\
	d_3(r^2+2y_n^2)+d_4(2x_ny_n)\\
	\end{array}\right)\f$
	
	\f$\left(\begin{array}{c}
	u_d\\
	v_d\\
	\end{array}\right)=
	\left(\begin{array}{c}
	c_x\\
	c_y\\
	\end{array}\right)+
	\left(\begin{array}{c}
	x_d \cdot f_x\\
	y_d \cdot f_y\\
	\end{array}\right)\f$
 
	The camera model is described in detail in (P. Azad, T. Gockel, R. Dillmann, "Computer Vision - Principles and Practice", Elektor, Netherlands).
	It is the same camera model as used in OpenCV 1.0.
*/
class CCalibration
{
public:
	/*!
		\brief Struct containing all parameters of the camera model.
	
		The camera model is described in the general information about the class CCalibration.
	*/
	struct CCameraParameters
	{
		/*! The width of the image sensor in pixels. */
		int width;
		/*! The height of the image sensor in pixels. */
		int height;
		/*! The principal point, given in image coordinates (pixels). In literature, often the variable names cx, cy are used. */
		Vec2d principalPoint;
		/*! The focal lengths in horizontal and vertical direction, given in pixels (also often called camera constants). In literature, often the variable names fx, fy are used. */
		Vec2d focalLength;
		/*! The four distortion parameters of the distortion model, the first two (d1, d2) model radial lens distortion, the second two (d3, d4) model tangential lens distortion. */
		float distortion[4];
		/*! The extrinsic camera parameter describing the 3D rotation. In literature, often the variable name R is used. */
		Mat3d rotation;
		/*! The extrinsic camera parameter describing the 3D translation. In literature, often the variable name t is used. */
		Vec3d translation;
	};


	/*!
		\brief The default constructor.
	 
		The default constructor initializes the camera model with the values width = 640, height = 480, fx = fy = 580, cx = 320, cy = 240, R = I, t = 0, d1 = d2 = d3 = d4 = 0.
		See CCameraParameters for further information on these parameters.
	 
		Camera parameters computed with calibration algorithms can be loaded by calling the method CCalibration::LoadCameraParameters.
	*/
	CCalibration();

	/*!
		\brief The destructor.
	*/
	~CCalibration();

	
	/*!
		\brief Initializes the camera model, given an instance of CCalibration.
		@param calibration The template instance.
	*/
	void Set(const CCalibration &calibration);
	
	/*!
		\brief Initializes the camera model, given a complete parameter set.
	 
		@param[in] fx The focal length in horizontal direction.
		@param[in] fy The focal length in vertical direction.
		@param[in] cx The x-coordinate (or u-coordinate) of the principal point.
		@param[in] cy The y-coordinate (or v-coordinate) of the principal point.
		@param[in] d1 The first radial lens distortion parameter.
		@param[in] d2 The second radial lens distortion parameter.
		@param[in] d3 The first tangential lens distortion parameter.
		@param[in] d4 The second tangential lens distortion parameter.
		@param[in] R The rotation matrix as part of the extrinsic camera parameters.
		@param[in] t The translation vector as part of the extrinsic camera parameters.
		@param[in] width The width of the image sensor in pixels.
		@param[in] height The height of the image sensor in pixels.
	*/
	void SetCameraParameters(float fx, float fy, float cx, float cy,
			  float d1, float d2, float d3, float d4,
			  const Mat3d &R, const Vec3d &t, int width, int height);
	
	/*!
		\brief Sets the principal point and the focal lengths.
	 
		@param[in] cx The x-coordinate (or u-coordinate) of the principal point.
		@param[in] cy The y-coordinate (or v-coordinate) of the principal point.
		@param[in] fx The focal length in horizontal direction.
		@param[in] fy The focal length in vertical direction.
	*/
	void SetIntrinsicBase(float cx, float cy, float fx, float fy);
	
	/*!
		\brief Sets the distortion parameters of the distortion model.
	 
		@param[in] d1 The first radial lens distortion parameter.
		@param[in] d2 The second radial lens distortion parameter.
		@param[in] d3 The first tangential lens distortion parameter.
		@param[in] d4 The second tangential lens distortion parameter.
	*/
	void SetDistortion(float d1, float d2, float d3, float d4);

	/*!
		\brief Sets the extrinsic parameter R (rotation matrix).
		
		The member variables CCalibration::m_rotation_inverse and CCalibration::m_translation_inverse are updated automatically.
	 
		@param[in] R The rotation matrix as part of the extrinsic camera parameters.
	*/
	void SetRotation(const Mat3d &R);
	
	/*!
		\brief Sets the extrinsic parameter t (translation vector).
		
		The member variable CCalibration::m_translation_inverse is updated automatically.
	 
		@param[in] t The translation vector as part of the extrinsic camera parameters.
	*/
	void SetTranslation(const Vec3d &t);
	
	
	/*!
		\brief Sets up the calibration matrix K.
	 
		The calibration matrix is defined as:
	 
		\f$K = \left(\begin{array}{ccc}
		f_x & 0 & c_x\\
		0 & f_y & c_y\\
		0 & 0 & 1
		\end{array}\right)\f$
	 
		The camera model is described in the general information about the class CCalibration.
	 
		@param[out] K The result parameter for the 3x3 calibration matrix.
	*/
	void GetCalibrationMatrix(Mat3d &K) const;
	
	/*!
		\brief Sets up the projection matrix P.
		
		The projection matrix P is a 3x4 matrix defined as:
		
		\f$P = \left(K \cdot R | K \cdot {\bf t}\right)\f$, where K denotes the calibration matrix (see CCalibration::GetCalibrationMatrix) and R, t are the extrinsic camera parameters.
		
		The camera model is described in the general information about the class CCalibration.
		
		@param[out] P1 The left 3x3 part of the projection matrix, i.e. P1 = K R.
		@param[out] p2 The right 3x1 part of the projection matrix, i.e. p2 = K t.
	*/
	void GetProjectionMatrix(Mat3d &P1, Vec3d &p2) const;
	
	/*!
		\brief Gives access to the camera parameters.
		\return a reference to the internal instance of CCameraParameters.
	*/
	const CCameraParameters& GetCameraParameters() const { return m_cameraParameters; }
	

	/*!
		\brief Initializes the camera model, given a file path to a camera parameter file.
	 
		The format of the file is the same as the one used in OpenCV 1.0. The file format is described in CStereoCalibration::LoadCameraParameters.
	 
		Camera calibration can be performed with the application found in IVT/examples/CalibrationApp (resp. IVT/win32/CalibrationApp). See also the class CDLTCalibration.
		
		@param[in] pCameraParameterFileName The file path to the camera parameter file to be loaded.
		@param[in] nCamera If the camera parameter file contains a stereo calibration, this parameter indicates the parameters of which camera are to be loaded (0: first (usually left) camera, 1 : second (usually right) camera).
		@param[in] bSetExtrinsicToIdentity If set to true, then the extrinsic camera parameters R and t will be set to the identity transformation, i.e. R will be set to the identity matrix and t will be set to the translation vector.
		@return true on success and false on failure.
	*/
	bool LoadCameraParameters(const char *pCameraParameterFileName, int nCamera = 0, bool bSetExtrinsicToIdentity = false);
	
	/*!
		\brief Writes the parameters of the camera model to camera parameter file.
	 
		The format of the file is the same as the one used in OpenCV 1.0. The file format is described in CStereoCalibration::LoadCameraParameters.
	 
		@param[in] pCameraParameterFileName The file path to the target camera parameter file.
		@return true on success and false on failure.
	*/
	bool SaveCameraParameters(const char *pCameraParameterFileName) const;
	
	/*!
		\brief Prints all camera parameters in the console window (STDOUT).
	*/
	void PrintCameraParameters() const;
	
	
	/*!
		\brief Transforms 3D world coordinates to 2D image coordinates.
		
		Performs the mapping from the world coordinate system to the image coordinate system, i.e. application of the extrinsic camera parameters and the projection.
	 
		@param[in] worldPoint The 3D world coordinates.
		@param[out] imagePoint The result parameter for the 2D image coordinates.
		@param[in] bUseDistortionParameters If set to true, the distortion model will be applied after linear projection, i.e. distorted image coordinates will be written to the parameter imagePoint.
				If set to false, the distortion model is skipped, i.e. undistorted image coordinates will be written to the parameter imagePoint.
	*/
	void WorldToImageCoordinates(const Vec3d &worldPoint, Vec2d &imagePoint, bool bUseDistortionParameters = true) const;
	
	/*!
		\brief Transforms 2D image coordinates to 3D world coordinates.
	 
		Performs the mapping from the image coordinate system to the world coordinate system, i.e. back projection and application of the (inverse) extrinsic camera parameters.
		
		This mapping is ambiguous: All points that are mapped to 2D image point are located on straight line through the center of projection.
		
		@param[in] imagePoint The 2D image coordinates.
		@param[out] worldPoint The result parameter for the 3D world coordinates.
		@param[in] zc As the 2D => 3D mapping is ambiguous, this parameter is required as an additional constraint to specify a unique point. zc specifies the z-coordinate of the target point in the <b>camera</b> coordinate system (<b>not world coordinate system!</b>).
		@param[in] bUseDistortionParameters If set to true, the (inverse) distortion model will be applied before linear back projection, i.e. the input parameter imagePoint must contain distorted image coordinates.
				If set to false, the distortion model is skipped, i.e. the input parameter imagePoint must contain undistorted image coordinates.
	*/
	void ImageToWorldCoordinates(const Vec2d &imagePoint, Vec3d &worldPoint, float zc = 1.0f, bool bUseDistortionParameters = true) const;
	
	
	/*!
		\brief Transforms 3D world coordinates to 3D camera coordinates.
	 
		Performs the coordinate transformation from the world coordinate system to the camera coordinate system, i.e. application of the extrinsic camera parameters.
	 
		@param[in] worldPoint The 3D world coordinates.
		@param[out] cameraPoint The result parameter for the 3D camera coordinates.
	*/
	void WorldToCameraCoordinates(const Vec3d &worldPoint, Vec3d &cameraPoint) const;
	
	/*!
		\brief Transforms 3D camera coordinates to 3D world coordinates.
		
		Performs the coordinate transformation from the camera coordinate system to the world coordinate system, i.e. application of the inverse extrinsic camera parameters.
		
		@param[in] cameraPoint The 3D camera coordinates.
		@param[out] worldPoint The result parameter for the 3D world coordinates.
	*/
	void CameraToWorldCoordinates(const Vec3d &cameraPoint, Vec3d &worldPoint) const;

	
	/*!
		\brief Transforms 3D camera coordinates to 2D image coordinates.
	 
		Performs the mapping from the camera coordinate system to the image coordinate system, i.e. projection.
	 
		@param[in] cameraPoint The 3D camera coordinates.
		@param[out] imagePoint The result parameter for the 2D image coordinates.
		@param[in] bUseDistortionParameters If set to true, the distortion model will be applied after linear projection, i.e. distorted image coordinates will be written to the parameter imagePoint.
				If set to false, the distortion model is skipped, i.e. undistorted image coordinates will be written to the parameter imagePoint.
	 */
	void CameraToImageCoordinates(const Vec3d &cameraPoint, Vec2d &imagePoint, bool bUseDistortionParameters = true) const;
	
	/*!
		\brief Transforms 2D image coordinates to 3D camera coordinates.
	 
		Performs the mapping from the image coordinate system to the camera coordinate system, i.e. back projection.
	 
		This mapping is ambiguous: All points that are mapped to 2D image point are located on straight line through the center of projection.
	 
		@param[in] imagePoint The 2D image coordinates.
		@param[out] cameraPoint The result parameter for the 3D camera coordinates.
		@param[in] zc As the 2D => 3D mapping is ambiguous, this parameter is required as an additional constraint to specify a unique point. zc specifies the z-coordinate of the target point in the camera coordinate system.
		@param[in] bUseDistortionParameters If set to true, the (inverse) distortion model will be applied before linear back projection, i.e. the input parameter imagePoint must contain distorted image coordinates.
				If set to false, the distortion model is skipped, i.e. the input parameter imagePoint must contain undistorted image coordinates.
	*/	
	void ImageToCameraCoordinates(const Vec2d &imagePoint, Vec3d &cameraPoint, float zc = 1.0f, bool bUseDistortionParameters = true) const;
	
	
	/*!
		\brief Transforms 2D distorted image coordinates to 2D undistorted image coordinates.
	 
		Applies the inverse distortion model to 2D image coordinates.
	 
		Note that the distortion model cannot be inverted algebraically. Therfore, an iterative algorithm is applied.
	 
		@param[in] distortedImagePoint The 2D distorted image coordinates.
		@param[out] undistortedImagePoint The result parameter for the 2D undistorted image coordinates.
	*/
	void UndistortImageCoordinates(const Vec2d &distortedImagePoint, Vec2d &undistortedImagePoint) const;
	
	/*!
		\brief Transforms 2D undistorted image coordinates to 2D distorted image coordinates.
	 
		Applies the distortion model to 2D image coordinates.
	 
		@param[in] undistortedImagePoint The 2D undistorted image coordinates.
		@param[out] distortedImagePoint The result parameter for the 2D distorted image coordinates.
	*/
	void DistortImageCoordinates(const Vec2d &undistortedImagePoint, Vec2d &distortedImagePoint) const;
	
	
	/*!
		\deprecated
		\brief Deprecated.
		
		This method is deprecated. It internally calls UndistortImageCoordinates(const Vec2d&, Vec2d&); please refer to the documentation of this function for more information.
	*/
	void UndistortCameraCoordinates(const Vec2d &distortedCameraPoint, Vec2d &undistortedCameraPoint) const;
	
	/*!
		\deprecated
		\brief Deprecated.
		
		This method is deprecated. It internally calls DistortImageCoordinates(const Vec2d&, Vec2d&); please refer to the documentation of this function for more information.
	*/
	void DistortCameraCoordinates(const Vec2d &undistortedCameraPoint, Vec2d &distortedCameraPoint) const;
	
	/*!
		\deprecated
		\brief Deprecated.
		
		This method is deprecated. It internally calls WorldToImageCoordinates(const Vec3d&, Vec2d&, bool); please refer to the documentation of this function for more information.
	*/
	void GetCameraCoordinates(const Vec3d &worldPoint, Vec2d &imagePoint, bool bUseDistortionParameters = true) const;
	
	/*!
		\deprecated
		\brief Deprecated.
		
		This method is deprecated. It internally calls ImageToWorldCoordinates(const Vec2d&, Vec3d&, float, bool); please refer to the documentation of this function for more information.
	*/
	void GetWorldCoordinates(const Vec2d &imagePoint, Vec3d &worldPoint, float zc = 1.0f, bool bUseDistortionParameters = true) const;
	
	
	/*!
		\brief Rotation matrix of the inverted extrinsic transformation.
		
		The extrinsic parameters specify the mapping \f${\bf x}_c = R \cdot {\bf x}_w + {\bf t}\f$.
	 
		The inverse extrinsic parameters specify \f${\bf x}_w = R^{-1} \cdot {\bf x}_c- R^{-1}{\bf t}\f$.
	 
		CCalibration::m_rotation_inverse thus is \f$R^{-1}\f$.
	*/
	Mat3d m_rotation_inverse;
	
	/*!
		\brief Translation vector of the inverted extrinsic transformation.
	 
		The extrinsic parameters specify the mapping \f${\bf x}_c = R \cdot {\bf x}_w + {\bf t}\f$.
	 
		The inverse extrinsic parameters specify \f${\bf x}_w = R^{-1} \cdot {\bf x}_c - R^{-1}{\bf t}\f$.
	 
		CCalibration::m_translation_inverse thus is \f$-R^{-1}{\bf t}\f$. It also equals the world coordinates of the center of projection.
	*/
	Vec3d m_translation_inverse;


private:
	// private methods
	void CalculateInverseTransformation();
	void CalculateRadialLensDistortion();
	
	// private attributes

	// intrinsic camera parameters
	CCameraParameters m_cameraParameters;
};



#endif /* _CALIBRATION_H_ */
