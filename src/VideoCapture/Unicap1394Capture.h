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
// Filename:  Unicap1394Capture.h
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


#ifndef _UNICAP_1394_CAPTURE_H_
#define _UNICAP_1394_CAPTURE_H_


// ****************************************************************************
// Necessary includes
// ****************************************************************************

#include <unicap.h>
#include <unicap_status.h>

#include "Interfaces/VideoCaptureInterface.h"


// ****************************************************************************
// Defines
// ****************************************************************************

#define UNICAP_MAX_CAMERAS	10



// ****************************************************************************
// CUnicap1394Capture
// ****************************************************************************

class CUnicap1394Capture : public CVideoCaptureInterface
{
public:
	CUnicap1394Capture(int nCameras, VideoMode mode);
	~CUnicap1394Capture();

	bool OpenCamera();
	void CloseCamera();
	bool CaptureImage(CByteImage **ppImages);

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	CByteImage::ImageType GetType()  { return CByteImage::eRGB24; }
	int GetNumberOfCameras() { return 2; }

private:
	// private methods
	void Convert(const unsigned char *input, unsigned char *output);


	unicap_handle_t handle[UNICAP_MAX_CAMERAS];
	unicap_device_t device[UNICAP_MAX_CAMERAS];
	unicap_data_buffer_t buffer[UNICAP_MAX_CAMERAS];

	unsigned int source, norm;
	int width, height;
	VideoMode m_mode;
	int m_nCameras;
};



#endif /* _UNICAP_1394_CAPTURE_H_ */
