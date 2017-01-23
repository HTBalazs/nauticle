/*
    Copyright 2017 Balazs Toth
    This file is part of LEMPS.

    LEMPS is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LEMPS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LEMPS.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://BalazsToth@bitbucket.org/BalazsToth/lemps
*/
    
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