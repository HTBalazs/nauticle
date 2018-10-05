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

#include "pmRandom.h"
#include "pmParticle_splitter.h"

using namespace Nauticle;

size_t pmParticle_splitter::counter = 0;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_splitter::pmParticle_splitter() {
    counter++;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Splits particles if conditions meet by generating new particles in the same system.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::update() {
    static int update_count = -1;
    update_count++;
    // check if splitting can be performed
    if(update_count%(int)(period->evaluate(0)[0]) != 0) {
        return;
    }
    std::vector<size_t> candidates = this->get_candidates();
    double step = 2.0*NAUTICLE_PI/daughters;
    std::vector<size_t> delete_indices;
    for(auto const& it:candidates) {
        double R_original = radius->evaluate(it,0)[0];
        double m_original = mass->evaluate(it,0)[0];
        if(parent) {
            radius->set_value(alpha*R_original,it);
            mass->set_value(m_original/(daughters+1),it);
        } else {
            delete_indices.push_back(it);
        }
        double angle = pmRandom::random(0,2.0*NAUTICLE_PI);
        for(int i=0; i<daughters; i++) {
            workspace->duplicate_particle(it);
            size_t num_nodes = workspace->get_number_of_nodes();
            std::shared_ptr<pmParticle_system> ps = workspace->get<pmParticle_system>()[0];
            pmTensor new_pos = ps->evaluate(num_nodes-1,0);
            new_pos[0] += R_original*epsilon*std::cos(i*step+angle);
            new_pos[1] += R_original*epsilon*std::sin(i*step+angle);
            ps->set_value(new_pos,num_nodes-1);
            radius->set_value(alpha*R_original,num_nodes-1);
            mass->set_value(m_original/(daughters+1),num_nodes-1);
        }
    }
    workspace->delete_particle_set(delete_indices);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the splitter properties.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::print() const {
    static bool print_header = true;
    if(print_header) {
        ProLog::pLogger::headerf<ProLog::LBL>("Particle splitter:");
        print_header = false;
    }
    ProLog::pLogger::titlef<ProLog::LMA>("Splitter");
    ProLog::pLogger::logf<ProLog::YEL>("        condition: "); condition->print();
    ProLog::pLogger::line_feed(1);
    ProLog::pLogger::logf<ProLog::YEL>("        radius: ");
    ProLog::pLogger::logf<ProLog::NRM>("%s\n", radius->get_name().c_str());
    ProLog::pLogger::logf<ProLog::YEL>("        mass: ");
    ProLog::pLogger::logf<ProLog::NRM>("%s\n", mass->get_name().c_str());
    ProLog::pLogger::logf<ProLog::YEL>("        period: "); period->print();
    ProLog::pLogger::line_feed(1);
    ProLog::pLogger::footerf<ProLog::LBL>();
}


/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_modifier> pmParticle_splitter::clone_impl() const {
    return std::make_shared<pmParticle_splitter>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a copy of the field.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_splitter> pmParticle_splitter::clone() const {
    return std::static_pointer_cast<pmParticle_splitter, pmParticle_modifier>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set parameter alpha as an expression.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::set_alpha(double alp) {
    alpha = alp;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set parameter epsilon as an expression.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::set_epsilon(double eps) {
    epsilon = eps;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set parameter daughter as an expression.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::set_daughters(size_t dau) {
    daughters = dau;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set parameter parent as an expression.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::set_parent(bool par) {
    parent = par;
}



































