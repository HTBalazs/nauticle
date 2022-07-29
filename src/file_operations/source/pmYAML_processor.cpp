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
	std::shared_ptr<pmGrid_space> grid_space = get_grid_space(psys.begin(), psys.end(), workspace, domain.get_dimensions());
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
    	std::string printable = "true";
    	std::string file_name = "";
        for(YAML::const_iterator it = fld_node[i].begin();it!=fld_node[i].end();it++) {
        	if(it->first.as<std::string>()=="symmetric") {
        		symmetric = it->second.as<std::string>();
        	}  else if(it->first.as<std::string>()=="print") {
        		printable = it->second.as<std::string>();
        	} else if(it->first.as<std::string>()=="file") {
        		file_name = it->second.as<std::string>();
        	} else {
				name = it->first.as<std::string>();
				value = it->second.as<std::string>();
        	}
        }
		pmTensor_parser tensor_parser{};
		if(file_name=="") {
        	workspace->add_field(name, tensor_parser.string_to_tensor_field(value, workspace), (bool)tensor_parser.string_to_tensor(symmetric, workspace)[0], (bool)tensor_parser.string_to_tensor(printable, workspace)[0]);
		} else {
        	workspace->add_field(name, tensor_parser.string_to_tensor(value, workspace), (bool)tensor_parser.string_to_tensor(symmetric, workspace)[0], (bool)tensor_parser.string_to_tensor(printable, workspace)[0], file_name);
		}
    }
	return workspace;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the grid objects wrapped in grid space.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmGrid_space> pmYAML_processor::get_grid_space(YAML::iterator it, YAML::const_iterator it_end, std::shared_ptr<pmWorkspace> workspace, size_t const& dims) const {
	std::shared_ptr<pmGrid_space> grid_space = std::make_shared<pmGrid_space>();
	pmTensor gid{1,1,0};
	pmTensor gpos{3,1,0};
	pmTensor gsize{3,1,1};
	pmTensor goffset{3,1,0};
	pmTensor gip_dist{3,1,0.1};
	for(;it!=it_end;it++) {
		if(it->first.as<std::string>()=="grid") {
			std::shared_ptr<pmGrid> grid = std::make_shared<pmGrid>();
			grid->set_dimensions(dims);
			for(YAML::const_iterator grid_nodes=it->second.begin();grid_nodes!=it->second.end();grid_nodes++) {
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
	if(grid_space->is_empty()) {
		ProLog::pLogger::error_msgf("Grid must be defined if no initial condition is provided.\n");
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
	workspace->add_variable("write_case", pmTensor{1,1,0});
	workspace->add_variable("substeps", pmTensor{1,1,0});
	workspace->add_variable("all_steps", pmTensor{1,1,0});
	workspace->add_variable("finished", pmTensor{1,1,0});
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
	auto parameter_space = std::make_shared<pmParameter_space>();
	if(!sim["parameter_space"]) {
		return parameter_space;
	}
	// default values
	std::string simulated_time = "1";
	std::string run_simulation = "true";
	std::string print_interval = "1";
	std::string confirm_on_exit = "false";
	std::string output_format = "ASCII";
	std::string file_start = "0";
	std::string compile_case = "false";
	for(YAML::const_iterator sim_nodes=sim.begin();sim_nodes!=sim.end();sim_nodes++) {
		if(sim_nodes->first.as<std::string>()=="parameter_space") {
			auto expr_parser = std::make_shared<pmExpression_parser>();
			for(YAML::const_iterator parameter_nodes=sim_nodes->second.begin();parameter_nodes!=sim_nodes->second.end();parameter_nodes++) {
				// read from configuration file
				if(parameter_nodes->first.as<std::string>()=="simulated_time") {
					simulated_time = parameter_nodes->second.as<std::string>();
				}
				if(parameter_nodes->first.as<std::string>()=="run_simulation") {
					run_simulation = parameter_nodes->second.as<std::string>();
				}
				if(parameter_nodes->first.as<std::string>()=="print_interval") {
					print_interval = parameter_nodes->second.as<std::string>();
				}
				if(parameter_nodes->first.as<std::string>()=="confirm_on_exit") {
					confirm_on_exit = parameter_nodes->second.as<std::string>();
				}
				if(parameter_nodes->first.as<std::string>()=="output_format") {
					output_format = parameter_nodes->second.as<std::string>();
				}
				if(parameter_nodes->first.as<std::string>()=="file_start") {
					file_start = parameter_nodes->second.as<std::string>();
				}
				if(parameter_nodes->first.as<std::string>()=="compile_case") {
					compile_case = parameter_nodes->second.as<std::string>();
				}
			}
			auto expr_simulated_time = expr_parser->analyse_expression<pmExpression>(simulated_time,workspace);
			auto expr_run_simulation = expr_parser->analyse_expression<pmExpression>(run_simulation,workspace);
			auto expr_log_time = expr_parser->analyse_expression<pmExpression>(print_interval,workspace);
			auto expr_confirm = expr_parser->analyse_expression<pmExpression>(confirm_on_exit,workspace);
			auto expr_output_format = expr_parser->analyse_expression<pmExpression>(output_format,workspace);
			auto expr_file_start = expr_parser->analyse_expression<pmExpression>(file_start,workspace);
			auto expr_compile_case = expr_parser->analyse_expression<pmExpression>(compile_case,workspace);
			parameter_space->add_parameter("simulated_time", expr_simulated_time);
			parameter_space->add_parameter("run_simulation", expr_run_simulation);
			parameter_space->add_parameter("print_interval", expr_log_time);
			parameter_space->add_parameter("confirm_on_exit", expr_confirm);
			parameter_space->add_parameter("output_format", expr_output_format);
			parameter_space->add_parameter("file_start", expr_file_start);
			parameter_space->add_parameter("compile_case", expr_compile_case);
		}
	}
	return parameter_space;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the collection of background functions if specified in the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmBackground>> pmYAML_processor::get_background(std::shared_ptr<pmWorkspace> workspace/*=std::make_shared<pmWorkspace>()*/) const {
	YAML::Node sim = data["simulation"];
	std::vector<std::shared_ptr<pmBackground>> background_list;
	if(!sim["background"]) {
		return background_list;
	}
	// default values
	std::string file_name = "background.vtk";
	std::string interpolate_to = "";
	std::string condition = "true";
	std::string particle_condition = "true";
	for(YAML::const_iterator sim_nodes=sim.begin();sim_nodes!=sim.end();sim_nodes++) {
		if(sim_nodes->first.as<std::string>()=="background") {
			auto background = std::make_shared<pmBackground>();
			auto expr_parser = std::make_shared<pmExpression_parser>();
			for(YAML::const_iterator background_nodes=sim_nodes->second.begin();background_nodes!=sim_nodes->second.end();background_nodes++) {
				// read from configuration file
				if(background_nodes->first.as<std::string>()=="interpolate_to") {
					interpolate_to = background_nodes->second.as<std::string>();
				}
				if(background_nodes->first.as<std::string>()=="file") {
					file_name = background_nodes->second.as<std::string>();
				}
				if(background_nodes->first.as<std::string>()=="condition") {
					condition = background_nodes->second.as<std::string>();
				}
				if(background_nodes->first.as<std::string>()=="prticle_condition") {
					particle_condition =background_nodes->second.as<std::string>();
				}
			}
			auto expr_interpolate_to = expr_parser->analyse_expression<pmField>(interpolate_to,workspace);
			auto expr_condition = expr_parser->analyse_expression<pmExpression>(condition,workspace);
			auto expr_particle_condition = expr_parser->analyse_expression<pmExpression>(particle_condition,workspace);
			background->set_file_name(file_name);
			background->set_field(expr_interpolate_to);
			background->set_condition(expr_condition);
			background->set_particle_condition(expr_condition);
			background->set_particle_system(workspace->get_particle_system());
			background_list.push_back(background);
		}
	}
	return background_list;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the collection of time series functions if specified in the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmTime_series>> pmYAML_processor::get_time_series(std::shared_ptr<pmWorkspace> workspace/*=std::make_shared<pmWorkspace>()*/) const {
	YAML::Node sim = data["simulation"];
	std::vector<std::shared_ptr<pmTime_series>> time_series_list;
	if(!sim["time_series"]) {
		return time_series_list;
	}
	// default values
	std::string file_name = "time_series.txt";
	std::string interpolate_to = "";
	std::string condition = "true";
	for(YAML::const_iterator sim_nodes=sim.begin();sim_nodes!=sim.end();sim_nodes++) {
		if(sim_nodes->first.as<std::string>()=="time_series") {
			auto time_series = std::make_shared<pmTime_series>();
			auto expr_parser = std::make_shared<pmExpression_parser>();
			for(YAML::const_iterator time_series_nodes=sim_nodes->second.begin();time_series_nodes!=sim_nodes->second.end();time_series_nodes++) {
				// read from configuration file
				if(time_series_nodes->first.as<std::string>()=="interpolate_to") {
					interpolate_to = time_series_nodes->second.as<std::string>();
				}
				if(time_series_nodes->first.as<std::string>()=="file") {
					file_name = time_series_nodes->second.as<std::string>();
				}
				if(time_series_nodes->first.as<std::string>()=="condition") {
					condition = time_series_nodes->second.as<std::string>();
				}
			}
			auto expr_interpolate_to = expr_parser->analyse_expression<pmVariable>(interpolate_to,workspace);
			auto expr_condition = expr_parser->analyse_expression<pmExpression>(condition,workspace);
			time_series->set_file_name(file_name);
			time_series->set_variable(expr_interpolate_to);
			time_series->set_condition(expr_condition);
			time_series->set_particle_system(workspace->get_particle_system());
			time_series_list.push_back(time_series);
		}
	}
	return time_series_list;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the collection of background functions if specified in the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmScript>> pmYAML_processor::get_script(std::shared_ptr<pmWorkspace> workspace/*=std::make_shared<pmWorkspace>()*/) const {
	YAML::Node sim = data["simulation"];
	std::vector<std::shared_ptr<pmScript>> script_list;
	if(!sim["script"]) {
		return script_list;
	}
	// default values
	std::string file_name = "script.sh";
	std::string condition = "true";
	for(YAML::const_iterator sim_nodes=sim.begin();sim_nodes!=sim.end();sim_nodes++) {
		if(sim_nodes->first.as<std::string>()=="script") {
			auto script = std::make_shared<pmScript>();
			auto expr_parser = std::make_shared<pmExpression_parser>();
			for(YAML::const_iterator script_nodes=sim_nodes->second.begin();script_nodes!=sim_nodes->second.end();script_nodes++) {
				// read from configuration file
				if(script_nodes->first.as<std::string>()=="file") {
					file_name = script_nodes->second.as<std::string>();
				}
				if(script_nodes->first.as<std::string>()=="condition") {
					condition = script_nodes->second.as<std::string>();
				}
			}
			auto expr_condition = expr_parser->analyse_expression<pmExpression>(condition,workspace);
			script->set_file_name(file_name);
			script->set_condition(expr_condition);
			script_list.push_back(script);
		}
	}
	return script_list;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the collection of particle splitters specified in the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmParticle_splitter>> pmYAML_processor::get_particle_splitter(std::shared_ptr<pmWorkspace> workspace/*=std::make_shared<pmWorkspace>()*/) const {
	YAML::Node sim = data["simulation"];
	std::vector<std::shared_ptr<pmParticle_splitter>> splitter_list;
	if(!sim["splitter"]) {
		return splitter_list;
	}
	// default values
	std::string condition = "false";
	std::string radius_field = "h";
	std::string mass_field = "m";
	std::string smoothing_ratio = "0.9";
	std::string separation_parameter = "0.4";
	std::string daughters = "6";
	std::string parent = "1";
	std::string rotation = "rand(-pi,pi)";
	std::string passive = "";
	std::string active = "";
	for(YAML::const_iterator sim_nodes=sim.begin();sim_nodes!=sim.end();sim_nodes++) {
		if(sim_nodes->first.as<std::string>()=="splitter") {
			auto splitter = std::make_shared<pmParticle_splitter>();
			splitter->set_workspace(workspace);
			auto expr_parser = std::make_shared<pmExpression_parser>();
			for(YAML::const_iterator splitter_nodes=sim_nodes->second.begin();splitter_nodes!=sim_nodes->second.end();splitter_nodes++) {
				// read from configuration file
				if(splitter_nodes->first.as<std::string>()=="condition") {
					condition = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="radius_field") {
					radius_field = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="mass_field") {
					mass_field = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="smoothing_ratio") {
					smoothing_ratio = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="separation_parameter") {
					separation_parameter = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="daughters") {
					daughters = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="parent") {
					parent = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="rotation") {
					rotation = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="passive") {
					passive = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="active") {
					active = splitter_nodes->second.as<std::string>();
				}
			}
			auto expr_condition = expr_parser->analyse_expression<pmExpression>(condition,workspace);
			auto expr_radius = expr_parser->analyse_expression<pmField>(radius_field,workspace);
			auto expr_mass = expr_parser->analyse_expression<pmField>(mass_field,workspace);
			auto expr_smoothing_ratio = expr_parser->analyse_expression<pmExpression>(smoothing_ratio,workspace);
			auto expr_separation_parameter = expr_parser->analyse_expression<pmExpression>(separation_parameter,workspace);
			auto expr_daughter = expr_parser->analyse_expression<pmExpression>(daughters,workspace);
			auto expr_parent = expr_parser->analyse_expression<pmExpression>(parent,workspace);
			auto expr_rotation = expr_parser->analyse_expression<pmExpression>(rotation,workspace);
			splitter->set_condition(expr_condition);
			splitter->set_radius(expr_radius);
			splitter->set_mass(expr_mass);
			splitter->set_smoothing_ratio(expr_smoothing_ratio);
			splitter->set_separation_parameter(expr_separation_parameter);
			splitter->set_daughters(expr_daughter);
			splitter->set_parent(expr_parent);
			splitter->set_rotation(expr_rotation);
			if(passive!="") {
				auto expr_passive = expr_parser->analyse_expression<pmField>(passive,workspace);
				splitter->set_passive(expr_passive);
			}
			if(active!="") {
				auto expr_active = expr_parser->analyse_expression<pmField>(active,workspace);
				splitter->set_active(expr_active);
			}
			splitter_list.push_back(splitter);
		}
	}
	return splitter_list;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the collection of particle mergerss specified in the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmParticle_merger>> pmYAML_processor::get_particle_merger(std::shared_ptr<pmWorkspace> workspace/*=std::make_shared<pmWorkspace>()*/) const {
	YAML::Node sim = data["simulation"];
	std::vector<std::shared_ptr<pmParticle_merger>> merger_list;
	if(!sim["merger"]) {
		return merger_list;
	}
	// default values
	std::string condition = "false";
	std::string neighbor_condition = "true";
	std::string radius_field = "h";
	std::string mass_field = "m";
	std::string velocity_field = "v";
	std::string max_neighbor_distance = "1e12";
	for(YAML::const_iterator sim_nodes=sim.begin();sim_nodes!=sim.end();sim_nodes++) {
		if(sim_nodes->first.as<std::string>()=="merger") {
			auto merger = std::make_shared<pmParticle_merger>();
			merger->set_workspace(workspace);
			auto expr_parser = std::make_shared<pmExpression_parser>();
			for(YAML::const_iterator splitter_nodes=sim_nodes->second.begin();splitter_nodes!=sim_nodes->second.end();splitter_nodes++) {
				// read from configuration file
				if(splitter_nodes->first.as<std::string>()=="condition") {
					condition = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="radius_field") {
					radius_field = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="mass_field") {
					mass_field = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="velocity_field") {
					velocity_field = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="neighbor_condition") {
					neighbor_condition = splitter_nodes->second.as<std::string>();
				}
				if(splitter_nodes->first.as<std::string>()=="max_neighbor_distance") {
					max_neighbor_distance = splitter_nodes->second.as<std::string>();
				}
			}
			auto expr_condition = expr_parser->analyse_expression<pmExpression>(condition,workspace);
			auto expr_neighbor_condition = expr_parser->analyse_expression<pmExpression>(neighbor_condition,workspace);
			auto expr_radius = expr_parser->analyse_expression<pmField>(radius_field,workspace);
			auto expr_mass = expr_parser->analyse_expression<pmField>(mass_field,workspace);
			auto expr_velocity = expr_parser->analyse_expression<pmField>(velocity_field,workspace);
			auto expr_max_neighbor_distance = expr_parser->analyse_expression<pmExpression>(max_neighbor_distance,workspace);
			merger->set_condition(expr_condition);
			merger->set_neighbor_condition(expr_neighbor_condition);
			merger->set_radius(expr_radius);
			merger->set_mass(expr_mass);
			merger->set_velocity(expr_velocity);
			merger->set_max_neighbor_distance(expr_max_neighbor_distance);
			merger_list.push_back(merger);
		}
	}
	return merger_list;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the collection of particle sinks if specified in the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmParticle_sink>> pmYAML_processor::get_particle_sink(std::shared_ptr<pmWorkspace> workspace/*=std::make_shared<pmWorkspace>()*/) const {
	YAML::Node sim = data["simulation"];
	std::vector<std::shared_ptr<pmParticle_sink>> particle_sink_list;
	if(!sim["sink"]) {
		return particle_sink_list;
	}
	// default values
	std::string condition = "false";
	for(YAML::const_iterator sim_nodes=sim.begin();sim_nodes!=sim.end();sim_nodes++) {
		if(sim_nodes->first.as<std::string>()=="sink") {
			auto particle_sink = std::make_shared<pmParticle_sink>();
			auto expr_parser = std::make_shared<pmExpression_parser>();
			for(YAML::const_iterator particle_sink_nodes=sim_nodes->second.begin();particle_sink_nodes!=sim_nodes->second.end();particle_sink_nodes++) {
				// read from configuration file
				if(particle_sink_nodes->first.as<std::string>()=="condition") {
					condition = particle_sink_nodes->second.as<std::string>();
				}
			}
			auto expr_condition = expr_parser->analyse_expression<pmExpression>(condition,workspace);
			particle_sink->set_condition(expr_condition);
			particle_sink->set_workspace(workspace);
			particle_sink_list.push_back(particle_sink);
		}
	}
	return particle_sink_list;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the collection of particle emitters if specified in the configuration file.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmParticle_emitter>> pmYAML_processor::get_particle_emitter(std::shared_ptr<pmWorkspace> workspace/*=std::make_shared<pmWorkspace>()*/) const {
	YAML::Node sim = data["simulation"];
	std::vector<std::shared_ptr<pmParticle_emitter>> particle_emitter_list;
	if(!sim["emitter"]) {
		return particle_emitter_list;
	}
	// default values
	std::string condition = "false";
	for(YAML::iterator sim_nodes=sim.begin();sim_nodes!=sim.end();sim_nodes++) {
		if(sim_nodes->first.as<std::string>()=="emitter") {
			auto particle_emitter = std::make_shared<pmParticle_emitter>();
			std::shared_ptr<pmGrid_space> grid_space = get_grid_space(sim_nodes->second.begin(), sim_nodes->second.end(), workspace, workspace->get_dimensions());
			auto expr_parser = std::make_shared<pmExpression_parser>();
			for(YAML::const_iterator particle_emitter_nodes=sim_nodes->second.begin();particle_emitter_nodes!=sim_nodes->second.end();particle_emitter_nodes++) {
				// read from configuration file
				if(particle_emitter_nodes->first.as<std::string>()=="condition") {
					condition = particle_emitter_nodes->second.as<std::string>();
				}
			}
			auto initializers = get_initializers(sim_nodes->second.begin(), sim_nodes->second.end(), workspace);
			auto expr_condition = expr_parser->analyse_expression<pmExpression>(condition,workspace);
			particle_emitter->set_condition(expr_condition);
			particle_emitter->set_workspace(workspace);
			particle_emitter->add_grid(grid_space);
			for(auto const& it:initializers) {
				particle_emitter->add_initializer(it);
			}
			particle_emitter_list.push_back(particle_emitter);
		}
	}
	return particle_emitter_list;
}

std::vector<pmInitializer> pmYAML_processor::get_initializers(YAML::iterator it, YAML::const_iterator it_end, std::shared_ptr<pmWorkspace> workspace) const {
	std::vector<pmInitializer> initializers;
	for(;it!=it_end;it++) {
		if(it->first.as<std::string>()=="initializer") {
				std::cout << "ok" << std::endl;
	        for(YAML::const_iterator in = it->second.begin();in!=it->second.end();in++) {
				auto expr_parser = std::make_shared<pmExpression_parser>();
				pmInitializer init;
				std::string name;
		    	std::string expression;
				name = in->first.as<std::string>();
				expression = in->second.as<std::string>();
		        init.field = std::dynamic_pointer_cast<pmField>(workspace->get_instance(name).lock());
		        init.expression = expr_parser->analyse_expression<pmExpression>(expression,workspace);
				initializers.push_back(init);
	        }
		}
	}
	return initializers;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the grid objects wrapped in grid space.
/////////////////////////////////////////////////////////////////////////////////////////
void pmYAML_processor::get_springs(std::shared_ptr<pmWorkspace> workspace) const {
	YAML::Node psys = data["simulation"]["case"]["workspace"]["particle_system"];
	// Read domain
	YAML::Node dm = psys["springs"];
	std::string str_connectivity = dm["connectivity"].as<std::string>();
	std::string str_initial_length = dm["initial_length"].as<std::string>();
	std::string str_strength = dm["strength"].as<std::string>();
	std::string str_damping = dm["damping"].as<std::string>();

	pmData_reader data_reader;
	data_reader.set_file_name(str_connectivity);
	data_reader.read_file(2);
	std::vector<pmTensor> connectivity = data_reader.get_data();

	data_reader.set_file_name(str_initial_length);
	data_reader.read_file(1);
	std::vector<pmTensor> initial_length = data_reader.get_data();

	data_reader.set_file_name(str_strength);
	data_reader.read_file(1);
	std::vector<pmTensor> strength = data_reader.get_data();

	data_reader.set_file_name(str_damping);
	data_reader.read_file(1);
	std::vector<pmTensor> damping = data_reader.get_data();

	std::vector<std::shared_ptr<pmExpression>> interactions = workspace->get_interactions();
	for(auto& it:interactions) {
		auto springs = std::dynamic_pointer_cast<pmSpring>(it);
		if(springs) {
			springs->update();
			for(int i=0; i<connectivity.size(); i++) {
				std::vector<double> pair_data{initial_length[i][0],strength[i][0],damping[i][0]};
				springs->add_pair(connectivity[i][0],connectivity[i][1],pair_data);
			}
		}
	}
}




























