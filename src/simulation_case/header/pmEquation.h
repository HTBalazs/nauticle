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

#ifndef _EQUATION_H_
#define _EQUATION_H_

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

namespace Nauticle {
	/** This class represents an algebraic or differential equation with pmExpression-s
	//  on the lhs and rhs. The lhs must contain a pmVariable which is overwritten by
	//  the "evaluate" and "solve" member functions.
	//  When a pmEquation object is destroyed the lhs and rhs pmExpression-s are destroyed
	//  as well except if they are pmVariable or pmConstant objects. They are always managed
	//  by the pmWorkspace object. (TODO: std::shared_ptr) 
	*/
	class pmEquation final {
	protected:
		std::string name = "";
		std::shared_ptr<pmSymbol> lhs;
		std::shared_ptr<pmExpression> rhs;
		std::shared_ptr<pmExpression> condition;
		bool rhs_interaction;
	public:
		pmEquation(std::string n, std::shared_ptr<pmSymbol> ex1, std::shared_ptr<pmExpression> ex2, std::shared_ptr<pmExpression> cond);
		pmEquation(pmEquation const&);
		pmEquation(pmEquation&&);
		pmEquation& operator=(pmEquation const&);
		pmEquation& operator=(pmEquation&&);
		bool operator==(pmEquation const& rhs) const;
		bool operator!=(pmEquation const& rhs) const;
		virtual ~pmEquation() {}
		void print() const;
		void solve(size_t const& num_threads);
		std::string get_name() const;
		void set_name(std::string const& n);
		void evaluate(int const& p_begin, int const& p_end);
		void assign_particle_system(std::shared_ptr<pmParticle_system> psys);
		std::shared_ptr<pmEquation> clone() const;
		void write_to_string(std::ostream& os) const;
		std::shared_ptr<pmSymbol> get_lhs() const;
		std::shared_ptr<pmExpression> get_rhs() const;
		std::shared_ptr<pmExpression> get_condition() const;
		void set_lhs(std::shared_ptr<pmSymbol> left);
		void set_rhs(std::shared_ptr<pmExpression> right);
		void set_condition(std::shared_ptr<pmExpression> cond);
		std::string generate_evaluator_code() const;
		bool const& is_interaction() const;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmEquation const* obj) {
		obj->write_to_string(os);
		return os;
	}
}

#endif //_EQUATION_H_

