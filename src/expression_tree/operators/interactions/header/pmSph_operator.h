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
    
#ifndef _PM_SPH_OPERATOR_H_
#define _PM_SPH_OPERATOR_H_

#include "pmFilter.h"
#include "prolog/pLogger.h"
#include "commonutils/Common.h"
#include "nauticle_constants.h"
#include "Color_define.h"
#include <cstdlib>
#include <array>
#include <string>

namespace Nauticle {
	enum OPERATOR_TYPE { XSAMPLE, SAMPLE, INERTIA, GRADIENT, DIVERGENCE, LAPLACE, TENSILE, AVISC };

	/** This class implements the SPH meshless interpolant operators.
	//  It requires a pmParticle_system assigned to it.
	*/
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	class pmSph_operator : public pmFilter<NOPS> {
		std::string input_type;
	private:
		std::shared_ptr<pmExpression> clone_impl() const override;
#ifdef JELLYFISH
	protected:
		c2c::c2CPP_type get_cpp_input_type() const;
		c2c::c2CPP_type get_cpp_return_type() const override;
		std::string generate_cpp_contributor() const;
		std::string generate_cpp_evaluator_function_content() const override;
#endif // JELLYFISH
	public:
		//pmSph_operator() {}
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
#ifdef JELLYFISH
		c2c::c2CPP_class_member_function generate_cpp_evaluator_function() const override;
		std::string get_cpp_evaluation(std::string const& idx, size_t const& level=0) const override;
#endif // JELLYFISH
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implements the << operator for SPH.
	/////////////////////////////////////////////////////////////////////////////////////////
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
		size_t type = (int)this->operand[NOPS==6?4:3]->evaluate(0)[0];
		this->kernel = std::make_shared<pmKernel>();
		this->kernel->set_kernel_type(type, !(OP_TYPE==INERTIA || OP_TYPE==SAMPLE || OP_TYPE==XSAMPLE));
		this->op_name = std::string{"sph_"};
		switch(OP_TYPE) {
			case SAMPLE: this->op_name+=std::string{"S"}; break;
			case XSAMPLE: this->op_name+=std::string{"X"}; break;
			case INERTIA: this->op_name+=std::string{"I"}; break;
			case GRADIENT: this->op_name+=std::string{"G"}; break;
			case DIVERGENCE: this->op_name+=std::string{"D"}; break;
			case LAPLACE: this->op_name+=std::string{"L"}+Common::to_string(VAR); break;
			case TENSILE: this->op_name+=std::string{"T"}; break;
			case AVISC: this->op_name+=std::string{"A"}; break;
		}
		if((OP_TYPE==GRADIENT || OP_TYPE==DIVERGENCE) && VAR!=2) {
			this->op_name+=Common::to_string(VAR)+Common::to_string(K);
		}
		this->input_type = this->get_cpp_input_type().get_type();
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
		this->return_type = other.return_type;
		this->input_type = other.input_type;
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
		this->return_type = std::move(other.return_type);
		this->input_type = std::move(other.input_type);
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
			this->return_type = other.return_type;
			this->input_type = other.input_type;
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
			this->return_type = std::move(other.return_type);
			this->input_type = std::move(other.input_type);
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
		ProLog::pLogger::logf<NAUTICLE_COLOR>(this->op_name.c_str());
		this->print_operands();
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator for the ith node.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	pmTensor pmSph_operator<OP_TYPE,VAR,K,NOPS>::evaluate(int const& i, size_t const& level/*=0*/) const {
		if(!this->assigned) { ProLog::pLogger::error_msgf("\"%s\" is not assigned to any particle system.\n", this->op_name.c_str()); }
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
		auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
			pmTensor contribution;
			double d_ji = rel_pos.norm();
			if(d_ji > NAUTICLE_EPS || OP_TYPE==SAMPLE) {
				double h_j = this->operand[4+sh]->evaluate(j,level)[0];
				double h_ij = (h_i+h_j)/2.0;
				if(d_ji < h_ij) {
					pmTensor B_ij{1,1,1};
					if(NOPS==6 && OP_TYPE==LAPLACE) {
						pmTensor B_j = this->operand[0]->evaluate(j, level);
						B_ij = (B_i+B_j)/2.0f;
					}
					pmTensor A_j;
					if(this->operand[0+sh]->is_position()) {
						A_j = rel_pos+A_i;
					} else {
						A_j = this->operand[0+sh]->evaluate(j,level).reflect_perpendicular(guide);
					}
					// TODO: optimise
					if(!this->operand[0+sh]->is_symmetric()) {
						int flip = 1;
						for(int i=0; i<guide.numel(); i++) {
							if(guide[i]!=0) {
								flip *= -1;
							}
						}
						A_j *= pmTensor{(double)flip};
					}
					double m_j = this->operand[1+sh]->evaluate(j,level)[0];
					double rho_j = this->operand[2+sh]->evaluate(j,level)[0];
					double W_ij = this->kernel->evaluate(d_ji, h_ij);
					if(OP_TYPE==TENSILE) {
						pmKernel W;
						W.set_kernel_type((int)this->operand[3+sh]->evaluate(0)[0], false);
						double f = W.evaluate(d_ji, h_ij)/W.evaluate(B_ij[0], h_ij);
						f*=f; f*=f;
						contribution += f*this->process(A_i, A_j, rho_i, rho_j, m_i, m_j, rel_pos, d_ji, W_ij);
					} else if(OP_TYPE==AVISC && NOPS==6) {
						double B_hat_i = std::max(0.0,std::min(1.0,(A_j[0]-A_i[0])/rel_pos[0]/B_i[0]))*B_i[0];
						pmTensor A_R = A_j-B_hat_i*rel_pos/2;
						pmTensor A_L = A_i+B_hat_i*rel_pos/2;
						contribution += this->process(A_L, A_R, rho_i, rho_j, m_i, m_j, rel_pos, d_ji, W_ij);
					} else {
						contribution += B_ij*this->process(A_i, A_j, rho_i, rho_j, m_i, m_j, rel_pos, d_ji, W_ij);
					}
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
	inline pmTensor pmSph_operator<INERTIA,0,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		pmTensor tensor = A_j-A_i;
		return tensor.to_column()*tensor.to_row()*(m_j/rho_j*W_ij);
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
	inline pmTensor pmSph_operator<LAPLACE,2,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		pmTensor e_ij = -r_ji/d_ji;
		return e_ij.transpose()*e_ij*(A_j)*(2.0*m_j/rho_j*W_ij/d_ji);
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

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<TENSILE,1,1,6>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		pmTensor R = pmTensor::make_tensor(A_i,0.0);
		if(R.numel()==1) {
			if(A_i[0]>0.0 && A_j[0]>0.0) {
				R = 0.01*(A_i/rho_i/rho_i+A_j/rho_j/rho_j);
			} else {
				double R_i = A_i[0]<0?std::abs(A_i[0])/rho_i/rho_i:0.0;
				double R_j = A_j[0]<0?std::abs(A_j[0])/rho_j/rho_j:0.0;
				R = pmTensor{0.2*(R_i+R_j)};
			}
		} else {
			for(int idx=0;idx<A_i.numel();idx++) {
				double R_i = A_i[idx]>0?-A_i[idx]/rho_i/rho_i:0.0;
				double R_j = A_j[idx]>0?-A_j[idx]/rho_j/rho_j:0.0;
				R[idx] = 0.2*(R_i+R_j);
			}
		}
		return -R*r_ji*(m_j*rho_i*W_ij/d_ji);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<AVISC,0,0,5>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		pmTensor A_ji = A_j-A_i;
		pmTensor A_jir_ji = A_ji.transpose()*r_ji;
		if(A_jir_ji[0]<0) {
			return -r_ji*(A_jir_ji*m_j/rho_j/d_ji/d_ji/d_ji*W_ij);
		}
		return r_ji*0.0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<>
	inline pmTensor pmSph_operator<AVISC,1,0,6>::process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const {
		pmTensor A_ji = A_j-A_i;
		pmTensor A_jir_ji = A_ji.transpose()*r_ji;
		if(A_jir_ji[0]<0) {
			return -r_ji*(A_jir_ji*m_j/rho_j/d_ji/d_ji/d_ji*W_ij);
		}
		return r_ji*0.0;
	}

#ifdef JELLYFISH
	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	c2c::c2CPP_type pmSph_operator<OP_TYPE,VAR,K,NOPS>::get_cpp_return_type() const {
		if(OP_TYPE==SAMPLE || OP_TYPE==XSAMPLE || OP_TYPE==LAPLACE || OP_TYPE==AVISC) {
			return this->operand[0]->evaluate(0).get_cpp_type();
		} else if(OP_TYPE==GRADIENT) {
			if(this->input_type=="double") {
				return c2c::c2CPP_type{"Vector"};
			} else if(this->input_type=="Vector") {
				return c2c::c2CPP_type{"Matrix"};
			} else {
				return c2c::c2CPP_type{"unknown"};
			}
		} else if(OP_TYPE==DIVERGENCE) {
			if(this->input_type=="double") {
				return c2c::c2CPP_type{"unknown"};
			} else if(this->input_type=="Vector") {
				return c2c::c2CPP_type{"double"};
			} else {
				return c2c::c2CPP_type{"Vector"};
			}
		} else if(OP_TYPE==INERTIA) {
			if(this->input_type=="double") {
				return c2c::c2CPP_type{"double"};
			} else if(this->input_type=="Vector") {
				return c2c::c2CPP_type{"Matrix"};
			} else {
				return c2c::c2CPP_type{"Matrix"};
			}
		} else { // TENSILE
			return c2c::c2CPP_type{"Vector"};
		}
		return c2c::c2CPP_type{"double"};
	}

	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	c2c::c2CPP_type pmSph_operator<OP_TYPE,VAR,K,NOPS>::get_cpp_input_type() const {
		return this->operand[NOPS-5]->evaluate(0).get_cpp_type().get_type();
	}

	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	std::string pmSph_operator<OP_TYPE,VAR,K,NOPS>::generate_cpp_contributor() const {
		if(OP_TYPE==SAMPLE) {
			return "contribution += ("+this->operand[0]->get_cpp_evaluation("j")+")*"+this->operand[1]->get_cpp_evaluation("j")+"/"+this->operand[2]->get_cpp_evaluation("j")+"*Wij;\n";
		}
		if(OP_TYPE==XSAMPLE) {
			return "contribution += ("+this->operand[0]->get_cpp_evaluation("j")+"-"+this->operand[0]->get_cpp_evaluation("i")+")*"+this->operand[1]->get_cpp_evaluation("j")+"/"+this->operand[2]->get_cpp_evaluation("j")+"*Wij;\n";
		}
		if(OP_TYPE==GRADIENT) {
			if(this->input_type=="double") {
				return "contribution += -("+this->operand[0]->get_cpp_evaluation("j")+(K==1&&VAR==1?"/std::pow("+this->operand[2]->get_cpp_evaluation("j")+",2)*std::pow("+this->operand[2]->get_cpp_evaluation("i")+",2)":"")+(VAR==2?"":((VAR==0?"-":"+")+this->operand[0]->get_cpp_evaluation("i")))+")*rel_pos*"+this->operand[1]->get_cpp_evaluation("j")+"/"+this->operand[2]->get_cpp_evaluation(K==0?"j":"i")+"*Wij/d_ji;\n";
			} else if(this->input_type=="Vector") {
				return "contribution += -(("+this->operand[0]->get_cpp_evaluation("j")+(K==1&&VAR==1?"/std::pow("+this->operand[2]->get_cpp_evaluation("j")+",2)*std::pow("+this->operand[2]->get_cpp_evaluation("i")+",2)":"")+(VAR==2?"":((VAR==0?"-":"+")+this->operand[0]->get_cpp_evaluation("i")))+")*transpose(rel_pos)).transpose()*"+this->operand[1]->get_cpp_evaluation("j")+"/"+this->operand[2]->get_cpp_evaluation(K==0?"j":"i")+"*Wij/d_ji;\n";
			} else {
				return "contribution += /* ERROR: Cannot compute gradient of a matrix field. */";
			}
		}
		if(OP_TYPE==DIVERGENCE) {
			if(this->input_type=="Vector") {
				return "contribution += -("+this->operand[0]->get_cpp_evaluation("j")+(K==1&&VAR==1?"/std::pow("+this->operand[2]->get_cpp_evaluation("j")+",2)*std::pow("+this->operand[2]->get_cpp_evaluation("i")+",2)":"")+(VAR==2?"":((VAR==0?"-":"+")+this->operand[0]->get_cpp_evaluation("i")))+").dot(rel_pos)*"+this->operand[1]->get_cpp_evaluation("j")+"/"+this->operand[2]->get_cpp_evaluation(K==0?"j":"i")+"*Wij/d_ji;\n";
			} else if(this->input_type=="Matrix") {
				return "contribution += -(("+this->operand[0]->get_cpp_evaluation("j")+(K==1&&VAR==1?"/std::pow("+this->operand[2]->get_cpp_evaluation("j")+",2)*std::pow("+this->operand[2]->get_cpp_evaluation("i")+",2)":"")+(VAR==2?"":((VAR==0?"-":"+")+this->operand[0]->get_cpp_evaluation("i")))+")*rel_pos)*"+this->operand[1]->get_cpp_evaluation("j")+"/"+this->operand[2]->get_cpp_evaluation(K==0?"j":"i")+"*Wij/d_ji;\n";
			} else {
				return "contribution += /* ERROR: Cannot compute divergence of a scalar field. */";
			}
		}
		if(OP_TYPE==AVISC) {
			size_t idxshift = NOPS-5;
			if(this->input_type=="double") {
				return "contribution += (("+this->operand[idxshift+0]->get_cpp_evaluation("j")+"-"+this->operand[idxshift+0]->get_cpp_evaluation("i")+")*rel_pos)>0?(-rel_pos*(("+this->operand[idxshift+0]->get_cpp_evaluation("j")+"-"+this->operand[idxshift+0]->get_cpp_evaluation("i")+")*rel_pos)*"+this->operand[idxshift+1]->get_cpp_evaluation("j")+"/"+this->operand[idxshift+2]->get_cpp_evaluation("j")+"/std::pow(d_ji,3)*Wij):0.0;\n";
			} else if(this->input_type=="Vector") {
				return "contribution += (("+this->operand[idxshift+0]->get_cpp_evaluation("j")+"-"+this->operand[idxshift+0]->get_cpp_evaluation("i")+").dot(rel_pos))>0?(-rel_pos*(("+this->operand[idxshift+0]->get_cpp_evaluation("j")+"-"+this->operand[idxshift+0]->get_cpp_evaluation("i")+").dot(rel_pos))*"+this->operand[idxshift+1]->get_cpp_evaluation("j")+"/"+this->operand[idxshift+2]->get_cpp_evaluation("j")+"/std::pow(d_ji,3)*Wij):create<Vector>();\n";
			} else {
				return "contribution += (("+this->operand[idxshift+0]->get_cpp_evaluation("j")+"-"+this->operand[idxshift+0]->get_cpp_evaluation("i")+")*rel_pos)>0?(-rel_pos*(("+this->operand[idxshift+0]->get_cpp_evaluation("j")+"-"+this->operand[idxshift+0]->get_cpp_evaluation("i")+")*rel_pos)*"+this->operand[idxshift+1]->get_cpp_evaluation("j")+"/"+this->operand[idxshift+2]->get_cpp_evaluation("j")+"/std::pow(d_ji,3)*Wij):create<Matrix>();\n";
			}
		}
		if(OP_TYPE==LAPLACE) {
			size_t idxshift = NOPS-5;
			if(this->input_type=="double") {
				return "contribution += ("+(VAR==2?"":this->operand[idxshift+0]->get_cpp_evaluation("i")+"-")+this->operand[idxshift+0]->get_cpp_evaluation("j")+")*2.0*"+this->operand[idxshift+1]->get_cpp_evaluation("j")+"/"+this->operand[idxshift+2]->get_cpp_evaluation("j")+"/d_ji*Wij;\n";
			} else {
				return "contribution += rel_pos.dot(rel_pos)/d_ji/dji*("+(VAR==2?"+":this->operand[idxshift+0]->get_cpp_evaluation("i")+"-")+this->operand[idxshift+0]->get_cpp_evaluation("j")+")*2.0*"+this->operand[idxshift+1]->get_cpp_evaluation("j")+"/"+this->operand[idxshift+2]->get_cpp_evaluation("j")+"/d_ji*Wij;\n";
			}
		}
		if(OP_TYPE==INERTIA) {
			std::string dyad = "\t\t\tdouble dyad = "+this->operand[0]->get_cpp_evaluation("j")+"-"+this->operand[0]->get_cpp_evaluation("i")+";\n";
			return "\t\t\tcontribution += tranpsose(dyad)*dyad"+this->operand[1]->get_cpp_evaluation("j")+"/"+this->operand[2]->get_cpp_evaluation("j")+"*Wij;\n";
		}
		return "";
	}

	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	std::string pmSph_operator<OP_TYPE,VAR,K,NOPS>::generate_cpp_evaluator_function_content() const {
		std::stringstream ss;
		ss << this;
		bool derivative = false;
		if(OP_TYPE==GRADIENT || OP_TYPE==DIVERGENCE || OP_TYPE==LAPLACE || OP_TYPE==TENSILE || OP_TYPE==AVISC) {
			derivative = true;
		}
		std::string contrib = this->generate_cpp_contributor();
		std::string content = "\t//  Operator: "+ss.str()+"\n";
		std::string radius_value = "";
		if(std::dynamic_pointer_cast<pmSingle>(this->operand[4]).use_count()>0) {
			radius_value += "\t\tdouble radius = " + this->operand[4]->get_cpp_evaluation("0")+";\n";
		} else {
			radius_value += "\t\tdouble radius_i = " + this->operand[4]->get_cpp_evaluation("i")+";\n";
			radius_value += "\t\tdouble radius_j = " + this->operand[4]->get_cpp_evaluation("j")+";\n";
			radius_value += "\t\tdouble radius = (radius_i+radius_j)/2;\n";
		}
		return content+\
	"\tstatic "+pmKernel::get_kernel_cpp_type(this->operand[3-(NOPS-5)]->evaluate(0)[0],derivative)+" kernel{};\n\
	auto contribute = [this](Vector const& rel_pos, int i, int j)->"+this->return_type+" {\n\
		"+this->return_type+" contribution = create<"+this->return_type+">();\n\
		double d_ji = normal(rel_pos);\n"+radius_value+"\
		if(d_ji<radius"+std::string{OP_TYPE==SAMPLE?"":" && d_ji>NAUTICLE_EPS"}+") {\n\
			double Wij = kernel.evaluate(d_ji,radius);\n\
			"+contrib+ \
		"\t\t}\n\
		return contribution;\n\
	};\n\
	return interaction<"+this->return_type+">(i, contribute);";
	}

	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	inline c2c::c2CPP_class_member_function pmSph_operator<OP_TYPE,VAR,K,NOPS>::generate_cpp_evaluator_function() const {
		return c2c::c2CPP_class_member_function{this->return_type, this->get_name()+"_"+this->op_name, false, "", std::vector<c2c::c2CPP_declaration>{c2c::c2CPP_declaration{"int", "i"}}, c2c::PRIVATE, this->generate_cpp_evaluator_function_content()};
	}

	template <OPERATOR_TYPE OP_TYPE, size_t VAR, size_t K, size_t NOPS>
	std::string pmSph_operator<OP_TYPE,VAR,K,NOPS>::get_cpp_evaluation(std::string const& idx, size_t const& level/*=0*/) const {
		return this->get_name()+"_"+this->op_name+"("+idx+")";
	}
#endif // JELLYFISH
}

#include "Color_undefine.h"

#endif //_PM_SPH_OPERATOR_H_
