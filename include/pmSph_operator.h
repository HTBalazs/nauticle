/*
    Copyright 2016 Balazs Toth
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

    For more information please visit: https://BalazsToth@bitbucket.org/BalazsToth/lemps
*/
    
#ifndef _SPH_OPERATOR_H_
#define _SPH_OPERATOR_H_

#include <cstdlib>
#include <array>
#include <string>
#include "pmSph.h"
#include "prolog/pLogger.h"
#include "commonutils/Common.h"

enum OPERATOR_TYPE { XSAMPLE, SAMPLE, GRADIENT, DIVERGENCE, LAPLACE };

template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NUM_OP>
class pmSph_operator : public pmSph<NUM_OP> {
	std::string op_name;
private:
	std::shared_ptr<pmExpression> clone_impl() const override;
public:
	pmSph_operator() {}
	pmSph_operator(std::array<std::shared_ptr<pmExpression>,NUM_OP> op);
	pmSph_operator(pmSph_operator const& other);
	pmSph_operator(pmSph_operator&& other);
	pmSph_operator& operator=(pmSph_operator const& other);
	pmSph_operator& operator=(pmSph_operator&& other);
	virtual ~pmSph_operator() {}
	void print() const override;
	pmTensor process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const;
	pmTensor evaluate(int const& i, Eval_type eval_type/*=current*/) const override;
	std::shared_ptr<pmSph_operator> clone() const;
	virtual void write_to_string(std::ostream& os) const override;
};

