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

#include "pmCase.h"
#include "pmSph_operator.h"
#ifdef JELLYFISH
#include "pmConstant.h"
#include "pmRuntime_interface.h"
#include "pmDirectives.h"
#include "c2c/c2CPP_source_file.h"
#include "c2c/c2CPP_class.h"
#include "c2c/c2Compiler.h"
#include "c2c/c2Loader.h"
#include "c2c/c2CPP_declaration.h"
#include <Eigen/Eigen>
#endif // JELLYFISH

using namespace Nauticle;
using namespace ProLog;
using namespace c2c;

/*static*/ std::string const pmCase::session_name = "Binary_case";

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmCase::pmCase(pmCase const& other) {
	this->workspace = other.workspace->clone();
	for(auto const& it:other.equations) {
		this->equations.push_back(it->clone());
	}
	assign_particle_system_to_equations();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmCase& pmCase::operator=(pmCase const& rhs) {
	if(this!=&rhs) {
		this->workspace = rhs.workspace->clone();
		for(auto const& it:rhs.equations) {
			this->equations.push_back(it->clone());
		}
		assign_particle_system_to_equations();
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmCase::pmCase(pmCase&& other) {
	this->workspace = std::move(other.workspace);
	this->equations = std::move(other.equations);
	assign_particle_system_to_equations();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmCase& pmCase::operator=(pmCase&& rhs) {
	if(this!=&rhs) {
		this->workspace = std::move(rhs.workspace);
		this->equations = std::move(rhs.equations);
		assign_particle_system_to_equations();
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements the identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmCase::operator==(pmCase const& rhs) const {
	bool identical = *(this->workspace)==*(rhs.workspace);
	if(!identical || this->equations.size()!=rhs.equations.size()) { return false; }
	for(int i=0; i<this->equations.size(); i++) {
		identical *= this->equations[i] == rhs.equations[i];
	}
	return identical;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements the non-identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmCase::operator!=(pmCase const& rhs) const {
	return !this->operator==(rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds the given workspace to the solver. Previously stored workspace is destroyed.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::add_workspace(std::shared_ptr<pmWorkspace> ws) {
	workspace = ws;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Recives a equation and pushes it to the equations vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::add_equation(std::shared_ptr<pmEquation> func) {
	for(auto const& it:equations) {
		if(it->get_name()==func->get_name()){
			pLogger::warning_msg("Equation \"%s\" is already existing.\n",func->get_name().c_str());
			return;
		}
	}
	equations.push_back(func);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Recieves a vector of equations and pushes it to the end of the equations vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::add_equation(std::vector<std::shared_ptr<pmEquation>> func) {
	for(auto const& it:func) {
		this->add_equation(it);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the content of the pmCase object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::print() const {
	pLogger::headerf<LBL>("Case");
	workspace->print();
	int f=0;
	pLogger::titlef<LMA>("Equations");
	for(auto const& it:equations) {
		f++;
		pLogger::logf<YEL>("         %i) ", f);
		it->print();
		pLogger::logf("\n");
	}
	if(f==0) {
		pLogger::logf<WHT>("            < empty >\n");
	}
	for(auto const& it:particle_modifier) {
		it->print();
	}
	for(auto const& it:background) {
		it->print();
	}
	for(auto const& it:time_series) {
		it->print();
	}
	for(auto const& it:output) {
		it->print();
	}
	if(rbsys.use_count()>0) {
		rbsys->print();
	}
	pLogger::footerf<LBL>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the workspace. 
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmWorkspace> pmCase::get_workspace() const {
	return workspace;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the equations.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmEquation>> pmCase::get_equations() const {
	return equations;
}

std::vector<std::shared_ptr<pmBackground>> const& pmCase::get_background() const {
	return background;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Solves the equation with the given name or all equations in order if name is empty.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmCase::solve(double const& current_time, size_t const& num_threads, std::string const& name/*=""*/) {
	this->update_particle_modifiers(num_threads);
	this->update_background_fields(workspace->get_instance("dt").lock()->evaluate(0)[0]);
	this->update_time_series_variables(current_time);
	this->update_rigid_bodies(workspace->get_instance("dt").lock()->evaluate(0)[0]);
	bool success = workspace->update();
	if(!success) {
		return false;
	}
	if(name=="") {
		for(auto const& it:equations) {
			if(it->is_interaction()) {
				success = workspace->update();
				if(!success) { return false; }
			}
			it->solve(num_threads);
			if(it->get_lhs()->get_name()=="r") {
				success = workspace->update();
				if(!success) { return false; }
			}
		}
	} else {
		for(auto const& it:equations) {
			if(it->get_name()==name) {
				if(it->is_interaction() && it->get_lhs()->get_name()=="r") {
					workspace->update();
				}
				it->solve(num_threads);
			}
		}
		pLogger::warning_msg("No equation found with name \"%s\"\n.", name.c_str());
	}
	this->update_output();
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Assigns the particle system of the workspace to all equations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::assign_particle_system_to_equations() {
	std::shared_ptr<pmParticle_system> psys = workspace->get_particle_system();
	for(auto const& it:equations) {
		it->assign_particle_system(psys);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy tof the object. 
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmCase> pmCase::clone() const {
	return std::make_shared<pmCase>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Merge cases.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::merge(std::shared_ptr<pmCase> const& other) {
	this->workspace->merge(other->workspace);
	size_t i=0;
	for(auto const& it:other->equations) {
		this->add_equation(it);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates particle splitters and mergers.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::update_particle_modifiers(size_t const& num_threads) {
	workspace->update();
	for(auto& it:particle_modifier) {
		it->update(num_threads);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates background interpolations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::update_background_fields(double const& dt) {
	for(auto& it:background) {
		it->update(dt);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates background interpolations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::update_rigid_bodies(double const& time_step) {
	if(rbsys.use_count()>0) {
		rbsys->update(time_step);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates background interpolations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::update_output() {
	for(auto const& it:output) {
		it->update();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates time sereies interpolations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::update_time_series_variables(double const& t) {
	for(auto& it:time_series) {
		it->update(t);
	}
}

void pmCase::add_output(std::shared_ptr<pmOutput> outp) {
	output.push_back(outp);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Assigns pmParticle_system object in the pmWorkspace to all equations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::initialize() {
	this->assign_particle_system_to_equations();
}

void pmCase::add_particle_modifier(std::shared_ptr<pmParticle_modifier> pmod) {
	particle_modifier.push_back(pmod);
}

void pmCase::add_background(std::shared_ptr<pmBackground> bckg) {
	background.push_back(bckg);
}

void pmCase::add_time_series(std::shared_ptr<pmTime_series> ts) {
	time_series.push_back(ts);
}

void pmCase::add_rigid_body_system(std::shared_ptr<pmRigid_body_system> rbs) {
	rbsys = rbs;
}

#ifdef JELLYFISH
c2CPP_header_file pmCase::generate_header(std::string const& hname) const {
	c2CPP_header_file header{hname};
	header.add_namespace("Nauticle");
	header.add_include("nauticle/pmRuntime_interface.h",false);
	header.add_include("nauticle/pmWorkspace.h",false);
	header.add_include("nauticle/pmKernel_includes.h",false);
	header.add_include("nauticle/pmHistory.h",false);
	header.add_include("nauticle/pmDirectives.h",false);
	header.add_include("nauticle/pmHelper_functions.h",false);
	header.add_include("nauticle/pmRandom.h",false);
	header.add_include("Eigen/Eigen",true);
	header.add_include("string",true);
	header.add_include("vector",true);
	header.add_include("memory",true);
	header.add_include("cmath",true);
	header.add_include("utility",true);
	header.add_include("numeric",true);
	header.add_include("algorithm",true);
	header.add_include("thread",true);
	header.add_include("type_traits",true);
	header.add_include("functional",true);
	return header;
}

c2Cmake_generator pmCase::generate_cmake_file(std::string const& session_name) const {
	c2Cmake_generator cmakegen;
	cmakegen.set_directory(session_name+"/");
    cmakegen.set_project_name(session_name);
    cmakegen.set_cxx_standard("17");
    cmakegen.set_cxx_flags("-pthread -O3 -fPIC");
    cmakegen.add_link_dir("~/local/lib/c2c/");
    cmakegen.add_link_dir("~/local/lib/prolog/");
    cmakegen.add_link_dir("~/local/lib/commonutils/");
    cmakegen.add_link_dir("~/local/lib/nauticle/");
    cmakegen.add_include_dir("~/local/include/");
    cmakegen.add_dependency("c2c");
    cmakegen.add_dependency("prolog");
    cmakegen.add_dependency("commonutils");
    cmakegen.add_dependency("nauticle");
    cmakegen.add_definitions("-DJELLYFISH=1");
    cmakegen.generate_cmake_file();
    return cmakegen;
}

void pmCase::generate_binary_case() const {
	std::shared_ptr<pmParticle_system> psys = this->workspace->get_particle_system();
	size_t dimensions = psys->get_dimensions();

	c2CPP_header_file header = generate_header(session_name);
	header.get_namespace("Nauticle").add_definition(c2CPP_declaration{"using", "Scalar", false, "", "double;"});
	header.get_namespace("Nauticle").add_definition(c2CPP_declaration{"using", "Vector", false, "", dimensions>1?"Eigen::Vector"+std::to_string(dimensions)+"d;":"double;"});
	header.get_namespace("Nauticle").add_definition(c2CPP_declaration{"using", "Matrix", false, "", dimensions>1?"Eigen::Matrix"+std::to_string(dimensions)+"d;":"double;"});

	c2CPP_class binary_case{session_name};
	binary_case.add_interface("pmRuntime_interface");
	binary_case.add_member_type(c2CPP_class_member_type{c2CPP_declaration{"std::vector<size_t>", "del", false, "", ""},PRIVATE});
	for(auto const& it:this->workspace->get_definitions()) {
		binary_case.add_member_type(c2CPP_class_member_type{it->generate_cpp_declaration(),PRIVATE});
	}
	binary_case.add_member_type(c2CPP_class_member_type{c2CPP_declaration{"std::shared_ptr<pmWorkspace>", "workspace", false, "", "std::make_shared<pmWorkspace>()"},PRIVATE});
	binary_case.add_member_type(c2CPP_class_member_type{c2CPP_declaration{"Vector", "minimum", false, "", psys->get_minimum().get_cpp_initialization()},PRIVATE});
	binary_case.add_member_type(c2CPP_class_member_type{c2CPP_declaration{"Vector", "maximum", false, "", psys->get_maximum().get_cpp_initialization()},PRIVATE});
	binary_case.add_member_type(c2CPP_class_member_type{c2CPP_declaration{"Vector", "cell_size", false, "", psys->get_cell_size().get_cpp_initialization()},PRIVATE});
	binary_case.add_member_type(c2CPP_class_member_type{c2CPP_declaration{"Vector", "boundary", false, "", psys->get_boundary().get_cpp_initialization()},PRIVATE});
	binary_case.add_member_type(c2CPP_class_member_type{c2CPP_declaration{"bool", "up_to_date", false, "", "false"},PRIVATE});
	binary_case.add_member_type(c2CPP_class_member_type{c2CPP_declaration{"std::vector<int>", "pidx", false, "", ""},PRIVATE});
	binary_case.add_member_type(c2CPP_class_member_type{c2CPP_declaration{"std::vector<int>", "cidx", false, "", ""},PRIVATE});
	switch(dimensions) {
		case 1: binary_case.add_member_type(c2CPP_class_member_type{c2CPP_declaration{"std::vector<int>", "cell_iterator", false, "", "{-1,0,1}"},PRIVATE}); break;
		case 2: binary_case.add_member_type(c2CPP_class_member_type{c2CPP_declaration{"std::vector<Vector>", "cell_iterator", false, "", "{Vector{-1,-1},Vector{0,-1},Vector{1,-1},Vector{-1,0},Vector{0,0},Vector{1,0},Vector{-1,1},Vector{0,1},Vector{1,1}}"},PRIVATE}); break;
		default:
		case 3: binary_case.add_member_type(c2CPP_class_member_type{c2CPP_declaration{"std::vector<Vector>", "cell_iterator", false, "", "{Vector{-1,-1,-1},Vector{-1,-1,0},Vector{-1,-1,1},Vector{-1,0,-1},Vector{-1,0,0},Vector{-1,0,1},Vector{-1,1,-1},Vector{-1,1,0},Vector{-1,1,1},Vector{0,-1,-1},Vector{0,-1,0},Vector{0,-1,1},Vector{0,0,-1},Vector{0,0,0},Vector{0,0,1},Vector{0,1,-1},Vector{0,1,0},Vector{0,1,1},Vector{1,-1,-1},Vector{1,-1,0},Vector{1,-1,1},Vector{1,0,-1},Vector{1,0,0},Vector{1,0,1},Vector{1,1,-1},Vector{1,1,0},Vector{1,1,1}}"},PRIVATE}); break;
	}

	std::string initializations = "";
	initializations += "\tpidx.resize(this->workspace->get_number_of_nodes(),-1);\n";
	initializations += "\tcidx.resize(this->workspace->get_particle_system()->get_num_cells(),-1);\n";
	for(auto const& it:this->workspace->get<pmField>()) {
		initializations += "\t"+it->get_cpp_name()+" = std::dynamic_pointer_cast<pmField>(this->workspace->get_instance(\""+it->get_name()+"\").lock())->get_cpp_data<"+it->get_cpp_type().get_type()+">();\n";
	}
	initializations.pop_back();

	binary_case.add_member_function("void", "set_workspace", false, "", std::vector<c2CPP_declaration>{c2CPP_declaration{"std::shared_ptr<pmWorkspace>","ws"}}, PUBLIC, "\tworkspace=ws;", false, true);
	binary_case.add_member_function("void", "initialize", false, "", std::vector<c2CPP_declaration>{}, PUBLIC, initializations, false, true);
	std::string solver_content = "";
	solver_content += "\trestrict_particles();\n\tupdate_neighbors();\n";
	for(auto const& it:equations) {
		solver_content += it->generate_cpp_caller(session_name);
	}
	solver_content += "\tdump();";
	solver_content += "\n\treturn false;";

	binary_case.add_member_function("bool", "solve", false, "", std::vector<c2CPP_declaration>{c2CPP_declaration{"double", "current_time", true, "&", ""},c2CPP_declaration{"size_t", "num_threads", true, "&", ""},c2CPP_declaration{"std::string", "name", true, "&", "\"\""}}, PUBLIC, solver_content, false, true);
	binary_case.add_member_function("void", "parallel", false, "", std::vector<c2CPP_declaration>{c2CPP_declaration{"std::function<void(int, int)>","process"},c2CPP_declaration{"int", "num_threads",true,"&",""}}, PRIVATE, PARALLEL, false, false);
	binary_case.add_member_function("int", "flatten", false, "", std::vector<c2CPP_declaration>{c2CPP_declaration{"Vector","cells",true,"&",""},c2CPP_declaration{"Vector", "grid_pos",true,"&",""},c2CPP_declaration{"size_t", "i"}}, PRIVATE, FLATTEN, true, false);
	binary_case.add_member_function("int", "hash_key", false, "", std::vector<c2CPP_declaration>{c2CPP_declaration{"Vector","grid_pos",true,"&",""}}, PRIVATE, HASH_KEY, true, false);
	binary_case.add_member_function("Vector", "grid_coordinates", false, "", std::vector<c2CPP_declaration>{c2CPP_declaration{"Vector","point",true,"&",""}}, PRIVATE, GRID_COORDINATES, true, false);
	binary_case.add_member_function("bool", "update_neighbors", false, "", std::vector<c2CPP_declaration>{}, PRIVATE, UPDATE_NEIGHBORS, false, false);
	binary_case.add_member_function("T", "interaction", false, "", std::vector<c2CPP_declaration>{c2CPP_declaration{"int","i"},c2CPP_declaration{"std::function<T(Vector const&, int, int)>","contribute"}}, PRIVATE, INTERACTION, false, false, true);
	binary_case.get_member_function("interaction").add_template_argument(c2CPP_declaration{"class","T"});
	binary_case.add_member_function("void", "restrict_particles", false, "", std::vector<c2CPP_declaration>{},PRIVATE,RESTRICT_PARTICLES,false,false,false);

	for(auto const& it:this->equations) {
		binary_case.add_member_function(it->generate_cpp_function());
	}


	std::array<std::shared_ptr<pmExpression>,5> ops;
	ops[0] = std::dynamic_pointer_cast<pmExpression>(std::make_shared<pmConstant>(pmTensor{1}));
	ops[1] = std::dynamic_pointer_cast<pmExpression>(std::make_shared<pmConstant>(pmTensor{1}));
	ops[2] = std::dynamic_pointer_cast<pmExpression>(std::make_shared<pmConstant>(pmTensor{1}));
	ops[3] = std::dynamic_pointer_cast<pmExpression>(std::make_shared<pmConstant>(pmTensor{13}));
	ops[4] = std::dynamic_pointer_cast<pmExpression>(std::make_shared<pmConstant>(pmTensor{0.01}));

	for(auto const& it:this->workspace->get_interactions()) {
		binary_case.add_member_function(it->generate_cpp_evaluator_function());
	}

	binary_case.add_member_function("void","dump",false,"",std::vector<c2CPP_declaration>{},PRIVATE,"\tFILE* file;\n\tfile = fopen(\"data.txt\",\"w\");\n\tfor(int i=0; i<SYM_psys_r.size(); i++) {\n\t\tfprintf(file, \"%1.6e %1.6e %1.6e %1.6e\\n\", SYM_psys_r[i][0][0], SYM_psys_r[i][0][1], SYM_psys_r[i][0][2], SYM_f_rho[i][0]);\n\t}\n\tfclose(file);\n",false,false,false);

	header.get_namespace("Nauticle").add_class(binary_case);
	c2CPP_source_file source{header};

	c2Cmake_generator cmakegen = generate_cmake_file(session_name);

	c2Compiler compiler{session_name};
	compiler.set_directory(session_name+"/");
	compiler.add_header(std::make_shared<c2CPP_header_file>(header));
	compiler.add_source(std::make_shared<c2CPP_source_file>(source));
	compiler.set_cmake_generator(std::make_shared<c2Cmake_generator>(cmakegen));

	
	compiler.compile();
std::cout << 1 << std::endl;
	c2Loader loader{"./"+session_name,session_name};
std::cout << 2 << std::endl;
	pmRuntime_interface* binary_case_interface = dynamic_cast<pmRuntime_interface*>(loader.create_object());
std::cout << 3 << std::endl;
	binary_case_interface->set_workspace(workspace);
std::cout << 4 << std::endl;
	binary_case_interface->initialize();
std::cout << 5 << std::endl;
	binary_case_interface->solve(1,8,"");
std::cout << 6 << std::endl;

	loader.destroy_object(binary_case_interface);
}
#endif // JELLYFISH















