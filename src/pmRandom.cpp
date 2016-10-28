/////////////////////////////////////////////////////////////////////////////
// Name:        pmRandom.cpp
// Purpose:     pmRandom namespace
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "pmRandom.h"

namespace pmRandom {
/////////////////////////////////////////////////////////////////////////////////////////
/// Generates a uniform distributed random number between minimum and maximum.
/////////////////////////////////////////////////////////////////////////////////////////
	float random(float const& minimum, float const& maximum) {
		if(minimum>=maximum) {
			pLogger::warning_msgf("Random number cannot be generated if the range is incorrect. Returns zero.\n");
			return 0.0f;
		}
		return ((float)(rand()%10000))/1e4f*(maximum-minimum)+minimum;
	}
/////////////////////////////////////////////////////////////////////////////////////////
/// Generates a uniform distributed random tensor between minimum and maximum. If the 
/// dimensions of the given tensors do not match it returns an empty tensor.
/////////////////////////////////////////////////////////////////////////////////////////
	pmTensor random(pmTensor const& minimum, pmTensor const& maximum) {
		if(minimum.get_numrows()!=maximum.get_numrows() || minimum.get_numcols()!=maximum.get_numcols()) {
			pLogger::error_msgf("Random numbers cannot be generated if the dimensions of the limist does not match.\n");
			return pmTensor{};
		}
		pmTensor random_tensor{minimum.get_numrows(), minimum.get_numcols()};
		for(int i=0; i<minimum.numel(); i++) {
			random_tensor[i] = random(minimum[i], maximum[i]);
		}
		return random_tensor;
	}
}


