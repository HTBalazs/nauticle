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
    
#ifndef _CASE_H_
#define _CASE_H_

#include <unistd.h>
#include <iostream>
#include <memory>
#include <string>
#include "prolog/pLogger.h"
#include "pmXML_processor.h"
#include "pmFunction_space.h"
#include "pmVTK_manager.h"

/** This class represents the problem to solve. The contructor recieves the file
//  name of the *.XML configuration file.
//	It contains three objects:
//  	- function space: contains all the functions to solve
//		- parameter space: this object holds the general constant values relating
//		to the calculations but not come up in any of the functions.
//		- particle system: if contains a particle cloud on which the functions
//		are interpreted.
*/
class pmCase final {
private:
	std::shared_ptr<pmFunction_space> function_space;
	std::shared_ptr<pmParameter_space> parameter_space;
	write_mode vtk_write_mode = ASCII;
	void print() const;
	float calculate_print_interval() const;
	void simulate(size_t const& num_threads);
	void write_step() const;
	void read_file(std::string const& filename);
	static void set_working_directory(std::string const& working_dir);
public:
	pmCase() {}
	pmCase(pmCase const& other);
	pmCase(pmCase&& other);
	pmCase& operator=(pmCase const& other);
	pmCase& operator=(pmCase&& other);
	~pmCase() {}
	static void execute(std::string const& xmlname, std::string const& working_dir, size_t const& num_threads=8);
};

#endif //_CASE_H_