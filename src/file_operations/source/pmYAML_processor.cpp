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

#include "pmYAML_processor.h"
#include "pmTensor_parser.h"
#include "pmVTK_reader.h"
#include "prolog/pLogger.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the default configuration file name.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ std::string pmYAML_processor::get_default_file_name() {
	return "config.yaml";
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads YAML configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmYAML_processor::read_file(std::string const& file_name) {
	data = YAML::LoadFile(file_name);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a workspace constructed based on the initial condition.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmCase> pmYAML_processor::get_initial_condition_case() const {
	pmVTK_reader reader;
	YAML::Node ic = data["simulation"]["initial_condition"];
	if(ic) {
		if(ic["file"]) {
			reader.set_file_name(ic["file"].as<std::string>());
		} else {
			ProLog::pLogger::error_msgf("No VTK-file name defined.");
		}
		reader.update();
		return reader.get_case();
	}
	return std::shared_ptr<pmCase>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the workspace constructed based on the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmWorkspace> pmYAML_processor::get_workspace() const {
	std::shared_ptr<pmWorkspace> workspace = std::make_shared<pmWorkspace>();
	YAML::Node ws = data["simulation"]["case"]["workspace"];
	// Read constants
	YAML::Node const_node = data["simulation"]["case"]["workspace"]["constants"];
    for(int i=0; i<const_node.size(); i++) {
    	std::string name;
    	std::string value;
        for(YAML::const_iterator it = const_node[i].begin();it!=const_node[i].end();it++) {
			name = it->first.as<std::string>();
			value = it->second.as<std::string>();
        }
		pmTensor_parser tensor_parser{};
        workspace->add_constant(name, tensor_parser.string_to_tensor(value, workspace));
    }
	// Read variables
	YAML::Node var_node = data["simulation"]["case"]["workspace"]["variables"];
    for(int i=0; i<var_node.size(); i++) {
    	std::string name;
    	std::string value;
        for(YAML::const_iterator it = var_node[i].begin();it!=var_node[i].end();it++) {
			name = it->first.as<std::string>();
			value = it->second.as<std::string>();
        }
		pmTensor_parser tensor_parser{};
        workspace->add_variable(name, tensor_parser.string_to_tensor(value, workspace));
    }
	if(!workspace->is_existing("dt")) {
		workspace->add_variable("dt", pmTensor{1,1,0.001});
	}
	// Read particle system
	YAML::Node psys = data["simulation"]["case"]["workspace"]["particle_system"];
	// Read domain
	YAML::Node dm = psys["domain"];
	pmTensor_parser tensor_parser{};
	pmTensor cell_size = tensor_parser.string_to_tensor(dm["cell_size"].as<std::string>(), workspace);
	pmTensor minimum = tensor_parser.string_to_tensor(dm["minimum"].as<std::string>(), workspace);
	pmTensor maximum = tensor_parser.string_to_tensor(dm["maximum"].as<std::string>(), workspace);
	pmTensor boundary = tensor_parser.string_to_tensor(dm["boundary"].as<std::string>(), workspace);
	pmDomain domain = pmDomain{minimum, maximum, cell_size, boundary};
	// Read grids
	std::shared_ptr<pmGrid_space> grid_space = get_grid_space(psys, workspace, domain);
	std::shared_ptr<pmGrid> tmp = grid_space->get_merged_grid();
	std::vector<pmTensor> particle_grid = tmp->get_grid();
	workspace->add_particle_system(particle_grid, domain);
	workspace->delete_instance("gid");
	workspace->add_field("gid", grid_space->get_grid_id_field());
	// Read fields
	YAML::Node fld_node = data["simulation"]["case"]["workspace"]["fields"];
    for(int i=0; i<fld_node.size(); i++) {
    	std::string name;
    	std::string value;
    	std::string symmetric = "true";
        for(YAML::const_iterator it = fld_node[i].begin();it!=fld_node[i].end();it++) {
        	if(it->first.as<std::string>()=="symmetric") {
        		symmetric = it->second.as<std::string>();
        	} else {
				name = it->first.as<std::string>();
				value = it->second.as<std::string>();
        	}
        }
		pmTensor_parser tensor_parser{};
        workspace->add_field(name, tensor_parser.string_to_tensor_field(value, workspace), (bool)tensor_parser.string_to_tensor(symmetric, workspace)[0]);
    }
	return workspace;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the grid objects wrapped in grid space.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmGrid_space> pmYAML_processor::get_grid_space(YAML::Node particle_system, std::shared_ptr<pmWorkspace> workspace, pmDomain const& domain) const {
	if(!particle_system["grid"]) {
		ProLog::pLogger::error_msgf("Grid must be defined if no initial condition is provided.\n");
	}
	std::shared_ptr<pmGrid_space> grid_space = std::make_shared<pmGrid_space>();
	pmTensor gid{1,1,0};
	pmTensor gpos{3,1,0};
	pmTensor gsize{3,1,1};
	pmTensor goffset{3,1,0};
	pmTensor gip_dist{3,1,0.1};
	for(YAML::const_iterator ps_nodes=particle_system.begin();ps_nodes!=particle_system.end();ps_nodes++) {
		if(ps_nodes->first.as<std::string>()=="grid") {
			std::shared_ptr<pmGrid> grid = std::make_shared<pmGrid>();
			grid->set_dimensions(domain.get_dimensions());
			for(YAML::const_iterator grid_nodes=ps_nodes->second.begin();grid_nodes!=ps_nodes->second.end();grid_nodes++) {
				pmTensor_parser tensor_parser{};
				if(grid_nodes->first.as<std::string>()=="gid") {
					gid = tensor_parser.string_to_tensor(grid_nodes->second.as<std::string>(),workspace);
					grid->set_grid_id(gid);
				}
				if(grid_nodes->first.as<std::string>()=="file") {
					grid->set_file_name(grid_nodes->second.as<std::string>());
				}
				if(grid_nodes->first.as<std::string>()=="gpos") {
					gpos = tensor_parser.string_to_tensor(grid_nodes->second.as<std::string>(),workspace);
					grid->set_position(gpos);
				}
				if(grid_nodes->first.as<std::string>()=="gsize") {
					gsize = tensor_parser.string_to_tensor(grid_nodes->second.as<std::string>(),workspace);
					grid->set_size(gsize);
				}
				if(grid_nodes->first.as<std::string>()=="goffset") {
					goffset = tensor_parser.string_to_tensor(grid_nodes->second.as<std::string>(),workspace);
					grid->set_offset(goffset);
				}
				if(grid_nodes->first.as<std::string>()=="gip_dist") {
					gip_dist = tensor_parser.string_to_tensor(grid_nodes->second.as<std::string>(),workspace);
					grid->set_distance(gip_dist);
				}
			}
			grid->generate();
			grid_space->add_grid(grid);
		}
	}
	return grid_space;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the equation set constructed based on the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmEquation>> pmYAML_processor::get_equations(std::shared_ptr<pmWorkspace> workspace) const {
    YAML::Node eqs_node = data["simulation"]["case"]["equations"];
	std::vector<std::shared_ptr<pmEquation>> eqs;
    for(int i=0; i<eqs_node.size(); i++) {
    	std::string name;
    	std::string value;
    	std::string condition = "true";
        for(YAML::const_iterator it = eqs_node[i].begin();it!=eqs_node[i].end();it++) {
        	if(it->first.as<std::string>()=="condition") {
        		condition = it->second.as<std::string>();
        	} else {
				name = it->first.as<std::string>();
				value = it->second.as<std::string>();
        	}
        }
        std::unique_ptr<pmEquation_parser> ep{new pmEquation_parser};
		std::shared_ptr<pmEquation> eq = ep->analyse_equation(name, value, condition, workspace);
		if(eq.use_count()>0) { eqs.push_back(eq); }
    }
	return eqs;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the case constructed based on the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmCase> pmYAML_processor::get_case() const {
	// Read initial conditions
	std::shared_ptr<pmCase> ic_case = get_initial_condition_case();
	if(ic_case.use_count()>0) {
		ProLog::pLogger::logf<NAUTICLE_COLOR>("    Initial conditions: from VTK file.\n");
		return ic_case;
	}
	// Read YAML case if initial condition not found.
	std::shared_ptr<pmWorkspace> workspace = get_workspace();
	std::vector<std::shared_ptr<pmEquation>> equations = this->get_equations(workspace);
	std::shared_ptr<pmCase> cas = std::make_shared<pmCase>();
	cas->add_workspace(workspace);
	cas->add_equation(equations);
	cas->initialize();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("    Initial conditions: from YAML file.\n");
	return cas;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the parameter space constructed based on the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParameter_space> pmYAML_processor::get_parameter_space(std::shared_ptr<pmWorkspace> workspace /*=std::make_shared<pmWorkspace>()*/) const {
	YAML::Node sim = data["simulation"];
	std::shared_ptr<pmParameter_space> parameter_space = std::make_shared<pmParameter_space>();
	pmTensor sim_time{1,1,1.0};
	pmTensor log_time{1,1,0.1};
	pmTensor confirm{1,1,0.0};
	pmTensor vtk_format{1,1,0.0};
	pmTensor file_start{1,1,0.0};
	for(YAML::const_iterator sim_nodes=sim.begin();sim_nodes!=sim.end(); sim_nodes++) {
		if(sim_nodes->first.as<std::string>()=="parameter_space") {
			for(YAML::const_iterator param_nodes=sim_nodes->second.begin();param_nodes!=sim_nodes->second.end();param_nodes++) {

				pmTensor_parser tensor_parser{};
				if(param_nodes->first.as<std::string>()=="simulated_time") {
					std::string str_sim_time = param_nodes->second.as<std::string>();
					pmTensor tmp = tensor_parser.string_to_tensor(str_sim_time, workspace);
					if(!tmp.is_scalar()) {
						ProLog::pLogger::warning_msgf("Simulated time must be scalar! Default value is applied.\n");
					} else {
						sim_time = tmp;
					}
				}
				if(param_nodes->first.as<std::string>()=="print_interval") {
					std::string str_log_time = param_nodes->second.as<std::string>();
					pmTensor tmp = tensor_parser.string_to_tensor(str_log_time, workspace);
					if(!tmp.is_scalar()) {
						ProLog::pLogger::warning_msgf("Print interval must be scalar! Default value is applied.\n");
					} else {
						log_time = tmp;
					}
				}
				if(param_nodes->first.as<std::string>()=="confirm_on_exit") {
					std::string str_confirm = param_nodes->second.as<std::string>();
					pmTensor tmp= tensor_parser.string_to_tensor(str_confirm, workspace);
					if(!tmp.is_scalar()) {
						ProLog::pLogger::warning_msgf("Print interval must be scalar! Default value is applied.\n");
					} else {
						confirm = tmp;
					}
				}
				if(param_nodes->first.as<std::string>()=="output_format") {
					std::string str_vtk_format = param_nodes->second.as<std::string>();
					pmTensor tmp = tensor_parser.string_to_tensor(str_vtk_format, workspace);
					if(!tmp.is_scalar()) {
						ProLog::pLogger::warning_msgf("Output format must be scalar! Default value is applied.\n");
					} else {
						vtk_format = tmp;
					}
				}
				if(param_nodes->first.as<std::string>()=="file_start") {
					std::string str_file_start = param_nodes->second.as<std::string>();
					pmTensor tmp = tensor_parser.string_to_tensor(str_file_start, workspace);
					if(!tmp.is_scalar()) {
						ProLog::pLogger::warning_msgf("Starting number must be scalar! Default value is applied.\n");
					} else {
						file_start = tmp;
					}
				}
			}
		}
	}
	parameter_space->add_parameter("simulated_time", sim_time);
	parameter_space->add_parameter("print_interval", log_time);
	parameter_space->add_parameter("confirm_on_exit", confirm);
	parameter_space->add_parameter("output_format", vtk_format);
	parameter_space->add_parameter("file_start", file_start);
	return parameter_space;
}



std::vector<std::shared_ptr<pmParticle_splitter>> pmYAML_processor::get_particle_splitter(std::shared_ptr<pmWorkspace> workspace/*=std::make_shared<pmWorkspace>()*/) const {
	YAML::Node sim = data["simulation"];
	std::vector<std::shared_ptr<pmParticle_splitter>> splitter_list;
	if(!sim["splitter"]) {
		return splitter_list;
	}
	std::string condition = "false";
	std::string radius_field = "h";
	for(YAML::const_iterator sim_nodes=sim.begin();sim_nodes!=sim.end();sim_nodes++) {
		if(sim_nodes->first.as<std::string>()=="splitter") {
			std::shared_ptr<pmParticle_splitter> splitter = std::make_shared<pmParticle_splitter>();
			splitter->set_workspace(workspace);
			for(YAML::const_iterator splitter_nodes=sim_nodes->second.begin();splitter_nodes!=sim_nodes->second.end();splitter_nodes++) {
				std::shared_ptr<pmExpression_parser> expr_parser = std::make_shared<pmExpression_parser>();
				if(splitter_nodes->first.as<std::string>()=="condition") {
					condition = splitter_nodes->second.as<std::string>();
					std::shared_ptr<pmExpression> expr = expr_parser->analyse_expression<pmExpression>(condition,workspace);
					splitter->set_condition(expr);
				}
				if(splitter_nodes->first.as<std::string>()=="radius_field") {
					radius_field = splitter_nodes->second.as<std::string>();
					std::shared_ptr<pmField> expr = expr_parser->analyse_expression<pmField>(radius_field,workspace);
					splitter->set_radius(expr);
				}
			}
			splitter_list.push_back(splitter);
		}
	}
	return splitter_list;
}