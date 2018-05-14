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

#include "pmDpd_operator.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements << operator.
/////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, pmDpd_operator const* obj) {
	obj->write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmDpd_operator::pmDpd_operator(std::array<std::shared_ptr<pmExpression>,7> op) {
	this->operand = std::move(op);
	size_t type = (int)this->operand[6]->evaluate(0)[0];
	this->kernel = std::make_shared<pmKernel>();
	this->kernel->set_kernel_type(type, false);
	this->op_name = std::string{"dpd"};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmDpd_operator::pmDpd_operator(pmDpd_operator const& other) {
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
pmDpd_operator::pmDpd_operator(pmDpd_operator&& other) {
	this->psys = std::move(other.psys);
	this->kernel = std::move(other.kernel);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
	this->op_name = std::move(other.op_name);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmDpd_operator& pmDpd_operator::operator=(pmDpd_operator const& other) {
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
pmDpd_operator& pmDpd_operator::operator=(pmDpd_operator&& other) {
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
std::shared_ptr<pmExpression> pmDpd_operator::clone_impl() const {
	return std::make_shared<pmDpd_operator>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmDpd_operator> pmDpd_operator::clone() const {
	return std::static_pointer_cast<pmDpd_operator, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints SPH operator content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDpd_operator::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>(op_name.c_str());
	this->print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator for the ith node.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDpd_operator::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!this->assigned) { ProLog::pLogger::error_msgf("\"%s\" is not assigned to any particle system.\n", op_name.c_str()); }
	size_t dimension = this->psys.lock()->get_particle_space()->get_domain().get_dimensions();
	double cell_size_min = this->psys.lock()->get_particle_space()->get_domain().get_cell_size().min();
	pmTensor vi  = this->operand[0]->evaluate(i,level);
	double mi  = this->operand[1]->evaluate(i,level)[0];
	double pii  = this->operand[2]->evaluate(i,level)[0];
	double phii = this->operand[3]->evaluate(i,level)[0];
	double sigmai = this->operand[4]->evaluate(i,level)[0];
	double thetai = this->operand[5]->evaluate(i,level)[0];
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
		pmTensor vj  = this->operand[0]->evaluate(j,level);
		double mj  = this->operand[1]->evaluate(j,level)[0];
		double pij  = this->operand[2]->evaluate(j,level)[0];
		double phij = this->operand[3]->evaluate(j,level)[0];
		double sigmaj = this->operand[4]->evaluate(j,level)[0];
		double thetaj = this->operand[5]->evaluate(j,level)[0];
		pmTensor contribution{(int)dimension,1,0.0};
		double d_ji = rel_pos.norm();
		if(d_ji > NAUTICLE_EPS && d_ji < cell_size_min) {
			double W_ij = this->kernel->evaluate(d_ji, cell_size_min);
			pmTensor n_ji = rel_pos/d_ji;
			double F_c = (pii+pij)/2.0*W_ij;
			double F_d = -(phii+phij)*(mi+mj)/4.0*W_ij*(n_ji.transpose()*(vj-vi))[0];
			double F_b = (sigmai+sigmaj)*(thetai+thetaj)/4.0*W_ij;
			contribution -= (F_c+F_d+F_b)*n_ji;
		}
		return contribution;
	};
	return this->interact(i, contribute);
}

























