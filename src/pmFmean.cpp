/////////////////////////////////////////////////////////////////////////////
// Name:        pmFmean.h
// Purpose:     pmFmean class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "pmFmean.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmean::pmFmean(std::shared_ptr<pmExpression> op0) {
	operand[0] = op0;
}
pmFmean::pmFmean(pmFmean const& other) {
	this->operand[0] = other.operand[0]->clone();
}
pmFmean::pmFmean(pmFmean&& other) {
	this->operand = std::move(other.operand);
}
pmFmean& pmFmean::operator=(pmFmean const& other) {
	if(this!=&other) {
		this->operand[0] = other.operand[0]->clone();
	}
	return *this;
}
pmFmean& pmFmean::operator=(pmFmean&& other) {
	if(this!=&other) {
		this->operand = std::move(other.operand);
	}
	return *this;
}
std::shared_ptr<pmExpression> pmFmean::clone_impl() const {
	return std::make_shared<pmFmean>(*this);
}
std::shared_ptr<pmFmean> pmFmean::clone() const {
	return std::static_pointer_cast<pmFmean, pmExpression>(clone_impl());
}
void pmFmean::print() const {
	pLogger::logf<COLOR>("fmean(");
	operand[0]->print();
	pLogger::logf<COLOR>(")");
}
pmTensor pmFmean::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	static pmTensor mean_value{1,1,0};
	if(i!=0) return mean_value;

	mean_value = operand[0]->evaluate(0, eval_type);
	int num_points = operand[0]->get_field_size();
	for(int j=1; j<num_points; j++) {
		mean_value += operand[0]->evaluate(j, eval_type);
	}
	mean_value/=pmTensor{1,1,(float)num_points};

	return mean_value;
}
int pmFmean::get_field_size() const {
	return 1;
}
bool pmFmean::is_assigned() const {
	return true;
}
void pmFmean::write_to_string(std::ostream& os) const {
	os << "fmean(" << operand[0] << ")";
}

