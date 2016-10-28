/////////////////////////////////////////////////////////////////////////////
// Name:        pmConstant.cpp
// Purpose:     pmConstant class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "pmConstant.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor. It creates an anonymous constant object.
/////////////////////////////////////////////////////////////////////////////////////////
pmConstant::pmConstant(pmTensor const& value) {
	current_value = value;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor. It creates a named constant object.
/////////////////////////////////////////////////////////////////////////////////////////
pmConstant::pmConstant(std::string n, pmTensor const& value, bool const& hh/*=false*/) {
	name = n;
	current_value = value;
	hidden = hh;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the name of the object if it is named and the value itself if it is anonymous.
/////////////////////////////////////////////////////////////////////////////////////////
void pmConstant::print() const {
	if(name=="") {
		current_value.print();
	} else {
		pLogger::logf<COLOR>("%s", name.c_str());
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
/// Since the object is constant, this member does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void pmConstant::set_value(pmTensor const& value, int const& i/*=0*/) {}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the constant object is hidden.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmConstant::is_hidden() const {
	return hidden;
}

void pmConstant::write_to_string(std::ostream& os) const {
	if(name=="") {
		os << current_value;
	} else {
		os << name;
	}
}

