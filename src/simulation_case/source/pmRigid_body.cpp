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
	body_mass = 0.0;
	for(int i=0; i<particle_idx.size(); i++) {
		pmTensor const& global_pos_idx = psys->evaluate(particle_idx[i])+psys->get_periodic_shift(particle_idx[i]);
		double mass_i = particle_mass->evaluate(particle_idx[i])[0];
		cog += global_pos_idx*mass_i;
		body_mass += mass_i;
	}
	cog /= body_mass;
	theta = pmTensor{3,3,0};
	for(int i=0; i<particle_idx.size(); i++) {
		pmTensor local_pos_idx = psys->evaluate(particle_idx[i])+psys->get_periodic_shift(particle_idx[i]) - cog;
		double mass_i = particle_mass->evaluate(particle_idx[i])[0];
		theta[0] += mass_i*(local_pos_idx[1]*local_pos_idx[1] + local_pos_idx[2]*local_pos_idx[2]);
		theta[1] -= mass_i*local_pos_idx[0]*local_pos_idx[1];
		theta[2] -= mass_i*local_pos_idx[0]*local_pos_idx[2];
		theta[3] -= mass_i*local_pos_idx[0]*local_pos_idx[1];
		theta[4] += mass_i*(local_pos_idx[0]*local_pos_idx[0] + local_pos_idx[2]*local_pos_idx[2]);
		theta[5] -= mass_i*local_pos_idx[1]*local_pos_idx[2];
		theta[6] -= mass_i*local_pos_idx[0]*local_pos_idx[2];
		theta[7] -= mass_i*local_pos_idx[1]*local_pos_idx[2];
		theta[8] += mass_i*(local_pos_idx[0]*local_pos_idx[0] + local_pos_idx[1]*local_pos_idx[1]);
	}
}

void pmRigid_body::update(std::shared_ptr<pmParticle_system> psys, std::shared_ptr<pmExpression> particle_force, std::shared_ptr<pmSymbol> particle_velocity, std::shared_ptr<pmSymbol> particle_mass, double const& time_step) {
	static bool first_update = true;
	if(first_update) {
		for(int i=0; i<particle_velocity->get_field_size(); i++) {
			linear_velocity += particle_velocity->evaluate(i);
		}
		linear_velocity /= particle_velocity->get_field_size();
		first_update = false;
	}

	this->initialize(psys, particle_mass);
	pmTensor body_torque{3,1,0};
	pmTensor body_force{3,1,0};
	for(int i=0; i<particle_idx.size(); i++) {
		body_force += particle_force->evaluate(particle_idx[i]);
		body_torque += cross(psys->evaluate(particle_idx[i])+psys->get_periodic_shift(particle_idx[i])-cog,particle_force->evaluate(particle_idx[i]));
	}
	// calculate motion state
	omega += time_step*theta.inverse()*(-body_torque/*-cross(omega,theta*omega)*/);
	linear_velocity += time_step*body_force/body_mass;
	pmTensor dir = omega/(omega.norm()+NAUTICLE_EPS);
	double phi = omega.norm()*time_step;
	rq = pmQuaternion<double>::make_rotation_quaternion(dir, phi);
	// update particle velocity; rotate and translate body according to the motion state
	for(int i=0; i<particle_idx.size(); i++) {
		pmTensor const& local_pos_idx = psys->evaluate(particle_idx[i])+psys->get_periodic_shift(particle_idx[i])-cog;
		pmQuaternion<double> p = pmQuaternion<double>::vector2quaternion(local_pos_idx);
		pmTensor new_local_pos = pmQuaternion<double>::quaternion2vector((rq.inverse()*p)*rq);
		psys->set_value(new_local_pos+cog+linear_velocity*time_step-psys->get_periodic_shift(particle_idx[i]),particle_idx[i],true);
		particle_velocity->set_value(linear_velocity-cross(omega,new_local_pos),particle_idx[i],true);
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