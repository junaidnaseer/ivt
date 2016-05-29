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
// Filename:  ParticleFilterFramework.cpp
// Author:    Pedram Azad
// Date:      2004
// ****************************************************************************


// ****************************************************************************
// Includes
// ****************************************************************************

#include <new> // for explicitly using correct new/delete operators on VC DSPs

#include "ParticleFilterFramework.h"

#include "Helpers/helpers.h"

#include <math.h>
#include <stdio.h>
#include <float.h>



// ****************************************************************************
// Constructor / Destructor
// ****************************************************************************

CParticleFilterFramework::CParticleFilterFramework(int nParticles, int nDimension)
{
	m_nParticles = nParticles;
	m_nDimension = nDimension;

	mean_configuration = new double[nDimension];
	last_configuration = new double[nDimension];

	sigma = new double[nDimension];
	lower_limit = new double[nDimension];
	upper_limit = new double[nDimension];

	int i;

	// allocate memory
	s = new double*[nParticles];
	for (i = 0; i < nParticles; i++)
		s[i] = new double[nDimension];

	s_temp = new double*[nParticles];
	for (i = 0; i < nParticles; i++)
		s_temp[i] = new double[nDimension];

	c = new double[nParticles];
	pi = new double[nParticles];

	temp = new double[nDimension];
	
	for (i = 0; i < nParticles; i++)
	{
		c[i] = 0.0;
		pi[i] = 0.0;
	}
}

CParticleFilterFramework::~CParticleFilterFramework()
{
	delete [] mean_configuration;
	delete [] last_configuration;
	delete [] sigma;
	delete [] lower_limit;
	delete [] upper_limit;

	int i;

	for (i = 0; i < m_nParticles; i++)
		delete [] s[i];
	delete [] s;

	for (i = 0; i < m_nParticles; i++)
		delete [] s_temp[i];
	delete [] s_temp;

	delete [] c;
	delete [] pi;

	delete [] temp;
}


// ****************************************************************************
// Methods
// ****************************************************************************

void CParticleFilterFramework::GetConfiguration(double *pBestConfiguration, double dMeanFactor)
{
	double mean = 0;
	int nConfigurations = 0, i;
	
	for (i = 0; i < m_nDimension; i++)
		pBestConfiguration[i] = 0;

	for (i = 0; i < m_nParticles; i++)
		mean += pi[i];
		
	mean /= m_nParticles * dMeanFactor;
	
	for (i = 0; i < m_nParticles; i++)
		if (pi[i] > mean)
		{
			for (int j = 0; j < m_nDimension; j++)
				pBestConfiguration[j] += s[i][j];
			
			nConfigurations++;
		}

	if (nConfigurations > 0)
	{
		for (i = 0; i < m_nDimension; i++)
			pBestConfiguration[i] /= nConfigurations;
	}
}

void CParticleFilterFramework::GetBestConfiguration(double *pBestConfiguration)
{
	double max = -DBL_MAX;
	int best_i, i;

	for (i = 0; i < m_nParticles; i++)
		if (pi[i] > max)
		{
			best_i = i;
			max = pi[i];
		}

	
	for (i = 0; i < m_nDimension; i++)
		pBestConfiguration[i] = s[best_i][i];
}

void CParticleFilterFramework::GetMeanConfiguration(double *pMeanConfiguration)
{
	for (int i = 0; i < m_nDimension; i++)
		pMeanConfiguration[i] = mean_configuration[i];
}

void CParticleFilterFramework::GetPredictedConfiguration(double *pPredictedConfiguration)
{
	for (int i = 0; i < m_nDimension; i++)
		pPredictedConfiguration[i] = mean_configuration[i] + 0.8 * (mean_configuration[i] - last_configuration[i]);
}


int CParticleFilterFramework::PickBaseSample()
{
	double choice = uniform_random() * c_total;

	int low = 0;
	int high = m_nParticles;

	while (high > (low + 1))
	{
		int middle = (high + low) / 2;
    
		if (choice > c[middle])
			low = middle;
		else
			high = middle;
	}

	return low;
}

double CParticleFilterFramework::ParticleFilter(double *pResultMeanConfiguration, double dSigmaFactor)
{
	// push previous state through process model
	// use dynamic model and add noise
	PredictNewBases(dSigmaFactor);

	// apply bayesian measurement weighting
	c_total = 0;
	int i;
	for (i = 0; i < m_nParticles; i++)
	{
		// update model (calculate forward kinematics)
		UpdateModel(i);

		// evaluate likelihood function (compare edges,...)
		pi[i] = CalculateProbability(false);
	}
	
	CalculateFinalProbabilities();
	
	for (i = 0; i < m_nParticles; i++)
	{
		c[i] = c_total;
		c_total += pi[i];
	}

	// normalize
	const double factor = 1 / c_total;
	for (i = 0; i < m_nParticles; i++)
		pi[i] *= factor;

	CalculateMean();

	GetMeanConfiguration(pResultMeanConfiguration);
	
	return CalculateProbabilityForConfiguration(pResultMeanConfiguration);
}

void CParticleFilterFramework::CalculateMean()
{
	int i;
	double max = -1;

	for (i = 0; i < m_nDimension; i++)
	{
		last_configuration[i] = mean_configuration[i];
		mean_configuration[i] = 0;
	}

	for (i = 0; i < m_nParticles; i++)
	{
		for (int j = 0; j < m_nDimension; j++)
			mean_configuration[j] += pi[i] * s[i][j];
	}
}

double CParticleFilterFramework::CalculateProbabilityForConfiguration(const double *pConfiguration)
{
	// little "trick" for calculating the probability for the mean configuration
	// without introducing a new virtual function by using s[0]
	
	int i;
	
	for (i = 0; i < m_nDimension; i++)
	{
		temp[i] = s[0][i];
		s[0][i] = pConfiguration[i];
	}

	UpdateModel(0);

	for (i = 0; i < m_nDimension; i++)
		s[0][i] = temp[i];

	const double dResult = CalculateProbability(true);

	for (i = 0; i < m_nDimension; i++)
		s[0][i] = temp[i];
		
	return dResult;
}
