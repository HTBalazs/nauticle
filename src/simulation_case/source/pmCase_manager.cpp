/*
    Copyright 2016-2017 Balazs Toth
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

#include "pmCase_manager.h"
#include "pmLog_stream.h"
#include "pmYAML_processor.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads the configureation file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase_manager::read_file(std::string const& filename) {
	std::unique_ptr<pmYAML_processor> yaml_loader{new pmYAML_processor};
	yaml_loader->read_file(filename);
	cas = yaml_loader->get_case();
	parameter_space = yaml_loader->get_parameter_space(cas->get_workspace());
	vtk_write_mode = parameter_space->get_parameter_value("output_format")[0] ? BINARY : ASCII;
	ProLog::pLogger::log<ProLog::LCY>("  Case initialization is completed.\n");
	ProLog::pLogger::footer<ProLog::LCY>();
	ProLog::pLogger::line_feed(1);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmCase_manager::pmCase_manager(pmCase_manager const& other) {
	this->cas = std::make_shared<pmCase>(*other.cas);
	this->parameter_space = std::make_shared<pmParameter_space>(*other.parameter_space);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmCase_manager::pmCase_manager(pmCase_manager&& other) {
	this->cas = std::move(other.cas);
	this->parameter_space = std::move(other.parameter_space);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmCase_manager& pmCase_manager::operator=(pmCase_manager const& other) {
	if(this!=&other) {
		this->cas = std::make_shared<pmCase>(*other.cas);
		this->parameter_space = std::make_shared<pmParameter_space>(*other.parameter_space);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmCase_manager& pmCase_manager::operator=(pmCase_manager&& other) {
	if(this!=&other) {
		this->cas = std::move(other.cas);
		this->parameter_space = std::move(other.parameter_space);
	}
	return *this;
}
double pmCase_manager::calculate_print_interval() const {
	static bool constant = false;
	static double interval = 0;
	if(!constant) {
		bool governed_by_workspace = cas->get_workspace()->is_existing("print_interval");
		if(governed_by_workspace) {
			interval = cas->get_workspace()->get_value("print_interval")[0];
		} else {
			interval = parameter_space->get_parameter_value("print_interval")[0];
			constant = true;
		}
	}
	return interval;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Runs the calculation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase_manager::simulate(size_t const& num_threads) {
	size_t max_num_threads = std::thread::hardware_concurrency();
	ProLog::pLogger::logf<ProLog::LGN>("   Number of threads used: %i (%i available)\n", num_threads, max_num_threads);
	pmLog_stream log_stream{(int)parameter_space->get_parameter_value("file_start")[0]};
	log_stream.print_start();
	double current_time=0;
	double previous_printing_time=0;
	int substeps=0;
	double simulated_time = parameter_space->get_parameter_value("simulated_time")[0];
	double dt = cas->get_workspace()->get_value("dt")[0];
	log_stream.print_step_info(dt, substeps, current_time, simulated_time);
	write_step();
	bool printing;
	while(current_time < simulated_time) {
		dt = cas->get_workspace()->get_value("dt")[0];
		double next_dt = dt;
		// get printing interval
		double print_interval = calculate_print_interval();
		// time to next printing
		double to_next_print = previous_printing_time + print_interval - current_time;
		// do we have to print?
		printing = dt >= to_next_print-dt/1e4;
		// calculate new dt (adaptive-constant dt or the time to next print)
		if(printing) {
			next_dt = to_next_print;
			cas->get_workspace()->get_instance("dt").lock()->set_value(pmTensor{1,1,next_dt});
		}
		// Solve equations
		cas->solve(num_threads);
		current_time += next_dt;
		substeps++;
		if(printing) {
			if(cas->get_workspace()->get_value("dt")[0]==next_dt) {
				cas->get_workspace()->get_instance("dt").lock()->set_value(pmTensor{1,1,dt});
			}
			write_step();
			log_stream.print_step_info(dt>print_interval?print_interval:dt, substeps, current_time, simulated_time);
			substeps=0;
			previous_printing_time = current_time;
		}
	}
	log_stream.print_finish((bool)parameter_space->get_parameter_value("confirm_on_exit")[0]);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the object content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase_manager::print() const {
	ProLog::pLogger::headerf<ProLog::LGN>("Case");
	if(cas!=nullptr)		cas->print();
	if(parameter_space!=nullptr)	parameter_space->print();
	ProLog::pLogger::footerf<ProLog::LGN>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes case data to file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase_manager::write_step() const {
	static int counter = parameter_space->get_parameter_value("file_start")[0];
    char ch[5];
    sprintf(&ch[0], "%04i", counter);
	std::string file_name{"step_"};
    file_name += ch;
    file_name += ".vtk";
    std::unique_ptr<pmVTK_writer> vtk_writer{new pmVTK_writer{}};
    vtk_writer->set_write_mode(vtk_write_mode);
    vtk_writer->set_case(cas);
    vtk_writer->set_file_name(file_name);
    vtk_writer->update();
	counter++;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets working directory.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ void pmCase_manager::set_working_directory(std::string const& working_dir) {
	char directory[1024];
	chdir(working_dir.c_str());
	getcwd(directory, sizeof(directory));
	ProLog::pLogger::logf<ProLog::LCY>("  Working directory is set to: %s\n", directory);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Forwards the xmlname to a pmCase_manager object and executes the simulation.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ void pmCase_manager::execute(std::string const& xmlname, std::string const& working_dir, size_t const& num_threads/*=8*/) {
	set_working_directory(working_dir);
	std::shared_ptr<pmCase_manager> cas = std::make_shared<pmCase_manager>();
	cas->read_file(xmlname);
	cas->print();
	cas->simulate(num_threads);
}

