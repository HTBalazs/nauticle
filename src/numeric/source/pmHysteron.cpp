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

#include "pmHysteron.h"
#include "prolog/pLogger.h"

using namespace Nauticle;

pmHysteron::pmHysteron() {
    alpha = 0.0;
    beta = 0.01;
    state = false;
    event = UNCHANGED;
}

double const& pmHysteron::get_alpha() const {
	return alpha;
}

double const& pmHysteron::get_beta() const {
    return beta;
}

void pmHysteron::set_boundaries(double const& a, double const& b) {
    alpha = a;
    beta = b;
}

void pmHysteron::update(double const& x) {
    if(x<alpha && state) {
        state = false;
        event = DOWN;
    } else if(x>beta && !state) {
        state = true;
        event = UP;
    } else {
        event = UNCHANGED;
    }
}

bool const& pmHysteron::get_state() const {
    return state;
}

Hysteron_event const& pmHysteron::get_event() const {
    return event;
}
