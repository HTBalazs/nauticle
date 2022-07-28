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

#include "pmKuramoto_operator.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Implementaton of << operator.
/////////////////////////////////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& os, pmKuramoto_operator const* obj) {
	obj->write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmKuramoto_operator::pmKuramoto_operator(std::array<std::shared_ptr<pmExpression>,3> op) {
	this->operand = std::move(op);
	this->op_name = "kuramoto";
	if(3>2) {
		size_t type = (int)this->operand[2]->evaluate(0)[0];
		this->kernel = std::make_shared<pmKernel>();
		this->kernel->set_kernel_type(type,false);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmKuramoto_operator::pmKuramoto_operator(pmKuramoto_operator const& other) {
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->kernel = std::shared_ptr<pmKernel>(other.kernel);
		this->operand[i] = other.operand[i]->clone();
		if(3>2) {
			this->op_name = "kuramoto";
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmKuramoto_operator::pmKuramoto_operator(pmKuramoto_operator&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
	this->op_name = "kuramoto";
	if(3>2) {
		this->kernel = std::move(other.kernel);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmKuramoto_operator& pmKuramoto_operator::operator=(pmKuramoto_operator const& other) {
	if(this!=&other) {
		this->assigned = false;
		for(int i=0; i<this->operand.size(); i++) {
			this->operand[i] = other.operand[i]->clone();
			this->op_name = "kuramoto";
			if(3>2) {
				this->kernel = std::shared_ptr<pmKernel>(other.kernel);
			}
		}
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmKuramoto_operator& pmKuramoto_operator::operator=(pmKuramoto_operator&& other) {
	if(this!=&other) {
		this->psys = std::move(other.psys);
		this->assigned = std::move(other.assigned);
		this->operand = std::move(other.operand);
		this->op_name = "kuramoto";
		if(3>2) {
			this->kernel = std::move(other.kernel);
		}
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmKuramoto_operator::clone_impl() const {
	return std::make_shared<pmKuramoto_operator>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmKuramoto_operator> pmKuramoto_operator::clone() const {
	return std::static_pointer_cast<pmKuramoto_operator, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints kuramoto operator content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmKuramoto_operator::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>(this->op_name.c_str());
	this->print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmKuramoto_operator::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!this->assigned) { ProLog::pLogger::error_msgf("Kuramoto operator is not assigned to any particle system.\n"); }

	double theta_i = this->operand[0]->evaluate(i,level)[0];
	double rad = this->operand[1]->evaluate(i,level)[0];
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
		pmTensor contribution;
		double d_ji = rel_pos.norm();
		if(d_ji < rad + NAUTICLE_EPS) {
			double W_ij = this->kernel->evaluate(d_ji, rad);
			double theta_j = this->operand[0]->evaluate(j,level)[0];
			contribution = sin(theta_j-theta_i)*W_ij;
		}
		return contribution;
	};
	return this->interact(i, contribute);
}
