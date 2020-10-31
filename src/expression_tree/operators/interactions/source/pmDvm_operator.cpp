/*
    Copyright 2017 Balazs Toth
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

#include "pmDvm_operator.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements << operator.
/////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, pmDvm_operator const* obj) {
	obj->write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmDvm_operator::pmDvm_operator(std::array<std::shared_ptr<pmExpression>,2> op) {
	this->operand = std::move(op);
	size_t type = (int)this->operand[1]->evaluate(0)[0];
	op_name = std::string{"dvm"};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmDvm_operator::pmDvm_operator(pmDvm_operator const& other) {
	this->kernel = std::shared_ptr<pmKernel>(other.kernel);
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
	}
	this->op_name = other.op_name;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmDvm_operator::pmDvm_operator(pmDvm_operator&& other) {
	this->kernel = std::move(other.kernel);
	this->operand = std::move(other.operand);
	this->op_name = std::move(other.op_name);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmDvm_operator& pmDvm_operator::operator=(pmDvm_operator const& other) {
	if(this!=&other) {
		this->kernel = std::shared_ptr<pmKernel>(other.kernel);
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
pmDvm_operator& pmDvm_operator::operator=(pmDvm_operator&& other) {
	if(this!=&other) {
		this->kernel = std::move(other.kernel);
		this->operand = std::move(other.operand);
		this->op_name = std::move(other.op_name);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmDvm_operator::clone_impl() const {
	return std::make_shared<pmDvm_operator>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmDvm_operator> pmDvm_operator::clone() const {
	return std::static_pointer_cast<pmDvm_operator, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints SPH operator content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDvm_operator::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>(op_name.c_str());
	this->print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator for the ith node.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDvm_operator::evaluate(int const& i, size_t const& level/*=0*/) const {
	size_t dimension = this->workspace->get_dimensions();
	double cell_size_min = this->workspace->get_cell_size().min();
	double eps = this->operand[1]->evaluate(i,level)[0];
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size)->pmTensor{
		pmTensor w_j = this->operand[0]->evaluate(j,level);
		pmTensor contribution{(int)dimension,1,0.0};
		if(w_j.norm() == 0) { return contribution; }
		double d_ji = rel_pos.norm();
		if(d_ji > NAUTICLE_EPS && d_ji < cell_size_min) {
			double d_ji2 = d_ji*d_ji;
			double temp = 0.5/d_ji2/NAUTICLE_PI*(1.0-std::exp(-d_ji2/eps/eps));
			if(dimension==2) {
				contribution[0] -= w_j[0]*rel_pos[1]*temp;
				contribution[1] += w_j[0]*rel_pos[0]*temp;
			} else if(dimension==3) {
				contribution += cross(w_j,rel_pos)*temp;
			}
		}
		return contribution;
	};
	return this->interact(i, contribute);
}









