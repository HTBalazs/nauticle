/*
    Copyright 2016 Balazs Toth
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

    For more information please visit: https://BalazsToth@bitbucket.org/BalazsToth/lemps
*/

#include "pmVariable.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmVariable::pmVariable(std::string const& n, pmTensor const& value/*=pmTensor{0}*/) {
	name = n;
	current_value = value;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the stored data based on the evaluation type given as the optional parameter.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmVariable::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	if(eval_type==current) {
		return current_value;
	} else {
		return previous_value;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the variable value. If the two_step option is on, the current value is copied
//  and stored in the previous value.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVariable::set_value(pmTensor const& value, int const& i/*=0*/) {
	previous_value = current_value;
	current_value = value;
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
void pmVariable::write_to_string(std::ostream& os) const {
	os << name;
}