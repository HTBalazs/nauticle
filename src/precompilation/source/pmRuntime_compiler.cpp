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

#include "pmRuntime_compiler.h"
#include "c2c/c2CPP_class.h"
#include "c2c/c2CPP_code_generator.h"
#include "c2c/c2Compiler.h"

using namespace Nauticle;
using namespace c2c;

/////////////////////////////////////////////////////////////////////////////////////////
/// Set simulation case.
/////////////////////////////////////////////////////////////////////////////////////////
void pmRuntime_compiler::set_case(std::shared_ptr<pmCase> c) {
	cas = c;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set the nem of the runtime compilation session name. It will be used for the file naming.
/////////////////////////////////////////////////////////////////////////////////////////
void pmRuntime_compiler::set_session_name(std::string const& n) {
	session_name = n;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Add includes for the generated header file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmRuntime_compiler::add_includes(std::shared_ptr<c2CPP_header_file> header) const {
	header->add_include("nauticle/pmInterface.h", false);
	header->add_include("nauticle/pmConstant.h", false);
	header->add_include("nauticle/pmVariable.h", false);
	header->add_include("nauticle/pmParticle_system.h", false);
	header->add_include("nauticle/pmWorkspace.h", false);
	header->add_include("nauticle/pmRandom.h", false);
	header->add_include("nauticle/pmQueue.h", false);
	header->add_include("nauticle/pmCells.h", false);
	header->add_include("nauticle/pmSort.h", false);
	header->add_include("nauticle/nauticle_constants.h", false);
	header->add_include("Eigen/Eigen", false);
	header->add_include("string", true);
	header->add_include("iterator", true);
	header->add_include("vector", true);
	header->add_include("algorithm", true);
	header->add_include("memory", true);
	header->add_include("numeric", true);
	header->add_include("iostream", true);
	header->add_include("functional", true);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generate data for file generation based on the simulation case.
/////////////////////////////////////////////////////////////////////////////////////////
void pmRuntime_compiler::generate_code(std::shared_ptr<c2CPP_header_file>& header, std::shared_ptr<c2CPP_source_file>& source) const {
	header = std::make_shared<c2CPP_header_file>(session_name);
	this->add_includes(header);
	header->add_namespace("Nauticle");
	
	c2CPP_class cl{session_name};
	cl.add_interface("pmInterface");

	std::string init_code = "\tws = cas->get_workspace();\n";
	std::vector<c2CPP_declaration> declarations = cas->get_workspace()->generate_declarations(init_code);
	cl.add_member_type("std::shared_ptr<pmWorkspace>", "ws", false, "", "");
	for(auto const& it:declarations) {
		cl.add_member_type(c2CPP_class_member_type{it});
	}

	cl.add_member_function("void", "initialize", false, "", std::vector<c2CPP_declaration>{c2CPP_declaration{"std::shared_ptr<pmCase>", "cas", false, "", ""}}, PUBLIC, init_code, false, false);
	std::string solver="";
	auto equations = cas->get_equations();
	solver += "\tthis->sort_fields();\n";
	for(auto const& it:equations) {
		solver += it->generate_evaluator_code();
	}

	size_t dimensions = cas->get_workspace()->get_particle_system().lock()->get_particle_space()->get_domain().get_dimensions();
	// Member function to decide if a cell is to be neglected.
	{
		std::string cutoff = "";
		cutoff += "\tbool cutoff_cell = false;\n";
		cutoff += "\tfor(int k=0; k<ws_domain.get_dimensions(); k++) {\n";
		cutoff += "\t\tif(std::abs(beta[k])>=2.0-NAUTICLE_EPS && std::abs(delta[k])>NAUTICLE_EPS) {\n";
		cutoff += "\t\t\tcutoff_cell = true;\n";
		cutoff += "\t\t\tbreak;\n";
		cutoff += "\t\t}\n";
		cutoff += "\t}\n";
		cutoff += "\treturn cutoff_cell;";
		std::vector<c2CPP_declaration> arguments;
		arguments.push_back(c2CPP_declaration{vector_type(dimensions,"i"), "beta", true, "&", ""});
		arguments.push_back(c2CPP_declaration{vector_type(dimensions,"i"), "delta", true, "&", ""});
		cl.add_member_function("bool", "is_cutoff_cell", false, "", arguments, PRIVATE, cutoff, false, false);
	}
	// Member function to implement interaction
	if(!cas->get_workspace()->get_interactions().empty()) {
		std::string interact = "";
		interact += "\tstd::vector<size_t> const& start = ws_domain.get_start();\n";
		interact += "\tstd::vector<size_t> const& end = ws_domain.get_end();\n";
		interact += "\tstd::vector<"+vector_type(dimensions,"i")+"> const& cell_iterator = ws_domain.get_cell_iterator();\n";
		interact += "\t"+vector_type(dimensions)+" cell_size = ws_domain.get_cell_size();\n";
		interact += "\t"+vector_type(dimensions,"i")+" cell_num = ws_domain.get_number_of_cells();\n";
		interact += "\t"+vector_type(dimensions)+" domain_minimum = ws_domain.get_minimum();\n";
		interact += "\t"+vector_type(dimensions)+" domain_maximum = ws_domain.get_maximum();\n";
		interact += "\t"+vector_type(dimensions,"i")+" domain_size = domain_maximum - domain_minimum;\n";
		interact += "\tsize_t dimensions = ws_domain.get_dimensions();\n";
		interact += "\t"+vector_type(dimensions,"i")+" const beta = ws_domain.get_boundary();\n";
		interact += "\t"+vector_type(dimensions,"i")+" const ones{1,1};\n";
		interact += "\t"+vector_type(dimensions)+" pos_i = ws_r(level)[i];\n";
		interact += "\t"+vector_type(dimensions,"i")+" grid_pos_i = ws_domain.grid_pos(pos_i);\n";
		interact += "\tU result;\n";
		interact += "\tfor(auto const& it:cell_iterator) {\n";
		interact += "\t\t"+vector_type(dimensions,"i")+" grid_pos_j{grid_pos_i+it};\n";
		interact += "\t\t"+vector_type(dimensions,"i")+" delta = -((grid_pos_j).cwiseQuotient(cell_num));\n";
		interact += "\t\tif(is_cutoff_cell(beta, delta)) {\n";
		interact += "\t\t\tcontinue;\n";
		interact += "\t\t}\n";
		interact += "\t\t"+vector_type(dimensions,"i")+" guide = delta.cwiseProduct(beta);\n";
		interact += "\t\tgrid_pos_j += delta.cwiseProduct(cell_num-beta.cwiseProduct(cell_num)+beta);\n";
		interact += "\t\tint hash_j = ws_domain.compute_hash_key(grid_pos_j);\n";
		interact += "\t\tif(start[hash_j]!=0xFFFFFFFF) {\n";
		interact += "\t\t\tfor(int j=start[hash_j]; j<=end[hash_j]; j++) {\n";
		interact += "\t\t\t\tEigen::Vector2d pos_j = ws_r(level)[j];\n";
		interact += "\t\t\t\tfor(int k=0; k<2; k++) {\n";
		interact += "\t\t\t\t\tif(beta(k)==1) {\n";
		interact += "\t\t\t\t\t\tpos_j(k) += delta(k)*(delta(k)-1)*(domain_maximum(k)-pos_j(k)) + delta(k)*(delta(k)+1)*(domain_minimum(k)-pos_j(k));\n";
		interact += "\t\t\t\t\t} else {\n";
		interact += "\t\t\t\t\t\tpos_j(k) -= delta(k)*domain_size(k);\n";
		interact += "\t\t\t\t\t}\n";
		interact += "\t\t\t\t}\n";
		interact += "\t\t\t\tresult += contribute(pos_j-pos_i, i, j, cell_size, guide);\n";
		interact += "\t\t\t}\n";
		interact += "\t\t}\n";
		interact += "\t}\n";
		interact += "\treturn result;";
		std::vector<c2CPP_declaration> arguments;
		arguments.push_back(c2CPP_declaration{"std::function<U("+vector_type(dimensions)+" const&, size_t, size_t, "+vector_type(dimensions)+" const&, "+vector_type(dimensions,"i")+" const&)>", "contribute", false, "", ""});
		arguments.push_back(c2CPP_declaration{"size_t", "i", false, "", ""});
		arguments.push_back(c2CPP_declaration{"size_t", "level", false, "", ""});
		cl.add_member_function("template <typename T, typename U> U", "interact", false, "", arguments, PRIVATE, interact, false, false);
	}
	{ // Interaction functions
		auto interactions = cas->get_workspace()->get_interactions();
		for(auto it:interactions) {
			std::vector<c2CPP_declaration> args;
			std::string fcn_name = it->get_arguments(args);
			// arguments.push_back(c2CPP_declaration{vector_type(dimensions), "rel_pos", true, "&", ""});
			// arguments.push_back(c2CPP_declaration{"size_t", "i", false, "", ""});
			// arguments.push_back(c2CPP_declaration{"size_t", "j", false, "", ""});
			// arguments.push_back(c2CPP_declaration{vector_type(dimensions), "cell_size", true, "&", ""});
			// arguments.push_back(c2CPP_declaration{vector_type(dimensions,"i"), "guide", true, "&", ""});
			// cl.add_member_function(vector_type(dimensions), "contribute", false, "", arguments, PRIVATE, contribute, false, false);
		}
	}

	// Member function to solve the equations.
	cl.add_member_function("void", "solve", false, "", std::vector<c2CPP_declaration>{c2CPP_declaration{"size_t", "num_threads", true, "&", ""}}, PUBLIC, solver, false, false);
	std::string sorting = "";
	sorting += "\tws_domain.compute_hash_key(ws_r(0),hash_key);\n";
	sorting += "\tsorted_index.resize(hash_key.size());\n";
	sorting += "\tstd::iota(std::begin(sorted_index),std::end(sorted_index),0);\n";
	sorting += "\tpmSort::sort_by_vector(sorted_index, hash_key, pmSort::ascending);\n";
	for(auto const& it:cas->get_workspace()->get<pmField>()) {
		sorting += "\tws_"+it->get_name()+".reorder(sorted_index);\n";
	}
	cl.add_member_function("void", "sort_fields", false, "", std::vector<c2CPP_declaration>{}, PUBLIC, sorting, false, false);
	
	header->get_namespace("Nauticle").add_class(cl);
	source = std::make_shared<c2CPP_source_file>(*header);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Create CMakeLists.txt including all the necessary header and lib files for the compilation
//  of the generated files. 
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<c2Cmake_generator> pmRuntime_compiler::create_cmake_generator() const {
	auto cmake_gen = std::make_shared<c2Cmake_generator>();
	cmake_gen->set_directory(directory);
	cmake_gen->set_cmake_version("2.8.9");
	cmake_gen->set_project_name(session_name);
	cmake_gen->set_build_type(c2Cmake_generator::RELEASE);
	cmake_gen->set_cxx_flags("-pthread -O3 -fPIC");
	cmake_gen->set_cxx_standard("14");
	cmake_gen->add_link_dir("~/local/lib/prolog");
	cmake_gen->add_link_dir("~/local/lib/c2c");
	cmake_gen->add_link_dir("~/local/lib/nauticle");
	cmake_gen->add_include_dir("~/local/include/");
	cmake_gen->add_dependency("prolog");
	cmake_gen->add_dependency("c2c");
	cmake_gen->add_dependency("nauticle");
	return cmake_gen;
}

/*static*/ std::string pmRuntime_compiler::vector_type(size_t dims, std::string typ/*="d"*/) {
	switch(dims) {
		default:
		case 1: return "double";
		case 2: return "Eigen::Vector2"+typ;
		case 3: return "Eigen::Vector3"+typ;
	}
}

/*static*/ std::string pmRuntime_compiler::tensor_type(size_t dims, std::string typ/*="d"*/) {
	switch(dims) {
		default:
		case 1: return "double";
		case 2: return "Eigen::Matrix2"+typ;
		case 3: return "Eigen::Matrix3"+typ;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
/// Compile the interpreter simulation case to binary case.
/////////////////////////////////////////////////////////////////////////////////////////
void pmRuntime_compiler::compile() const {
	std::shared_ptr<c2c::c2CPP_header_file> header;
	std::shared_ptr<c2c::c2CPP_source_file> source;
	this->generate_code(header, source);
	auto compiler = std::make_shared<c2Compiler>(session_name);
	compiler->set_directory(directory);
	compiler->add_header(header);
	compiler->add_source(source);
	compiler->set_cmake_generator(this->create_cmake_generator());
	compiler->compile();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Creates the object for dynamically loaded classes.
/////////////////////////////////////////////////////////////////////////////////////////
pmInterface* pmRuntime_compiler::create_object() {
	loader = std::make_shared<c2Loader>(directory,session_name);
	return dynamic_cast<pmInterface*>(loader->create_object());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Destroys the given object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmRuntime_compiler::destroy_object(pmInterface* obj) {
	loader->destroy_object(obj);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Cleans up the whole generated directory.
/////////////////////////////////////////////////////////////////////////////////////////
void pmRuntime_compiler::clean_up() const {
	if(directory=="") return;
	std::string command = "rm -r -f " + directory;
	system(command.c_str());
}


