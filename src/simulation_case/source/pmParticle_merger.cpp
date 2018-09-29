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
#include "pmParticle_merger.h"
#include "pmKernel.h"

using namespace Nauticle;

size_t pmParticle_merger::counter = 0;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_merger::pmParticle_merger() {
    nearest = std::make_shared<pmNearest_neighbor>();
    counter++;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmParticle_merger::pmNearest_neighbor::clone_impl() const {
    return std::make_shared<pmNearest_neighbor>(*this);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_modifier> pmParticle_merger::clone_impl() const {
    return std::make_shared<pmParticle_merger>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a copy of the field.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_merger> pmParticle_merger::clone() const {
    return std::static_pointer_cast<pmParticle_merger, pmParticle_modifier>(clone_impl());
}

void pmParticle_merger::make_pairs(std::pair<std::vector<size_t>,std::vector<size_t>>& pairs, std::vector<size_t> candidates) const {
    size_t i = 0;
    std::vector<size_t> id1;
    std::vector<size_t> id2;
    while(!candidates.empty() && i<candidates.size()) {
        int other = (int)nearest->evaluate(candidates[i])[0];
        if(other>=0) {
            std::vector<size_t>::iterator it = std::find(id2.begin(), id2.end(), other);
            if(it==id2.end()) {
                id1.push_back(candidates[i]);
                id2.push_back(other);
                std::vector<size_t>::iterator it = std::find(candidates.begin(), candidates.end(), other);
                if(it!=candidates.end()) {
                    *it = candidates.back();
                    candidates.pop_back();
                }
                candidates[i] = candidates.back();
                candidates.pop_back();
            } else {
                i++;
            }
        } else {
            i++;
        }
    }
    pairs.first = id1;
    pairs.second = id2;
}

pmTensor pmParticle_merger::pmNearest_neighbor::evaluate(int const& i, size_t const& level/*=0*/) const {
    if(!assigned) { ProLog::pLogger::error_msgf("Particle merger is not assigned to any particle system.\n"); }
    int nearest = -1;
    double distance = 1e12;
    auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
        if(i!=j) {
            double d_ji = rel_pos.norm();
            if(d_ji < distance) {
                distance = d_ji;
                nearest = j;
            }
        }
        return pmTensor{};
    };
    pmTensor T = interact(i, contribute);
    return pmTensor{1,1,(double)nearest};
}

void pmParticle_merger::update() {
    static int count = -1;
    count++;
    if(count%(int)(period->evaluate(0)[0]) != 0) {
        return;
    }
    std::pair<std::vector<size_t>,std::vector<size_t>> pairs;
    this->make_pairs(pairs, this->get_candidates());
    std::shared_ptr<pmParticle_system> ps = workspace->get<pmParticle_system>()[0];
    pmKernel W;
    std::vector<size_t> to_delete;
    for(int i=0; i<pairs.first.size(); i++) {
        size_t id1 = pairs.first[i];
        size_t id2 = pairs.second[i];
        pmTensor mass1 = mass->evaluate(id1);
        pmTensor mass2 = mass->evaluate(id2);
        pmTensor rad1 = radius->evaluate(id1);
        pmTensor rad2 = radius->evaluate(id2);
        pmTensor vel1 = velocity->evaluate(id1);
        pmTensor vel2 = velocity->evaluate(id2);
        pmTensor pos1 = ps->evaluate(id1);
        pmTensor pos2 = ps->evaluate(id2);

        pmTensor mass_M = mass1 + mass2;
        pmTensor pos_M = (pos1*mass1+pos2*mass2)/mass_M;
        pmTensor vel_M = (vel1*mass1+vel2*mass2)/mass_M;
        W.set_kernel_type(10, false);
        double W_M1 = W.evaluate((pos_M-pos1).norm(),rad1[0]);
        double W_M2 = W.evaluate((pos_M-pos2).norm(),rad2[0]);
        pmTensor rad_M = std::sqrt(7.0*NAUTICLE_PI*mass_M[0]/(10.0*(W_M1*mass1[0]+W_M2*mass2[0])));
        workspace->duplicate_particle(id1);
        size_t num_nodes = workspace->get_number_of_nodes();
        mass->set_value(mass_M,num_nodes-1);
        ps->set_value(pos_M,num_nodes-1);
        velocity->set_value(vel_M,num_nodes-1);
        radius->set_value(rad_M,num_nodes-1);
        to_delete.push_back(id1);
        to_delete.push_back(id2);
    }
    for(auto const& it:to_delete) {
        workspace->delete_particle(it);
    }
}


void pmParticle_merger::set_workspace(std::shared_ptr<pmWorkspace> ws) {
    pmParticle_modifier::set_workspace(ws);
    nearest->assign(ws->get_particle_system());
}

void pmParticle_merger::set_velocity(std::shared_ptr<pmField> vel) {
    velocity = vel;
}

std::shared_ptr<pmField> const& pmParticle_merger::get_velocity() const {
    return velocity;
}

void pmParticle_merger::print() const {
    static bool print_header = true;
    if(print_header) {
        ProLog::pLogger::headerf<ProLog::LBL>("Particle merger:");
        print_header = false;
    }
    ProLog::pLogger::titlef<ProLog::LMA>("Splitter %i:\n", counter);
    ProLog::pLogger::logf<ProLog::YEL>("        condition: "); condition->print();
    ProLog::pLogger::logf<ProLog::YEL>("        radius: %s\n", radius->get_name().c_str());
    ProLog::pLogger::logf<ProLog::YEL>("        mass: %s\n", mass->get_name().c_str());
    ProLog::pLogger::logf<ProLog::YEL>("        velocity: %s\n", velocity->get_name().c_str());
    ProLog::pLogger::logf<ProLog::YEL>("        period: "); period->print();
    
    ProLog::pLogger::footerf<ProLog::LBL>();
}

















