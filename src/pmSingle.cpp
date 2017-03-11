/*
    Copyright 2016-2017 Balazs Toth
    This file is part of LEMPS.

    LEMPS is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LEMPS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LEMPS.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://bitbucket.org/lempsproject/
*/

#include "pmSingle.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value stored for constant or variable.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmSingle::get_value(int const& i/*=0*/) const {
	return current_value;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Preints the content with name and value.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSingle::printv() const {
	this->print();
	pLogger::logf<COLOR>(" = ");
	current_value.print();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the single-valued constant or variable.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmSingle::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	return current_value;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmSingle> pmSingle::clone() const {
	return std::static_pointer_cast<pmSingle, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the type in pmSingle format. It returns on of the followings: SCALAR, VECTOR, TENSOR. 
/////////////////////////////////////////////////////////////////////////////////////////
std::string pmSingle::get_type() const {
    if(current_value.is_scalar()) { return "SCALAR"; }
    if(current_value.is_vector()) { return "VECTOR"; }
    return "TENSOR";
}


