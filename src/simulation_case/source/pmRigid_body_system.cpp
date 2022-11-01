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

#include "pmRigid_body_system.h"
#include "Color_define.h"
#include "vtkDelimitedTextReader.h"
#include "vtkTable.h"
#include "vtkSmartPointer.h"
#include "vtkVariant.h"
#include "prolog/pLogger.h"

using namespace Nauticle;
using namespace ProLog;

void pmRigid_body_system::initialize(std::string const& fn, std::shared_ptr<pmParticle_system> ps, std::shared_ptr<pmExpression> force, std::shared_ptr<pmSymbol> velocity, std::shared_ptr<pmField> rmatrix, std::shared_ptr<pmSymbol> mass, std::shared_ptr<pmExpression> ptheta, std::shared_ptr<pmField> rid) {
    rigid_body.clear();
    psys = ps;
    particle_force = force;
    particle_velocity = velocity;
    particle_mass = mass;
    rotation_matrix = rmatrix;
    particle_theta = ptheta;
    rigid_body_id = rid;
    file_name = fn;
    vtkSmartPointer<vtkDelimitedTextReader> reader = vtkSmartPointer<vtkDelimitedTextReader>::New();
    reader->SetFileName(file_name.c_str());
    reader->DetectNumericColumnsOn();
    reader->SetFieldDelimiterCharacters(" ");
    reader->SetDefaultIntegerValue(-1);
    reader->Update();
    vtkTable* table = reader->GetOutput();
    for(vtkIdType i=0; i<table->GetNumberOfRows(); i++) {
        rigid_body.push_back(std::make_shared<pmRigid_body>());
        for(vtkIdType j=0; j<table->GetNumberOfColumns(); j++) {
            int particle_idx = (table->GetValue(i, j)).ToInt();
            if(particle_idx<0 || particle_idx>=psys->get_field_size()) {
                continue;
            }
            rigid_body[i]->add_particle(particle_idx);
            particle_velocity->set_lock(particle_idx);
            psys->set_lock(particle_idx);
        }
    }
}

void pmRigid_body_system::print() const {
    pLogger::headerf<LBL>("Rigid body system");
    pLogger::titlef<LMA>("Properties");
    pLogger::logf<YEL>("        file: ");
    int num_rb = rigid_body.size();
    if(num_rb>1) {
        pLogger::logf<NRM>("%s   (%d rigid bodies)\n", file_name.c_str(), num_rb);
    } else {
        pLogger::logf<NRM>("%s   (1 rigid body)\n", file_name.c_str());
    }
    pLogger::logf<YEL>("        velocity: ");
    pLogger::logf<NRM>("%s\n", particle_velocity->get_name().c_str());
    pLogger::logf<YEL>("        force: ");
    particle_force->print();
    pLogger::line_feed(1);
    pLogger::logf<YEL>("        mass: ");
    pLogger::logf<NRM>("%s\n", particle_mass->get_name().c_str());
    if(rotation_matrix.use_count()>0) {
        pLogger::logf<YEL>("        rotation matrix: ");
        pLogger::logf<NRM>("%s\n", rotation_matrix->get_name().c_str());
    }
    if(particle_theta.use_count()>0) {
        particle_theta->print();
    }
    pLogger::line_feed(1);
    pLogger::footerf<LBL>();
}

void pmRigid_body_system::update(double const& time_step) {
    for(auto& it:rigid_body) {
        it->update(psys, particle_force, particle_velocity, particle_mass, particle_theta, rotation_matrix, rigid_body_id, time_step);
    }
}

#include "Color_undefine.h"

