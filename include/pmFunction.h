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

#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <cmath>
#include <sstream>
#include "prolog/pLogger.h"
#include "pmExpression.h"
#include "pmWorkspace.h"

/** This class represents an algebraic or differential equation with pmExpression-s
//  on the lhs and rhs. The lhs must contain a pmVariable which is overwritten by
//  the "evaluate" and "solve" member functions.
//  When a pmFunction object is destroyed the lhs and rhs pmExpression-s are destroyed
//  as well except if they are pmVariable or pmConstant objects. They are always managed
//  by the pmWorkspace object. (TODO: std::shared_ptr) 
*/
class pmFunction final {
protected:
	std::string name = "";
	std::shared_ptr<pmTerm> lhs;
	std::shared_ptr<pmExpression> rhs;
public:
	pmFunction(std::string n, std::shared_ptr<pmTerm> ex1, std::shared_ptr<pmExpression> ex2);
	pmFunction(pmFunction const&);
	pmFunction(pmFunction&&);
	pmFunction& operator=(pmFunction const&);
	pmFunction& operator=(pmFunction&&);
	virtual ~pmFunction() {}
	void print() const;
	void solve(size_t const& num_threads);
	std::string get_name() const;
	void evaluate(int const& p_begin, int const& p_end);
	void assign_particle_system(std::weak_ptr<pmParticle_system> psys);
	std::shared_ptr<pmFunction> clone() const;
	void write_to_string(std::ostream& os) const;
};

inline std::ostream& operator<<(std::ostream& os, pmFunction const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_FUNCTION_H_

