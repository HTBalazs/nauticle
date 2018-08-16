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

#ifndef _CODE_GEN_H_
#define _CODE_GEN_H_

#include "c2c/c2CPP_header_file.h"
#include "c2c/c2Loader.h"
#include "pmInterface.h"
#include "pmCase.h"
#include "pmConstant.h"
#include "pmVariable.h"
#include "pmParticle_system.h"
#include <string>
#include <memory>
#include <vector>

namespace Nauticle {
    class pmRuntime_compiler {
        std::shared_ptr<pmCase> cas;
        std::string session_name = "pmBinary_case";
        std::shared_ptr<c2c::c2Loader> loader;
        void generate_code() const;
        void add_includes(c2c::c2CPP_header_file& header) const;
    public:
        void set_case(std::shared_ptr<pmCase> c);
        void set_name(std::string const& n);
        void compile() const;
        pmInterface* create_object();
        void destroy_object(pmInterface* obj);
        void clean_up() const;
    };
}

#endif // _CODE_GEN_H_