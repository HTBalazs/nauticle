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
	enum Ari_fn_type {ABS, ACOS, ACOT, AND, ASIN, ATAN, ATAN2, COS, COSH, COT, COTH, CROSS, ELEM, EXP, FLOOR, GT, GTE, IF, LOG, LOGM, LT, LTE, MAGNITUDE, MAX, MIN, MOD, NOT, OR, URAND, NRAND, LNRAND, SGN, SIN, SINH, SQRT, TAN, TANH, TRACE, DEQ, DER, TRANSPOSE, TRUNC, XOR, IDENTITY, DETERMINANT, INVERSE, EIGSYS, EIGVAL, EQUAL, NOTEQUAL, EULER, PREDICTOR, CORRECTOR, VERLET_R, VERLET_V, LIMIT};
	
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
#ifdef JELLYFISH
		std::string get_cpp_evaluation(std::string const& idx, size_t const& level=0) const override;
#endif // JELLYFISH
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
			case GT : return pmTensor{(double)((tensor_cast<bool>(this->operand[0]->evaluate(i, level)) > tensor_cast<bool>(this->operand[1]->evaluate(i, level))))};
			case GTE : return pmTensor{(double)((tensor_cast<bool>(this->operand[0]->evaluate(i, level)) >= tensor_cast<bool>(this->operand[1]->evaluate(i, level))))};
			case EQUAL : return pmTensor{(double)((tensor_cast<bool>(this->operand[0]->evaluate(i, level)) == tensor_cast<bool>(this->operand[1]->evaluate(i, level))))};
			case NOTEQUAL : return !pmTensor{(double)((tensor_cast<bool>(this->operand[0]->evaluate(i, level)) == tensor_cast<bool>(this->operand[1]->evaluate(i, level))))};
			case IF : return tensor_if(tensor_cast<bool>(this->operand[0]->evaluate(i, level)), this->operand[1]->evaluate(i, level), this->operand[2]->evaluate(i, level));
			case LOG : return log(this->operand[0]->evaluate(i, level));
			case LOGM : return logm(this->operand[0]->evaluate(i, level));
			case LT : return pmTensor{(double)((tensor_cast<bool>(this->operand[0]->evaluate(i, level)) < tensor_cast<bool>(this->operand[1]->evaluate(i, level))))};
			case LTE : return pmTensor{(double)((tensor_cast<bool>(this->operand[0]->evaluate(i, level)) <= tensor_cast<bool>(this->operand[1]->evaluate(i, level))))};
			case MAGNITUDE : return pmTensor{this->operand[0]->evaluate(i, level).norm()};
			case MAX : return max(this->operand[0]->evaluate(i, level), this->operand[1]->evaluate(i, level));
			case MIN : return min(this->operand[0]->evaluate(i, level), this->operand[1]->evaluate(i, level));
			case MOD : return mod(this->operand[0]->evaluate(i, level), this->operand[1]->evaluate(i, level));
			case NOT : return pmTensor{(double)(!tensor_cast<bool>(this->operand[0]->evaluate(i, level)))};
			case OR : return pmTensor{(double)((tensor_cast<bool>(this->operand[0]->evaluate(i, level)) || tensor_cast<bool>(this->operand[1]->evaluate(i, level))))};
			case URAND : return random<UNIFORM>(this->operand[0]->evaluate(i, level), this->operand[1]->evaluate(i, level));
			case NRAND : return random<NORMAL>(this->operand[0]->evaluate(i, level), this->operand[1]->evaluate(i, level));
			case LNRAND : return random<LOGNORMAL>(this->operand[0]->evaluate(i, level), this->operand[1]->evaluate(i, level));
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
			case XOR : return pmTensor{(double)((tensor_cast<bool>(this->operand[0]->evaluate(i, level)) != tensor_cast<bool>(this->operand[1]->evaluate(i, level))))};
			case DETERMINANT : return this->operand[0]->evaluate(i, level).determinant();
			case INVERSE : return this->operand[0]->evaluate(i, level).inverse();
			case IDENTITY : return pmTensor::make_identity((int)this->operand[0]->evaluate(i, level)[0]);
			case EULER : return this->operand[0]->evaluate(i, 0)+this->operand[1]->evaluate(i, 0) * this->operand[2]->evaluate(i, 0);
			case PREDICTOR : return this->operand[0]->evaluate(i, 0)+this->operand[1]->evaluate(i, 0) * this->operand[2]->evaluate(i, 0);
			case CORRECTOR : return this->operand[0]->evaluate(i, 1)+this->operand[1]->evaluate(i, 0) * this->operand[2]->evaluate(i, 0);
			case VERLET_R : return this->operand[0]->evaluate(i, 0)+this->operand[1]->evaluate(i, 0) * this->operand[3]->evaluate(i, 0) + this->operand[2]->evaluate(i, 0) * std::pow(this->operand[3]->evaluate(i, 0)[0],2) / 2.0;
			case VERLET_V : return this->operand[0]->evaluate(i, 0)+ (this->operand[1]->evaluate(i, 0)+this->operand[1]->evaluate(i, 1))*this->operand[2]->evaluate(i, 0)/2.0;
			case LIMIT : return limit(this->operand[0]->evaluate(i, level)[0], this->operand[1]->evaluate(i, level)[0], this->operand[2]->evaluate(i, level)[0]);
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

