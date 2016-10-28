/////////////////////////////////////////////////////////////////////////////
// Name:        pmExpression.h
// Purpose:     pmExpression class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include <memory>
#include <cmath>
#include "pmTensor.h"
#include "prolog/pLogger.h"

enum Eval_type { current, previous };
class pmParticle_system;

/** This interface represents an algebraic expression as an expression tree.
*/
class pmExpression {
protected:
	virtual std::shared_ptr<pmExpression> clone_impl() const=0;
public:
	virtual ~pmExpression() {}
	virtual pmTensor evaluate(int const&, Eval_type=current) const=0;
	virtual void print() const=0;
	virtual int get_field_size() const=0;
	virtual void double_steps(bool const&)=0;
	std::shared_ptr<pmExpression> clone() const;
	virtual void assign(std::weak_ptr<pmParticle_system> ps) {}
	virtual bool is_assigned() const=0;
	virtual void write_to_string(std::ostream& os) const=0;
};

inline std::ostream& operator<<(std::ostream& os, pmExpression const* obj) {
	obj->write_to_string(os);
	return os;
}


#endif // _EXPRESSION_H_