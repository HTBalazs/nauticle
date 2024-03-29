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

#include "pmVariable.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmVariable::pmVariable(std::string const& n, pmTensor const& v/*=pmTensor{0}*/) {
	name = n;
	value = v;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implement identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmVariable::operator==(pmVariable const& rhs) const {
    if(this->name != rhs.name || this->value != rhs.value || this->depth != rhs.depth) {
        return false;
    } else {
        return true;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements the non-identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmVariable::operator!=(pmVariable const& rhs) const {
    return !this->operator==(rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set the number of the previous instants to store.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVariable::set_storage_depth(size_t const& d) {
    depth = d;
    value.set_storage_depth(depth);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the stored data based on the evaluation type given as the optional parameter.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmVariable::evaluate(int const& i, size_t const& level/*=0*/) const {
    return value[level];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the variable value. If the two_step option is on, the current value is copied
//  and stored in the previous value.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVariable::set_value(pmTensor const& v, int const& i/*=0*/, bool const& forced/*=false*/) {
	value = v;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmVariable::clone_impl() const {
	return std::make_shared<pmVariable>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmVariable> pmVariable::clone() const {
	return std::static_pointer_cast<pmVariable, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes object to string.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVariable::write_to_string(std::ostream& os) const {
	os << name;
}