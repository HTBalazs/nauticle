/*
    Copyright 2017 Balazs Toth
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
    
#ifndef _OPERATOR_H_
#define _OPERATOR_H_

#include <array>
#include <memory>
#include "pmExpression.h"
#include "prolog/pLogger.h"

/** This interface forms the base for any operator acting on the pmTerms like 
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
	virtual void double_steps(bool const&) override;
	virtual int get_field_size() const override;
	void assign(std::weak_ptr<pmParticle_system> ps) override;
	bool is_assigned() const override;
	void print_operands() const;
	void write_operands_to_string(std::ostream& os) const;
};


template <size_t S>
pmOperator<S>::pmOperator(pmOperator const& other) {
	for(int i=0; i<S; i++) {
		this->operand[i] = other.operand[i]->clone();
	}
}

template <size_t S>
pmOperator<S>::pmOperator(pmOperator&& other) {
	this->operand = std::move(other.operand);
}

template <size_t S>
pmOperator<S>& pmOperator<S>::operator=(pmOperator const& other) {
	if(this!=&other) {
		for(int i=0; i<S; i++) {
			this->operand[i] = other.operand[i]->clone();
		}		
	}
	return *this;
}

template <size_t S>
pmOperator<S>& pmOperator<S>::operator=(pmOperator&& other) {
	if(this!=&other) {
		this->operand = std::move(other.operand);
	}
	return *this;
}



template<size_t S>
void pmOperator<S>::double_steps(bool const& use_double_steps) {
	for(auto const& it:operand) {
		it->double_steps(use_double_steps);
	}
}

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

template <size_t S>
void pmOperator<S>::assign(std::weak_ptr<pmParticle_system> ps) {
	for(auto const& it:operand) {
		it->assign(ps);
	}
}

template <size_t S>
bool pmOperator<S>::is_assigned() const {
	for(auto const& it:operand) {
		if(!it->is_assigned()) {
			return false;
		}
	}
	return true;
}

template <size_t S>
void pmOperator<S>::print_operands() const {
	pLogger::logf<WHT>("(");
	bool first = true;
	for(auto const& it:operand) {
		if(!first) { pLogger::logf<WHT>(","); }
		it->print();
		first = false;
	}
	pLogger::logf<WHT>(")");
}

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

// 
#endif //_OPERATOR_H_