/*
    Copyright 2016-2019 Balazs Toth
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

#include "pmCollision_counter.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Implementaton of << operator.
/////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, pmCollision_counter const* obj) {
	obj->write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmCollision_counter::pmCollision_counter(std::array<std::shared_ptr<pmExpression>,3> op) {
	this->operand = std::move(op);
	this->op_name = "collision_counter";
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmCollision_counter::pmCollision_counter(pmCollision_counter const& other) {
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
	}
	this->op_name = other.op_name;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmCollision_counter::pmCollision_counter(pmCollision_counter&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
	this->op_name = std::move(other.op_name);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmCollision_counter& pmCollision_counter::operator=(pmCollision_counter const& other) {
	if(this!=&other) {
		this->assigned = false;
		for(int i=0; i<this->operand.size(); i++) {
			this->operand[i] = other.operand[i]->clone();
		}
		this->op_name = other.op_name;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmCollision_counter& pmCollision_counter::operator=(pmCollision_counter&& other) {
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
std::shared_ptr<pmExpression> pmCollision_counter::clone_impl() const {
	return std::make_shared<pmCollision_counter>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmCollision_counter> pmCollision_counter::clone() const {
	return std::static_pointer_cast<pmCollision_counter, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints DEM content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCollision_counter::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>(this->op_name.c_str());
	this->print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmCollision_counter::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!this->assigned) { ProLog::pLogger::error_msgf("Collision counter is not assigned to any particle system.\n"); }
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
		return pmTensor{1,1,(double)(i==j ? count[i] : 0)};
	};
	return this->interact(i, contribute);
}

void pmCollision_counter::update_collision_counter(int const& i, size_t const& level/*=0*/) {
	if(!this->assigned) { ProLog::pLogger::error_msgf("Collision counter is not assigned to any particle system.\n"); }
	double Ri = this->operand[0]->evaluate(i,level)[0];
	double condition_i = this->operand[2]->evaluate(i,level)[0];
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
		double d_ji = rel_pos.norm();
		if(d_ji > NAUTICLE_EPS && i<j) {
			double Rj = this->operand[0]->evaluate(j,level)[0];
			double condition_j = this->operand[2]->evaluate(j,level)[0];
			double min_dist = Ri + Rj;
			int pair_idx = this->pairs.get_pair_index(i,j);
			if(d_ji < min_dist && condition_i<NAUTICLE_EPS && condition_j<NAUTICLE_EPS) {
				if(pair_idx<0) {
					this->pairs.add_pair(i,j,d_ji);
					pair_idx = this->pairs.size()-1;
				}
				pmHysteron& hysteron = this->pairs.get_hysteron(pair_idx);
				hysteron.update(min_dist-d_ji);
			} else if(pair_idx!=-1) {
				this->pairs.delete_pair(pair_idx);
			}
		}
		return pmTensor{1,1,0};
	};
	pmTensor tensor = this->interact(i, contribute);
}

void pmCollision_counter::count_collisions() const {
	count.resize(this->psys.lock()->get_field_size());
	std::fill(count.begin(), count.end(), 0);
	auto first = this->pairs.get_first();
	auto second = this->pairs.get_second();
	auto hysteron = this->pairs.get_hysteron();
	for(int i=0; i<first.size(); i++) {
		int event = hysteron[i].get_event()==UP?1:0;
		count[first[i]] += event;
		count[second[i]] += event;
	}
}

void pmCollision_counter::update(size_t const& level/*=0*/) {
	auto ps = this->psys.lock();
	this->renumber_pairs(ps->get_sorted_idx());
	for(int i=0; i<ps->get_field_size(); i++) {
		update_collision_counter(i, level);
	}
	count_collisions();
}








