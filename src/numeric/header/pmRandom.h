/*
    Copyright 2016-2019 Balazs Toth
    This file is part of Nauticle.

    Nauticle is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Nauticle is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Nauticle.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://bitbucket.org/nauticleproject/
*/
    
#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <cstdlib>
#include "prolog/pLogger.h"
#include "pmTensor.h"

namespace Nauticle {
    /** This namespace contains functions for random number generation.
    */
    namespace pmRandom {
    	double random(double const& minimum, double const& maximum);
    	pmTensor random(pmTensor const& minimum, pmTensor const& maximum);
    }
}

#endif //_RANDOM_H_