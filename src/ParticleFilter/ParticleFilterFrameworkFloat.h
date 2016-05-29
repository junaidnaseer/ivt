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
// Filename:  ParticleFilterFrameworkFloat.h
// Author:    Pedram Azad
// Date:      07.05.2009
// ****************************************************************************


#ifndef __PARTICLE_FILTER_FRAMEWORK_FLOAT_H__
#define __PARTICLE_FILTER_FRAMEWORK_FLOAT_H__



// ****************************************************************************
// CParticleFilterFrameworkFloat
// ****************************************************************************

/*!
	\ingroup Tracking
	\brief Framework for the implementation of particle filters using the data type float.
*/
class CParticleFilterFrameworkFloat
{
public:
	// constructor
	CParticleFilterFrameworkFloat(int nParticles, int nDimension);

	// destructor
	~CParticleFilterFrameworkFloat();


	// public methods
	double ParticleFilter(float *pResultMeanConfiguration, float fSigmaFactor = 1.0f);
	double CalculateProbabilityForConfiguration(const float *pConfiguration);
	virtual void GetConfiguration(float *pBestConfiguration, float fMeanFactor);
	virtual void GetBestConfiguration(float *pBestConfiguration);
	virtual void GetMeanConfiguration(float *pMeanConfiguration);
	virtual void GetPredictedConfiguration(float *pPredictedConfiguration);


protected:
	// protected methods
	int PickBaseSample();
	void CalculateMean();

	// virtual methods (framework methods to be implemented: design pattern "framwork" with "template methods")
	virtual void UpdateModel(int nParticle) = 0;
	virtual void PredictNewBases(float fSigmaFactor) = 0;
	virtual double CalculateProbability(bool bSeparateCall = true) = 0;
	virtual void CalculateFinalProbabilities() { }
	

	// protected attributes
	float *mean_configuration;
	float *last_configuration;

	float *sigma;
	float *lower_limit;
	float *upper_limit;

	// particle related attributes
	int m_nDimension;
	int m_nParticles;
	double c_total;
	float **s;
	float **s_temp;
	double *c;
	double *pi;
	float *temp;
};



#endif /* __PARTICLE_FILTER_FRAMEWORK_FLOAT_H__ */
