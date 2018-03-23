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

#ifndef _KERNEL_H_
#define _KERNEL_H_

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
	class pmKernel {
	private:
		// double (pmKernel::*kernel_ptr)(double const&, double const&) const;
		typedef  double (pmKernel::*ptr)(double const&, double const&) const;
		ptr kernel_ptr;
		double Wp0110_raw(double const& q) const;
		double Wp1110_raw(double const& q) const;
		double Wp2220_raw(double const& q) const;
		double Wp3220_raw(double const& q) const;
		double Wp5220_raw(double const& q) const;
		double We2100_raw(double const& q) const;
		double dWp0110_raw(double const& q) const;
		double dWp1110_raw(double const& q) const;
		double dWp2220_raw(double const& q) const;
		double dWp3220_raw(double const& q) const;
		double dWp5220_raw(double const& q) const;
		double dWe2100_raw(double const& q) const;

		double Wp01110(double const& distance, double const& cell_size) const;
		double Wp01120(double const& distance, double const& cell_size) const;
		double Wp01130(double const& distance, double const& cell_size) const;
		double dWp01110(double const& distance, double const& cell_size) const;
		double dWp01120(double const& distance, double const& cell_size) const;
		double dWp01130(double const& distance, double const& cell_size) const;
		double Wp11110(double const& distance, double const& cell_size) const;
		double Wp11120(double const& distance, double const& cell_size) const;
		double Wp11130(double const& distance, double const& cell_size) const;
		double dWp11110(double const& distance, double const& cell_size) const;
		double dWp11120(double const& distance, double const& cell_size) const;
		double dWp11130(double const& distance, double const& cell_size) const;
		double Wp22210(double const& distance, double const& cell_size) const;
		double Wp22220(double const& distance, double const& cell_size) const;
		double Wp22230(double const& distance, double const& cell_size) const;
		double dWp22210(double const& distance, double const& cell_size) const;
		double dWp22220(double const& distance, double const& cell_size) const;
		double dWp22230(double const& distance, double const& cell_size) const;
		double Wp32210(double const& distance, double const& cell_size) const;
		double Wp32220(double const& distance, double const& cell_size) const;
		double Wp32230(double const& distance, double const& cell_size) const;
		double dWp32210(double const& distance, double const& cell_size) const;
		double dWp32220(double const& distance, double const& cell_size) const;
		double dWp32230(double const& distance, double const& cell_size) const;
		double Wp52210(double const& distance, double const& cell_size) const;
		double Wp52220(double const& distance, double const& cell_size) const;
		double Wp52230(double const& distance, double const& cell_size) const;
		double dWp52210(double const& distance, double const& cell_size) const;
		double dWp52220(double const& distance, double const& cell_size) const;
		double dWp52230(double const& distance, double const& cell_size) const;
		double We21010(double const& distance, double const& cell_size) const;
		double We21020(double const& distance, double const& cell_size) const;
		double We21030(double const& distance, double const& cell_size) const;
		double dWe21010(double const& distance, double const& cell_size) const;
		double dWe21020(double const& distance, double const& cell_size) const;
		double dWe21030(double const& distance, double const& cell_size) const;

	public:
		enum kernel_type { Quadratic, Cubic, Wendland, Gaussian };
		pmKernel();
		pmKernel(pmKernel const& other);
		pmKernel& operator=(pmKernel const& other);
		void set_kernel_type(size_t const& i, bool const& derivative);
		double evaluate(double const& distance, double const& cell_size) const;
	};
}

#endif //_KERNEL_H_