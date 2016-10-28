/////////////////////////////////////////////////////////////////////////////
// Name:        pmFunction.h
// Purpose:     pmFunction class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

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
	void solve();
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

