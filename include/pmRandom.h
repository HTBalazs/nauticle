/////////////////////////////////////////////////////////////////////////////
// Name:        pmRandom.h
// Purpose:     pmRandom namespace
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <cstdlib>
#include "prolog/pLogger.h"
#include "pmTensor.h"

/** This namespace contains functions for random number generation.
*/
namespace pmRandom {
	float random(float const& minimum, float const& maximum);
	pmTensor random(pmTensor const& minimum, pmTensor const& maximum);
}

#endif //_RANDOM_H_