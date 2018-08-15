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

#ifndef INSTALL_DIR
#define INSTALL_DIR std::string("/usr/local/include/nauticle/")
#endif //INSTALL_DIR

#include "pmRuntime_compiler.h"

using namespace Nauticle;
using namespace c2c;

void pmRuntime_compiler::add_case(std::shared_ptr<pmCase> c) {
    cas = c;
}

void pmRuntime_compiler::set_name(std::string const& n) {
    session_name = n;
}

void pmRuntime_compiler::generate_code() const {
    // Generate C++ code
    c2CPP_header_file header{session_name};
    header.add_include("pmInterface.h", false);
    header.add_include("string", true);
    header.add_include("vector", true);
    header.add_include("algorithm", true);
    header.add_include("memory", true);
    header.add_include("iostream", true);
    header.add_namespace("Nauticle");
    c2CPP_class cl{"pmHardcoded_case"};
    cl.add_interface("pmInterface");
    cl.add_member_type("std::shared_ptr<pmCase>", "cas", false, "", "");
    c2CPP_declaration arg{"std::shared_ptr<pmCase>", "c", false, "", ""};
    cl.add_member_function("void", "set_case", false, "", std::vector<c2CPP_declaration>{arg}, PUBLIC, "\tcas = c;", false, true);
    cl.add_member_function("void", "update", false, "", std::vector<c2CPP_declaration>{}, PUBLIC, "\tstd::cout << 2 << std::endl;", false, true);
    header.get_namespace("Nauticle").add_class(cl);
    c2CPP_source_file source{header};
    c2CPP_code_generator cgen;
    cgen.add_header(header);
    cgen.add_source(source);
    cgen.write_files();
}

void pmRuntime_compiler::compile() const {
    this->generate_code();
    // Compile generated files
    c2Compiler compiler{session_name};
    compiler.set_compiler("clang++");
    compiler.add_flag("-std=c++14 -O3 -I " + INSTALL_DIR);
    compiler.compile();
}

pmInterface* pmRuntime_compiler::create_object() {
    loader = std::make_shared<c2Loader>(session_name);
    return dynamic_cast<pmInterface*>(loader->create_object());
}

void pmRuntime_compiler::destroy_object(pmInterface* obj) {
    loader->destroy_object(obj);
}

void pmRuntime_compiler::clean_up() const {
    std::string deletion = "rm " + session_name + ".h";
    deletion += " " + session_name + ".cpp ";
    deletion += " " + session_name + ".o ";
    deletion += " lib" + session_name + ".so ";
    system(deletion.c_str());
}




