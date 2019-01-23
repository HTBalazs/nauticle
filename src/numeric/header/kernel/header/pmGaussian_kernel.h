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

#ifndef _GAUSSIAN_KERNEL_H_
#define _GAUSSIAN_KERNEL_H_

#include "pmKernel_function.h"
#include <cmath>
#include "nauticle_constants.h"

namespace Nauticle {
	/** This class contains the exponential (Gaussian) smoothing kernel implementations for 1, 2 and 3 dimensions.
	*/
    template<size_t dimension, bool derivative>
    class pmGaussian_kernel : public pmKernel_function {
        double coefficient(double const& sigma) const override;
    public:
        pmGaussian_kernel();
        double evaluate(double const& r, double const& sigma) const override;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Constructor.
    /////////////////////////////////////////////////////////////////////////////////////////
    template<size_t dimension, bool derivative>
    pmGaussian_kernel<dimension,derivative>::pmGaussian_kernel() {
        this->name = derivative?std::string("d"):std::string("")+std::string("We210")+std::to_string(dimension)+std::string("0");
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the normalization coefficient of the kernel function or its derivative.
    /////////////////////////////////////////////////////////////////////////////////////////
    template<size_t dimension, bool derivative>
    double pmGaussian_kernel<dimension,derivative>::coefficient(double const& sigma) const {
        switch(dimension) {
            default:
            case 1 : return 1.0/std::sqrt(2.0*NAUTICLE_PI*sigma*sigma);
            case 2 : return 1.0/NAUTICLE_PI/sigma/sigma;
            case 3 : return 1.0/std::pow(NAUTICLE_PI,1.5)/sigma/sigma/sigma;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the kernel value or its derivative at a given distance r and radius influence_radius.
    /////////////////////////////////////////////////////////////////////////////////////////
    template<size_t dimension, bool derivative>
    double pmGaussian_kernel<dimension,derivative>::evaluate(double const& r, double const& sigma) const {
        if(!derivative) {
            return coefficient(sigma)*std::exp(-r*r/sigma/sigma);
        } else {
            return -r/sigma/sigma*coefficient(sigma)*std::exp(-r*r/sigma/sigma);
        }
    }
}

#endif // _GAUSSIAN_KERNEL_H_