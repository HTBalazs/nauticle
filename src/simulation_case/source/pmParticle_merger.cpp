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
#include "pmSmallest.h"
#include <utility>

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

/////////////////////////////////////////////////////////////////////////////////////////
/// Collect tuples from the given candidates. Non-candidate particles can also be 
//  selected for merge.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_merger::make_tuples(std::tuple<std::vector<size_t>,std::vector<size_t>,std::vector<size_t>>& tuples, std::vector<size_t> candidates) const {
    size_t i = 0;
    std::vector<size_t> id0;
    std::vector<size_t> id1;
    std::vector<size_t> id2;
    std::vector<bool> selected(workspace->get_number_of_nodes(), false);
    while(!candidates.empty() && i<candidates.size()) {
        pmTensor two_nn = nearest->evaluate(candidates[i]);
        std::pair<int,int> others{two_nn[0],two_nn[1]};
        if(others.first>=0 && others.second>=0) {
            if(!selected[others.first] && !selected[others.second] && !selected[candidates[i]]) {
                id0.push_back(candidates[i]);
                id1.push_back(others.first);
                id2.push_back(others.second);
                selected[candidates[i]] = true;
                selected[others.first] = true;
                selected[others.second] = true;
            }
        }
        i++;
    }
    std::get<0>(tuples) = id0;
    std::get<1>(tuples) = id1;
    std::get<2>(tuples) = id2;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Finds the nearest neighbor of the given particle within range.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmParticle_merger::pmNearest_neighbor::evaluate(int const& i, size_t const& level/*=0*/) const {
    if(!assigned) { ProLog::pLogger::error_msgf("Particle merger is not assigned to any particle system.\n"); }
    pmSmallest<double, 2> nearest;
    auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
        if(i!=j) {
            double d_ji = rel_pos.norm();
            nearest.push_value(d_ji,j);
        }
        return pmTensor{};
    };
    pmTensor T = interact(i, contribute);
    pmTensor two_nn{2,1,-1};
    if(nearest.get_number_of_values()==2) {
        two_nn[0] = (double)nearest[0].second;
        two_nn[1] = (double)nearest[1].second;
    }
    return two_nn;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Performs the particle merging.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_merger::update() {
    static int count = -1;
    count++;
    if(count%(int)(period->evaluate(0)[0]) != 0) {
        return;
    }
    std::tuple<std::vector<size_t>,std::vector<size_t>,std::vector<size_t>> tuples;
    this->make_tuples(tuples, this->get_candidates());
    std::shared_ptr<pmParticle_system> ps = workspace->get<pmParticle_system>()[0];
    pmKernel W;
    W.set_kernel_type(10, false);
    std::vector<size_t> delete_indices;
    for(int i=0; i<std::get<0>(tuples).size(); i++) {
        size_t const id0 = std::get<0>(tuples)[i];
        size_t const id1 = std::get<1>(tuples)[i];
        size_t const id2 = std::get<2>(tuples)[i];
        double const mass0 = mass->evaluate(id0)[0];
        double const mass1 = mass->evaluate(id1)[0];
        double const mass2 = mass->evaluate(id2)[0];
        double const h0 = radius->evaluate(id0)[0];
        double const h1 = radius->evaluate(id1)[0];
        double const h2 = radius->evaluate(id2)[0];
        pmTensor const vel0 = velocity->evaluate(id0);
        pmTensor const vel1 = velocity->evaluate(id1);
        pmTensor const vel2 = velocity->evaluate(id2);
        pmTensor const pos0 = ps->evaluate(id0);
        pmTensor const pos1 = ps->evaluate(id1);
        pmTensor const pos2 = ps->evaluate(id2);
        double mass_M = (mass0 + mass1 + mass2)/2.0;
        pmTensor pos_p = (pos0*mass0+pos1*mass1+pos2*mass2)/mass_M/2.0;
        pmTensor vel_p = (vel0*mass0+vel1*mass1+vel2*mass2)/mass_M/2.0;

        pmTensor rp0 = pos_p-pos0;
        pmTensor rp1 = pos_p-pos1;
        pmTensor rp2 = pos_p-pos2;
        pmTensor vp0 = vel_p-vel0;
        pmTensor vp1 = vel_p-vel1;
        pmTensor vp2 = vel_p-vel2;

        double dp0 = rp0.norm();
        double dp1 = rp1.norm();
        double dp2 = rp2.norm();
        
        W.set_kernel_type(16, false);
        double W_M0 = W.evaluate(dp0,2.0*h0);
        double W_M1 = W.evaluate(dp1,2.0*h1);
        double W_M2 = W.evaluate(dp2,2.0*h2);
        double Wp = (W_M0*mass0+W_M1*mass1+W_M2*mass2)/2.0/mass_M;
        double hM = std::sqrt(1.0/std::exp(1.0)/NAUTICLE_PI/Wp);
        double d = 0.9*(dp0+dp1+dp2)/3.0;

        if(d>hM) {
            d = hM;
        } else {
            auto iterate = [](double const& r, double const& W, double const& h_init)->double {
                double h = h_init;
                size_t maxit = 100;
                for(int i=0; i<maxit; i++) {
                    double h_prev = h;
                    h = std::sqrt(1.0/NAUTICLE_PI/W*std::exp(-r*r/h*h));
                    if(std::abs((h-h_prev)/h)<1e-6) {
                        break;
                    }
                    if(i==maxit-1) {
                        return -1.0;
                    }
                }
                return h;
            };
            hM = iterate(d,Wp,hM);
        }

        pmTensor direction;
        pmTensor pos01 = pos0-pos1;
        pmTensor pos02 = pos0-pos2;
        pmTensor pos12 = pos1-pos2;
        if(pos01.norm()>pos02.norm() && pos01.norm()>pos12.norm()) {
            direction = pos01/pos01.norm();   
        } else if(pos02.norm()>pos01.norm() && pos02.norm()>pos12.norm()) {
            direction = pos02/pos02.norm();
        } else {
            direction = pos12/pos12.norm();   
        }

        pmTensor pos_b = pos_p+direction*d;
        pmTensor pos_a = 2*pos_p-pos_b;

        double G = mass0*(rp0[0]*vp0[1]-rp0[1]*vp0[0])+mass1*(rp1[0]*vp1[1]-rp1[1]*vp1[0])+mass2*(rp2[0]*vp2[1]-rp2[1]*vp2[0]);

        double tangential_vel = G/2.0/d/mass_M;
        
        pmTensor pos_ap = (pos_a-pos_p);
        pos_ap = pos_ap/pos_ap.norm();
        pmTensor vel_M{2,1,0};
        vel_M[0] = pos_ap[1];
        vel_M[1] = -pos_ap[0];

        workspace->duplicate_particle(id1);
        workspace->duplicate_particle(id1);
        size_t num_nodes = workspace->get_number_of_nodes();
        mass->set_value(mass_M,num_nodes-1);
        mass->set_value(mass_M,num_nodes-2);
        radius->set_value(hM,num_nodes-1);
        radius->set_value(hM,num_nodes-2);
        ps->set_value(pos_a,num_nodes-1);
        ps->set_value(pos_b,num_nodes-2);
        velocity->set_value(vel_p-vel_M*tangential_vel,num_nodes-1);
        velocity->set_value(vel_p+vel_M*tangential_vel,num_nodes-2);
        delete_indices.push_back(id0);
        delete_indices.push_back(id1);
        delete_indices.push_back(id2);
    }
    workspace->delete_particle_set(delete_indices);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the workspace object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_merger::set_workspace(std::shared_ptr<pmWorkspace> ws) {
    pmParticle_modifier::set_workspace(ws);
    nearest->assign(ws->get_particle_system());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the velocity object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_merger::set_velocity(std::shared_ptr<pmField> vel) {
    velocity = vel;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the velocity object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmField> const& pmParticle_merger::get_velocity() const {
    return velocity;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the merger properties.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_merger::print() const {
    static bool print_header = true;
    if(print_header) {
        ProLog::pLogger::headerf<ProLog::LBL>("Particle merger:");
        print_header = false;
    }
    ProLog::pLogger::titlef<ProLog::LMA>("Merger");
    ProLog::pLogger::logf<ProLog::YEL>("        condition: "); condition->print(); ProLog::pLogger::line_feed(1);
    ProLog::pLogger::logf<ProLog::YEL>("        radius: ");
    ProLog::pLogger::logf<ProLog::NRM>("%s\n", radius->get_name().c_str());
    ProLog::pLogger::logf<ProLog::YEL>("        mass: ");
    ProLog::pLogger::logf<ProLog::NRM>("%s\n", mass->get_name().c_str());
    ProLog::pLogger::logf<ProLog::YEL>("        velocity: ");
    ProLog::pLogger::logf<ProLog::NRM>("%s\n", velocity->get_name().c_str());
    ProLog::pLogger::logf<ProLog::YEL>("        period: "); period->print(); ProLog::pLogger::line_feed(1);
    
    ProLog::pLogger::footerf<ProLog::LBL>();
}

















