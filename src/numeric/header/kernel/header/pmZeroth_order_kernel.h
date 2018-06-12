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

#ifndef _ZEROTH_ORDER_KERNEL_H_
#define _ZEROTH_ORDER_KERNEL_H_

#include "pmKernel_function.h"
#include <cmath>
#include "nauticle_constants.h"

namespace Nauticle {
	/** This class contains the zeroth order kernel smoothing kernel implementations for 1, 2 and 3 dimensions.
	*/
	template<size_t dimension, bool derivative>
	class pmZeroth_order_kernel : public pmKernel_function {
		double coefficient(double const& influence_radius) const override;
		double kernel_at(double const& q) const;
	public:
		pmZeroth_order_kernel();
		double evaluate(double const& r, double const& influence_radius) const override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<size_t dimension, bool derivative>
	pmZeroth_order_kernel<dimension,derivative>::pmZeroth_order_kernel() {
		this->name = derivative?std::string("d"):std::string("")+std::string("Wp011")+std::to_string(dimension)+std::string("0");
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the normalization coefficient of the kernel function or its derivative.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<size_t dimension, bool derivative>
	double pmZeroth_order_kernel<dimension,derivative>::coefficient(double const& influence_radius) const {
		switch(dimension) {
			default:
			case 1 : return derivative ? 0.0 : 0.5/influence_radius;
			case 2 : return derivative ? 0.0 : 1.0/NAUTICLE_PI/influence_radius/influence_radius;
			case 3 : return derivative ? 0.0 : 3.0/4.0/NAUTICLE_PI/influence_radius/influence_radius/influence_radius;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the value of the kernel function or its derivative at a given place q=r/h.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<size_t dimension, bool derivative>
	double pmZeroth_order_kernel<dimension,derivative>::kernel_at(double const& q) const {
		return !derivative;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the kernel value or its derivative at a given distance r and radius influence_radius.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<size_t dimension, bool derivative>
	double pmZeroth_order_kernel<dimension,derivative>::evaluate(double const& r, double const& influence_radius) const {
		double q = r/influence_radius;
		return coefficient(influence_radius)*kernel_at(q);
	}
}

#endif // _ZEROTH_ORDER_KERNEL_H_