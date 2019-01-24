/*
    Copyright 2016-2019 Balazs Toth
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

#ifndef _YAML_PROCESSOR_H_
#define _YAML_PROCESSOR_H_

#include <string>
#include <vector>
#include <memory>
#include "yaml-cpp/yaml.h"
#include "pmWorkspace.h"
#include "pmEquation.h"
#include "pmCase.h"
#include "pmGrid_space.h"
#include "pmParameter_space.h"
#include "pmParticle_splitter.h"
#include "pmParticle_merger.h"
#include "pmBackground.h"

namespace Nauticle {
    /**	This class extracts data from YAML configuration file.
    */
    class pmYAML_processor final {
        YAML::Node data;
        std::shared_ptr<pmWorkspace> get_workspace() const;
        std::vector<std::shared_ptr<pmEquation>> get_equations(std::shared_ptr<pmWorkspace> workspace) const;
        std::shared_ptr<pmCase> get_initial_condition_case() const;
    public:
        void read_file(std::string const& file_name);
        static std::string get_default_file_name();
        std::shared_ptr<pmGrid_space> get_grid_space(YAML::Node particle_system, std::shared_ptr<pmWorkspace> workspace, pmDomain const& domain) const;
        std::shared_ptr<pmCase> get_case() const;
        std::shared_ptr<pmParameter_space> get_parameter_space(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
        std::vector<std::shared_ptr<pmParticle_splitter>> get_particle_splitter(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
        std::vector<std::shared_ptr<pmParticle_merger>> get_particle_merger(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
        std::vector<std::shared_ptr<pmBackground>> get_background(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
    };
}

#endif //_YAML_PROCESSOR_H_