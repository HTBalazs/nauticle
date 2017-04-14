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
    
#ifndef _SOLVER_H_
#define _SOLVER_H_

#include <iostream>
#include <string>
#include <memory>
#include "prolog/pLogger.h"
#include "pmWorkspace.h"
#include "pmEquation.h"

/** This class represents the mathematical problem to solve. It contains two 
//	objects:
//		- workspace: it manages all the variables and named constants,
//		- equations: vector of equations governing the problem.
//	Destroying a equation space object destroys the workspace and equations
//  either.
*/
class pmSolver final {
	std::shared_ptr<pmWorkspace> workspace;
	std::vector<std::shared_ptr<pmEquation>> equations;
public:
	pmSolver() {}
	pmSolver(pmSolver const& other);
	pmSolver(pmSolver&& other);
	~pmSolver() {}
	pmSolver& operator=(pmSolver const& rhs);
	pmSolver& operator=(pmSolver&& rhs);
	void print() const;
	void solve(size_t const& num_threads, std::string const& name="");
	void assign_particle_system_to_equations();
	std::shared_ptr<pmSolver> clone() const;
	void merge(std::shared_ptr<pmSolver> const& other);
	std::shared_ptr<pmWorkspace> get_workspace() const;
	std::vector<std::shared_ptr<pmEquation>> get_equations() const;
	void add_workspace(std::shared_ptr<pmWorkspace> ws);
	void add_equation(std::shared_ptr<pmEquation> func);
	void add_equation(std::vector<std::shared_ptr<pmEquation>> func);
	void initialize();
};

#endif //_SOLVER_H_