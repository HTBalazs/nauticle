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
    
#include "pmParticle_system.h"
#include <numeric>
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
//////////																		/////////
//////////							pmParticle_system							/////////
//////////																		/////////
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints particle system content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>("{%s}", "r");
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmParticle_system::clone_impl() const {
	return std::make_shared<pmParticle_system>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_system> pmParticle_system::clone() const {
	return std::static_pointer_cast<pmParticle_system, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns if the object is the position.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::is_position() const {
    return true;
}
