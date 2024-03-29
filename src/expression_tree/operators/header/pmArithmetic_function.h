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

#ifndef _PM_ARITHMFC_H_  
#define _PM_ARITHMFC_H_  

#include "pmOperator.h"
#include "pmRandom.h"
#include "prolog/pLogger.h"
#include "Color_define.h"
#include <cmath>

namespace Nauticle {
	enum Ari_fn_type {ABS, ACOS, ACOT, AND, ASIN, ATAN, ATAN2, COS, COSH, COT, COTH, CROSS, ELEM, EXP, FLOOR, GT, GTE, IF, LOG, LOGM, LT, LTE, MAGNITUDE, MAX, MIN, MOD, NOT, OR, URAND, NRAND, LNRAND, SGN, SIN, SINH, SQRT, TAN, TANH, TRACE, DEQ, DER, TRANSPOSE, TRUNC, XOR, IDENTITY, DETERMINANT, INVERSE, EIGSYS, EIGVAL, EQUAL, NOTEQUAL, EULER, PREDICTOR, CORRECTOR, VERLET_R, VERLET_V, LIMIT, HYSTERON};
	
	/** This class implements the following operations for the expression tree: summation, subtraction,
	//  multiplication, division, power, term-by-term product for two operands furthermore addition and 
	//  negation with one operand.
	*/
	template <Ari_fn_type ARI_TYPE, size_t S>
	class pmArithmetic_function final : public pmOperator<S> {
		std::string op_name;
	protected:
		virtual std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmArithmetic_function(std::array<std::shared_ptr<pmExpression>,S> op);
		pmArithmetic_function(pmArithmetic_function const& other);
		pmArithmetic_function(pmArithmetic_function&& other);
		pmArithmetic_function& operator=(pmArithmetic_function const& other);
		pmArithmetic_function& operator=(pmArithmetic_function&& other);
		~pmArithmetic_function() override {}
		void print() const override;
		pmTensor evaluate(int const&, size_t const& level=0) const override;
		std::shared_ptr<pmArithmetic_function> clone() const;
		void write_to_string(std::ostream& os) const override;
		virtual int get_precedence() const { return 0; }
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <Ari_fn_type ARI_TYPE, size_t S>
	pmArithmetic_function<ARI_TYPE,S>::pmArithmetic_function(std::array<std::shared_ptr<pmExpression>,S> op) : pmOperator<S>{op} {
		switch(ARI_TYPE) {
			case ABS : op_name="abs"; break;
			case ACOS : op_name="acos"; break;
			case ACOT : op_name="acot"; break;
			case AND : op_name="and"; break;
			case ASIN : op_name="asin"; break;
			case ATAN : op_name="atan"; break;
			case ATAN2 : op_name="atan2"; break;
			case COS : op_name="cos"; break;
			case COSH : op_name="cosh"; break;
			case COT : op_name="cot"; break;
			case COTH : op_name="coth"; break;
			case CROSS : op_name="cross"; break;
			case ELEM : op_name="elem"; break;
			case EXP : op_name="exp"; break;
			case FLOOR : op_name="floor"; break;
			case GT : op_name="gt"; break;
			case GTE : op_name="gte"; break;
			case IF : op_name="if"; break;
			case LOG : op_name="log"; break;
			case LOGM : op_name="logm"; break;
			case LT : op_name="lt"; break;
			case LTE : op_name="lte"; break;
			case MAGNITUDE : op_name="magnitude"; break;
			case MAX : op_name="max"; break;
			case MIN : op_name="min"; break;
			case MOD : op_name="mod"; break;
			case NOT : op_name="not"; break;
			case OR : op_name="or"; break;
			case URAND : op_name="urand"; break;
			case NRAND : op_name="nrand"; break;
			case LNRAND : op_name="lnrand"; break;
			case SGN : op_name="sgn"; break;
			case SIN : op_name="sin"; break;
			case SINH : op_name="sinh"; break;
			case SQRT : op_name="sqrt"; break;
			case TAN : op_name="tan"; break;
			case TANH : op_name="tanh"; break;
			case TRACE : op_name="trace"; break;
			case DEQ : op_name="deQ"; break;
			case DER : op_name="deR"; break;
			case TRANSPOSE : op_name="transpose"; break;
			case TRUNC : op_name="trunc"; break;
			case XOR : op_name="xor"; break;
			case IDENTITY : op_name="identity"; break;
			case DETERMINANT : op_name="determinant"; break;
			case INVERSE : op_name="inverse"; break;
			case EIGSYS : op_name="eigsys"; break;
			case EIGVAL : op_name="eigval"; break;
			case EQUAL : op_name="eq"; break;
			case NOTEQUAL : op_name="neq"; break;
			case EULER : op_name="euler"; break;
			case PREDICTOR : op_name="predictor"; break;
			case CORRECTOR : op_name="corrector"; break;
			case VERLET_R : op_name="verlet_r"; break;
			case VERLET_V : op_name="verlet_v"; break;
			case LIMIT : op_name="limit"; break;
			case HYSTERON : op_name="hysteron"; break;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy constructor
	/////////////////////////////////////////////////////////////////////////////////////////
	template <Ari_fn_type ARI_TYPE, size_t S>
	pmArithmetic_function<ARI_TYPE,S>::pmArithmetic_function(pmArithmetic_function const& other) : pmOperator<S>{other} {
		this->op_name = other.op_name;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Move constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <Ari_fn_type ARI_TYPE, size_t S>
	pmArithmetic_function<ARI_TYPE,S>::pmArithmetic_function(pmArithmetic_function&& other) : pmOperator<S>{other} {
		this->op_name = other.op_name;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton for << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <Ari_fn_type ARI_TYPE, size_t S>
	inline std::ostream& operator<<(std::ostream& os, pmArithmetic_function<ARI_TYPE,S> const* obj) {
		obj->write_to_string(os);
		return os;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton for << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <Ari_fn_type ARI_TYPE, size_t S>
	inline std::ostream& operator<<(std::ostream& os, pmArithmetic_function<ARI_TYPE,S> const& obj) {
		obj.write_to_string(os);
		return os;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy assignment operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <Ari_fn_type ARI_TYPE, size_t S>
	pmArithmetic_function<ARI_TYPE,S>& pmArithmetic_function<ARI_TYPE,S>::operator=(pmArithmetic_function const& other) {
		return pmOperator<S>::operator=(other);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Move assignment operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <Ari_fn_type ARI_TYPE, size_t S>
	pmArithmetic_function<ARI_TYPE,S>& pmArithmetic_function<ARI_TYPE,S>::operator=(pmArithmetic_function&& other) {
		return pmOperator<S>::operator=(other);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Prints out the arithmetic operator with its operands.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <Ari_fn_type ARI_TYPE, size_t S>
	void pmArithmetic_function<ARI_TYPE,S>::print() const {
		ProLog::pLogger::logf<NAUTICLE_COLOR>(op_name.c_str());
		this->print_operands();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates artihmetic operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <Ari_fn_type ARI_TYPE, size_t S>
	pmTensor pmArithmetic_function<ARI_TYPE,S>::evaluate(int const& i, size_t const& level/*=0*/) const {
		switch(ARI_TYPE) {
			case ABS : return abs(this->operand[0]->evaluate(i, level));
			case ACOS : return acos(this->operand[0]->evaluate(i, level));
			case ACOT : return acot(this->operand[0]->evaluate(i, level));
			case AND : return (this->operand[0]->evaluate(i, level) && this->operand[1]->evaluate(i, level));
			case ASIN : return asin(this->operand[0]->evaluate(i, level));
			case ATAN : return atan(this->operand[0]->evaluate(i, level));
			case ATAN2 : return atan2(this->operand[0]->evaluate(i, level),this->operand[1]->evaluate(i, level));
			case COS : return cos(this->operand[0]->evaluate(i, level));
			case COSH : return cosh(this->operand[0]->evaluate(i, level));
			case COT : return cot(this->operand[0]->evaluate(i, level));
			case COTH : return coth(this->operand[0]->evaluate(i, level));
			case CROSS : return cross(this->operand[0]->evaluate(i, level),this->operand[1]->evaluate(i, level));
			case ELEM : return this->operand[0]->evaluate(i, level).elem(this->operand[1]->evaluate(i, level)[0],this->operand[2]->evaluate(i, level)[0]);
			case EXP : return exp(this->operand[0]->evaluate(i, level));
			case FLOOR : return floor(this->operand[0]->evaluate(i, level));
			case GT : return (tensor_cast<double>(this->operand[0]->evaluate(i, level)) > tensor_cast<double>(this->operand[1]->evaluate(i, level)));
			case GTE : return (tensor_cast<double>(this->operand[0]->evaluate(i, level)) >= tensor_cast<double>(this->operand[1]->evaluate(i, level)));
			case EQUAL : return (tensor_cast<double>(this->operand[0]->evaluate(i, level)) == tensor_cast<double>(this->operand[1]->evaluate(i, level)));
			case NOTEQUAL : return !(tensor_cast<double>(this->operand[0]->evaluate(i, level)) == tensor_cast<double>(this->operand[1]->evaluate(i, level)));
			case IF : return tensor_if(tensor_cast<bool>(this->operand[0]->evaluate(i, level)), this->operand[1]->evaluate(i, level), this->operand[2]->evaluate(i, level));
			case LOG : return log(this->operand[0]->evaluate(i, level));
			case LOGM : return logm(this->operand[0]->evaluate(i, level));
			case LT : return (tensor_cast<double>(this->operand[0]->evaluate(i, level)) < tensor_cast<double>(this->operand[1]->evaluate(i, level)));
			case LTE : return (tensor_cast<double>(this->operand[0]->evaluate(i, level)) <= tensor_cast<double>(this->operand[1]->evaluate(i, level)));
			case MAGNITUDE : return this->operand[0]->evaluate(i, level).norm();
			case MAX : return max(this->operand[0]->evaluate(i, level), this->operand[1]->evaluate(i, level));
			case MIN : return min(this->operand[0]->evaluate(i, level), this->operand[1]->evaluate(i, level));
			case MOD : return mod(this->operand[0]->evaluate(i, level), this->operand[1]->evaluate(i, level));
			case NOT : return !tensor_cast<bool>(this->operand[0]->evaluate(i, level));
			case OR : return (tensor_cast<double>(this->operand[0]->evaluate(i, level)) || tensor_cast<double>(this->operand[1]->evaluate(i, level)));
			case URAND : return pmRandom::random<pmRandom::UNIFORM>(this->operand[0]->evaluate(i, level), this->operand[1]->evaluate(i, level));
			case NRAND : return pmRandom::random<pmRandom::NORMAL>(this->operand[0]->evaluate(i, level), this->operand[1]->evaluate(i, level));
			case LNRAND : return pmRandom::random<pmRandom::LOGNORMAL>(this->operand[0]->evaluate(i, level), this->operand[1]->evaluate(i, level));
			case SGN : return sgn(this->operand[0]->evaluate(i, level));
			case SIN : return sin(this->operand[0]->evaluate(i, level));
			case SINH : return sinh(this->operand[0]->evaluate(i, level));
			case SQRT : return sqrt(this->operand[0]->evaluate(i, level));
			case TAN : return tan(this->operand[0]->evaluate(i, level));
			case TANH : return tanh(this->operand[0]->evaluate(i, level));
			case TRACE : return this->operand[0]->evaluate(i, level).trace();
			case EIGSYS : return this->operand[0]->evaluate(i, level).eigensystem();
			case EIGVAL : return this->operand[0]->evaluate(i, level).eigenvalues();
			case DEQ : return this->operand[0]->evaluate(i, level).deQ();
			case DER : return this->operand[0]->evaluate(i, level).deR();
			case TRANSPOSE : return this->operand[0]->evaluate(i, level).transpose();
			case TRUNC : return trunc(this->operand[0]->evaluate(i, level));
			case XOR : return (tensor_cast<double>(this->operand[0]->evaluate(i, level)) != tensor_cast<double>(this->operand[1]->evaluate(i, level)));
			case DETERMINANT : return this->operand[0]->evaluate(i, level).determinant();
			case INVERSE : return this->operand[0]->evaluate(i, level).inverse();
			case IDENTITY : return pmTensor::make_identity((int)this->operand[0]->evaluate(i, level)[0]);
			case EULER : return this->operand[0]->evaluate(i, 0)+this->operand[1]->evaluate(i, 0) * this->operand[2]->evaluate(i, 0);
			case PREDICTOR : return this->operand[0]->evaluate(i, 0)+this->operand[1]->evaluate(i, 0) * this->operand[2]->evaluate(i, 0);
			case CORRECTOR : return this->operand[0]->evaluate(i, 1)+this->operand[1]->evaluate(i, 0) * this->operand[2]->evaluate(i, 0);
			case VERLET_R : return this->operand[0]->evaluate(i, 0)+this->operand[1]->evaluate(i, 0) * this->operand[3]->evaluate(i, 0) + this->operand[2]->evaluate(i, 0) * std::pow(this->operand[3]->evaluate(i, 0)[0],2) / 2.0;
			case VERLET_V : return this->operand[0]->evaluate(i, 0)+ (this->operand[1]->evaluate(i, 0)+this->operand[1]->evaluate(i, 1))*this->operand[2]->evaluate(i, 0)/2.0;
			case LIMIT : return limit(this->operand[0]->evaluate(i, level)[0], this->operand[1]->evaluate(i, level)[0], this->operand[2]->evaluate(i, level)[0]);
			case HYSTERON : 
				bool state = this->operand[0]->evaluate(i, 0)[0];
				double alpha = this->operand[1]->evaluate(i, 0)[0];
				double beta = this->operand[2]->evaluate(i, 0)[0];
				double value = this->operand[3]->evaluate(i, 0)[0];
			    if(value<alpha && state) {
			    	std::dynamic_pointer_cast<pmSymbol>(this->operand[0])->set_value(pmTensor{1,1,0.0},i);
			        return pmTensor{1,1,-1};
			    } else if(value>beta && !state) {
			    	std::dynamic_pointer_cast<pmSymbol>(this->operand[0])->set_value(pmTensor{1,1,1.0},i);
			        return pmTensor{1,1,1};
			    } else {
			        return pmTensor{1,1,0};
			    }
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Clone implementation.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <Ari_fn_type ARI_TYPE, size_t S>
	std::shared_ptr<pmExpression> pmArithmetic_function<ARI_TYPE,S>::clone_impl() const {
		return std::make_shared<pmArithmetic_function>(*this);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the deep copy of the object.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <Ari_fn_type ARI_TYPE, size_t S>
	std::shared_ptr<pmArithmetic_function<ARI_TYPE,S>> pmArithmetic_function<ARI_TYPE,S>::clone() const {
		return std::static_pointer_cast<pmArithmetic_function<ARI_TYPE,S>, pmExpression>(clone_impl());
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Writes the operands and arithmetic operator to stream.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <Ari_fn_type ARI_TYPE, size_t S>
	void pmArithmetic_function<ARI_TYPE,S>::write_to_string(std::ostream& os) const {
		os<<op_name;
		this->write_operands_to_string(os);
	}

}

#include "Color_undefine.h"

#endif //_PM_ARITHMFC_H_
