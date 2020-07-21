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
    
#ifndef _OPERATOR_H_
#define _OPERATOR_H_

#include <array>
#include <memory>
#include "pmExpression.h"
#include "prolog/pLogger.h"

namespace Nauticle {
	/** This interface forms the base for any operator acting on the pmSymbols like 
	//  constants (pmConstants), variables (pmVariables), fields (pmFields), or 
	//  particle systems (pmParticle_system). The number of operands of the operator
	//  in question is controlled by the template parameter S.
	*/
	template <size_t S>
	class pmOperator : public pmExpression {
	protected:
		std::array<std::shared_ptr<pmExpression>, S> operand;
	protected:
		pmOperator() {}
		pmOperator(std::array<std::shared_ptr<pmExpression>, S> op) {
			operand = op;
		}
		pmOperator(pmOperator const& other);
		pmOperator(pmOperator&& other);
		pmOperator& operator=(pmOperator const& other);
		pmOperator& operator=(pmOperator&& other);
		virtual ~pmOperator() override {};
		virtual void set_storage_depth(size_t const& d) override;
		virtual int get_field_size() const override;
		void assign(std::weak_ptr<pmParticle_system> ps) override;
		bool is_assigned() const override;
		void print_operands() const;
		void write_operands_to_string(std::ostream& os) const;
		virtual bool is_interaction() const override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	inline std::ostream& operator<<(std::ostream& os, pmOperator<S> const* obj) {
		obj->write_to_string(os);
		return os;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	pmOperator<S>::pmOperator(pmOperator const& other) {
		for(int i=0; i<S; i++) {
			this->operand[i] = other.operand[i]->clone();
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Move constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	pmOperator<S>::pmOperator(pmOperator&& other) {
		this->operand = std::move(other.operand);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy assigment operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	pmOperator<S>& pmOperator<S>::operator=(pmOperator const& other) {
		if(this!=&other) {
			for(int i=0; i<S; i++) {
				this->operand[i] = other.operand[i]->clone();
			}		
		}
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Move assigment operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	pmOperator<S>& pmOperator<S>::operator=(pmOperator&& other) {
		if(this!=&other) {
			this->operand = std::move(other.operand);
		}
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Option for two-step integrators.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<size_t S>
	void pmOperator<S>::set_storage_depth(size_t const& d) {
		for(auto const& it:operand) {
			it->set_storage_depth(d);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the maximum field size out of the objects in the parameter list.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	int pmOperator<S>::get_field_size() const {
		int size = 0;
		for(auto const& it:operand) {
			int s = it->get_field_size();
			if(s>size) {
				size = s;
			}
		}
		return size;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Assigns pmParticle_system to the operands.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	void pmOperator<S>::assign(std::weak_ptr<pmParticle_system> ps) {
		for(auto const& it:operand) {
			it->assign(ps);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Check if all operands are assigned to a pmParticle_system object.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	bool pmOperator<S>::is_assigned() const {
		for(auto const& it:operand) {
			if(!it->is_assigned()) {
				return false;
			}
		}
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Prints the operands in order.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	void pmOperator<S>::print_operands() const {
		ProLog::pLogger::logf<ProLog::WHT>("(");
		bool first = true;
		for(auto const& it:operand) {
			if(!first) { ProLog::pLogger::logf<ProLog::WHT>(","); }
			it->print();
			first = false;
		}
		ProLog::pLogger::logf<ProLog::WHT>(")");
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Writes the perands to strings.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	void pmOperator<S>::write_operands_to_string(std::ostream& os) const {
		os<<"(";
		bool first = true;
		for(auto const& it:operand) {
			if(!first) { os<<","; }
			os<<it;
			first = false;
		}
		os<<")";
	}

	template <size_t S>
	bool pmOperator<S>::is_interaction() const {
		for(auto const& it:operand) {
			if(it->is_interaction()) {
				return true;
			}
		}
		return false;
	}
}
 
#endif //_OPERATOR_H_