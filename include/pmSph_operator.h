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
    
#ifndef _SPH_OPERATOR_H_
#define _SPH_OPERATOR_H_

#include <cstdlib>
#include <array>
#include <string>
#include "pmFilter.h"
#include "prolog/pLogger.h"
#include "commonutils/Common.h"
#include "nauticle_constants.h"

namespace Nauticle {

	enum OPERATOR_TYPE { XSAMPLE, SAMPLE, GRADIENT, DIVERGENCE, LAPLACE };

	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	class pmSph_operator : public pmFilter<NOPS> {
		std::string op_name;
	private:
		std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmSph_operator() {}
		pmSph_operator(std::array<std::shared_ptr<pmExpression>,NOPS> op);
		pmSph_operator(pmSph_operator const& other);
		pmSph_operator(pmSph_operator&& other);
		pmSph_operator& operator=(pmSph_operator const& other);
		pmSph_operator& operator=(pmSph_operator&& other);
		virtual ~pmSph_operator() {}
		void print() const override;
		pmTensor process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		std::shared_ptr<pmSph_operator> clone() const;
		virtual void write_to_string(std::ostream& os) const override;
	};

	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	void pmSph_operator<OP_TYPE,VAR,K,NOPS>::write_to_string(std::ostream& os) const {
		os << op_name << "(";
		for(int i=0; i<NOPS; i++) {
			os << this->operand[i];
			if(i!=NOPS-1) {
				os << ",";
			}
		}
		os << ")";
	}
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	std::ostream& operator<<(std::ostream& os, pmSph_operator<OP_TYPE, VAR, K, NOPS> const* obj) {
		obj->write_to_string(os);
		return os;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	pmSph_operator<OP_TYPE,VAR,K,NOPS>::pmSph_operator(std::array<std::shared_ptr<pmExpression>,NOPS> op) {
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
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	pmSph_operator<OP_TYPE,VAR,K,NOPS>::pmSph_operator(pmSph_operator const& other) {
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
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	pmSph_operator<OP_TYPE,VAR,K,NOPS>::pmSph_operator(pmSph_operator&& other) {
		this->psys = std::move(other.psys);
		this->kernel = std::move(other.kernel);
		this->assigned = std::move(other.assigned);
		this->operand = std::move(other.operand);
		this->op_name = std::move(other.op_name);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy assignment operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	pmSph_operator<OP_TYPE,VAR,K,NOPS>& pmSph_operator<OP_TYPE,VAR,K,NOPS>::operator=(pmSph_operator const& other) {
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
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	pmSph_operator<OP_TYPE,VAR,K,NOPS>& pmSph_operator<OP_TYPE,VAR,K,NOPS>::operator=(pmSph_operator&& other) {
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
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	std::shared_ptr<pmExpression> pmSph_operator<OP_TYPE,VAR,K,NOPS>::clone_impl() const {
		return std::make_shared<pmSph_operator<OP_TYPE,VAR,K,NOPS>>(*this);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the copy of the object.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	std::shared_ptr<pmSph_operator<OP_TYPE,VAR,K,NOPS>> pmSph_operator<OP_TYPE,VAR,K,NOPS>::clone() const {
		return std::static_pointer_cast<pmSph_operator, pmExpression>(clone_impl());
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Prints SPH operator content.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	void pmSph_operator<OP_TYPE,VAR,K,NOPS>::print() const {
		pLogger::logf<COLOR>(op_name.c_str());
		this->print_operands();
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator for the ith node.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	pmTensor pmSph_operator<OP_TYPE,VAR,K,NOPS>::evaluate(int const& i, size_t const& level/*=0*/) const {
		if(!this->assigned) { pLogger::error_msgf("\"%s\" is not assigned to any particle system.\n", op_name.c_str()); }
		size_t sh = 0;
		pmTensor B_i{1,1,1};
		if(NOPS==6) {
			B_i = this->operand[0]->evaluate(i, level);
			sh++;
		}
		pmTensor A_i = this->operand[0+sh]->evaluate(i,level);
		double m_i = this->operand[1+sh]->evaluate(i,level)[0];
		double rho_i = this->operand[2+sh]->evaluate(i,level)[0];
		double h_i = this->operand[4+sh]->evaluate(i,level)[0];
		auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, double const& cell_size, pmTensor const& guide)->pmTensor{
			pmTensor contribution;
			double d_ji = rel_pos.norm();
			if(d_ji > NAUTICLE_EPS || OP_TYPE==SAMPLE) {
				double h_j = this->operand[4+sh]->evaluate(j,level)[0];
				if(d_ji < h_i || d_ji < h_j) {
					pmTensor B_j{1,1,1};
					if(NOPS==6) {
						B_j = this->operand[0]->evaluate(j, level);
					}
					pmTensor A_j = this->operand[0+sh]->evaluate(j,level).reflect_perpendicular(guide);
					double m_j = this->operand[1+sh]->evaluate(j,level)[0];
					double rho_j = this->operand[2+sh]->evaluate(j,level)[0];
					double W_ij = this->kernel->evaluate(d_ji, (h_i+h_j)/2.0f);
					pmTensor B_ij{1,1,1};
					if(NOPS==6) {
						B_ij = (B_i+B_j)/2.0f;
					}
					contribution += B_ij*this->process(A_i, A_j, rho_i, rho_j, m_i, m_j, rel_pos, d_ji, W_ij);
				}
			}
			return contribution;
		};
		return this->interact(i, contribute);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<SAMPLE,0,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		return A_j*(m_j/rho_j*W_ij);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<XSAMPLE,0,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		return (A_j-A_i)*(m_j/rho_j*W_ij);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<GRADIENT,0,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		return -((A_j-A_i)*r_ji.transpose()).to_column()*(m_j/rho_j*W_ij/d_ji);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<GRADIENT,1,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		return -((A_j+A_i)*r_ji.transpose()).to_column()*(m_j/rho_j*W_ij/d_ji);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<GRADIENT,2,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		return -(A_j*r_ji.transpose()).to_column()*(m_j/rho_j*W_ij/d_ji);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<DIVERGENCE,0,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		return -(A_j-A_i).to_row()*r_ji*(m_j/rho_j*W_ij/d_ji);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<DIVERGENCE,1,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		return -(A_j+A_i).to_row()*r_ji*(m_j/rho_j*W_ij/d_ji);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<DIVERGENCE,2,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		return -A_j.to_row()*r_ji*(m_j/rho_j*W_ij/d_ji);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<LAPLACE,0,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		pmTensor e_ij = -r_ji/d_ji;
		// return 2.0*e_ij.transpose()/d_ji*(A_i-A_j)*m_j/rho_j*W_ij*e_ij;
		return e_ij.transpose()*e_ij*(A_i-A_j)*(2.0*m_j/rho_j/d_ji*W_ij);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<LAPLACE,1,0,6>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		pmTensor e_ij = -r_ji/d_ji;
		return e_ij.transpose()*e_ij*(A_i-A_j)*(2.0*m_j/rho_j*W_ij/d_ji);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<GRADIENT,0,1,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		return -((A_j-A_i)*r_ji.transpose()).to_column()*(m_j/rho_i*W_ij/d_ji);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<GRADIENT,1,1,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		pmTensor A_rho_i = A_i/(rho_i*rho_i);
		pmTensor A_rho_j = A_j/(rho_j*rho_j);
		return -((A_rho_i+A_rho_j)*r_ji.transpose()).to_column()*(m_j*rho_i*W_ij/d_ji);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<DIVERGENCE,0,1,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		return -(A_j-A_i).to_row()*r_ji*(m_j/rho_i*W_ij/d_ji);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<DIVERGENCE,1,1,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		pmTensor A_rho_i = A_i/(rho_i*rho_i);
		pmTensor A_rho_j = A_j/(rho_j*rho_j);
		return -(A_rho_i+A_rho_j).to_row()*r_ji*(m_j*rho_i*W_ij/d_ji);
	}
}

#endif // _SPH_OPERATOR_H_