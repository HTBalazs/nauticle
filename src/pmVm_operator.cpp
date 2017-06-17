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

#include "pmVm_operator.h"

void pmVm_operator::write_to_string(std::ostream& os) const {
	os << op_name << "(";
	for(int i=0; i<3; i++) {
		os << this->operand[i];
		if(i!=2) {
			os << ",";
		}
	}
	os << ")";
}

std::ostream& operator<<(std::ostream& os, pmVm_operator const* obj) {
	obj->write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmVm_operator::pmVm_operator(std::array<std::shared_ptr<pmExpression>,3> op) {
	this->operand = std::move(op);
	size_t type = (int)this->operand[1]->evaluate(0)[0];
	this->kernel = std::make_shared<pmKernel>();
	this->kernel->set_kernel_type(type, false);
	op_name = std::string{"vm"};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmVm_operator::pmVm_operator(pmVm_operator const& other) {
	this->assigned = false;
	this->kernel = std::shared_ptr<pmKernel>(other.kernel);
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
	}
	this->op_name = other.op_name;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmVm_operator::pmVm_operator(pmVm_operator&& other) {
	this->psys = std::move(other.psys);
	this->kernel = std::move(other.kernel);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
	this->op_name = std::move(other.op_name);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmVm_operator& pmVm_operator::operator=(pmVm_operator const& other) {
	if(this!=&other) {
		this->assigned = false;
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
pmVm_operator& pmVm_operator::operator=(pmVm_operator&& other) {
	if(this!=&other) {
		this->psys = std::move(other.psys);
		this->kernel = std::move(other.kernel);
		this->assigned = std::move(other.assigned);
		this->operand = std::move(other.operand);
		this->op_name = std::move(other.op_name);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmVm_operator::clone_impl() const {
	return std::make_shared<pmVm_operator>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmVm_operator> pmVm_operator::clone() const {
	return std::static_pointer_cast<pmVm_operator, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints SPH operator content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVm_operator::print() const {
	pLogger::logf<COLOR>(op_name.c_str());
	this->print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator for the ith node.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmVm_operator::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!this->assigned) { pLogger::error_msgf("\"%s\" is not assigned to any particle system.\n", op_name.c_str()); }
	size_t dimension = this->psys.lock()->get_particle_space()->get_domain().get_dimensions();
	double eps_i = this->operand[2]->evaluate(i,level)[0];
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, double const& cell_size, pmTensor const& guide)->pmTensor{
		pmTensor contribution;
		double d_ji = rel_pos.norm();
		if(d_ji > NAUTICLE_EPS) {
			double eps_j = this->operand[2]->evaluate(j,level)[0];
			if(d_ji < eps_i || d_ji < eps_j) {
				pmTensor w_j = this->operand[0]->evaluate(j,level).reflect(guide);
				double W_ij = this->kernel->evaluate(d_ji, (eps_i+eps_j)/2.0f);
				if(dimension==2) {
					pmTensor wj{3,1,0};
					wj[2] = w_j[0];
					contribution += cross(wj,rel_pos.append(3,1)).sub_tensor(0,1,0,0)/d_ji/d_ji*W_ij;
				} else if(dimension==3) {
					contribution += cross(w_j,rel_pos)/d_ji/d_ji*W_ij;
				}
			}
		}
		return contribution;
	};
	return this->interact(i, contribute);
}