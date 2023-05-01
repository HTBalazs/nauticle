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

#include "pmSpring.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Implementaton of << operator.
/////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, pmSpring const* obj) {
	obj->write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmSpring::pmSpring(std::array<std::shared_ptr<pmExpression>,1> op) {
	this->operand = std::move(op);
	this->op_name = "spring";
	pairs.resize(1);
	for(auto& it:pairs) {
		it.add_data("initial_length");
		it.add_data("strength");
		it.add_data("damping");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmSpring::pmSpring(pmSpring const& other) {
	this->psys = other.psys;
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
		this->pairs[i] = other.pairs[i];
	}
	this->op_name = other.op_name;
	this->force = other.force;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmSpring::pmSpring(pmSpring&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
	this->op_name = std::move(other.op_name);
	this->pairs = std::move(other.pairs);
	this->force = std::move(other.force);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmSpring& pmSpring::operator=(pmSpring const& other) {
	if(this!=&other) {
		this->assigned = false;
		for(int i=0; i<this->operand.size(); i++) {
			this->operand[i] = other.operand[i]->clone();
			this->pairs[i] = other.pairs[i];
		}
		this->op_name = other.op_name;
		this->force = other.force;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmSpring& pmSpring::operator=(pmSpring&& other) {
	if(this!=&other) {
		this->psys = std::move(other.psys);
		this->assigned = std::move(other.assigned);
		this->operand = std::move(other.operand);
		this->op_name = std::move(other.op_name);
		this->pairs = std::move(other.pairs);
		this->force = std::move(other.force);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmSpring::clone_impl() const {
	return std::make_shared<pmSpring>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmSpring> pmSpring::clone() const {
	return std::static_pointer_cast<pmSpring, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints DEM content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSpring::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>(this->op_name.c_str());
	this->print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmSpring::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!this->assigned) { ProLog::pLogger::error_msgf("Spring interaction is not assigned to any particle system.\n"); }
	// auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
	// 	return force[i];
	// };
	// pmTensor t = this->interact(i, contribute);
	// return t;
	return force[i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Computes spring forces.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSpring::update(size_t const& level/*=0*/) {
	force.resize(this->psys->get_field_size(), pmTensor{(int)this->psys->get_dimensions(),1,0});
	std::fill(force.begin(), force.end(), pmTensor{(int)this->psys->get_dimensions(),1,0});
	this->set_number_of_nodes(this->psys->get_field_size());
	if(!this->assigned) { ProLog::pLogger::error_msgf("Spring interaction is not assigned to any particle system.\n"); }
	auto first = this->pairs[level].get_first();
	auto second = this->pairs[level].get_second();
	std::vector<double> const& initial_length = this->pairs[level].get_data("initial_length");
	std::vector<double> const& strength = this->pairs[level].get_data("strength");
	std::vector<double> const& damping = this->pairs[level].get_data("damping");
	for(int pi=0; pi<pairs[level].get_number_of_pairs(); pi++) {
		int i = first[pi];
		int j = second[pi];
		pmTensor pos_i = this->psys->get_value(i)+psys->get_periodic_shift(i);
		pmTensor pos_j = this->psys->get_value(j)+psys->get_periodic_shift(j);
		pmTensor rel_pos = pos_j-pos_i;
		double d_ji = rel_pos.norm();
		pmTensor vel_i = this->operand[0]->evaluate(i,level);
		pmTensor vel_j = this->operand[0]->evaluate(j,level);
		pmTensor rel_vel = vel_j-vel_i;
		pmTensor dir = rel_pos/d_ji;
		pmTensor fstr = dir*(d_ji-initial_length[pi])*strength[pi];
		pmTensor fdmp = -dir*(rel_vel.transpose()*dir)*damping[pi];
		force[i] += fstr-fdmp;
		force[j] -= fstr-fdmp;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the list of the first particles' indices.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSpring::set_storage_depth(size_t const& d) {
	pmLong_range::set_storage_depth(d);
	force.resize(d);
}


