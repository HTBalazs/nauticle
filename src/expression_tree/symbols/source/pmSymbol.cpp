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

#include "pmSymbol.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of nodes in the field.
/////////////////////////////////////////////////////////////////////////////////////////
int pmSymbol::get_field_size() const {
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the name of the field.
/////////////////////////////////////////////////////////////////////////////////////////
std::string pmSymbol::get_name() const {
	return name;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the content of the field object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSymbol::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>("%s", name.c_str());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmSymbol> pmSymbol::clone() const {
	return std::static_pointer_cast<pmSymbol, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the field is assigned. It returns true (see class description).
/////////////////////////////////////////////////////////////////////////////////////////
bool pmSymbol::is_assigned() const {
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns false. The pmSymbol object is visible by default.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmSymbol::is_hidden() const {
	return false;
}

