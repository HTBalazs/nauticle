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

#include "pmRigid_body.h"
#include "pmQuaternion.h"
#include "Color_define.h"

using namespace Nauticle;

void pmRigid_body::add_particle(size_t const& idx) {
	for(auto const& it:particle_idx) {
		if(idx==it) {
			return;
		}
	}
	particle_idx.push_back(idx);
}

void pmRigid_body::remove_particle(size_t const& idx) {
	for(int i=0; i<particle_idx.size(); i++) {
		if(particle_idx[i]==idx) {
			particle_idx.erase(particle_idx.begin()+i);
			return;
		}
	}
}

void pmRigid_body::initialize(std::shared_ptr<pmParticle_system> psys, std::shared_ptr<pmSymbol> particle_mass) {
	cog = pmTensor{3,1,0};
	theta = pmTensor{3,3,0};
	body_mass = 0.0;
	for(int i=0; i<particle_idx.size(); i++) {
		pmTensor const& pos_i = psys->evaluate(particle_idx[i]);
		double mass_i = particle_mass->evaluate(i)[0];
		cog += pos_i*mass_i;
		body_mass += mass_i;
	}
	cog /= body_mass;
	for(int i=0; i<particle_idx.size(); i++) {
		pmTensor pos_i = psys->evaluate(particle_idx[i]) - cog;
		double mass_i = particle_mass->evaluate(i)[0];
		theta[0] += mass_i*(pos_i[1]*pos_i[1] + pos_i[2]*pos_i[2]);
		theta[1] -= mass_i*pos_i[0]*pos_i[1];
		theta[2] -= mass_i*pos_i[0]*pos_i[2];
		theta[3] -= mass_i*pos_i[0]*pos_i[1];
		theta[4] += mass_i*(pos_i[0]*pos_i[0] + pos_i[2]*pos_i[2]);
		theta[5] -= mass_i*pos_i[1]*pos_i[2];
		theta[6] -= mass_i*pos_i[0]*pos_i[2];
		theta[7] -= mass_i*pos_i[1]*pos_i[2];
		theta[8] += mass_i*(pos_i[0]*pos_i[0] + pos_i[1]*pos_i[1]);
	}
}

void pmRigid_body::update(std::shared_ptr<pmParticle_system> psys, std::shared_ptr<pmSymbol> particle_force, std::shared_ptr<pmSymbol> particle_velocity, std::shared_ptr<pmSymbol> particle_mass, double const& time_step_size) {
	this->initialize(psys, particle_mass);
	pmTensor body_torque{3,1,0};
	pmTensor body_force{3,1,0};
	for(int i=0; i<particle_idx.size(); i++) {
		body_force += particle_force->evaluate(i);
		body_torque += cross(psys->evaluate(i)-cog,particle_force->evaluate(i));
	}
}

std::vector<size_t> const& pmRigid_body::get_index() {
	return particle_idx;
}

void pmRigid_body::print() {
	for(auto const& it:particle_idx) {
		std::cout << it << " ";
	}
	std::cout << std::endl;
	theta.print();
	std::cout << std::endl;
	cog.print();
	std::cout << std::endl;
	std::cout << body_mass << std::endl;
}

#include "Color_undefine.h"