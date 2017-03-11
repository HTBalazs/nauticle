/*
    Copyright 2016-2017 Balazs Toth
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

    For more information please visit: https://bitbucket.org/lempsproject/
*/

#ifndef _ARITHMOP_H_
#define _ARITHMOP_H_
    
#include "pmOperator.h"
#include "pmConstant.h"
#include "prolog/pLogger.h"

/** This class implements the following operations for the expression tree: summation, subtraction,
//  multiplication, division, power, term-by-term product for two operands furthermore summation and 
//  subtraction with one operand.
*/
template <char ARI_TYPE, size_t S>
class pmArithmetic_operator final: public pmOperator<S> {
protected:
	virtual std::shared_ptr<pmExpression> clone_impl() const override;
public:
	pmArithmetic_operator(std::array<std::shared_ptr<pmExpression>,S> op) : pmOperator<S>{op} {}
	pmArithmetic_operator(pmArithmetic_operator const& other) : pmOperator<S>{other} {}
	pmArithmetic_operator(pmArithmetic_operator&& other) : pmOperator<S>{other} {}
	pmArithmetic_operator& operator=(pmArithmetic_operator const& other);
	pmArithmetic_operator& operator=(pmArithmetic_operator&& other);
	~pmArithmetic_operator() override {}
	void print() const override;
	pmTensor evaluate(int const&, Eval_type=current) const override;
	std::shared_ptr<pmArithmetic_operator> clone() const;
	void write_to_string(std::ostream& os) const override;
};
/////////////////////////////////////////////////////////////////////////////////////////
/// Implementaton for << operator.
/////////////////////////////////////////////////////////////////////////////////////////
template <char ARI_TYPE, size_t S>
inline std::ostream& operator<<(std::ostream& os, pmArithmetic_operator<ARI_TYPE,S> const* obj) {
	obj->write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implementaton for << operator.
/////////////////////////////////////////////////////////////////////////////////////////
template <char ARI_TYPE, size_t S>
inline std::ostream& operator<<(std::ostream& os, pmArithmetic_operator<ARI_TYPE,S> const& obj) {
	obj.write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
template <char ARI_TYPE, size_t S>
pmArithmetic_operator<ARI_TYPE,S>& pmArithmetic_operator<ARI_TYPE,S>::operator=(pmArithmetic_operator const& other) {
	return pmOperator<S>::operator=(other);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
template <char ARI_TYPE, size_t S>
pmArithmetic_operator<ARI_TYPE,S>& pmArithmetic_operator<ARI_TYPE,S>::operator=(pmArithmetic_operator&& other) {
	return pmOperator<S>::operator=(other);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the arithmetic operator with its operands.
/////////////////////////////////////////////////////////////////////////////////////////
template <char ARI_TYPE, size_t S>
void pmArithmetic_operator<ARI_TYPE,S>::print() const {
	if(S==1) {
		if(ARI_TYPE=='+') {
			this->operand[0]->print();
			return;
		} else if(ARI_TYPE=='-') {
			pLogger::logf<COLOR>("(-");
			this->operand[0]->print();
			pLogger::logf<COLOR>(")");
			return;
		}
	}
	pLogger::logf<COLOR>("(");
	this->operand[0]->print();
	switch(ARI_TYPE) {
		case '+' : pLogger::logf<COLOR>("+"); break;
		case '-' : pLogger::logf<COLOR>("-"); break;
		case '*' : pLogger::logf<COLOR>("*"); break;
		case '/' : pLogger::logf<COLOR>("/"); break;
		case ':' : pLogger::logf<COLOR>(":"); break;
		case '^' : pLogger::logf<COLOR>("^"); break;
	}
	this->operand[1]->print();
	pLogger::logf<COLOR>(")");
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates artihmetic operator.
/////////////////////////////////////////////////////////////////////////////////////////
template <char ARI_TYPE, size_t S>
pmTensor pmArithmetic_operator<ARI_TYPE,S>::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	if(S==1) {
		if(ARI_TYPE=='+') {
			return this->operand[0]->evaluate(i, eval_type);
		} else if(ARI_TYPE=='-') {
			return -this->operand[0]->evaluate(i, eval_type);
		}
	}
	switch(ARI_TYPE) {
		case '+' : return this->operand[0]->evaluate(i, eval_type)+this->operand[1]->evaluate(i, eval_type); break;
		case '-' : return this->operand[0]->evaluate(i, eval_type)-this->operand[1]->evaluate(i, eval_type); break;
		case '*' : return this->operand[0]->evaluate(i, eval_type)*this->operand[1]->evaluate(i, eval_type); break;
		case '/' : return this->operand[0]->evaluate(i, eval_type)/this->operand[1]->evaluate(i, eval_type); break;
		case ':' : return this->operand[0]->evaluate(i, eval_type).multiply_term_by_term(this->operand[1]->evaluate(i, eval_type)); break;
		case '^' : return pow(this->operand[0]->evaluate(i, eval_type), this->operand[1]->evaluate(i, eval_type)); break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
template <char ARI_TYPE, size_t S>
std::shared_ptr<pmExpression> pmArithmetic_operator<ARI_TYPE,S>::clone_impl() const {
	return std::make_shared<pmArithmetic_operator<ARI_TYPE,S>>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
template <char ARI_TYPE, size_t S>
std::shared_ptr<pmArithmetic_operator<ARI_TYPE,S>> pmArithmetic_operator<ARI_TYPE,S>::clone() const {
	return std::static_pointer_cast<pmArithmetic_operator<ARI_TYPE,S>, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes the operands and arithmetic operator to stream.
/////////////////////////////////////////////////////////////////////////////////////////
template <char ARI_TYPE, size_t S>
void pmArithmetic_operator<ARI_TYPE,S>::write_to_string(std::ostream& os) const {
	if(S==1) {
		if(ARI_TYPE=='+') {
			os<<this->operand[0];
			return;
		} else if(ARI_TYPE=='-') {
			os<<"(-"<<this->operand[0]<<")";
			return;
		}
	} else {
		os << "(" << this->operand[0] << ARI_TYPE << this->operand[1] << ")";
	}
}

#endif // _ARITHMOP_H_