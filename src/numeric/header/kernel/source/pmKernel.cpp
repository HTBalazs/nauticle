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

#include <iostream>
#include <cmath>
#include "pmKernel.h"
#include "nauticle_constants.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmKernel::pmKernel() {
	kernel = std::make_shared<pmSecond_order_kernel<1,false>>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set kernel type of the object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmKernel::set_kernel_type(size_t const& i, bool const& derivative) {
	if(derivative) {
		switch(i) {
			default:
			case 0: kernel = std::make_shared<pmZeroth_order_kernel<1,true>>(); break;
			case 1: kernel = std::make_shared<pmZeroth_order_kernel<2,true>>(); break;
			case 2: kernel = std::make_shared<pmZeroth_order_kernel<3,true>>(); break;
			case 3: kernel = std::make_shared<pmFirst_order_kernel<1,true>>(); break;
			case 4: kernel = std::make_shared<pmFirst_order_kernel<2,true>>(); break;
			case 5: kernel = std::make_shared<pmFirst_order_kernel<3,true>>(); break;
			case 6: kernel = std::make_shared<pmSecond_order_kernel<1,true>>(); break;
			case 7: kernel = std::make_shared<pmSecond_order_kernel<2,true>>(); break;
			case 8: kernel = std::make_shared<pmSecond_order_kernel<3,true>>(); break;
			case 9: kernel = std::make_shared<pmThird_order_kernel<1,true>>(); break;
			case 10: kernel = std::make_shared<pmThird_order_kernel<2,true>>(); break;
			case 11: kernel = std::make_shared<pmThird_order_kernel<3,true>>(); break;
			case 12: kernel = std::make_shared<pmFifth_order_kernel<1,true>>(); break;
			case 13: kernel = std::make_shared<pmFifth_order_kernel<2,true>>(); break;
			case 14: kernel = std::make_shared<pmFifth_order_kernel<3,true>>(); break;
			case 15: kernel = std::make_shared<pmGaussian_kernel<1,true>>(); break;
			case 16: kernel = std::make_shared<pmGaussian_kernel<2,true>>(); break;
			case 17: kernel = std::make_shared<pmGaussian_kernel<3,true>>(); break;
		}
	} else {
		switch(i) {
			default:
			case 0: kernel = std::make_shared<pmZeroth_order_kernel<1,false>>(); break;
			case 1: kernel = std::make_shared<pmZeroth_order_kernel<2,false>>(); break;
			case 2: kernel = std::make_shared<pmZeroth_order_kernel<3,false>>(); break;
			case 3: kernel = std::make_shared<pmFirst_order_kernel<1,false>>(); break;
			case 4: kernel = std::make_shared<pmFirst_order_kernel<2,false>>(); break;
			case 5: kernel = std::make_shared<pmFirst_order_kernel<3,false>>(); break;
			case 6: kernel = std::make_shared<pmSecond_order_kernel<1,false>>(); break;
			case 7: kernel = std::make_shared<pmSecond_order_kernel<2,false>>(); break;
			case 8: kernel = std::make_shared<pmSecond_order_kernel<3,false>>(); break;
			case 9: kernel = std::make_shared<pmThird_order_kernel<1,false>>(); break;
			case 10: kernel = std::make_shared<pmThird_order_kernel<2,false>>(); break;
			case 11: kernel = std::make_shared<pmThird_order_kernel<3,false>>(); break;
			case 12: kernel = std::make_shared<pmFifth_order_kernel<1,false>>(); break;
			case 13: kernel = std::make_shared<pmFifth_order_kernel<2,false>>(); break;
			case 14: kernel = std::make_shared<pmFifth_order_kernel<3,false>>(); break;
			case 15: kernel = std::make_shared<pmGaussian_kernel<1,false>>(); break;
			case 16: kernel = std::make_shared<pmGaussian_kernel<2,false>>(); break;
			case 17: kernel = std::make_shared<pmGaussian_kernel<3,false>>(); break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates kernel.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::evaluate(double const& distance, double const& cell_size) const {
	return kernel->evaluate(distance, cell_size);
}

#undef NAUTICLE_PI
