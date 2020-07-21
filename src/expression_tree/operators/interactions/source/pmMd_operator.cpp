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

#include "pmMd_operator.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements << operator.
/////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, pmMd_operator const* obj) {
	obj->write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmMd_operator::pmMd_operator(std::array<std::shared_ptr<pmExpression>,3> op) {
	this->operand = std::move(op);
	this->op_name = std::string{"md"};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmMd_operator::pmMd_operator(pmMd_operator const& other) {
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
	}
	this->op_name = other.op_name;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmMd_operator::pmMd_operator(pmMd_operator&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
	this->op_name = std::move(other.op_name);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmMd_operator& pmMd_operator::operator=(pmMd_operator const& other) {
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
pmMd_operator& pmMd_operator::operator=(pmMd_operator&& other) {
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
std::shared_ptr<pmExpression> pmMd_operator::clone_impl() const {
	return std::make_shared<pmMd_operator>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmMd_operator> pmMd_operator::clone() const {
	return std::static_pointer_cast<pmMd_operator, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints SPH operator content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmMd_operator::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>(op_name.c_str());
	this->print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator for the ith node.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmMd_operator::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!this->assigned) { ProLog::pLogger::error_msgf("\"%s\" is not assigned to any particle system.\n", op_name.c_str()); }
	size_t dimension = this->psys.lock()->get_particle_space()->get_domain().get_dimensions();
	double eps = this->operand[0]->evaluate(i,level)[0];
	double sigma = this->operand[1]->evaluate(i,level)[0];
	double R = this->operand[2]->evaluate(i,level)[0];
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
		pmTensor contribution{(int)dimension,1,0.0};
		double d_ji = rel_pos.norm();
		if(d_ji > NAUTICLE_EPS && d_ji < R) {
			double sd6 = std::pow(sigma/d_ji,6);
			contribution -= 48.0*eps/d_ji/d_ji*(sd6*sd6-0.5*sd6)*rel_pos;
		}
		return contribution;
	};
	return this->interact(i, contribute);
}

























