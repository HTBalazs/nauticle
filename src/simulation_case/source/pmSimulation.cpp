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

#include "pmSimulation.h"
#include "pmLog_stream.h"
#include "pmYAML_processor.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmSimulation::pmSimulation(pmSimulation const& other) {
	this->cas = std::make_shared<pmCase>(*other.cas);
	this->parameter_space = std::make_shared<pmParameter_space>(*other.parameter_space);
	for(auto const& it:other.particle_modifier) {
		this->particle_modifier.push_back(it->clone());
	}
	for(auto const& it:other.background) {
		this->background.push_back(it->clone());
	}
	this->vtk_write_mode = other.vtk_write_mode;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmSimulation::pmSimulation(pmSimulation&& other) {
	this->cas = std::move(other.cas);
	this->parameter_space = std::move(other.parameter_space);
	this->particle_modifier = std::move(other.particle_modifier);
	this->background = std::move(other.background);
	this->vtk_write_mode = std::move(other.vtk_write_mode);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmSimulation& pmSimulation::operator=(pmSimulation const& other) {
	if(this!=&other) {
		this->cas = std::make_shared<pmCase>(*other.cas);
		this->parameter_space = std::make_shared<pmParameter_space>(*other.parameter_space);
		for(auto const& it:other.particle_modifier) {
			this->particle_modifier.push_back(it->clone());
		}
		for(auto const& it:other.background) {
			this->background.push_back(it->clone());
		}
		this->vtk_write_mode = other.vtk_write_mode;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmSimulation& pmSimulation::operator=(pmSimulation&& other) {
	if(this!=&other) {
		this->cas = std::move(other.cas);
		this->parameter_space = std::move(other.parameter_space);
		this->particle_modifier = std::move(other.particle_modifier);
		this->background = std::move(other.background);
		this->vtk_write_mode = std::move(other.vtk_write_mode);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Runs the calculation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSimulation::simulate(size_t const& num_threads) {
	size_t max_num_threads = std::thread::hardware_concurrency();
	ProLog::pLogger::logf<ProLog::LGN>("   Number of threads used: %i (%i available)\n", num_threads, max_num_threads);
	pmLog_stream log_stream{(int)parameter_space->get_parameter_value("file_start")[0]};
	log_stream.print_start();
	double current_time=0;
	double previous_printing_time=0;
	double dt = cas->get_workspace()->get_value("dt")[0];
	double simulated_time = parameter_space->get_parameter_value("simulated_time")[0];
	bool printing;
	std::shared_ptr<pmVariable> ws_write_case = std::dynamic_pointer_cast<pmVariable>(cas->get_workspace()->get_instance("write_case").lock());
	std::shared_ptr<pmVariable> ws_substeps = std::dynamic_pointer_cast<pmVariable>(cas->get_workspace()->get_instance("substeps").lock());
	std::shared_ptr<pmVariable> ws_all_steps = std::dynamic_pointer_cast<pmVariable>(cas->get_workspace()->get_instance("all_steps").lock());
	log_stream.print_step_info(dt, (int)ws_substeps->get_value()[0], (int)ws_all_steps->get_value()[0], current_time, simulated_time);
	ws_substeps->set_value(0.0);
	write_step();
	while(current_time < simulated_time && (bool)parameter_space->get_parameter_value("run_simulation")[0]) {
		this->update_particle_modifiers();
		this->update_background_fields();
		dt = cas->get_workspace()->get_value("dt")[0];
		double next_dt = dt;
		// get printing interval
		double print_interval = parameter_space->get_parameter_value("print_interval")[0];
		// time to next printing
		double to_next_print = previous_printing_time + print_interval - current_time;
		// do we have to print?
		printing = dt >= to_next_print-dt/1e4;
		// calculate new dt (adaptive-constant dt or the time to next print)
		if(printing) {
			next_dt = to_next_print;
			cas->get_workspace()->get_instance("dt").lock()->set_value(pmTensor{1,1,next_dt});
			ws_write_case->set_value(pmTensor{1,1,1});
		}
		// Solve equations
		cas->solve(num_threads);

		current_time += next_dt;
		ws_substeps->set_value(ws_substeps->get_value()+1.0);
		ws_all_steps->set_value(ws_all_steps->get_value()+1.0);
		if(printing) {
			if(cas->get_workspace()->get_value("dt")[0]==next_dt) {
				cas->get_workspace()->get_instance("dt").lock()->set_value(pmTensor{1,1,dt});
			}
			write_step();
			log_stream.print_step_info(dt>print_interval?print_interval:dt, (int)ws_substeps->get_value()[0], (int)ws_all_steps->get_value()[0], current_time, simulated_time);
			ws_substeps->set_value(pmTensor{1,1,0});
			previous_printing_time = current_time;
			ws_write_case->set_value(pmTensor{1,1,0});
		}
	}
	log_stream.print_finish((bool)parameter_space->get_parameter_value("confirm_on_exit")[0]);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the object content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSimulation::print() const {
	ProLog::pLogger::headerf<ProLog::LGN>("Simulation");
	if(cas!=nullptr)		cas->print();
	if(parameter_space!=nullptr)	parameter_space->print();
	for(auto const& it:particle_modifier) {
		it->print();
	}
	for(auto const& it:background) {
		it->print();
	}
	ProLog::pLogger::footerf<ProLog::LGN>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes case data to file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSimulation::write_step() const {
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
void pmSimulation::set_working_directory(std::string const& working_dir) const {
	char directory[1024];
	int wdir = chdir(working_dir.c_str());
	char *path = getcwd(directory, sizeof(directory));
	ProLog::pLogger::logf<ProLog::LCY>("  Working directory is set to: %s\n", directory);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads the configureation file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSimulation::read_file(std::string const& filename) {
	std::unique_ptr<pmYAML_processor> yaml_loader{new pmYAML_processor};
	yaml_loader->read_file(filename);
	cas = yaml_loader->get_case();
	auto particle_splitter = yaml_loader->get_particle_splitter(cas->get_workspace());
	auto particle_merger = yaml_loader->get_particle_merger(cas->get_workspace());
	particle_modifier.insert(particle_modifier.end(), particle_splitter.begin(), particle_splitter.end());
	particle_modifier.insert(particle_modifier.end(), particle_merger.begin(), particle_merger.end());
	background = yaml_loader->get_background(cas->get_workspace());
	parameter_space = yaml_loader->get_parameter_space(cas->get_workspace());
	vtk_write_mode = parameter_space->get_parameter_value("output_format")[0] ? BINARY : ASCII;
	ProLog::pLogger::log<ProLog::LCY>("  Case initialization is completed.\n");
	ProLog::pLogger::footer<ProLog::LCY>();
	ProLog::pLogger::line_feed(1);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Forwards the yamlname to a pmSimulation object and executes the simulation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSimulation::execute(size_t const& num_threads/*=8*/) {
	print();
	simulate(num_threads);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates particle splitters and mergers.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSimulation::update_particle_modifiers() {
	cas->get_workspace()->sort_all_by_position();
	for(auto& it:particle_modifier) {
		it->update();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates background interpolations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSimulation::update_background_fields() {
	for(auto& it:background) {
		it->update();
	}
}