#ifdef JELLYFISH
	template <Ari_fn_type ARI_TYPE, size_t S>
	std::string pmArithmetic_function<ARI_TYPE,S>::get_cpp_evaluation(std::string const& idx, size_t const& level/*=0*/) const {
		auto bracket = [](std::string const& str, bool const& br)->std::string {
			return (br?"(":"")+str+(br?")":"");
		};
		bool lbracket = false;
		bool rbracket = false;
		switch(ARI_TYPE) {
			case ABS :
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::abs("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").cwiseAbs()";
				}
			case ACOS : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::acos("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").acos()";
				}
			case ACOT : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "(1/std::atan("+this->operand[0]->get_cpp_evaluation(idx, level)+"))";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").atan().cwiseInverse()";
				}
			case AND :
				lbracket = this->operand[0]->get_precedence()>0;
				rbracket = this->operand[1]->get_precedence()>0;
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),lbracket) + " && " + bracket(this->operand[1]->get_cpp_evaluation(idx, level),rbracket)+")";
				} else {
					return "/* ERROR: AND operator does not work on non-scalars. */";
				}
			case ASIN : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::asin("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").asin()";
				}
			case ATAN :
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::atan("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").atan()";
				}
			case ATAN2 : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::atan2("+this->operand[0]->get_cpp_evaluation(idx, level)+" , "+this->operand[1]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "cwise_operation("+this->operand[0]->get_cpp_evaluation(idx, level)+" , "+this->operand[1]->get_cpp_evaluation(idx, level)+",[](double lhs, double rhs)->double{ return std::atan2(lhs,rhs); })";
				}
			case COS : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::cos("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").cos()";
				}
			case COSH : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::cosh("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").cosh()";
				}
			case COT : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "(1/std::tan("+this->operand[0]->get_cpp_evaluation(idx, level)+"))";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").tan().cwiseInverse()";
				}
			case COTH : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "1(std::tanh("+this->operand[0]->get_cpp_evaluation(idx, level)+"))";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").tanh().cwiseInverse()";
				}
			case CROSS :
				lbracket = this->operand[0]->get_precedence()>0;
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "/* ERROR: CROSS operator does not work on scalars */ ";
				} else {
					return bracket(this->operand[0]->get_cpp_evaluation(idx, level),lbracket)+".cross("+this->operand[1]->get_cpp_evaluation(idx, level)+")";
				}
			case ELEM :
				lbracket = this->operand[0]->get_precedence()>0;
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "/* ERROR: ELEM operator does not work on scalars */ ";
				} else {
					return bracket(this->operand[0]->get_cpp_evaluation(idx, level),lbracket)+"("+this->operand[1]->get_cpp_evaluation(idx, level)+","+this->operand[2]->get_cpp_evaluation(idx, level)+")";
				}
			case EXP : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::exp("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").exp()";
				}
			case FLOOR : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::floor("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").floor()";
				}
			case GT : 
				lbracket = this->operand[0]->get_precedence()>0;
				rbracket = this->operand[1]->get_precedence()>0;
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),lbracket) + " > " + bracket(this->operand[1]->get_cpp_evaluation(idx, level),rbracket)+")";
				} else {
					return "/* ERROR: CROSS operator does not work on non-scalars */ ";
				}
			case GTE : 
				lbracket = this->operand[0]->get_precedence()>0;
				rbracket = this->operand[1]->get_precedence()>0;
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),lbracket) + " >= " + bracket(this->operand[1]->get_cpp_evaluation(idx, level),rbracket)+")";
				} else {
					return "/* ERROR: CROSS operator does not work on non-scalars */ ";
				}
			case EQUAL : 
				lbracket = this->operand[0]->get_precedence()>0;
				rbracket = this->operand[1]->get_precedence()>0;
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),lbracket) + " == " + bracket(this->operand[1]->get_cpp_evaluation(idx, level),rbracket)+")";
				} else {
					return "/* ERROR: CROSS operator does not work on non-scalars */ ";
				}
			case NOTEQUAL :
				lbracket = this->operand[0]->get_precedence()>0;
				rbracket = this->operand[1]->get_precedence()>0;
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),lbracket) + " != " + bracket(this->operand[1]->get_cpp_evaluation(idx, level),rbracket)+")";
				} else {
					return "/* ERROR: CROSS operator does not work on non-scalars */ ";
				}
			case IF :
				{
					bool bracket1 = this->operand[0]->get_precedence()>0;
					bool bracket2 = this->operand[1]->get_precedence()>0;
					bool bracket3 = this->operand[2]->get_precedence()>0;
					if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
						return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),bracket1)+"?"+bracket(this->operand[1]->get_cpp_evaluation(idx, level),bracket2)+":"+bracket(this->operand[2]->get_cpp_evaluation(idx, level),bracket3)+")";
					} else {
						return "/* ERROR: IF operator does not work on non-scalars */ ";
					}
				}
			case LOG : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::log("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").log()";
				}
			case LOGM : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::log("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "/* ERROR: Logm is not implemented yet :( */ ";
				}
			case LT : 
				lbracket = this->operand[0]->get_precedence()>0;
				rbracket = this->operand[1]->get_precedence()>0;
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),lbracket) + " < " + bracket(this->operand[1]->get_cpp_evaluation(idx, level),rbracket)+")";
				} else {
					return "/* ERROR: CROSS operator does not work on non-scalars */ ";
				}
			case LTE : 
				lbracket = this->operand[0]->get_precedence()>0;
				rbracket = this->operand[1]->get_precedence()>0;
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),lbracket) + " <= " + bracket(this->operand[1]->get_cpp_evaluation(idx, level),rbracket)+")";
				} else {
					return "/* ERROR: CROSS operator does not work on non-scalars */ ";
				}
			case MAGNITUDE :
				lbracket = this->operand[0]->get_precedence()>0;
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::abs("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return bracket(this->operand[0]->get_cpp_evaluation(idx, level),lbracket)+".norm()";
				}
			case MAX : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::max("+this->operand[0]->get_cpp_evaluation(idx, level) + " , " + this->operand[1]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "/* ERROR: MAX operator does not work on non-scalars */ ";
				}
			case MIN : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::min("+this->operand[0]->get_cpp_evaluation(idx, level) + " , " + this->operand[1]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "/* ERROR: MIN operator does not work on non-scalars */ ";
				}
			case MOD : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::fmod("+this->operand[0]->get_cpp_evaluation(idx, level)+" , "+this->operand[1]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "cwise_operation("+this->operand[0]->get_cpp_evaluation(idx, level)+" , "+this->operand[1]->get_cpp_evaluation(idx, level)+",[](double lhs, double rhs)->double{ return std::fmod(lhs,rhs); })";
				}
			case NOT : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "!("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "/* ERROR: NOT operator does not work on non-scalars */ ";
				}
			case OR : 
				lbracket = this->operand[0]->get_precedence()>0;
				rbracket = this->operand[1]->get_precedence()>0;
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),lbracket) + " || " + bracket(this->operand[1]->get_cpp_evaluation(idx, level),rbracket)+")";
				} else {
					return "/* ERROR: OR operator does not work on non-scalars. */";
				}
			case URAND : return "random<UNIFORM>("+this->operand[0]->get_cpp_evaluation(idx, level)+","+this->operand[1]->get_cpp_evaluation(idx, level)+")";
			case NRAND : return "random<NORMAL>("+this->operand[0]->get_cpp_evaluation(idx, level)+","+this->operand[1]->get_cpp_evaluation(idx, level)+")";
			case LNRAND : return "random<LOGNORMAL>("+this->operand[0]->get_cpp_evaluation(idx, level)+","+this->operand[1]->get_cpp_evaluation(idx, level)+")";
			case SGN :
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "sign("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "cwise_operation("+this->operand[0]->get_cpp_evaluation(idx, level)+" ,[](double val)->double{ return sign(val); })";
				}
			case SIN : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::sin("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").sin()";
				}
			case SINH : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::sinh("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").sinh()";
				}
			case SQRT : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::sqrt("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").cwiseSqrt()";
				}
			case TAN : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::tan("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").tan()";
				}
			case TANH : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::tanh("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").tanh()";
				}
			case TRACE : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return this->operand[0]->get_cpp_evaluation(idx, level);
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").trace()";
				}
			case EIGSYS : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()!="Matrix") {
					return "ERROR: eigenvectors require a matrix.";
				} else {
					return "eigenvectors("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				}
			case EIGVAL :
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()!="Matrix") {
					return "ERROR: eigenvalues require a matrix.";
				} else {
					return "eigenvalues<"+std::to_string(this->operand[0]->evaluate(0).get_numrows())+">("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				}
			case DEQ :
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()!="Matrix") {
					return "ERROR: QR decomposition requires a matrix.";
				} else {
					return "deQ("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				}
			case DER :
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()!="Matrix") {
					return "ERROR: QR decomposition requires a matrix.";
				} else {
					return "deR("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				}
			case TRANSPOSE : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return this->operand[0]->get_cpp_evaluation(idx, level);
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").transpose()";
				}
			case TRUNC : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "std::trunc("+this->operand[0]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "cwise_operation("+this->operand[0]->get_cpp_evaluation(idx, level)+" ,[](double val)->double{ return std::trunc(val); })";
				}
			case XOR : 
				lbracket = this->operand[0]->get_precedence()>0;
				rbracket = this->operand[1]->get_precedence()>0;
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "(!"+bracket(this->operand[0]->get_cpp_evaluation(idx, level),lbracket) + " != !" + bracket(this->operand[1]->get_cpp_evaluation(idx, level),rbracket)+")";
				} else {
					return "/* ERROR: XOR operator does not work on non-scalars. */";
				}
			case DETERMINANT : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return this->operand[0]->get_cpp_evaluation(idx, level);
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").determinant()";
				}
			case INVERSE : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return this->operand[0]->get_cpp_evaluation(idx, level);
				} else {
					return "("+this->operand[0]->get_cpp_evaluation(idx, level)+").inverse()";
				}
			case IDENTITY : 
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					if(this->operand[0]->evaluate(0)[0]==1) {
						return "1.0";
					} else if(this->operand[0]->evaluate(0)[0]==2) {
						return "Eigen::Matrix2d::Identity()";
					} else if(this->operand[0]->evaluate(0)[0]==3) {
						return "Eigen::Matrix3d::Identity()";
					}
				} else {
					return "IDENTITY requires scalar parameter (1..3)";
				}
			case EULER : 
				{
					bool bracket1 = this->operand[0]->get_precedence()>0;
					bool bracket2 = this->operand[1]->get_precedence()>0;
					bool bracket3 = this->operand[2]->get_precedence()>0;
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),bracket1)+"+"+bracket(this->operand[1]->get_cpp_evaluation(idx, level),bracket2)+"*"+bracket(this->operand[2]->get_cpp_evaluation(idx, level),bracket3)+")";
				}
			case PREDICTOR : 
				{
					bool bracket1 = this->operand[0]->get_precedence()>0;
					bool bracket2 = this->operand[1]->get_precedence()>0;
					bool bracket3 = this->operand[2]->get_precedence()>0;
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),bracket1)+"+"+bracket(this->operand[1]->get_cpp_evaluation(idx, level),bracket2)+"*"+bracket(this->operand[2]->get_cpp_evaluation(idx, level),bracket3)+")";
				}
			case CORRECTOR : 
				{
					bool bracket1 = this->operand[0]->get_precedence()>0;
					bool bracket2 = this->operand[1]->get_precedence()>0;
					bool bracket3 = this->operand[2]->get_precedence()>0;
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx,level+1),bracket1)+"+"+bracket(this->operand[1]->get_cpp_evaluation(idx, level),bracket2)+"*"+bracket(this->operand[2]->get_cpp_evaluation(idx, level),bracket3)+")";
				}
			case VERLET_R : 
				{
					bool bracket1 = this->operand[0]->get_precedence()>0;
					bool bracket2 = this->operand[1]->get_precedence()>0;
					bool bracket3 = this->operand[2]->get_precedence()>0;
					bool bracket4 = this->operand[3]->get_precedence()>0;
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),bracket1)+"+"+bracket(this->operand[1]->get_cpp_evaluation(idx, level),bracket2)+"*"+bracket(this->operand[3]->get_cpp_evaluation(idx, level),bracket4)+"+"+bracket(this->operand[2]->get_cpp_evaluation(idx, level),bracket3)+"+std::pow("+bracket(this->operand[3]->get_cpp_evaluation(idx, level),bracket4)+",2)/2.0)";
				}
			case VERLET_V : 
				{
					bool bracket1 = this->operand[0]->get_precedence()>0;
					bool bracket2 = this->operand[1]->get_precedence()>0;
					bool bracket3 = this->operand[2]->get_precedence()>0;
					return "("+bracket(this->operand[0]->get_cpp_evaluation(idx, level),bracket1)+"+("+bracket(this->operand[1]->get_cpp_evaluation(idx, level),bracket2)+"+"+bracket(this->operand[1]->get_cpp_evaluation(idx,level+1),bracket2)+")*"+bracket(this->operand[2]->get_cpp_evaluation(idx, level),bracket3)+"/2.0)";
				}
			case LIMIT :
				if(this->operand[0]->evaluate(0).get_cpp_type().get_type()=="double") {
					return "value_limiter("+this->operand[0]->get_cpp_evaluation(idx, level) + " , " + this->operand[1]->get_cpp_evaluation(idx, level) + " , " + this->operand[2]->get_cpp_evaluation(idx, level)+")";
				} else {
					return "/* ERROR: LIMIT operator does not work on non-scalars. */";
				}
		}
	}
#endif // JELLYFISH

}

#include "Color_undefine.h"

#endif //_PM_ARITHMFC_H_
