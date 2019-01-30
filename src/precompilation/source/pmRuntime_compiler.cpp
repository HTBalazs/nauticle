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
    header->add_include("pmInterface.h", false);
    header->add_include("pmConstant.h", false);
    header->add_include("pmVariable.h", false);
    header->add_include("pmParticle_system.h", false);
    header->add_include("pmWorkspace.h", false);
    header->add_include("pmRandom.h", false);
    header->add_include("string", true);
    header->add_include("vector", true);
    header->add_include("algorithm", true);
    header->add_include("memory", true);
    header->add_include("iostream", true);
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

    std::string init_code = "\t\tws = cas->get_workspace();\n";
    std::vector<c2CPP_declaration> declarations = cas->get_workspace()->generate_declarations(init_code);
    cl.add_member_type("std::shared_ptr<pmWorkspace>", "ws", false, "", "");
    for(auto const& it:declarations) {
        cl.add_member_type(c2CPP_class_member_type{it});
    }

    cl.add_member_function("void", "initialize", false, "", std::vector<c2CPP_declaration>{c2CPP_declaration{"std::shared_ptr<pmCase>", "cas", false, "", ""}}, PUBLIC, init_code, false, true);
    std::string solver="";
    auto equations = cas->get_equations();
    solver += "\tws->sort_all_by_position();\n";
    for(auto const& it:equations) {
        solver += it->generate_evaluator_code();
    }
    cl.add_member_function("void", "solve", false, "", std::vector<c2CPP_declaration>{c2CPP_declaration{"size_t", "num_threads", true, "&", ""}}, PUBLIC, solver, false, true);
    
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
    cmake_gen->add_link_dir("/usr/local/lib/prolog");
    // cmake_gen->add_link_dir("/usr/local/lib/commonutils");
    cmake_gen->add_link_dir("/usr/local/lib/c2c");
    cmake_gen->add_link_dir("/usr/local/lib/nauticle");
    cmake_gen->add_include_dir("/usr/local/include/nauticle");
    cmake_gen->add_include_dir("/usr/local/include/prolog");
    cmake_gen->add_include_dir("/usr/local/include/c2c");
    // cmake_gen->add_include_dir("/usr/local/include/commonutils");
    cmake_gen->add_dependency("/usr/local/lib/prolog/libprolog.a");
    // cmake_gen->add_dependency("/usr/local/lib/commonutils/libcommonutils.a");
    // cmake_gen->add_dependency("/usr/local/lib/libyaml-cpp.a");
    cmake_gen->add_dependency("/usr/local/lib/c2c/libc2c.a");
    cmake_gen->add_dependency("/usr/local/lib/nauticle/libnauticle.a");
    return cmake_gen;
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


