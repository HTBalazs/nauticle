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

#include "pmParticle_modifier.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the condition object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_modifier::set_condition(std::shared_ptr<pmExpression> cdn) {
    condition = cdn;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the condition object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> const& pmParticle_modifier::get_condition() const {
    return condition;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the workspace object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_modifier::set_workspace(std::shared_ptr<pmWorkspace> ws) {
	workspace = ws;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the mass object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_modifier::print() const {
    static bool print_header = true;
    if(print_header) {
        ProLog::pLogger::headerf<ProLog::LBL>("Particle modifiers:");
        print_header = false;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a copy of the objecct.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_modifier> pmParticle_modifier::clone() const {
    return clone_impl();
}