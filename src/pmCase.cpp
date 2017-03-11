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

    For more information please visit: https://bitbucket.org/lempsproject/
*/
    
#include "pmCase.h"
#include "pmLog_stream.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads the configureation file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::read_file(std::string const& filename) {
	std::unique_ptr<pmXML_processor> xml_loader{new pmXML_processor};
	xml_loader->read_file(filename);
	function_space = xml_loader->get_function_space();
	parameter_space = xml_loader->get_parameter_space(function_space->get_workspace());
	vtk_write_mode = parameter_space->get_parameter_value("output_format")[0] ? BINARY : ASCII;
	pLogger::log<LCY>("  Case initialization is completed.\n");
	pLogger::footer<LCY>();
	pLogger::line_feed(1);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmCase::pmCase(pmCase const& other) {
	this->function_space = std::make_shared<pmFunction_space>(*other.function_space);
	this->parameter_space = std::make_shared<pmParameter_space>(*other.parameter_space);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmCase::pmCase(pmCase&& other) {
	this->function_space = std::move(other.function_space);
	this->parameter_space = std::move(other.parameter_space);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmCase& pmCase::operator=(pmCase const& other) {
	if(this!=&other) {
		this->function_space = std::make_shared<pmFunction_space>(*other.function_space);
		this->parameter_space = std::make_shared<pmParameter_space>(*other.parameter_space);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmCase& pmCase::operator=(pmCase&& other) {
	if(this!=&other) {
		this->function_space = std::move(other.function_space);
		this->parameter_space = std::move(other.parameter_space);
	}
	return *this;
}
float pmCase::calculate_print_interval() const {
	static bool constant = false;
	static float interval = 0;
	if(!constant) {
		bool governed_by_workspace = function_space->get_workspace()->is_existing("print_interval");
		if(governed_by_workspace) {
			interval = function_space->get_workspace()->get_value("print_interval")[0];
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
void pmCase::simulate(size_t const& num_threads) {
	size_t max_num_threads = std::thread::hardware_concurrency();
	pLogger::logf<LGN>("   Number of threads used: %i (%i available)\n", num_threads, max_num_threads);
	pmLog_stream log_stream{};
	log_stream.print_start();
	float current_time=0;
	float previous_time=0;
	int substeps=0, all_steps=0, n=0;
	float simulated_time = parameter_space->get_parameter_value("simulated_time")[0];
	float print_interval = calculate_print_interval();
	float dt = function_space->get_workspace()->get_value("dt")[0];
	log_stream.print_step_info(n, dt, substeps, all_steps, current_time, (double)current_time/simulated_time*100.0f);
	write_step();
	while(current_time < simulated_time) {
		dt = function_space->get_workspace()->get_value("dt")[0];
		if(dt>print_interval) { 
			function_space->get_workspace()->get_instance("dt").lock()->set_value(pmTensor{1,1,print_interval});
			dt = print_interval;
		}
		bool printing = current_time+dt > previous_time+print_interval-dt/100.0f ? true : false;
		if(printing) {
			function_space->get_workspace()->get_instance("dt").lock()->set_value(pmTensor{1,1,current_time-previous_time-print_interval});
		}
		function_space->solve(num_threads);
		print_interval = calculate_print_interval();
		current_time += dt;
		substeps++;
		if(printing) {
			write_step();
			n++;
			all_steps+=substeps;
			log_stream.print_step_info(n, dt, substeps, all_steps, current_time, (double)current_time/simulated_time*100.0f);
			substeps=0;
			previous_time = current_time;
		}
	}
	write_step();
	log_stream.print_finish((bool)parameter_space->get_parameter_value("confirm_on_exit")[0]);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the object content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::print() const {
	pLogger::headerf<LGN>("Case");
	if(function_space!=nullptr)		function_space->print();
	if(parameter_space!=nullptr)	parameter_space->print();
	pLogger::footerf<LGN>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes case data to file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::write_step() const {
	static int counter=0;
    char ch[4];
    sprintf(&ch[0], "%04i", counter);
	std::string file_name{"step_"};
    file_name += ch;
    file_name += ".vtk";
    std::unique_ptr<pmVTK_writer> vtk_writer{new pmVTK_writer{}};
    vtk_writer->set_write_mode(vtk_write_mode);
    vtk_writer->set_function_space(function_space);
    vtk_writer->set_file_name(file_name);
    vtk_writer->update();
	counter++;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets working directory.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ void pmCase::set_working_directory(std::string const& working_dir) {
	char directory[1024];
	chdir(working_dir.c_str());
	getcwd(directory, sizeof(directory));
	pLogger::logf<LCY>("  Working directory is set to: %s\n", directory);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Forwards the xmlname to a pmCase object and executes the simulation.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ void pmCase::execute(std::string const& xmlname, std::string const& working_dir, size_t const& num_threads/*=8*/) {
	set_working_directory(working_dir);
	std::shared_ptr<pmCase> cas = std::make_shared<pmCase>();
	cas->read_file(xmlname);
	cas->print();
	cas->simulate(num_threads);
}

