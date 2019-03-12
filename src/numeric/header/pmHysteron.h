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

#ifndef _HYSTERON_H_
#define _HYSTERON_H_

#include <stdio.h>
#include <stdlib.h>

namespace Nauticle {
	class pmHysteron {
		double alpha;
	    double beta;
		mutable bool state;
		mutable bool upswitch_event;
	public:
		pmHysteron();
		double const& get_alpha() const;
		double const& get_beta() const;
		void set_boundaries(double const& a, double const& b);
		void update(double const& x) const;
		bool const& get_state() const;
		bool const& switched_up() const;
	};
}

#endif // _HYSTERON_H_