/////////////////////////////////////////////////////////////////////////////
// Name:        pmFunction_space.h
// Purpose:     pmFunction_space class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

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
	void solve(std::string const& name="");
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