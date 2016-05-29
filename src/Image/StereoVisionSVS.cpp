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
// Filename:  StereoVisionSVS.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include "StereoVisionSVS.h"

#include "ByteImage.h"
#include "Math/FloatMatrix.h"

#include <svsclass.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CStereoVisionSVS::CStereoVisionSVS()
{
	m_pSVSStereoImage = new svsStereoImage();
	m_pSVSStereoProcess = new svsStereoProcess();
	m_pSVSStereoImage->disparity = new short[640 * 480];
}

CStereoVisionSVS::~CStereoVisionSVS()
{
	delete m_pSVSStereoProcess;
	delete m_pSVSStereoImage;
}


// ****************************************************************************
// Methods
// ****************************************************************************

bool CStereoVisionSVS::Process(CByteImage *pLeftImage, CByteImage *pRightImage, CFloatMatrix *pDepthMap, int nWindowSize, int nDisparites, int nStep, int nThreshold)
{
	if (pLeftImage->width != pRightImage->width || pLeftImage->height != pRightImage->height ||
		pLeftImage->type != CByteImage::eGrayScale || pRightImage->type != CByteImage::eGrayScale ||
		pDepthMap->columns != pLeftImage->width || pDepthMap->rows != pLeftImage->height)
		return false;
		
	m_pSVSStereoImage->dp.corrsize = nWindowSize;
	m_pSVSStereoImage->dp.thresh = nThreshold;
	m_pSVSStereoImage->dp.lr = 1;
	m_pSVSStereoImage->dp.ndisp = nDisparites;
	m_pSVSStereoImage->dp.dpp = nStep;
	m_pSVSStereoImage->dp.offx = 0;
	m_pSVSStereoImage->dp.offy = 0;
	
	m_pSVSStereoImage->have3D = false;
	
	svsImageParams ip;
	ip.linelen = pLeftImage->width;
	ip.lines = pLeftImage->height;
	ip.ix = 0;
	ip.iy = 0;
	ip.width = pLeftImage->width;
	ip.height = pRightImage->height;
	ip.vergence = 0;
	ip.gamma = 1.0;
		
	m_pSVSStereoImage->SetImages(pLeftImage->pixels, pRightImage->pixels, 0, 0, &ip, 0, false, false);
	
	m_pSVSStereoProcess->CalcStereo(m_pSVSStereoImage, false);
	
	const int nPixels = pLeftImage->width * pLeftImage->height;
	const short *input = m_pSVSStereoImage->disparity;
	float *output = pDepthMap->data;
	
	for (int i = 0; i < nPixels; i++)
		output[i] = input[i];

	return true;
}
