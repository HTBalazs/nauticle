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
void pmFmax::process(pmTensor& value, Eval_type eval_type/*=current*/) const {
	pmTensor tensor = operand[0]->evaluate(0, eval_type);
	if(!tensor.is_scalar()) {
		pLogger::error_msgf("Fmax can be evaluated only on scalar fields!\n");
	}
	value = operand[0]->evaluate(0, eval_type);
	for(int j=1; j<operand[0]->get_field_size(); j++) {
		pmTensor tj = operand[0]->evaluate(j, eval_type);
		if(value(0,0)<tj(0,0)) {
			value = tj;
		}
	}
}
void pmFmax::write_to_string(std::ostream& os) const {
	os << "fmax(" << operand[0] << ")";
}