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

#ifndef _PM_KERNEL_FUNCTION_H_
#define _PM_KERNEL_FUNCTION_H_

#include <string>

namespace Nauticle {
	/** This abstract class implements the parent of the kernel functions.
	*/
	class pmKernel_function {
	protected:
		std::string name;
	protected:
		virtual double coefficient(double const& influence_radius) const=0;
	public:
		std::string get_name() const;
		virtual double evaluate(double const& r, double const& influence_radius) const=0;
		double operator()(double const& r, double const& influence_radius) const;
	};

	inline std::string pmKernel_function::get_name() const {
		return name;
	}

	inline double pmKernel_function::operator()(double const& r, double const& influence_radius) const {
		return evaluate(r, influence_radius);
	}
}

#endif //_PM_KERNEL_FUNCTION_H_