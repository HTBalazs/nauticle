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
#include "pmTime_series.h"
#include "pmParticle_sink.h"
#include "pmParticle_emitter.h"
#include "pmScript.h"
#include "pmRigid_body_system.h"

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
        std::shared_ptr<pmGrid_space> get_grid_space(YAML::iterator ps_nodes, YAML::const_iterator ps_nodes_end, std::shared_ptr<pmWorkspace> workspace, size_t const& dims) const;
        std::shared_ptr<pmCase> get_case() const;
        std::shared_ptr<pmParameter_space> get_parameter_space(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
        std::vector<std::shared_ptr<pmParticle_splitter>> get_particle_splitter(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
        std::vector<std::shared_ptr<pmParticle_merger>> get_particle_merger(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
        std::vector<std::shared_ptr<pmBackground>> get_background(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
        std::vector<std::shared_ptr<pmTime_series>> get_time_series(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
        std::vector<std::shared_ptr<pmParticle_sink>> get_particle_sink(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
        std::vector<std::shared_ptr<pmParticle_emitter>> get_particle_emitter(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
        std::vector<std::shared_ptr<pmScript>> get_script(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
        std::vector<pmInitializer> get_initializers(YAML::iterator it, YAML::const_iterator it_end, std::shared_ptr<pmWorkspace> workspace) const;
        void get_springs(std::shared_ptr<pmWorkspace> workspace) const;
        std::shared_ptr<pmRigid_body_system> get_rigid_bodies(std::shared_ptr<pmWorkspace> workspace) const;
    };
}

#endif //_YAML_PROCESSOR_H_