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

#ifndef _PM_HYSTERON_H_
#define _PM_HYSTERON_H_

#include <stdio.h>
#include <stdlib.h>

namespace Nauticle {
	enum Hysteron_event {UP, DOWN, UNCHANGED};
	class pmHysteron {
		double alpha;
	    double beta;
		bool state;
		Hysteron_event event;
	public:
		pmHysteron();
		pmHysteron(double const& a, double const& b, bool const& s=false):alpha{a},beta{b},state{s},event{UNCHANGED}{}
		double const& get_alpha() const;
		double const& get_beta() const;
		void set_boundaries(double const& a, double const& b);
		void update(double const& x);
		bool const& get_state() const;
		Hysteron_event const& get_event() const;
	};
}

#endif //_PM_HYSTERON_H_