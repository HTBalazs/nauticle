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
    
#ifndef _PM_SIMULATION_H_
#define _PM_SIMULATION_H_

#include <unistd.h>
#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include "prolog/pLogger.h"
#include "pmCase.h"
#include "pmVTK_writer.h"
#include "pmParameter_space.h"
#include "pmScript.h"

namespace Nauticle {
	/** This class represents the problem to solve. The contructor recieves the file
	//  name of the *.YAML configuration file.
	//	It contains three objects:
	//  	- cas: contains all the equations to solve
	//		- parameter space: this object holds the general constant values relating
	//		to the calculations but not appear up in any of the equations.
	//		- particle system: it contains a particle cloud on which the equations and fields
	//		are interpreted.
	*/
	class pmSimulation {
	protected:
		bool (pmSimulation::*solver)(double const&, size_t const&);
		// std::function<bool(double const&, size_t const&)> solver;
		std::shared_ptr<pmCase> cas;
		std::shared_ptr<pmParameter_space> parameter_space;
		std::vector<std::shared_ptr<pmScript>> script;
		write_mode vtk_write_mode = ASCII;
		void print() const;
		void simulate(size_t const& num_threads);
		void write_step(bool success) const;
	public:
		void set_working_directory(std::string const& working_dir) const;
		virtual void read_file(std::string const& filename);
		void execute(size_t const& num_threads=8);
		void update_script();
		std::shared_ptr<pmCase> get_case() const;
		bool interpreter_solve(double const& current_time, size_t const& num_threads=8);
		virtual bool binary_solve(double const& current_time, size_t const& num_threads=8);
	};
}

#endif //_PM_SIMULATION_H_