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
    
#ifndef _FUNCTIONSPACE_H_
#define _FUNCTIONSPACE_H_

#include <iostream>
#include <string>
#include <memory>
#include "prolog/pLogger.h"
#include "pmWorkspace.h"
#include "pmFunction.h"

/** This class represents the mathematical problem to solve. It contains two 
//	objects:
//		- workspace: it manages all the variables and named constants,
//		- functions: vector of functions governing the problem.
//	Destroying a function space object destroys the workspace and functions
//  either.
*/
class pmFunction_space final {
	std::shared_ptr<pmWorkspace> workspace;
	std::vector<std::shared_ptr<pmFunction>> functions;
public:
	pmFunction_space() {}
	pmFunction_space(pmFunction_space const& other);
	pmFunction_space(pmFunction_space&& other);
	~pmFunction_space() {}
	pmFunction_space& operator=(pmFunction_space const& rhs);
	pmFunction_space& operator=(pmFunction_space&& rhs);
	void print() const;
	void solve(size_t const& num_threads, std::string const& name="");
	void assign_particle_system_to_functions();
	std::shared_ptr<pmFunction_space> clone() const;
	void merge(std::shared_ptr<pmFunction_space> const& other);
	std::shared_ptr<pmWorkspace> get_workspace() const;
	std::vector<std::shared_ptr<pmFunction>> get_functions() const;
	void add_workspace(std::shared_ptr<pmWorkspace> ws);
	void add_function(std::shared_ptr<pmFunction> func);
	void add_function(std::vector<std::shared_ptr<pmFunction>> func);
	void initialize();
};

#endif //_FUNCTIONSPACE_H_