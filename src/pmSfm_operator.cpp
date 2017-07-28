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

#include "pmSfm_operator.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes object to string.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSfm_operator::write_to_string(std::ostream& os) const {
	os << op_name << "(";
	for(int i=0; i<10; i++) {
		os << this->operand[i];
		if(i!=8) {
			os << ",";
		}
	}
	os << ")";
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements << operator.
/////////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, pmSfm_operator const* obj) {
	obj->write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmSfm_operator::pmSfm_operator(std::array<std::shared_ptr<pmExpression>,10> op) {
	this->operand = std::move(op);
	size_t type = (int)this->operand[1]->evaluate(0)[0];
	op_name = std::string{"social_force"};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmSfm_operator::pmSfm_operator(pmSfm_operator const& other) {
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
pmSfm_operator::pmSfm_operator(pmSfm_operator&& other) {
	this->psys = std::move(other.psys);
	this->kernel = std::move(other.kernel);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
	this->op_name = std::move(other.op_name);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmSfm_operator& pmSfm_operator::operator=(pmSfm_operator const& other) {
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
pmSfm_operator& pmSfm_operator::operator=(pmSfm_operator&& other) {
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
std::shared_ptr<pmExpression> pmSfm_operator::clone_impl() const {
	return std::make_shared<pmSfm_operator>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmSfm_operator> pmSfm_operator::clone() const {
	return std::static_pointer_cast<pmSfm_operator, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints SPH operator content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSfm_operator::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>(op_name.c_str());
	this->print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator for the ith node.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmSfm_operator::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!this->assigned) { ProLog::pLogger::error_msgf("\"%s\" is not assigned to any particle system.\n", op_name.c_str()); }
	size_t dimension = this->psys.lock()->get_particle_space()->get_domain().get_dimensions();
	double cell_size = this->psys.lock()->get_particle_space()->get_domain().get_cell_size();
	pmTensor posi = this->psys.lock()->get_value(i);
	pmTensor vi  = this->operand[0]->evaluate(i,level);
	pmTensor p0  = this->operand[1]->evaluate(i,level);
	double v0  = this->operand[2]->evaluate(i,level)[0];
	double mi = this->operand[3]->evaluate(i,level)[0];
	double Ri = this->operand[4]->evaluate(i,level)[0];
	double Ai = this->operand[5]->evaluate(i,level)[0];
	double Bi = this->operand[6]->evaluate(i,level)[0];
	double k = this->operand[7]->evaluate(i,level)[0];
	double ci = this->operand[8]->evaluate(i,level)[0];
	double taui = this->operand[9]->evaluate(i,level)[0];
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, double const& cell_size, pmTensor const& guide)->pmTensor{
		double Rj = this->operand[4]->evaluate(j,level)[0];
		double cj = this->operand[8]->evaluate(j,level)[0];
		pmTensor contribution{dimension,1,0};
		double d_ji = rel_pos.norm();
		if(d_ji > NAUTICLE_EPS && d_ji < cell_size) {
			pmTensor n_ji = rel_pos/d_ji;
			double Rij = Ri+Rj;
			double cij = (ci+cj)*0.5;
			double body_force = d_ji-Rij<0.0 ? k*(Rij-d_ji) : 0.0;
			contribution += (-Ai*std::exp((Rij-d_ji)/Bi)*n_ji-body_force*n_ji+cij*n_ji)/mi;
		}
		return contribution;
	};
	pmTensor e0 = (p0-posi);
	e0 /= e0.norm();
	return (v0*e0-vi)/taui + this->interact(i, contribute);
}









