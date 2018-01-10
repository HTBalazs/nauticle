/*
    Copyright 2016-2018 Balazs Toth
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

#include "pmNbody_operator.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmNbody_operator::pmNbody_operator(std::array<std::shared_ptr<pmExpression>,2> op) {
	operand = std::move(op);
	op_name = std::string{"nbody"};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmNbody_operator::pmNbody_operator(pmNbody_operator const& other) {
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
		this->op_name = other.op_name;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmNbody_operator::pmNbody_operator(pmNbody_operator&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
	this->op_name = std::move(other.op_name);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmNbody_operator& pmNbody_operator::operator=(pmNbody_operator const& other) {
	if(this!=&other) {
		this->assigned = false;
		for(int i=0; i<this->operand.size(); i++) {
			this->operand[i] = other.operand[i]->clone();
			this->op_name = other.op_name;
		}
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmNbody_operator& pmNbody_operator::operator=(pmNbody_operator&& other) {
	if(this!=&other) {
		this->psys = std::move(other.psys);
		this->assigned = std::move(other.assigned);
		this->operand = std::move(other.operand);
		this->op_name = std::move(other.op_name);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmNbody_operator::clone_impl() const {
	return std::make_shared<pmNbody_operator>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmNbody_operator> pmNbody_operator::clone() const {
	return std::static_pointer_cast<pmNbody_operator, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints N-body content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmNbody_operator::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>(op_name.c_str());
	print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmNbody_operator::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!assigned) { ProLog::pLogger::error_msgf("N-body model is not assigned to any particle system.\n"); }
	std::shared_ptr<pmParticle_system> ps = psys.lock();

	pmTensor pos_i = ps->evaluate(i,level);
	pmTensor mass_i = operand[0]->evaluate(i,level);
	pmTensor coef = operand[1]->evaluate(0,level);

	pmTensor force;
	for(int j=0; j<ps->get_field_size(); j++) {
		if(i==j) { continue; }
		pmTensor mass_j = operand[0]->evaluate(j,level);
		pmTensor pos_j = ps->evaluate(j,level);
		pmTensor rel_pos = pos_j-pos_i;
		double distance = rel_pos.norm();
		pmTensor norm = rel_pos / distance;
		force += coef*mass_i*mass_j/distance/distance*norm;
	}
	return force;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the maximum size of the fields out of the operands.
/////////////////////////////////////////////////////////////////////////////////////////
int pmNbody_operator::get_field_size() const {
	return psys.lock()->get_field_size();
}
