/*
    Copyright 2016-2018 Balazs Toth
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

#ifndef _FIRST_ORDER_KERNEL_H_
#define _FIRST_ORDER_KERNEL_H_

#include "pmKernel_function.h"
#include <cmath>
#include "nauticle_constants.h"

namespace Nauticle {
	/** This class contains the first order smoothing kernel implementations for 1, 2 and 3 dimensions.
	*/
    template<size_t dimension, bool derivative>
    class pmFirst_order_kernel : public pmKernel_function {
        double coefficient(double const& influence_radius) const override;
        double kernel_at(double const& q) const;
    public:
        pmFirst_order_kernel();
        double evaluate(double const& r, double const& influence_radius) const override;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Constructor.
    /////////////////////////////////////////////////////////////////////////////////////////
    template<size_t dimension, bool derivative>
    pmFirst_order_kernel<dimension,derivative>::pmFirst_order_kernel() {
        this->name = derivative?std::string("d"):std::string("")+std::string("Wp111")+std::to_string(dimension)+std::string("0");
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the normalization coefficient of the kernel function or its derivative.
    /////////////////////////////////////////////////////////////////////////////////////////
    template<size_t dimension, bool derivative>
    double pmFirst_order_kernel<dimension,derivative>::coefficient(double const& influence_radius) const {
        switch(dimension) {
            default:
            case 1 : return derivative ? 1.0/influence_radius/influence_radius : 1.0/influence_radius;
            case 2 : return derivative ? 3.0/NAUTICLE_PI/influence_radius/influence_radius/influence_radius : 3.0/NAUTICLE_PI/influence_radius/influence_radius;
            case 3 : return derivative ? 3.0/NAUTICLE_PI/influence_radius/influence_radius/influence_radius/influence_radius : 3.0/NAUTICLE_PI/influence_radius/influence_radius/influence_radius;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the value of the kernel function or its derivative at a given place q=r/h.
    /////////////////////////////////////////////////////////////////////////////////////////
    template<size_t dimension, bool derivative>
    double pmFirst_order_kernel<dimension,derivative>::kernel_at(double const& q) const {
        if(!derivative) {
            return 1.0-q;
        } else {
            return -1.0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the kernel value or its derivative at a given distance r and radius influence_radius.
    /////////////////////////////////////////////////////////////////////////////////////////
    template<size_t dimension, bool derivative>
    double pmFirst_order_kernel<dimension,derivative>::evaluate(double const& r, double const& influence_radius) const {
        double q = r/influence_radius;
        return coefficient(influence_radius)*kernel_at(q);
    }
}

#endif // _FIRST_ORDER_KERNEL_H_