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

#include "pmRandom.h"
#include "pmParticle_splitter.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Splits particles if conditions meet by generating new particles in the same system.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::update(size_t const& num_threads) {
    std::vector<size_t> candidates = this->get_candidates();
    std::vector<size_t> delete_indices;
    std::shared_ptr<pmParticle_system> ps = workspace->get<pmParticle_system>()[0];
    int dims = ps->get_dimensions();
    for(auto const& it:candidates) {
        double alpha = smoothing_ratio->evaluate(it,0)[0];
        double epsilon = separation_parameter->evaluate(it,0)[0];
        bool generate_at_parent = (bool)parent->evaluate(it,0)[0];
        double R_original = radius->evaluate(it,0)[0];
        double m_original = mass->evaluate(it,0)[0];
        size_t num_daughters = (dims==3?(size_t)daughters->evaluate(it,0)[0]:((size_t)daughters->evaluate(it,0)[0]-(generate_at_parent?1:0)));
        if(passive.use_count()!=0) {
            if(generate_at_parent) {
                workspace->duplicate_particle(it);
                size_t num_nodes = workspace->get_number_of_nodes();
                radius->set_value(alpha*R_original,num_nodes-1);
                mass->set_value(m_original/(num_daughters+1),num_nodes-1);
            }
        } else {
            if(generate_at_parent) {
                radius->set_value(alpha*R_original,it);
                mass->set_value(m_original/(num_daughters+1),it);
            } else {
                delete_indices.push_back(it);
            }
        }
        double step = 2.0*NAUTICLE_PI/num_daughters;
        pmTensor angle = rotation->evaluate(it,0);
        for(int i=0; i<num_daughters; i++) {
            workspace->duplicate_particle(it);
            size_t num_nodes = workspace->get_number_of_nodes();
            std::shared_ptr<pmParticle_system> ps = workspace->get<pmParticle_system>()[0];
            pmTensor new_pos = ps->evaluate(num_nodes-1,0);
            if(dims==3) {
                new_pos[0] += R_original/std::sqrt(2.0)*epsilon*std::sin(i*step+angle[0]);
                new_pos[1] += R_original/std::sqrt(2.0)*epsilon*std::cos(i*step+angle[0]);
                new_pos[2] += (2.0*(i%2)-1.0)*R_original/std::sqrt(2.0)*epsilon;
            } else {
                new_pos[1] += R_original*epsilon*std::sin(i*step+angle[0]);
                new_pos[0] += R_original*epsilon*std::cos(i*step+angle[0]);
            }
            ps->set_value(new_pos,num_nodes-1);
            radius->set_value(alpha*R_original,num_nodes-1);
            mass->set_value(m_original/(num_daughters+(generate_at_parent?1:0)),num_nodes-1);
            if(active.use_count()!=0) {
                active->set_value(1,num_nodes-1);
            }
        }
        if(passive.use_count()!=0) {
            passive->set_value(1.0,it);
        }
    }
    workspace->delete_particle_set(delete_indices);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the splitter properties.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::print() const {
    pmParticle_modifier::print();
    ProLog::pLogger::titlef<ProLog::LMA>("Splitter");
    ProLog::pLogger::logf<ProLog::YEL>("        condition: "); condition->print();
    ProLog::pLogger::line_feed(1);
    ProLog::pLogger::logf<ProLog::YEL>("        radius: ");
    ProLog::pLogger::logf<ProLog::NRM>("%s\n", radius->get_name().c_str());
    ProLog::pLogger::logf<ProLog::YEL>("        mass: ");
    ProLog::pLogger::logf<ProLog::NRM>("%s\n", mass->get_name().c_str());
    ProLog::pLogger::logf<ProLog::YEL>("        number of daughters: "); daughters->print();
    ProLog::pLogger::line_feed(1);
    ProLog::pLogger::logf<ProLog::YEL>("        smoothing_ratio: "); smoothing_ratio->print();
    ProLog::pLogger::line_feed(1);
    ProLog::pLogger::logf<ProLog::YEL>("        separation parameter: "); separation_parameter->print();
    ProLog::pLogger::line_feed(1);
    ProLog::pLogger::logf<ProLog::YEL>("        generate at parent: "); parent->print();
    ProLog::pLogger::line_feed(1);
    ProLog::pLogger::logf<ProLog::YEL>("        rotation: "); rotation->print();
    ProLog::pLogger::line_feed(1);
    if(passive.use_count()!=0) {
        ProLog::pLogger::logf<ProLog::YEL>("        passive: ");
        ProLog::pLogger::logf<ProLog::NRM>("%s\n", passive->get_name().c_str());
    }
    if(active.use_count()!=0) {
        ProLog::pLogger::logf<ProLog::YEL>("        active: ");
        ProLog::pLogger::logf<ProLog::NRM>("%s\n", active->get_name().c_str());
    }
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
/// Set parameter smoothing_ratio as an expression.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::set_smoothing_ratio(std::shared_ptr<pmExpression> sr) {
    smoothing_ratio = sr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set parameter separation_parameter as an expression.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::set_separation_parameter(std::shared_ptr<pmExpression> sp) {
    separation_parameter = sp;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set parameter daughter as an expression.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::set_daughters(std::shared_ptr<pmExpression> dr) {
    daughters = dr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set parameter parent as an expression.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::set_parent(std::shared_ptr<pmExpression> pr) {
    parent = pr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set parameter rotation as an expression.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_splitter::set_rotation(std::shared_ptr<pmExpression> rot) {
    rotation = rot;
}

void pmParticle_splitter::set_passive(std::shared_ptr<pmField> psv) {
    passive = psv;
}

void pmParticle_splitter::set_active(std::shared_ptr<pmField> atv) {
    active = atv;
}





























