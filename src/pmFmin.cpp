/////////////////////////////////////////////////////////////////////////////
// Name:        pmFmin.h
// Purpose:     pmFmin class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "pmFmin.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmin::pmFmin(std::shared_ptr<pmExpression> op0) {
	operand[0] = op0;
}
pmFmin::pmFmin(pmFmin const& other) {
	this->operand[0] = other.operand[0]->clone();
}
pmFmin::pmFmin(pmFmin&& other) {
	this->operand = std::move(other.operand);
}
pmFmin& pmFmin::operator=(pmFmin const& other) {
	if(this!=&other) {
		this->operand[0] = other.operand[0]->clone();
	}
	return *this;
}
pmFmin& pmFmin::operator=(pmFmin&& other) {
	if(this!=&other) {
		this->operand = std::move(other.operand);
	}
	return *this;
}
std::shared_ptr<pmExpression> pmFmin::clone_impl() const {
	return std::make_shared<pmFmin>(*this);
}
std::shared_ptr<pmFmin> pmFmin::clone() const {
	return std::static_pointer_cast<pmFmin, pmExpression>(clone_impl());
}
void pmFmin::print() const {
	pLogger::logf<COLOR>("fmin(");
	operand[0]->print();
	pLogger::logf<COLOR>(")");
}
pmTensor pmFmin::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	static pmTensor min_value{1,1};
	if(i==0) {
		pmTensor tensor = operand[0]->evaluate(i, eval_type);
		if(!tensor.is_scalar()) {
			pLogger::error_msgf("Fmin can be evaluated only on scalar fields!\n");
		}
		min_value = operand[0]->evaluate(0, eval_type);
		for(int j=1; j<operand[0]->get_field_size(); j++) {
			pmTensor tj = operand[0]->evaluate(j, eval_type);
			if(min_value(0,0)>tj(0,0)) {
				min_value = tj;
			}
		}
	}
	return min_value;
}
int pmFmin::get_field_size() const {
	return 1;
}
bool pmFmin::is_assigned() const {
	return true;
}
void pmFmin::write_to_string(std::ostream& os) const {
	os << "fmin(" << operand[0] << ")";
}


