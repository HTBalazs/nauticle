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

    For more information please visit: https://bitbucket.org/lempsproject/
*/
    
#include "pmDem.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmDem::pmDem(std::array<std::shared_ptr<pmExpression>,6> op) {
	operand = std::move(op);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmDem::pmDem(pmDem const& other) {
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmDem::pmDem(pmDem&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmDem& pmDem::operator=(pmDem const& other) {
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
pmDem& pmDem::operator=(pmDem&& other) {
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
std::shared_ptr<pmExpression> pmDem::clone_impl() const {
	return std::make_shared<pmDem>(*this);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmDem> pmDem::clone() const {
	return std::static_pointer_cast<pmDem, pmExpression>(clone_impl());
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints DEM content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDem::print() const {
	pLogger::logf<COLOR>("dem");
	print_operands();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDem::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	if(!assigned) { pLogger::error_msgf("DEM model is not assigned to any particle system.\n"); }

	pmTensor vel_i = operand[0]->evaluate(i,eval_type);
	float radius_i = operand[1]->evaluate(i,eval_type)[0];
	float mass_i = operand[2]->evaluate(i,eval_type)[0];
	float spring_i = operand[3]->evaluate(i,eval_type)[0];
	float damping_i = operand[4]->evaluate(i,eval_type)[0];
	float shear_i = operand[5]->evaluate(i,eval_type)[0];

	auto contribute = [&vel_i, &radius_i, &mass_i, &spring_i, &damping_i, &shear_i, &eval_type, this](pmTensor const& rel_pos, int const& i, int const& j, float const& cell_size, pmTensor const& guide)->pmTensor{
		pmTensor force;
		float d_ji = rel_pos.norm();
		if(d_ji > 1e-6f) {
			float radius_j = operand[1]->evaluate(j,eval_type)[0];
			float min_dist = radius_i + radius_j;
			if(d_ji < min_dist) {
				float mass_j = operand[2]->evaluate(j,eval_type)[0];
				float spring_j = operand[3]->evaluate(j,eval_type)[0];
				float damping_j = operand[4]->evaluate(j,eval_type)[0];
				float shear_j = operand[5]->evaluate(j,eval_type)[0];
				pmTensor norm = rel_pos / d_ji;
				pmTensor vel_j = operand[0]->evaluate(j,eval_type).reflect(guide);
				pmTensor rel_vel = vel_j-vel_i;
				// relative tangential velocity
				pmTensor tan_vel = rel_vel - (rel_vel.transpose()*norm) * norm;
				// spring force
				force += -(spring_i*mass_i+spring_j*mass_j)/2*(min_dist-d_ji)*norm;
				// dashpot (damping) force
				force += (damping_i*mass_i+damping_j*mass_j)/2*rel_vel;
				// tangential shear force
				force += (shear_i*mass_i+shear_j*mass_j)/2*tan_vel;
			}
		}
		return force;
	};
	return interact(i, operand, contribute);
}
void pmDem::write_to_string(std::ostream& os) const {
	os << "dem(";
	for(auto const& it:operand) {
		os << it;
	}
	os << ")";
}


