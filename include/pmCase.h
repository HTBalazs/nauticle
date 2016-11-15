/////////////////////////////////////////////////////////////////////////////
// Name:        pmCase.h
// Purpose:     pmCase class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

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
	void print() const;
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