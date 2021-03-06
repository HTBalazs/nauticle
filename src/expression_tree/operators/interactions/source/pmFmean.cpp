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
    
#include "pmFmean.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmean::pmFmean(std::shared_ptr<pmExpression> op0) {
	operand[0] = op0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmean::pmFmean(pmFmean const& other) {
	this->operand[0] = other.operand[0]->clone();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmean::pmFmean(pmFmean&& other) {
	this->operand = std::move(other.operand);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmean& pmFmean::operator=(pmFmean const& other) {
	if(this!=&other) {
		this->operand[0] = other.operand[0]->clone();
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmean& pmFmean::operator=(pmFmean&& other) {
	if(this!=&other) {
		this->operand = std::move(other.operand);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements clone.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmFmean::clone_impl() const {
	return std::make_shared<pmFmean>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmFmean> pmFmean::clone() const {
	return std::static_pointer_cast<pmFmean, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFmean::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>("fmean(");
	operand[0]->print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>(")");
}

/////////////////////////////////////////////////////////////////////////////////////////
/// This function performs the mean calculation called by the member function evaluate(...)
/// inherited from pmFsearch.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFmean::process(pmTensor& value, size_t const& level/*=0*/) const {
	value = operand[0]->evaluate(0, level);
	int num_points = operand[0]->get_field_size();
	for(int j=1; j<num_points; j++) {
		value += operand[0]->evaluate(j, level);
	}
	value/=pmTensor{1,1,(double)num_points};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes object to string.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFmean::write_to_string(std::ostream& os) const {
	os << "fmean(" << operand[0] << ")";
}

