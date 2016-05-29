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
// Filename:  Normalizer.cpp
// Author:    Pedram Azad
// Date:      2005
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "Normalizer.h"

#include <float.h>



// ****************************************************************************
// Functions
// ****************************************************************************

void Normalizer::NormalizeLength(float *pInput, int nInputLength, float *pOutput, int nOutputLength)
{
	const float factor = float(nInputLength) / nOutputLength;

	int i;
	const int last_i = int((nInputLength - 1) / factor);

	for (i = 0; i <= last_i; i++)
	{
		const int offset = int(i * factor);
		const float w = i * factor - offset;

		pOutput[i] = (1 - w) * pInput[offset] + w * pInput[offset + 1];
	}

	for (i = last_i + 1; i < nOutputLength; i++)
		pOutput[i] = pInput[nInputLength - 1];
}


void Normalizer::NormalizeAmplitude(float *pData, int nLength)
{
	float min = FLT_MAX, max = -FLT_MAX;
	int i;

	if (nLength < 2)
		return;

	for (i = 0; i < nLength; i++)
	{
		if (pData[i] > max)
			max = pData[i];

		if (pData[i] < min)
			min = pData[i];
	}

	const float factor = 100.0f / (max - min);

	for (i = 0; i < nLength; i++)
		pData[i] = (pData[i] - min) * factor;
}

void Normalizer::NormalizeStartingPoint(float *pData, int nLength)
{
	if (nLength < 2)
		return;

	const float first = pData[0];

	for (int i = 0; i < nLength; i++)
		pData[i] -= first;
}
