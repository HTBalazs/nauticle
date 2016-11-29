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

template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
class pmSph_operator : public pmSph<4> {
	std::string op_name;
private:
	std::shared_ptr<pmExpression> clone_impl() const;
public:
	pmSph_operator() {}
	pmSph_operator(std::array<std::shared_ptr<pmExpression>,4> op);
	pmSph_operator(pmSph_operator const& other);
	pmSph_operator(pmSph_operator&& other);
	pmSph_operator& operator=(pmSph_operator const& other);
	pmSph_operator& operator=(pmSph_operator&& other);
	virtual ~pmSph_operator() {}
	void print() const override;
	pmTensor process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const;
	pmTensor evaluate(int const& i, Eval_type eval_type/*=current*/) const override;
	pmTensor evaluate(pmTensor const& pos_i, Eval_type eval_type/*=current*/) const;
	std::shared_ptr<pmSph_operator> clone() const;
	virtual void write_to_string(std::ostream& os) const override;
};

template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
void pmSph_operator<OP_TYPE,VAR,K>::write_to_string(std::ostream& os) const {
	os<<op_name<<"("<<operand[0]<<","<< operand[1]<<","<< operand[2]<<","<<operand[3]<<")";
}
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
std::ostream& operator<<(std::ostream& os, pmSph_operator<OP_TYPE, VAR, K> const* obj) {
	obj->write_to_string(os);
	return os;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
pmSph_operator<OP_TYPE,VAR,K>::pmSph_operator(std::array<std::shared_ptr<pmExpression>,4> op) {
	operand = std::move(op);
	size_t type = (int)operand[3]->evaluate(0)[0];
	kernel = std::make_shared<pmKernel>();
	kernel->set_kernel_type(type, OP_TYPE==SAMPLE?false:true);
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
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
pmSph_operator<OP_TYPE,VAR,K>::pmSph_operator(pmSph_operator const& other) {
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
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
pmSph_operator<OP_TYPE,VAR,K>::pmSph_operator(pmSph_operator&& other) {
	this->psys = std::move(other.psys);
	this->kernel = std::move(other.kernel);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
	this->op_name = std::move(other.op_name);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
pmSph_operator<OP_TYPE,VAR,K>& pmSph_operator<OP_TYPE,VAR,K>::operator=(pmSph_operator const& other) {
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
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
pmSph_operator<OP_TYPE,VAR,K>& pmSph_operator<OP_TYPE,VAR,K>::operator=(pmSph_operator&& other) {
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
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
std::shared_ptr<pmExpression> pmSph_operator<OP_TYPE,VAR,K>::clone_impl() const {
	return std::make_shared<pmSph_operator<OP_TYPE,VAR,K>>(*this);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
std::shared_ptr<pmSph_operator<OP_TYPE,VAR,K>> pmSph_operator<OP_TYPE,VAR,K>::clone() const {
	return std::static_pointer_cast<pmSph_operator, pmExpression>(clone_impl());
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints SPH operator content.
/////////////////////////////////////////////////////////////////////////////////////////
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
void pmSph_operator<OP_TYPE,VAR,K>::print() const {
	pLogger::logf<COLOR>(op_name.c_str());
	print_operands();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator for the ith node.
/////////////////////////////////////////////////////////////////////////////////////////
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
pmTensor pmSph_operator<OP_TYPE,VAR,K>::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	if(!assigned) { pLogger::error_msgf("\"%s\" is not assigned to any particle system.\n", op_name.c_str()); }
	pmTensor A_i = operand[0]->evaluate(i,eval_type);
	float m_i = operand[1]->evaluate(i,eval_type)[0];
	float rho_i = operand[2]->evaluate(i,eval_type)[0];
	auto contribute = [&A_i, &m_i, &rho_i, &eval_type, this](pmTensor const& rel_pos, int const& i, int const& j, float const& cell_size)->pmTensor{
		pmTensor contribution;
		if(i!=j || OP_TYPE==SAMPLE) {
			// pmTensor rel_pos = pos_j-pos_i;
			float d_ji = rel_pos.norm();
			if(d_ji < cell_size && (d_ji > 1e-6f || OP_TYPE==SAMPLE)) {
				pmTensor A_j = operand[0]->evaluate(j,eval_type);
				float m_j = operand[1]->evaluate(j,eval_type)[0];
				float rho_j = operand[2]->evaluate(j,eval_type)[0];
				float W_ij = kernel->evaluate(d_ji, cell_size);
				contribution += this->process(A_i, A_j, rho_i, rho_j, m_i, m_j, rel_pos, d_ji, W_ij);
			}
		}
		return contribution;
	};
	return interact(i, operand, contribute);
}
template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K>
pmTensor pmSph_operator<OP_TYPE,VAR,K>::evaluate(pmTensor const& pos_i, Eval_type eval_type/*=current*/) const {
	if(!assigned) { pLogger::error_msgf("\"%s\" is not assigned to any particle system.\n", op_name.c_str()); }
	if(OP_TYPE!=SAMPLE) { pLogger::error_msgf("Not supported for nodes out of particle system.\n", op_name.c_str()); }
	auto contribute = [&pos_i, &eval_type, this](pmTensor const& pos_j, int const& j, float const& cell_size)->pmTensor{
		pmTensor contribution;
		pmTensor rel_pos = pos_j-pos_i;
		float d_ji = rel_pos.norm();
		if(d_ji < cell_size && (d_ji > 1e-6f || OP_TYPE==SAMPLE)) {
			pmTensor A_j = operand[0]->evaluate(j,eval_type);
			float m_j = operand[1]->evaluate(j,eval_type)[0];
			float rho_j = operand[2]->evaluate(j,eval_type)[0];
			float W_ij = kernel->evaluate(d_ji, cell_size);
			contribution += this->process(A_j, A_j, rho_j, rho_j, m_j, m_j, rel_pos, d_ji, W_ij);
		}
		return contribution;
	};
	return interact(pos_i, operand, contribute);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<SAMPLE,0,0>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return A_j*m_j/rho_j*W_ij;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<XSAMPLE,0,0>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return (A_j-A_i)*m_j/rho_j*W_ij;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<GRADIENT,0,0>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j-A_i)*m_j/rho_j*W_ij*r_ji.transpose()/d_ji).to_column();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<GRADIENT,1,0>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j+A_i)*m_j/rho_j*W_ij*r_ji.transpose()/d_ji).to_column();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<DIVERGENCE,0,0>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j-A_i).to_row()*m_j/rho_j*W_ij*r_ji/d_ji);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<DIVERGENCE,1,0>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j+A_i).to_row()*m_j/rho_j*W_ij*r_ji/d_ji);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<LAPLACE,0,0>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	pmTensor e_ij = -r_ji/d_ji;
	return 2.0f*e_ij.transpose()/d_ji*(A_i-A_j)*m_j/rho_j*W_ij*e_ij;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<GRADIENT,0,1>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j-A_i)*m_j/rho_i*W_ij*r_ji.transpose()/d_ji).to_column();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<GRADIENT,1,1>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j/rho_j/rho_j+A_i/rho_i/rho_i)*m_j*rho_i*W_ij*r_ji.transpose()/d_ji).to_column();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<DIVERGENCE,0,1>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j-A_i).to_row()*m_j/rho_i*W_ij*r_ji/d_ji);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the operator.
/////////////////////////////////////////////////////////////////////////////////////////
template<>
inline pmTensor pmSph_operator<DIVERGENCE,1,1>::process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const {
	return -((A_j/rho_j/rho_j+A_i/rho_i/rho_i).to_row()*m_j*rho_i*W_ij*r_ji/d_ji);
}

#endif // _SPH_OPERATOR_H_