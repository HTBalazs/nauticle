/*
    Copyright 2016-2017 Balazs Toth
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

    For more information please visit: https://bitbucket.org/lempsproject/
*/

#include "pmNbody.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmNbody::pmNbody(std::array<std::shared_ptr<pmExpression>,2> op) {
	operand = std::move(op);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmNbody::pmNbody(pmNbody const& other) {
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmNbody::pmNbody(pmNbody&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmNbody& pmNbody::operator=(pmNbody const& other) {
	if(this!=&other) {
		this->assigned = false;
		for(int i=0; i<this->operand.size(); i++) {
			this->operand[i] = other.operand[i]->clone();
		}
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmNbody& pmNbody::operator=(pmNbody&& other) {
	if(this!=&other) {
		this->psys = std::move(other.psys);
		this->assigned = std::move(other.assigned);
		this->operand = std::move(other.operand);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmNbody::clone_impl() const {
	return std::make_shared<pmNbody>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmNbody> pmNbody::clone() const {
	return std::static_pointer_cast<pmNbody, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints N-body content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmNbody::print() const {
	pLogger::logf<COLOR>("nbody");
	print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmNbody::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!assigned) { pLogger::error_msgf("N-body model is not assigned to any particle system.\n"); }
	std::shared_ptr<pmParticle_system> ps = psys.lock();

	pmTensor pos_i = ps->evaluate(i,level);
	pmTensor mass_i = operand[0]->evaluate(i,level);
	pmTensor coef_i = operand[1]->evaluate(i,level);

	pmTensor force;
	for(int j=0; j<ps->get_field_size(); j++) {
		if(i==j) { continue; }
		pmTensor mass_j = operand[0]->evaluate(j,level);
		pmTensor coef_j = operand[1]->evaluate(j,level);
		pmTensor pos_j = ps->evaluate(j,level);
		pmTensor rel_pos = pos_j-pos_i;
		double distance = rel_pos.norm();
		pmTensor norm = rel_pos / distance;
		force += (coef_i+coef_j)/2*mass_i*mass_j/distance/distance*norm;
	}
	return force;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes object to string.
/////////////////////////////////////////////////////////////////////////////////////////
void pmNbody::write_to_string(std::ostream& os) const {
	os << "nbody(" << operand[0] << "," << operand[1] << ")";
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the maximum size of the fields out of the operands.
/////////////////////////////////////////////////////////////////////////////////////////
int pmNbody::get_field_size() const {
	return psys.lock()->get_field_size();
}