template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NUM_OP>
void pmSph_operator<OP_TYPE,VAR,K,NUM_OP>::write_to_string(std::ostream& os) const {
	os<<op_name<<"("<<this->operand[0]<<","<< this->operand[1]<<","<< this->operand[2]<<","<<this->operand[3]<<")";
}
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NUM_OP>
std::ostream& operator<<(std::ostream& os, pmSph_operator<OP_TYPE, VAR, K, NUM_OP> const* obj) {
	obj->write_to_string(os);
	return os;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NUM_OP>
pmSph_operator<OP_TYPE,VAR,K,NUM_OP>::pmSph_operator(std::array<std::shared_ptr<pmExpression>,NUM_OP> op) {
	this->operand = std::move(op);
	size_t type = (int)this->operand[3]->evaluate(0)[0];
	this->kernel = std::make_shared<pmKernel>();
	this->kernel->set_kernel_type(type, OP_TYPE==SAMPLE?false:true);
	op_name = std::string{"sph_"};
	switch(OP_TYPE) {
		case SAMPLE: op_name+=std::string{"S"}; break;
		case XSAMPLE: op_name+=std::string{"X"}; break;
		case GRADIENT: op_name+=std::string{"G"}; break;
		case DIVERGENCE: op_name+=std::string{"D"}; break;
		case LAPLACE: op_name+=std::string{"L"}; break;
	}
	op_name+=Common::to_string(VAR)+Common::to_string(K);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NUM_OP>
pmSph_operator<OP_TYPE,VAR,K,NUM_OP>::pmSph_operator(pmSph_operator const& other) {
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
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NUM_OP>
pmSph_operator<OP_TYPE,VAR,K,NUM_OP>::pmSph_operator(pmSph_operator&& other) {
	this->psys = std::move(other.psys);
	this->kernel = std::move(other.kernel);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
	this->op_name = std::move(other.op_name);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NUM_OP>
pmSph_operator<OP_TYPE,VAR,K,NUM_OP>& pmSph_operator<OP_TYPE,VAR,K,NUM_OP>::operator=(pmSph_operator const& other) {
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
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NUM_OP>
pmSph_operator<OP_TYPE,VAR,K,NUM_OP>& pmSph_operator<OP_TYPE,VAR,K,NUM_OP>::operator=(pmSph_operator&& other) {
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
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NUM_OP>
std::shared_ptr<pmExpression> pmSph_operator<OP_TYPE,VAR,K,NUM_OP>::clone_impl() const {
	return std::make_shared<pmSph_operator<OP_TYPE,VAR,K,NUM_OP>>(*this);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NUM_OP>
std::shared_ptr<pmSph_operator<OP_TYPE,VAR,K,NUM_OP>> pmSph_operator<OP_TYPE,VAR,K,NUM_OP>::clone() const {
	return std::static_pointer_cast<pmSph_operator, pmExpression>(clone_impl());
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints SPH operator content.
/////////////////////////////////////////////////////////////////////////////////////////
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NUM_OP>
void pmSph_operator<OP_TYPE,VAR,K,NUM_OP>::print() const {
	pLogger::logf<COLOR>(op_name.c_str());
	this->print_operands();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator for the ith node.
/////////////////////////////////////////////////////////////////////////////////////////
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NUM_OP>
pmTensor pmSph_operator<OP_TYPE,VAR,K,NUM_OP>::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	if(!this->assigned) { pLogger::error_msgf("\"%s\" is not assigned to any particle system.\n", op_name.c_str()); }
	size_t idx = NUM_OP==5?1:0;
	pmTensor b_i{1,1,0};
	if(NUM_OP==5) {
		b_i = this->operand[0]->evaluate(i,eval_type);
	}
	pmTensor A_i = this->operand[0+idx]->evaluate(i,eval_type);
	float m_i = this->operand[1+idx]->evaluate(i,eval_type)[0];
	float rho_i = this->operand[2+idx]->evaluate(i,eval_type)[0];
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, float const& cell_size, pmTensor const& guide)->pmTensor{
		pmTensor contribution;
		if(i!=j || OP_TYPE==SAMPLE) {
			float d_ji = rel_pos.norm();
			if(d_ji < cell_size && (d_ji > 1e-6f || OP_TYPE==SAMPLE)) {
				pmTensor b_j{1,1,0};
				size_t idx = NUM_OP==5?1:0;
				if(NUM_OP==5) {
					b_j = this->operand[0]->evaluate(j,eval_type);
				}
				pmTensor A_j = this->operand[0+idx]->evaluate(j,eval_type);
				float m_j = this->operand[1+idx]->evaluate(j,eval_type)[0];
				float rho_j = this->operand[2+idx]->evaluate(j,eval_type)[0];
				float W_ij = this->kernel->evaluate(d_ji, cell_size);
				pmTensor tmp = this->process(A_i, A_j, rho_i, rho_j, m_i, m_j, rel_pos, d_ji, W_ij);
				contribution += (NUM_OP==4 ? tmp : tmp*(b_i+b_j)/2.0);
			}
		}
		return contribution;
	};
	return this->interact(i, this->operand, contribute);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<SAMPLE,0,0,4>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return A_j*m_j/rho_j*W_ij;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<XSAMPLE,0,0,4>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return (A_j-A_i)*m_j/rho_j*W_ij;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<GRADIENT,0,0,4>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j-A_i)*m_j/rho_j*W_ij*r_ji.transpose()/d_ji).to_column();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<GRADIENT,1,0,4>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j+A_i)*m_j/rho_j*W_ij*r_ji.transpose()/d_ji).to_column();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<DIVERGENCE,0,0,4>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j-A_i).to_row()*m_j/rho_j*W_ij*r_ji/d_ji);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<DIVERGENCE,1,0,4>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j+A_i).to_row()*m_j/rho_j*W_ij*r_ji/d_ji);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<LAPLACE,0,0,4>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	pmTensor e_ij = -r_ji/d_ji;
	return (2.0f*m_j/rho_j*e_ij.transpose()/d_ji*(W_ij*e_ij))*(A_i-A_j);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<LAPLACE,1,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	pmTensor e_ij = -r_ji/d_ji;
	return (2.0f*m_j/rho_j*e_ij.transpose()/d_ji*(W_ij*e_ij))*(A_i-A_j);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<GRADIENT,0,1,4>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j-A_i)*m_j/rho_i*W_ij*r_ji.transpose()/d_ji).to_column();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<GRADIENT,1,1,4>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j/rho_j/rho_j+A_i/rho_i/rho_i)*m_j*rho_i*W_ij*r_ji.transpose()/d_ji).to_column();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<DIVERGENCE,0,1,4>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j-A_i).to_row()*m_j/rho_i*W_ij*r_ji/d_ji);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<DIVERGENCE,1,1,4>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j/rho_j/rho_j+A_i/rho_i/rho_i).to_row()*m_j*rho_i*W_ij*r_ji/d_ji);
}

#endif // _SPH_OPERATOR_H_