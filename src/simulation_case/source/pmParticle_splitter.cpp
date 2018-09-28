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

void pmParticle_splitter::update() {
    std::vector<size_t> candidates = this->get_candidates();
    double step = 2.0*NAUTICLE_PI/num_new;
    for(auto const& it:candidates) {
        double R_original = radius->evaluate(it,0)[0];
        double m_original = mass->evaluate(it,0)[0];
        radius->set_value(alpha*R_original,it);
        mass->set_value(m_original/(num_new+1),it);
        double angle = pmRandom::random(0,2.0*NAUTICLE_PI);
        for(int i=0; i<num_new; i++) {
            workspace->duplicate_particle(it);
            size_t num_nodes = workspace->get_number_of_nodes();
            std::shared_ptr<pmParticle_system> ps = workspace->get<pmParticle_system>()[0];
            pmTensor new_pos = ps->evaluate(num_nodes-1,0);
            new_pos[0] += R_original*epsilon*std::cos(i*step+angle);
            new_pos[1] += R_original*epsilon*std::sin(i*step+angle);
            ps->set_value(new_pos,num_nodes-1);
            radius->set_value(alpha*R_original,num_nodes-1);
            mass->set_value(m_original/(num_new+1),num_nodes-1);
        }
    }
}
