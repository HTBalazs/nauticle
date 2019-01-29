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

#ifndef _RUNTIME_COMPILER_H_
#define _RUNTIME_COMPILER_H_

#include "c2c/c2CPP_header_file.h"
#include "c2c/c2CPP_source_file.h"
#include "c2c/c2Cmake_generator.h"
#include "c2c/c2Loader.h"
#include "pmInterface.h"
#include "pmCase.h"
#include "pmConstant.h"
#include "pmVariable.h"
#include "pmParticle_system.h"
#include <string>
#include <memory>
#include <vector>
#include <unistd.h>

namespace Nauticle {
    /** This class implements the runtime code generation and compilation for Nauticle simulation case with c2c.
    */
    class pmRuntime_compiler {
        std::shared_ptr<pmCase> cas;
        std::string directory = "binary_case";
        std::string session_name = "pmBinary_case";
        std::shared_ptr<c2c::c2Loader> loader;
    private:
        void generate_code(std::shared_ptr<c2c::c2CPP_header_file>& header, std::shared_ptr<c2c::c2CPP_source_file>& source) const;
        void add_includes(std::shared_ptr<c2c::c2CPP_header_file> header) const;
        void generate_cmake_file() const;
        std::shared_ptr<c2c::c2Cmake_generator> create_cmake_generator() const;
    public:
        void set_case(std::shared_ptr<pmCase> c);
        void set_session_name(std::string const& n);
        void compile() const;
        pmInterface* create_object();
        void destroy_object(pmInterface* obj);
        void clean_up() const;
    };
}

#endif // _RUNTIME_COMPILER_H_