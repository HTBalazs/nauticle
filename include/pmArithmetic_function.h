/////////////////////////////////////////////////////////////////////////////
// Name:        pmArithmetic_function.h
// Purpose:     pmArithmetic_function class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "pmOperator.h"
#include "pmRandom.h"
#include "prolog/pLogger.h"

enum Ari_fn_type {ABS, ACOS, ACOT, AND, ASIN, ATAN, COS, COSH, COT, COTH, ELEM, EXP, FLOOR, GT, IF, INT1, INT2, LOG, LT, MAGNITUDE, MAX, MIN, MOD, NOT, OR, RAND, SGN, SIN, SINH, SQRT, TAN, TANH, TRACE, TRANSPOSE, TRUNC, XOR, IDENTITY};

/** This class implements the following operations for the expression tree: summation, subtraction,
//  multiplication, division, power, term-by-term product for two operands furthermore summation and 
//  subtraction with one operand.
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
	pmTensor evaluate(int const&, Eval_type=current) const override;
	std::shared_ptr<pmArithmetic_function> clone() const;
	void write_to_string(std::ostream& os) const override;
};


template <Ari_fn_type ARI_TYPE, size_t S>
pmArithmetic_function<ARI_TYPE,S>::pmArithmetic_function(std::array<std::shared_ptr<pmExpression>,S> op) : pmOperator<S>{op} {
	switch(ARI_TYPE) {
		case ABS : op_name="abs("; break;
		case ACOS : op_name="acos("; break;
		case ACOT : op_name="acot("; break;
		case AND : op_name="and("; break;
		case ASIN : op_name="asin("; break;
		case ATAN : op_name="atan("; break;
		case COS : op_name="cos("; break;
		case COSH : op_name="cosh("; break;
		case COT : op_name="cot("; break;
		case COTH : op_name="coth("; break;
		case ELEM : op_name="elem("; break;
		case EXP : op_name="exp("; break;
		case FLOOR : op_name="floor("; break;
		case GT : op_name="gt("; break;
		case IF : op_name="if("; break;
		case INT1 : op_name="int1("; break;
		case INT2 : op_name="int2("; break;
		case LOG : op_name="log("; break;
		case LT : op_name="lt("; break;
		case MAGNITUDE : op_name="magnitude("; break;
		case MAX : op_name="max("; break;
		case MIN : op_name="min("; break;
		case MOD : op_name="mod("; break;
		case NOT : op_name="not("; break;
		case OR : op_name="or("; break;
		case RAND : op_name="rand("; break;
		case SGN : op_name="sgn("; break;
		case SIN : op_name="sin("; break;
		case SINH : op_name="sinh("; break;
		case SQRT : op_name="sqrt("; break;
		case TAN : op_name="tan("; break;
		case TANH : op_name="tanh("; break;
		case TRACE : op_name="trace("; break;
		case TRANSPOSE : op_name="transpose("; break;
		case TRUNC : op_name="trunc("; break;
		case XOR : op_name="xor("; break;
		case IDENTITY : op_name="identity("; break;
	}
}
template <Ari_fn_type ARI_TYPE, size_t S>
pmArithmetic_function<ARI_TYPE,S>::pmArithmetic_function(pmArithmetic_function const& other) : pmOperator<S>{other} {
	this->op_name = other.op_name;
}
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
	pLogger::logf<COLOR>(op_name.c_str());
	this->print_operands();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates artihmetic operator.
/////////////////////////////////////////////////////////////////////////////////////////
template <Ari_fn_type ARI_TYPE, size_t S>
pmTensor pmArithmetic_function<ARI_TYPE,S>::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	switch(ARI_TYPE) {
		case ABS : return abs(this->operand[0]->evaluate(i, eval_type));
		case ACOS : return acos(this->operand[0]->evaluate(i, eval_type));
		case ACOT : return acot(this->operand[0]->evaluate(i, eval_type));
		case AND : return (this->operand[0]->evaluate(i, eval_type) && this->operand[1]->evaluate(i, eval_type));
		case ASIN : return asin(this->operand[0]->evaluate(i, eval_type));
		case ATAN : return atan(this->operand[0]->evaluate(i, eval_type));
		case COS : return cos(this->operand[0]->evaluate(i, eval_type));
		case COSH : return cosh(this->operand[0]->evaluate(i, eval_type));
		case COT : return cot(this->operand[0]->evaluate(i, eval_type));
		case COTH : return coth(this->operand[0]->evaluate(i, eval_type));
		case ELEM : {
						pmTensor t1 = this->operand[0]->evaluate(i, eval_type);
						pmTensor row = this->operand[1]->evaluate(i, eval_type);
						pmTensor column = this->operand[2]->evaluate(i, eval_type);
						if(!row.is_scalar() || !column.is_scalar()) { pLogger::error_msgf("Element indices must be scalars.\n"); }
						if(row[0]>t1.get_numrows() || column[0]>t1.get_numcols()) { pLogger::error_msgf("Element indices out of bounds.\n"); }
						return pmTensor{1,1,t1(row(0,0),column(0,0))};
					}
		case EXP : return exp(this->operand[0]->evaluate(i, eval_type));
		case FLOOR : return floor(this->operand[0]->evaluate(i, eval_type));
		case GT : return (this->operand[0]->evaluate(i, eval_type) > this->operand[1]->evaluate(i, eval_type));
		case IF : 	{
						pmTensor t2 = this->operand[0]->evaluate(i, eval_type);
						if(!t2.is_scalar()) { pLogger::error_msgf("Logical value should be scalar.\n"); }
						return (bool)t2[0] ? this->operand[1]->evaluate(i, eval_type) : this->operand[2]->evaluate(i, eval_type);
					}
		case INT1 : return this->operand[0]->evaluate(i, current) * this->operand[1]->evaluate(i, current);
		case INT2 : return (3*this->operand[0]->evaluate(i, current)-this->operand[0]->evaluate(i, previous)) / 2 * this->operand[1]->evaluate(i, current);
		case LOG : return log(this->operand[0]->evaluate(i, eval_type));
		case LT : return (this->operand[0]->evaluate(i, eval_type) < this->operand[1]->evaluate(i, eval_type));
		case MAGNITUDE : return this->operand[0]->evaluate(i, eval_type).norm();
		case MAX : return max(this->operand[0]->evaluate(i, eval_type), this->operand[1]->evaluate(i, eval_type));
		case MIN : return min(this->operand[0]->evaluate(i, eval_type), this->operand[1]->evaluate(i, eval_type));
		case MOD : return mod(this->operand[0]->evaluate(i, eval_type), this->operand[1]->evaluate(i, eval_type));
		case NOT : return !this->operand[0]->evaluate(i, eval_type);
		case OR : return (this->operand[0]->evaluate(i, eval_type) || this->operand[1]->evaluate(i, eval_type));
		case RAND : return pmRandom::random(this->operand[0]->evaluate(i, eval_type), this->operand[1]->evaluate(i, eval_type));
		case SGN : return sgn(this->operand[0]->evaluate(i, eval_type));
		case SIN : return sin(this->operand[0]->evaluate(i, eval_type));
		case SINH : return sinh(this->operand[0]->evaluate(i, eval_type));
		case SQRT : return sqrt(this->operand[0]->evaluate(i, eval_type));
		case TAN : return tan(this->operand[0]->evaluate(i, eval_type));
		case TANH : return tanh(this->operand[0]->evaluate(i, eval_type));
		case TRACE : return this->operand[0]->evaluate(i, eval_type).trace();
		case TRANSPOSE : return this->operand[0]->evaluate(i, eval_type).transpose();
		case TRUNC : return trunc(this->operand[0]->evaluate(i, eval_type));
		case XOR : return (this->operand[0]->evaluate(i, eval_type) != this->operand[1]->evaluate(i, eval_type));
		case IDENTITY : {
							pmTensor t3 = this->operand[0]->evaluate(i, eval_type);
							if(!t3.is_scalar()) { pLogger::error_msgf("Not scalar in identity.\n"); }
							return pmTensor::make_identity((int)t3[0]); 
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