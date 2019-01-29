/*
    Copyright 2016-2019 Balazs Toth
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
    
#include "pmExpression.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmExpression::clone() const {
	return clone_impl();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns if the object is symmetric.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmExpression::is_symmetric() const {
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns if the object is the position.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmExpression::is_position() const {
    return false;
}

std::string const& pmExpression::get_name() const {
    return name;
}