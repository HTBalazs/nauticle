/*
    Copyright 2016-2018 Balazs Toth
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
    
#ifndef _CASE_H_
#define _CASE_H_

#include <unistd.h>
#include <iostream>
#include <memory>
#include <string>
#include "prolog/pLogger.h"
#include "pmCase.h"
#include "pmVTK_writer.h"
#include "pmParameter_space.h"

namespace Nauticle {
	/** This class represents the problem to solve. The contructor recieves the file
	//  name of the *.XML configuration file.
	//	It contains three objects:
	//  	- cas: contains all the equations to solve
	//		- parameter space: this object holds the general constant values relating
	//		to the calculations but not appear up in any of the equations.
	//		- particle system: if contains a particle cloud on which the equations and fields
	//		are interpreted.
	*/
	class pmCase_manager final {
	private:
		std::shared_ptr<pmCase> cas;
		std::shared_ptr<pmParameter_space> parameter_space;
		write_mode vtk_write_mode = ASCII;
		void print() const;
		double calculate_print_interval() const;
		void simulate(size_t const& num_threads);
		void write_step() const;
		void read_file(std::string const& filename);
		static void set_working_directory(std::string const& working_dir);
	public:
		pmCase_manager() {}
		pmCase_manager(pmCase_manager const& other);
		pmCase_manager(pmCase_manager&& other);
		pmCase_manager& operator=(pmCase_manager const& other);
		pmCase_manager& operator=(pmCase_manager&& other);
		~pmCase_manager() {}
		static void execute(std::string const& yamlname, std::string const& working_dir, size_t const& num_threads=8);
	};
}

#endif //_CASE_H_