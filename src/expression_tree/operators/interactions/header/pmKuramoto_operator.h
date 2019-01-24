/*
    Copyright 2016-2019 Balazs Toth
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

#ifndef _KURAMOTO_H_
#define _KURAMOTO_H_

#include "pmFilter.h"
#include "prolog/pLogger.h"
#include "nauticle_constants.h"
#include "Color_define.h"

namespace Nauticle {
	/** This class calculates the number of neighbouring particles within the given range.
	//  It requires a pmParticle_system assigned to it.
	*/
	template <size_t NOPS>
	class pmKuramoto_operator : public pmFilter<NOPS> {
		std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmKuramoto_operator() {}
		pmKuramoto_operator(std::array<std::shared_ptr<pmExpression>,NOPS> op);
		pmKuramoto_operator(pmKuramoto_operator const& other);
		pmKuramoto_operator(pmKuramoto_operator&& other);
		pmKuramoto_operator& operator=(pmKuramoto_operator const& other);
		pmKuramoto_operator& operator=(pmKuramoto_operator&& other);
		virtual ~pmKuramoto_operator() {}
		void print() const override;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		std::shared_ptr<pmKuramoto_operator> clone() const;
		int get_field_size() const override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t NOPS>
	inline std::ostream& operator<<(std::ostream& os, pmKuramoto_operator<NOPS> const* obj) {
		obj->write_to_string(os);
		return os;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t NOPS>
	pmKuramoto_operator<NOPS>::pmKuramoto_operator(std::array<std::shared_ptr<pmExpression>,NOPS> op) {
		this->operand = std::move(op);
		this->op_name = "kuramoto"+std::string{NOPS==2?"_pure":"_weighted"};
		if(NOPS>2) {
			size_t type = (int)this->operand[2]->evaluate(0)[0];
			this->kernel = std::make_shared<pmKernel>();
			this->kernel->set_kernel_type(type,false);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t NOPS>
	pmKuramoto_operator<NOPS>::pmKuramoto_operator(pmKuramoto_operator const& other) {
		this->assigned = false;
		for(int i=0; i<this->operand.size(); i++) {
			this->kernel = std::shared_ptr<pmKernel>(other.kernel);
			this->operand[i] = other.operand[i]->clone();
			if(NOPS>2) {
				this->op_name = "kuramoto"+std::string{NOPS==2?"_pure":"_weighted"};
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Move constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t NOPS>
	pmKuramoto_operator<NOPS>::pmKuramoto_operator(pmKuramoto_operator&& other) {
		this->psys = std::move(other.psys);
		this->assigned = std::move(other.assigned);
		this->operand = std::move(other.operand);
		this->op_name = "kuramoto"+std::string{NOPS==2?"_pure":"_weighted"};
		if(NOPS>2) {
			this->kernel = std::move(other.kernel);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy assignment operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t NOPS>
	pmKuramoto_operator<NOPS>& pmKuramoto_operator<NOPS>::operator=(pmKuramoto_operator const& other) {
		if(this!=&other) {
			this->assigned = false;
			for(int i=0; i<this->operand.size(); i++) {
				this->operand[i] = other.operand[i]->clone();
				this->op_name = "kuramoto"+std::string{NOPS==2?"_pure":"_weighted"};
				if(NOPS>2) {
					this->kernel = std::shared_ptr<pmKernel>(other.kernel);
				}
			}
		}
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Move assignment operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t NOPS>
	pmKuramoto_operator<NOPS>& pmKuramoto_operator<NOPS>::operator=(pmKuramoto_operator&& other) {
		if(this!=&other) {
			this->psys = std::move(other.psys);
			this->assigned = std::move(other.assigned);
			this->operand = std::move(other.operand);
			this->op_name = "kuramoto"+std::string{NOPS==2?"_pure":"_weighted"};
			if(NOPS>2) {
				this->kernel = std::move(other.kernel);
			}
		}
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Clone implementation.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t NOPS>
	std::shared_ptr<pmExpression> pmKuramoto_operator<NOPS>::clone_impl() const {
		return std::make_shared<pmKuramoto_operator<NOPS>>(*this);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the copy of the object.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t NOPS>
	std::shared_ptr<pmKuramoto_operator<NOPS>> pmKuramoto_operator<NOPS>::clone() const {
		return std::static_pointer_cast<pmKuramoto_operator, pmExpression>(clone_impl());
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Prints DEM content.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t NOPS>
	void pmKuramoto_operator<NOPS>::print() const {
		ProLog::pLogger::logf<NAUTICLE_COLOR>(this->op_name.c_str());
		this->print_operands();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the interaction.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t NOPS>
	pmTensor pmKuramoto_operator<NOPS>::evaluate(int const& i, size_t const& level/*=0*/) const {
		if(!this->assigned) { ProLog::pLogger::error_msgf("Kuramoto operator is not assigned to any particle system.\n"); }

		double theta_i = this->operand[0]->evaluate(i,level)[0];
		double rad = this->operand[1]->evaluate(i,level)[0];
		auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
			pmTensor contribution;
			double d_ji = rel_pos.norm();
			if(d_ji < rad + NAUTICLE_EPS) {
				double W_ij = NOPS==2?1.0:this->kernel->evaluate(d_ji, rad);
				double theta_j = this->operand[0]->evaluate(j,level)[0];
				contribution = sin(theta_j-theta_i)*W_ij;
			}
			return contribution;
		};
		return this->interact(i, contribute);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the size of the field.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t NOPS>
	int pmKuramoto_operator<NOPS>::get_field_size() const {
		return this->psys.lock()->get_field_size();
	}

}

#endif // _KURAMOTO_H_