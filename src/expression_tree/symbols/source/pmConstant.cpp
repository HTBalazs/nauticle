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

#include "pmConstant.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor. It creates an anonymous constant object.
/////////////////////////////////////////////////////////////////////////////////////////
pmConstant::pmConstant(pmTensor const& v) {
	value = v;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor. It creates a named constant object.
/////////////////////////////////////////////////////////////////////////////////////////
pmConstant::pmConstant(std::string n, pmTensor const& v, bool const& hh/*=false*/) {
	name = n;
	value = v;
	hidden = hh;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implement identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmConstant::operator==(pmConstant const& rhs) const {
	if((this->name != rhs.name) || (this->value[0][0] != rhs.value[0][0]) || (this->hidden != rhs.hidden)) {
		return false;
	} else {
		return true;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements the non-identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmConstant::operator!=(pmConstant const& rhs) const {
	return !this->operator==(rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the single-valued constant or variable.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmConstant::evaluate(int const& i, size_t const& level/*=0*/) const {
	return value[0];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the name of the object if it is named and the value itself if it is anonymous.
/////////////////////////////////////////////////////////////////////////////////////////
void pmConstant::print() const {
	if(name=="") {
		value[0].print();
	} else {
		ProLog::pLogger::logf<NAUTICLE_COLOR>("%s", name.c_str());
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmConstant::clone_impl() const {
	return std::make_shared<pmConstant>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmConstant> pmConstant::clone() const {
	return std::static_pointer_cast<pmConstant, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the constant object is hidden.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmConstant::is_hidden() const {
	return hidden;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes pmConstant to string. If pmConstant is named, the name is written.
/////////////////////////////////////////////////////////////////////////////////////////
void pmConstant::write_to_string(std::ostream& os) const {
	if(name=="") {
		os << value[0];
	} else {
		os << name;
	}
}

#ifdef JELLYFISH
std::string pmConstant::get_cpp_name() const {
	if(this->name=="") {
		return value[0].get_cpp_initialization();
	}
	return (this->hidden?"h":"")+this->prefix + "c_" + this->name;
}

c2c::c2CPP_type pmConstant::get_cpp_type() const {
	c2c::c2CPP_type type = value[0].get_cpp_type();
	type.set_constant(true);
	return type;
}

std::string pmConstant::get_cpp_evaluation(std::string const& idx, size_t const& level/*=0*/) const {
    return this->get_cpp_name();
}

#endif // JELLYFISH
