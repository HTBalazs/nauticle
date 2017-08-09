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

#include "pmXML_processor.h"
#include "pmTensor_parser.h"
#include "pmVTK_reader.h"
#include "prolog/pLogger.h"
#include "Color_define.h"
// #include "nauticle_constants.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a workspace constructed based on the initial condition.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmCase> pmXML_processor::get_initial_condition_case() const {
	pmVTK_reader reader;
	std::vector<std::shared_ptr<pmBlock>> defs = block->find_block("initial_condition");
	if(!defs.empty()) {
		std::vector<std::shared_ptr<pmBlock>> file = defs.back()->find_block("file");
		if(!file.empty()) {
			std::string file_name = std::dynamic_pointer_cast<pmEntry>(file.back())->get_value("value");
			reader.set_file_name(file_name);
		}
		reader.update();
		return reader.get_case();
	}
	return std::shared_ptr<pmCase>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the workspace constructed based on the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmWorkspace> pmXML_processor::get_workspace() const {
	std::shared_ptr<pmWorkspace> workspace = std::make_shared<pmWorkspace>();
	std::vector<std::shared_ptr<pmBlock>> block_case = block->find_block("case");
	std::shared_ptr<pmBlock> block_workspace = block_case.back()->find_block("workspace").back();
	// Read constants
	std::vector<std::shared_ptr<pmBlock>> block_constant = block_workspace->find_block("constants");
	for(auto const& constants:block_constant) {
		for(auto const& it:constants->get_children()) {
			std::string data = std::dynamic_pointer_cast<pmEntry>(it)->get_value("value");
			pmTensor_parser tensor_parser{};
			workspace->add_constant(it->get_name(), tensor_parser.string_to_tensor(data, workspace));
		}
	}
	// Read variables
	std::vector<std::shared_ptr<pmBlock>> block_variable = block_workspace->find_block("variables");
	for(auto const& variables:block_variable) {
		for(auto const& it:variables->get_children()) {
			std::string data = std::dynamic_pointer_cast<pmEntry>(it)->get_value("value");
			pmTensor_parser tensor_parser{};
			workspace->add_variable(it->get_name(), tensor_parser.string_to_tensor(data, workspace));
		}
	}
	// Read particle system
	std::vector<std::shared_ptr<pmBlock>> block_psys = block_workspace->find_block("particle_system");
	for(auto const& ps:block_psys) {
		// Read domain
		std::vector<std::shared_ptr<pmBlock>> block_domain = ps->find_block("domain");
		pmDomain domain;
		for(auto const& dm:block_domain) {
			std::string str_min = dm->get_entry("minimum").back()->get_value("value");
			std::string str_max = dm->get_entry("maximum").back()->get_value("value");
			std::string str_cell_size = dm->get_entry("cell_size").back()->get_value("value");
			std::string str_bnd = dm->get_entry("boundary").back()->get_value("value");
			pmTensor_parser tensor_parser{};
			pmTensor minimum = tensor_parser.string_to_tensor(str_min, workspace);
			pmTensor maximum = tensor_parser.string_to_tensor(str_max, workspace);
			pmTensor cell_size = tensor_parser.string_to_tensor(str_cell_size, workspace);
			pmTensor boundary = tensor_parser.string_to_tensor(str_bnd, workspace);
			if(!cell_size.is_scalar()) { ProLog::pLogger::error_msgf("Cell size must be scalar!\n"); }
			domain = pmDomain{minimum, maximum, cell_size[0], boundary};
		}
		std::shared_ptr<pmGrid_space> grid_space = get_grid_space(ps,workspace, domain);
		std::shared_ptr<pmGrid> tmp = grid_space->get_merged_grid();
		std::vector<pmTensor> particle_grid = tmp->get_grid();
		workspace->add_particle_system(particle_grid, domain);
		workspace->delete_instance("gid");
		workspace->add_field("gid", grid_space->get_grid_id_field());
	}
	// Read fields
	std::vector<std::shared_ptr<pmBlock>> flds = block_workspace->find_block("fields");
	for(auto const& fields:flds) {
		for(auto const& it:fields->get_children()) {
			std::string data = std::dynamic_pointer_cast<pmEntry>(it)->get_value("value");
			std::string str_sym = std::dynamic_pointer_cast<pmEntry>(it)->get_value("symmetric");
			pmTensor_parser tensor_parser{};
			bool symmetric = true;
			if(!str_sym.empty()) {
				symmetric = tensor_parser.string_to_tensor(str_sym, workspace)[0];
			}
			workspace->add_field(it->get_name(), tensor_parser.string_to_tensor_field(data, workspace), symmetric);
		}
	}
	return workspace;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the grid objects wrapped in grid space.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmGrid_space> pmXML_processor::get_grid_space(std::shared_ptr<pmBlock> particle_system, std::shared_ptr<pmWorkspace> workspace, pmDomain const& domain) const {
	std::vector<std::shared_ptr<pmBlock>> block_grid = particle_system->find_block("grid");
	if(block_grid.empty()) {
		ProLog::pLogger::error_msgf("Grid must be defined if no initial condition is provided.\n");
	}
	std::shared_ptr<pmGrid_space> grid_space = std::make_shared<pmGrid_space>();
	for(auto const& gs:block_grid) {
		pmTensor_parser tensor_parser{};
		std::shared_ptr<pmGrid> grid = std::make_shared<pmGrid>();
		pmTensor grid_id = tensor_parser.string_to_tensor(gs->get_entry("gid").back()->get_value("value"),workspace);
		grid->set_grid_id(grid_id[0]);
		grid->set_dimensions(domain.get_dimensions());
		if(!gs->get_entry("file").empty()) {
			std::string str_file_name = gs->get_entry("file").back()->get_value("value");
			grid->set_file_name(str_file_name);
		} else {
			pmTensor position = tensor_parser.string_to_tensor(gs->get_entry("gpos").back()->get_value("value"),workspace);
			pmTensor size = tensor_parser.string_to_tensor(gs->get_entry("gsize").back()->get_value("value"),workspace);
			pmTensor offset = tensor_parser.string_to_tensor(gs->get_entry("goffset").back()->get_value("value"),workspace);
			pmTensor distance = tensor_parser.string_to_tensor(gs->get_entry("gip_dist").back()->get_value("value"),workspace);
			grid->set_position(position);
			grid->set_size(size);
			grid->set_offset(offset);
			grid->set_distance(distance);
		}
		grid->generate();
		grid_space->add_grid(grid);
	}
	return grid_space;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the equation set constructed based on the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmEquation>> pmXML_processor::get_equations(std::shared_ptr<pmWorkspace> workspace) const {
	std::vector<std::shared_ptr<pmBlock>> block_case = block->find_block("case");
	std::vector<std::shared_ptr<pmEquation>> equations;
	for(auto const& eqs:block_case) {
		for(auto const& it:eqs->get_children()) {
			if(it->get_name()=="equations") {
				for(auto const& fcs:it->get_children()) {
					std::unique_ptr<pmEquation_parser> ep{new pmEquation_parser};
					std::unique_ptr<pmTensor_parser> tp{new pmTensor_parser};
					std::shared_ptr<pmEquation> fc = ep->analyse_equation(fcs->get_name(), std::dynamic_pointer_cast<pmEntry>(fcs)->get_value("value"), std::dynamic_pointer_cast<pmEntry>(fcs)->get_value("condition"), workspace);
					if(fc.use_count()>0) { equations.push_back(fc); }
				}
			}
		}
	}
	return equations;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the case constructed based on the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmCase> pmXML_processor::get_case() const {
	// Read initial conditions
	std::shared_ptr<pmCase> ic_case = get_initial_condition_case();
	if(ic_case.use_count()>0) {
		ProLog::pLogger::logf<NAUTICLE_COLOR>("    Initial conditions: from VTK file.\n");
		return ic_case;
	}
	// Read xml case if initial condition not found.
	std::shared_ptr<pmWorkspace> workspace = get_workspace();
	std::vector<std::shared_ptr<pmEquation>> equations = this->get_equations(workspace);
	std::shared_ptr<pmCase> cas = std::make_shared<pmCase>();
	cas->add_workspace(workspace);
	cas->add_equation(equations);
	cas->initialize();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("    Initial conditions: from XML file.\n");
	return cas;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the parameter space constructed based on the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParameter_space> pmXML_processor::get_parameter_space(std::shared_ptr<pmWorkspace> workspace /*=std::make_shared<pmWorkspace>()*/) const {
	std::vector<std::shared_ptr<pmBlock>> parameters = block->find_block("parameter_space");
	std::shared_ptr<pmParameter_space> parameter_space = std::make_shared<pmParameter_space>();
	for(auto const& ps:parameters) {
		std::string str_sim_time = ps->get_entry("simulated_time").back()->get_value("value");
		std::string str_log_time = ps->get_entry("print_interval").back()->get_value("value");
		std::string str_confirm = ps->get_entry("confirm_on_exit").back()->get_value("value");
		std::string str_vtk_format = ps->get_entry("output_format").back()->get_value("value");

		pmTensor_parser tensor_parser{};
		pmTensor sim_time = tensor_parser.string_to_tensor(str_sim_time, workspace);
		pmTensor log_time = tensor_parser.string_to_tensor(str_log_time, workspace);
		pmTensor confirm = tensor_parser.string_to_tensor(str_confirm, workspace);
		pmTensor vtk_format = tensor_parser.string_to_tensor(str_vtk_format, workspace);
		pmTensor file_start = pmTensor{1,1,0};
		if(!ps->get_entry("file_start").empty()) {
			file_start = tensor_parser.string_to_tensor(ps->get_entry("file_start").back()->get_value("value"), workspace);
		}

		if(!sim_time.is_scalar()) { ProLog::pLogger::error_msgf("Simulated time must be scalar!\n"); }
		if(!log_time.is_scalar()) { ProLog::pLogger::error_msgf("Print interval must be scalar!\n"); }
		if(!confirm.is_scalar()) { ProLog::pLogger::error_msgf("Print interval must be scalar!\n"); }
		if(!vtk_format.is_scalar()) { ProLog::pLogger::error_msgf("Output format must be scalar!\n"); }
		if(!file_start.is_scalar()) { ProLog::pLogger::error_msgf("Starting number must be scalar!\n"); }

		parameter_space->add_parameter("simulated_time", sim_time);
		parameter_space->add_parameter("print_interval", log_time);
		parameter_space->add_parameter("confirm_on_exit", confirm);
		parameter_space->add_parameter("output_format", vtk_format);
		parameter_space->add_parameter("file_start", file_start);
	}
	return parameter_space;
}

