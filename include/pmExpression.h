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

    For more information please visit: https://bitbucket.org/lempsproject/
*/
    
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

/////////////////////////////////////////////////////////////////////////////////////////
/// Implementaton of << operator.
/////////////////////////////////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& os, pmExpression const* obj) {
	obj->write_to_string(os);
	return os;
}


#endif // _EXPRESSION_H_