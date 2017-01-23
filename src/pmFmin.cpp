/*
    Copyright 2017 Balazs Toth
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
void pmFmin::process(pmTensor& value, Eval_type eval_type/*=current*/) const {
	pmTensor tensor = operand[0]->evaluate(0, eval_type);
	if(!tensor.is_scalar()) {
		pLogger::error_msgf("Fmin can be evaluated only on scalar fields!\n");
	}
	value = operand[0]->evaluate(0, eval_type);
	for(int j=1; j<operand[0]->get_field_size(); j++) {
		pmTensor tj = operand[0]->evaluate(j, eval_type);
		if(value(0,0)>tj(0,0)) {
			value = tj;
		}
	}
}
void pmFmin::write_to_string(std::ostream& os) const {
	os << "fmin(" << operand[0] << ")";
}


