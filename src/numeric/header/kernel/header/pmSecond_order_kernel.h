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

#ifndef _PM_SECOND_ORDER_KERNEL_H_
#define _PM_SECOND_ORDER_KERNEL_H_

#include "pmKernel_function.h"
#include "nauticle_constants.h"
#include <cmath>
#include <string>

namespace Nauticle {
    /** This class contains the second order kernel smoothing kernel implementations for 1, 2 and 3 dimensions.
    */
    template<size_t dimension, bool derivative>
    class pmSecond_order_kernel : public pmKernel_function {
        double coefficient(double const& h) const override;
        double kernel_at(double const& q) const;
    public:
        pmSecond_order_kernel();
        double evaluate(double const& r, double const& influence_radius) const override;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Constructor.
    /////////////////////////////////////////////////////////////////////////////////////////
    template<size_t dimension, bool derivative>
    pmSecond_order_kernel<dimension,derivative>::pmSecond_order_kernel() {
        this->name = derivative?std::string("d"):std::string("")+std::string("Wp222")+std::to_string(dimension)+std::string("0");
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the normalization coefficient of the kernel function or its derivative.
    /////////////////////////////////////////////////////////////////////////////////////////
    template<size_t dimension, bool derivative>
    double pmSecond_order_kernel<dimension,derivative>::coefficient(double const& h) const {
        switch(dimension) {
            default:
            case 1 : return derivative ? 3.0/4.0/h/h : 1.0/h;
            case 2 : return derivative ? 6.0/4.0/NAUTICLE_PI/h/h/h : 2.0/NAUTICLE_PI/h/h;
            case 3 : return derivative ? 15.0/16.0/NAUTICLE_PI/h/h/h/h : 5.0/4.0/NAUTICLE_PI/h/h/h;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the value of the kernel function or its derivative at a given place q=r/h.
    /////////////////////////////////////////////////////////////////////////////////////////
    template<size_t dimension, bool derivative>
    double pmSecond_order_kernel<dimension,derivative>::kernel_at(double const& q) const {
        if(!derivative) {
            return 3.0/16.0*q*q-0.75*q+0.75;
        } else {
            return q/2.0-1.0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the kernel value or its derivative at a given distance r and radius influence_radius.
    /////////////////////////////////////////////////////////////////////////////////////////
    template<size_t dimension, bool derivative>
    double pmSecond_order_kernel<dimension,derivative>::evaluate(double const& r, double const& influence_radius) const {
        double h = influence_radius/2.0;
        double q = r/h;
        return coefficient(h)*kernel_at(q);
    }
}

#endif //_PM_SECOND_ORDER_KERNEL_H_