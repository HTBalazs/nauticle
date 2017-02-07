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

#include "pmXML_processor.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a workspace constructed based on the initial condition.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmFunction_space> pmXML_processor::get_initial_condition_function_space() const {
	pmVTK_reader reader;
	std::vector<std::shared_ptr<pmBlock>> defs = block->find_block("initial_condition");
	if(!defs.empty()) {
		std::vector<std::shared_ptr<pmBlock>> file = defs.back()->find_block("file");
		if(!file.empty()) {
			std::string file_name = std::dynamic_pointer_cast<pmEntry>(file.back())->get_value("value");
			reader.set_file_name(file_name);
		}
		reader.update();
		return reader.get_function_space();
	}
	return std::shared_ptr<pmFunction_space>();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the workspace constructed based on the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmWorkspace> pmXML_processor::get_workspace() const {
	std::shared_ptr<pmWorkspace> workspace = std::make_shared<pmWorkspace>();
	std::vector<std::shared_ptr<pmBlock>> block_function_space = block->find_block("function_space");
	std::shared_ptr<pmBlock> block_workspace = block_function_space.back()->find_block("workspace").back();
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
		std::string str_min = ps->get_entry("domain").back()->get_value("min");
		std::string str_max = ps->get_entry("domain").back()->get_value("max");
		std::string str_cell_size = ps->get_entry("cell_size").back()->get_value("value");

		pmTensor_parser tensor_parser{};
		pmTensor minimum = tensor_parser.string_to_tensor(str_min, workspace);
		pmTensor maximum = tensor_parser.string_to_tensor(str_max, workspace);
		pmTensor cell_size = tensor_parser.string_to_tensor(str_cell_size, workspace);
		if(!cell_size.is_scalar()) { pLogger::error_msgf("Cell size must be scalar!\n"); }
		std::shared_ptr<pmGrid_space> grid_space = get_grid_space(ps,workspace);
		std::shared_ptr<pmGrid> tmp = grid_space->get_merged_grid();
		std::vector<pmTensor> particle_grid = tmp->get_grid();
		workspace->add_particle_system(particle_grid, pmDomain{minimum, maximum, cell_size[0]});
		workspace->delete_instance("gid");
		workspace->add_field("gid", grid_space->get_grid_id_field());
	}
	// Read fields
	std::vector<std::shared_ptr<pmBlock>> flds = block_workspace->find_block("fields");
	for(auto const& fields:flds) {
		for(auto const& it:fields->get_children()) {
			std::string data = std::dynamic_pointer_cast<pmEntry>(it)->get_value("value");
			pmTensor_parser tensor_parser{};
			workspace->add_field(it->get_name(), tensor_parser.string_to_tensor_field(data, workspace));
		}
	}
	return workspace;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the grid objects wrapped in grid space.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmGrid_space> pmXML_processor::get_grid_space(std::shared_ptr<pmBlock> particle_system, std::shared_ptr<pmWorkspace> workspace) const {
	std::vector<std::shared_ptr<pmBlock>> block_grid = particle_system->find_block("grid");
	if(block_grid.empty()) {
		pLogger::error_msgf("Grid must be defined if no initial condition is provided.\n");
	}
	std::shared_ptr<pmGrid_space> grid_space = std::make_shared<pmGrid_space>();
	for(auto const& gs:block_grid) {
		pmTensor_parser tensor_parser{};
		pmTensor grid_id = tensor_parser.string_to_tensor(gs->get_entry("gid").back()->get_value("value"),workspace);
		pmTensor position = tensor_parser.string_to_tensor(gs->get_entry("gpos").back()->get_value("value"),workspace);
		pmTensor size = tensor_parser.string_to_tensor(gs->get_entry("gsize").back()->get_value("value"),workspace);
		pmTensor offset = tensor_parser.string_to_tensor(gs->get_entry("goffset").back()->get_value("value"),workspace);
		pmTensor distance = tensor_parser.string_to_tensor(gs->get_entry("gip_dist").back()->get_value("value"),workspace);
		std::shared_ptr<pmGrid> grid = std::make_shared<pmGrid>();
		grid->set_position(position);
		grid->set_size(size);
		grid->set_offset(offset);
		grid->set_distance(distance);
		grid->set_grid_id(grid_id[0]);
		grid->generate();
		grid_space->add_grid(grid);
	}
	return grid_space;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the function set constructed based on the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmFunction>> pmXML_processor::get_functions(std::shared_ptr<pmWorkspace> workspace) const {
	std::vector<std::shared_ptr<pmBlock>> block_function_space = block->find_block("function_space");
	std::vector<std::shared_ptr<pmFunction>> functions;
	for(auto const& eqs:block_function_space) {
		for(auto const& it:eqs->get_children()) {
			if(it->get_name()=="functions") {
				for(auto const& fcs:it->get_children()) {
					std::unique_ptr<pmFunction_parser> ep{new pmFunction_parser};
					std::shared_ptr<pmFunction> fc = ep->analyse_function(fcs->get_name(), std::dynamic_pointer_cast<pmEntry>(fcs)->get_value("value"), workspace);
					if(fc.use_count()>0) { functions.push_back(fc); }
				}
			}
		}
	}
	return functions;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the function space constructed based on the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmFunction_space> pmXML_processor::get_function_space() const {
	// Read initial conditions
	std::shared_ptr<pmFunction_space> ic_function_space = get_initial_condition_function_space();
	if(ic_function_space.use_count()>0) {
		pLogger::logf<COLOR>("    Initial conditions: from VTK file.\n");
		return ic_function_space;
	}
	// Read xml function space if initial condition not found.
	std::shared_ptr<pmWorkspace> workspace = get_workspace();
	std::vector<std::shared_ptr<pmFunction>> functions = this->get_functions(workspace);
	std::shared_ptr<pmFunction_space> function_space = std::make_shared<pmFunction_space>();
	function_space->add_workspace(workspace);
	function_space->add_function(functions);
	function_space->initialize();
	pLogger::logf<COLOR>("    Initial conditions: from XML file.\n");
	return function_space;
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
		if(!sim_time.is_scalar()) { pLogger::error_msgf("Simulated time must be scalar!\n"); }
		if(!log_time.is_scalar()) { pLogger::error_msgf("Print interval must be scalar!\n"); }
		if(!confirm.is_scalar()) { pLogger::error_msgf("Print interval must be scalar!\n"); }
		if(!vtk_format.is_scalar()) { pLogger::error_msgf("Output format must be scalar!\n"); }

		parameter_space->add_parameter("simulated_time", sim_time);
		parameter_space->add_parameter("print_interval", log_time);
		parameter_space->add_parameter("confirm_on_exit", confirm);
		parameter_space->add_parameter("output_format", vtk_format);
	}
	return parameter_space;
}

