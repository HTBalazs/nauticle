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

#ifndef INCLUDE_DIR
#define INCLUDE_DIR std::string{"-I /usr/local/include/nauticle/ -I /usr/local/VTK-7.0.0/Common/Core/"}
#endif //INCLUDE_DIR
#ifndef LIB_FILES
#define LIB_FILES std::string{"-L /usr/local/lib/nauticle/ -l nauticle -L /usr/local/lib/prolog/ -l prolog -L/usr/local/lib/c2c -l c2c"}
#endif //LIB_FILES

#include "pmRuntime_compiler.h"
#include "c2c/c2CPP_class.h"
#include "c2c/c2CPP_source_file.h"
#include "c2c/c2CPP_code_generator.h"
#include "c2c/c2Compiler.h"

using namespace Nauticle;
using namespace c2c;

void pmRuntime_compiler::set_case(std::shared_ptr<pmCase> c) {
    cas = c;
}

void pmRuntime_compiler::set_name(std::string const& n) {
    session_name = n;
}

void pmRuntime_compiler::add_includes(c2CPP_header_file& header) const {
    header.add_include("pmInterface.h", false);
    header.add_include("pmConstant.h", false);
    header.add_include("pmVariable.h", false);
    header.add_include("pmParticle_system.h", false);
    header.add_include("pmWorkspace.h", false);
    header.add_include("pmRandom.h", false);
    header.add_include("string", true);
    header.add_include("vector", true);
    header.add_include("algorithm", true);
    header.add_include("memory", true);
    header.add_include("iostream", true);
}

void pmRuntime_compiler::generate_code() const {
    // Generate C++ code
    c2CPP_header_file header{session_name};
    add_includes(header);
    header.add_namespace("Nauticle");
    
    c2CPP_class cl{"pmBinary_case"};
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
    
    header.get_namespace("Nauticle").add_class(cl);
    c2CPP_source_file source{header};
    c2CPP_code_generator cgen;
    cgen.add_header(header);
    cgen.add_source(source);
    cgen.set_directory(directory);
    cgen.write_files();
}

void pmRuntime_compiler::compile() const {
    this->generate_code();
    // Compile generated code
    c2Compiler compiler{session_name};
    compiler.set_directory(directory);
    compiler.generate_cmake_file();
    compiler.compile();
}

pmInterface* pmRuntime_compiler::create_object() {
    loader = std::make_shared<c2Loader>("binary_case",session_name);
    return dynamic_cast<pmInterface*>(loader->create_object());
}

void pmRuntime_compiler::destroy_object(pmInterface* obj) {
    loader->destroy_object(obj);
}

void pmRuntime_compiler::clean_up() const {
    std::string command = "rm -r -f binary_case";
    system(command.c_str());
}


