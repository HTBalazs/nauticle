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
    
#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "prolog/pLogger.h"
#include "pmWorkspace.h"
#include "pmEquation.h"
#include "pmParticle_splitter.h"
#include "pmParticle_merger.h"
#include "pmBackground.h"
#include "pmTime_series.h"
#include "pmParticle_sink.h"
#include "pmRigid_body_system.h"
#include <iostream>
#include <string>
#include <memory>

namespace Nauticle {
	/** This class represents the mathematical problem to solve. It contains two 
	//	objects:
	//		- workspace: it manages all the variables and named constants,
	//		- equations: vector of equations governing the problem.
	//	Destroying a solver object destroys the workspace and equations
	//  either.
	*/
	class pmCase {
		std::shared_ptr<pmWorkspace> workspace;
		std::vector<std::shared_ptr<pmEquation>> equations;
		std::vector<std::shared_ptr<pmParticle_modifier>> particle_modifier;
		std::vector<std::shared_ptr<pmBackground>> background;
		std::vector<std::shared_ptr<pmTime_series>> time_series;
		std::shared_ptr<pmRigid_body_system> rbsys;
	public:
		pmCase() {}
		pmCase(pmCase const& other);
		pmCase(pmCase&& other);
		~pmCase() {}
		pmCase& operator=(pmCase const& rhs);
		pmCase& operator=(pmCase&& rhs);
		bool operator==(pmCase const& rhs) const;
		bool operator!=(pmCase const& rhs) const;
		void print() const;
		bool solve(double const& current_time, size_t const& num_threads, std::string const& name="");
		void assign_particle_system_to_equations();
		std::shared_ptr<pmCase> clone() const;
		void merge(std::shared_ptr<pmCase> const& other);
		std::shared_ptr<pmWorkspace> get_workspace() const;
		std::vector<std::shared_ptr<pmEquation>> get_equations() const;
		void add_workspace(std::shared_ptr<pmWorkspace> ws);
		void add_equation(std::shared_ptr<pmEquation> func);
		void add_equation(std::vector<std::shared_ptr<pmEquation>> func);
		void update_particle_modifiers(size_t const& num_threads);
		void update_background_fields();
		void update_rigid_bodies(double const& time_step);
		void update_time_series_variables(double const& t);
		void add_particle_modifier(std::shared_ptr<pmParticle_modifier> pmod);
		void add_background(std::shared_ptr<pmBackground> bckg);
		void add_time_series(std::shared_ptr<pmTime_series> ts);
		void add_rigid_body_system(std::shared_ptr<pmRigid_body_system> rbs);
		void initialize();
	};
}

#endif //_SOLVER_H_