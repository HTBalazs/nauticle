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

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmHysteron::pmHysteron() {
    alpha = 0.0;
    beta = 0.0001;
    state = false;
    event = UNCHANGED;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the lower bound.
/////////////////////////////////////////////////////////////////////////////////////////
double const& pmHysteron::get_alpha() const {
	return alpha;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the upper bound.
/////////////////////////////////////////////////////////////////////////////////////////
double const& pmHysteron::get_beta() const {
    return beta;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set lower and upper bounds.
/////////////////////////////////////////////////////////////////////////////////////////
void pmHysteron::set_boundaries(double const& a, double const& b) {
    alpha = a;
    beta = b;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates the state of the hysteron and sets the event member if the state has changed.
/////////////////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the state of the hysteron.
/////////////////////////////////////////////////////////////////////////////////////////
bool const& pmHysteron::get_state() const {
    return state;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the event of the hysteron.
/////////////////////////////////////////////////////////////////////////////////////////
Hysteron_event const& pmHysteron::get_event() const {
    return event;
}
