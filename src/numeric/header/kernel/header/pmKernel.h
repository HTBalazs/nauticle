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

#ifndef _PM_KERNEL_H_
#define _PM_KERNEL_H_

#include "pmKernel_includes.h"
#include "pmNoncopyable.h"
#include <memory>
#include <cstdio>

namespace Nauticle {

	/** This class contains the smoothing kernel implementations for 1, 2 and 3 dimensions.
	//  An object of this class behaves as a smoohting kernel through the evaluate function.
	//  The type of the kernel to use is set by set_kernel_type() member function.
	//
	// Kernel notations:
	//  dWlnnnnn
	//	01234567
	//	
	//	0: optional flag for derivative.
	//	1: W (weight function).
	//  2: can be one of the followings: p (polynomial), e (exponential)
	//	3: order of the kernel.
	//  4: number of pieces along the influence radius.
	//  5: influence radius/smoothing radius. Zero for infinite influence radius.
	//	6: Dimensions.
	//	7: serial number.
	*/
	class pmKernel : public pmNoncopyable {
	private:
		std::shared_ptr<pmKernel_function> kernel;
	public:
		pmKernel();
		pmKernel(pmKernel const& other);
		pmKernel& operator=(pmKernel const& other);
		void set_kernel_type(size_t const& i, bool const& derivative);
		double evaluate(double const& distance, double const& cell_size) const;
	};
}

#endif //_PM_KERNEL_H_