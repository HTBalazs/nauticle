/*
    Copyright 2016-2020 Balazs Havasi-Toth
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
    
#ifndef _PM_RANDOM_H_
#define _PM_RANDOM_H_

#include "prolog/pLogger.h"
#include "pmTensor.h"
#include <cstdlib>
#include <random>

namespace Nauticle {
    /** This namespace contains functions for random number generation.
    */
    namespace pmRandom {
        enum RANDOM_TYPE {UNIFORM, NORMAL, LOGNORMAL};
        /////////////////////////////////////////////////////////////////////////////////////////
        /// Generates a random number with R_TYPE distribution using the parameters p1 and p2.
        /////////////////////////////////////////////////////////////////////////////////////////
        template <RANDOM_TYPE R_TYPE>
        double random(double const& p1, double const& p2) {
            static bool pseudo = true;
            if(!pseudo) {
                srand(time(NULL));
                pseudo = false;
            }
            if(p1>=p2 && R_TYPE==UNIFORM) {
                ProLog::pLogger::warning_msgf("Random number cannot be generated if the range is incorrect. Returns zero.\n");
                return 0.0;
            }
            static std::default_random_engine generator;
            switch(R_TYPE) {
                default:
                case UNIFORM:   { std::uniform_real_distribution<double> uniform(p1,p2);
                                return uniform(generator); }
                case NORMAL:    { std::normal_distribution<double> normal(p1,p2);
                                return normal(generator); }
                case LOGNORMAL:     { std::lognormal_distribution<double> lognormal(p1,p2);
                                    return lognormal(generator); }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /// Generates a uniform distributed random tensor between p1 and p2. If the 
        /// dimensions of the given tensors do not match it returns an empty tensor.
        /////////////////////////////////////////////////////////////////////////////////////////
        template <RANDOM_TYPE R_TYPE> 
        pmTensor random(Nauticle::pmTensor const& p1, Nauticle::pmTensor const& p2) {
            if(p1.get_numrows()!=p2.get_numrows() || p1.get_numcols()!=p2.get_numcols()) {
                ProLog::pLogger::error_msgf("Random numbers cannot be generated if the dimensions of the limits does not match.\n");
                return Nauticle::pmTensor{};
            }
            Nauticle::pmTensor random_tensor{p1.get_numrows(), p1.get_numcols()};
            for(int i=0; i<p1.numel(); i++) {
                random_tensor[i] = random<R_TYPE>((double)p1[i], (double)p2[i]);
            }
            return random_tensor;
        }
    }
}

#endif //_PM_RANDOM_H_