/////////////////////////////////////////////////////////////////////////////
// Name:        pmFmax.h
// Purpose:     pmFmax class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "pmFmax.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmax::pmFmax(std::shared_ptr<pmExpression> op0) {
	operand[0] = op0;
}
pmFmax::pmFmax(pmFmax const& other) {
	this->operand[0] = other.operand[0]->clone();
}
pmFmax::pmFmax(pmFmax&& other) {
	this->operand = std::move(other.operand);
}
pmFmax& pmFmax::operator=(pmFmax const& other) {
	if(this!=&other) {
		this->operand[0] = other.operand[0]->clone();
	}
	return *this;
}
pmFmax& pmFmax::operator=(pmFmax&& other) {
	if(this!=&other) {
		this->operand = std::move(other.operand);
	}
	return *this;
}
std::shared_ptr<pmExpression> pmFmax::clone_impl() const {
	return std::make_shared<pmFmax>(*this);
}
std::shared_ptr<pmFmax> pmFmax::clone() const {
	return std::static_pointer_cast<pmFmax, pmExpression>(clone_impl());
}
void pmFmax::print() const {
	pLogger::logf<COLOR>("fmax(");
	operand[0]->print();
	pLogger::logf<COLOR>(")");
}
pmTensor pmFmax::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	static pmTensor max_value{1,1};
	if(i==0) {
		pmTensor tensor = operand[0]->evaluate(i);
		if(!tensor.is_scalar()) {
			pLogger::error_msgf("Fmax can be evaluated only on scalar fields!\n");
		}
		max_value = operand[0]->evaluate(0, eval_type);
		for(int j=1; j<operand[0]->get_field_size(); j++) {
			pmTensor tj = operand[0]->evaluate(j, eval_type);
			if(max_value(0,0)<tj(0,0)) {
				max_value = tj;
			}
		}
	}
	return max_value;
}
int pmFmax::get_field_size() const {
	return 1;
}
bool pmFmax::is_assigned() const {
	return true;
}
void pmFmax::write_to_string(std::ostream& os) const {
	os << "fmax(" << operand[0] << ")";
}

