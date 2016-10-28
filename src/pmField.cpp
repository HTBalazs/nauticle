/////////////////////////////////////////////////////////////////////////////
// Name:        pmField.cpp
// Purpose:     pmField class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "pmField.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmField::pmField(std::string const& n, int const& size, pmTensor const& value/*=pmTensor{0}*/) {
	name = n;
	current_value.resize(size, value);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmField::pmField(std::string const& n, std::vector<pmTensor> const& value) {
	name = n;
	current_value = value;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmField::pmField(pmField const& other) {
	this->name = other.name;
	this->two_step = other.two_step;
	this->current_value = other.current_value;
	if(this->two_step) {
		this->previous_value = other.previous_value;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmField::pmField(pmField&& other) {
	this->name = std::move(other.name);
	this->two_step = std::move(other.two_step);
	this->current_value = std::move(other.current_value);
	if(this->two_step) {
		this->previous_value = std::move(other.previous_value);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmField& pmField::operator=(pmField const& other) {
	if(this!=&other) {
		this->name = other.name;
		this->two_step = other.two_step;
		this->current_value = other.current_value;
		if(this->two_step) {
			this->previous_value = other.previous_value;
		}
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmField& pmField::operator=(pmField&& other) {
	if(this!=&other) {
		this->name = std::move(other.name);
		this->two_step = std::move(other.two_step);
		this->current_value = std::move(other.current_value);
		if(this->two_step) {
			this->previous_value = std::move(other.previous_value);
		}
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the whole content of the field.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::printv() const {
	print();
	for(int i=0; i<20-name.size(); i++) {
		pLogger::logf<LMA>(" ");
	}
	pLogger::logf<LMA>("[%i by %i]", this->evaluate(0).get_numrows(), this->evaluate(0).get_numcols());
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the size of the field.
/////////////////////////////////////////////////////////////////////////////////////////
int pmField::get_field_size() const {
	return current_value.size();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Activates or deactivates the data regarding to the previous step.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::double_steps(bool const& use_double_steps) {
	if(use_double_steps) {
		two_step = true;
		previous_value = current_value;		
	} else {
		two_step = false;
		previous_value.resize(0);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the field.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmField::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	if(eval_type==current) {
		return current_value[i];
	} else {
		return previous_value[i];
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the value of the ith node.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::set_value(pmTensor const& value, int const& i/*=0*/) {
	if(two_step) {
		previous_value[i] = current_value[i];
	}
	current_value[i] = value;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the ith node.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmField::get_value(int const& i) const {
	return current_value[i];
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the field type.
/////////////////////////////////////////////////////////////////////////////////////////
std::string pmField::get_type() const {
    if(current_value[0].is_scalar()) { return "SCALAR"; }
    if(current_value[0].is_vector()) { return "VECTOR"; }
    return "TENSOR";
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmField::clone_impl() const {
	return std::make_shared<pmField>(*this);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a copy of the field.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmField> pmField::clone() const {
	return std::static_pointer_cast<pmField, pmExpression>(clone_impl());
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Sorts the field based on the given sorted index vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::sort_field(std::vector<int>& sorted_idx) {
	pmSort::reorder(current_value, sorted_idx);
	if(two_step) { pmSort::reorder(previous_value, sorted_idx); }
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Resizes the field. The last element is copied to the empty places if N>current size.
/// If N<current size, the elements above N are destroyed.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::set_number_of_nodes(size_t const& N) {
	if(N!=current_value.size()) {
		pmTensor ctensor = current_value.back();
		current_value.resize(N, ctensor);
		if(two_step) {
			pmTensor ptensor = previous_value.back();
			previous_value.resize(N, ptensor);
		}
	}
}

pmTensor pmField::interpolate(pmTensor const& position) const {
	
}

void pmField::write_to_string(std::ostream& os) const {
	os << name;
}



