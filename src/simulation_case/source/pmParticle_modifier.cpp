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

#include "pmParticle_modifier.h"

using namespace Nauticle;

std::vector<size_t> pmParticle_modifier::get_candidates() const {
	std::vector<size_t> indices;
    for(int i=0; i<workspace->get_number_of_nodes(); i++) {
        bool split = condition->evaluate(i,0)[0];
        if(split) {
            indices.push_back(i);
        }
    }
    return indices;
}
void pmParticle_modifier::set_condition(std::shared_ptr<pmExpression> cdn) {
    condition = cdn;
}
void pmParticle_modifier::set_radius(std::shared_ptr<pmField> rad) {
    radius = rad;
}
void pmParticle_modifier::set_mass(std::shared_ptr<pmField> ms) {
    mass = ms;
}
void pmParticle_modifier::set_workspace(std::shared_ptr<pmWorkspace> ws) {
	workspace = ws;
}