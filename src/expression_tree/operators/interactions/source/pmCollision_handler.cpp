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

#include "pmCollision_handler.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Implementaton of << operator.
/////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, pmCollision_handler const* obj) {
	obj->write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmCollision_handler::pmCollision_handler(std::array<std::shared_ptr<pmExpression>,3> op) {
	this->operand = std::move(op);
	this->op_name = "collision_handler";
	count.resize(depth);
	pairs.resize(depth);
	for(auto& it:pairs) {
		it.add_data("initial_length");
		it.add_data("alpha");
		it.add_data("beta");
		it.add_data("state");
		it.add_data("event");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmCollision_handler::pmCollision_handler(pmCollision_handler const& other) {
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
		this->count = other.count;
	}
	this->op_name = other.op_name;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmCollision_handler::pmCollision_handler(pmCollision_handler&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
	this->op_name = std::move(other.op_name);
	this->count = std::move(other.count);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmCollision_handler& pmCollision_handler::operator=(pmCollision_handler const& other) {
	if(this!=&other) {
		this->assigned = false;
		for(int i=0; i<this->operand.size(); i++) {
			this->operand[i] = other.operand[i]->clone();
			this->count = other.count;
		}
		this->op_name = other.op_name;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmCollision_handler& pmCollision_handler::operator=(pmCollision_handler&& other) {
	if(this!=&other) {
		this->psys = std::move(other.psys);
		this->assigned = std::move(other.assigned);
		this->operand = std::move(other.operand);
		this->op_name = std::move(other.op_name);
		this->count = std::move(other.count);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmCollision_handler::clone_impl() const {
	return std::make_shared<pmCollision_handler>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmCollision_handler> pmCollision_handler::clone() const {
	return std::static_pointer_cast<pmCollision_handler, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints DEM content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCollision_handler::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>(this->op_name.c_str());
	this->print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmCollision_handler::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!this->assigned) { ProLog::pLogger::error_msgf("Collision counter is not assigned to any particle system.\n"); }
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
		return pmTensor{1,1,(double)(i==j ? count[level][i] : 0)};
	};
	return this->interact(i, contribute);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Delete pairs based on the deletion condition.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCollision_handler::remove_unnecessary_pairs(size_t const& level/*=0*/) {
	if(!this->assigned) { ProLog::pLogger::error_msgf("Collision counter is not assigned to any particle system.\n"); }
	auto condition = [&](pmTensor const& rel_pos, int const& i, int const& j)->bool{
		double Ri = this->operand[0]->evaluate(i,level)[0];
		double condition_i = this->operand[2]->evaluate(i,level)[0];
		double Rj = this->operand[0]->evaluate(j,level)[0];
		double condition_j = this->operand[2]->evaluate(j,level)[0];
		double d_ji = rel_pos.norm();
		double min_dist = Ri + Rj;
		return (d_ji > min_dist || !condition_i || !condition_j);
	};
	this->delete_pairs(condition, level);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generate pairs if the pairing condition is met.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCollision_handler::create_pairs(int const& i, size_t const& level/*=0*/) {
	if(!this->assigned) { ProLog::pLogger::error_msgf("Collision counter is not assigned to any particle system.\n"); }
	double Ri = this->operand[0]->evaluate(i,level)[0];
	double condition_i = this->operand[2]->evaluate(i,level)[0];
	std::vector<size_t> const& pair_index_i = this->pairs[level].get_pair_index(i);
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
		for(auto const& it:pair_index_i) {
			if(this->pairs[level].get_first()[it]==j || this->pairs[level].get_second()[it]==j) {
				return pmTensor{1,1,0};
			}
		}
		double d_ji = rel_pos.norm();
		if(d_ji > NAUTICLE_EPS && i<j) {
			double Rj = this->operand[0]->evaluate(j,level)[0];
			double condition_j = this->operand[2]->evaluate(j,level)[0];
			double min_dist = Ri + Rj;
			if(d_ji < min_dist && condition_i && condition_j) {
				std::vector<double> data = {d_ji, 0.0, min_dist/100.0, 0.0, 0.0};
				this->pairs[level].add_pair(i,j,data);
			}
		}
		return pmTensor{1,1,0};
	};
	pmTensor tensor = this->interact(i, contribute);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluate the pairs.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCollision_handler::evaluate_pairs(size_t const& level/*=0*/) {
	if(!this->assigned) { ProLog::pLogger::error_msgf("Collision counter is not assigned to any particle system.\n"); }
	auto first = this->pairs[level].get_first();
	auto second = this->pairs[level].get_second();
	std::vector<double> const& alpha = this->pairs[level].get_data("alpha");
	std::vector<double> const& beta = this->pairs[level].get_data("beta");
	std::vector<double>& event = this->pairs[level].get_data("event");
	std::vector<double>& state = this->pairs[level].get_data("state");
	for(int pi=0; pi<pairs[level].size(); pi++) {
		int i = first[pi];
		int j = second[pi];
		double Ri = this->operand[0]->evaluate(i,level)[0];
		double Rj = this->operand[0]->evaluate(j,level)[0];
		pmTensor pos_i = this->psys.lock()->get_value(i);
		pmTensor pos_j = this->psys.lock()->get_value(j);
		pmTensor rel_pos = pos_j-pos_i;
		double d_ji = rel_pos.norm();
		double min_dist = Ri + Rj;
		{
			double x = min_dist-d_ji;
			if(x<alpha[pi] && (bool)state[pi]) {
		        state[pi] = 0.0;
		        event[pi] = -1.0;
		    } else if(x>beta[pi] && !(bool)state[pi]) {
		        state[pi] = 1.0;
		        event[pi] = 1.0;
		    } else {
		        event[pi] = 0.0;
		    }
		}
		if(event[pi]>0.0) {
			double r = 0.2;
			double Ri_new = Ri-0.001*(std::pow(Ri,1+r)+std::pow(Rj,1-r))/(Ri+Rj);
			double Rj_new = Rj-0.001*(std::pow(Ri,1-r)+std::pow(Rj,1+r))/(Ri+Rj);
			std::dynamic_pointer_cast<pmField>(this->operand[0])->set_value(Ri_new,i);
			std::dynamic_pointer_cast<pmField>(this->operand[0])->set_value(Rj_new,j);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Count the collisions per particles.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCollision_handler::count_collisions(size_t const& level/*=0*/) const {
	count[level].resize(this->psys.lock()->get_field_size());
	std::fill(count[level].begin(), count[level].end(), 0);
	auto first = this->pairs[level].get_first();
	auto second = this->pairs[level].get_second();
	std::vector<double> const& event = this->pairs[level].get_data("event");
	for(int i=0; i<first.size(); i++) {
		count[level][first[i]] += event[i];
		count[level][second[i]] += event[i];
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Update the long range interaction.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCollision_handler::update(size_t const& level/*=0*/) {
	pmLong_range::update(psys.lock()->get_sorted_idx());
	remove_unnecessary_pairs(level);
	for(int i=0; i<psys.lock()->get_field_size(); i++) {
		create_pairs(i, level);
	}
	evaluate_pairs(level);
	count_collisions(level);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the list of the first particles' indices.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCollision_handler::set_storage_depth(size_t const& d) {
	pmLong_range::set_storage_depth(d);
	count.resize(depth);
}





