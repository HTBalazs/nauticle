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

#include "pmParticle_resolve.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates and returns the listof potential candidates based on the condition expresion.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<size_t> pmParticle_resolve::get_candidates() const {
	std::vector<size_t> indices;
    for(int i=0; i<workspace->get_number_of_nodes(); i++) {
        if(condition->evaluate(i,0)[0]) {
            indices.push_back(i);
        }
    }
    return indices;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the radius object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_resolve::set_radius(std::shared_ptr<pmField> rad) {
    radius = rad;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the mass object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_resolve::set_mass(std::shared_ptr<pmField> ms) {
    mass = ms;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the radius object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmField> const& pmParticle_resolve::get_radius() const {
    return radius;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the mass object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmField> const& pmParticle_resolve::get_mass() const {
    return mass;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a copy of the objecct.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_resolve> pmParticle_resolve::clone() const {
    return std::static_pointer_cast<pmParticle_resolve, pmParticle_modifier>(clone_impl());
}