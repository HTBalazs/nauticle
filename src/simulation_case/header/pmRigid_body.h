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
    
#ifndef _PM_RIGID_BODY_H_
#define _PM_RIGID_BODY_H_

#include "pmTensor.h"
#include "pmParticle_system.h"
#include "pmQuaternion.h"
#include "pmCounter.h"
#include <vector>
#include <memory>

namespace Nauticle {
	class pmRigid_body : public pmCounter<size_t> {
		std::vector<size_t> particle_idx;
		pmTensor theta{3,3,0};
		pmTensor angular_velocity{3,1,0};
		pmTensor linear_velocity{3,1,0};
		pmTensor cog{3,1,0};
		double body_mass=0;
		pmQuaternion<double> rotation_quaternion;
		bool initialized = false;
		size_t identifier;
	protected:
		void initialize(std::shared_ptr<pmParticle_system> psys, std::shared_ptr<pmSymbol> particle_mass, std::shared_ptr<pmField> rid);
		void calculate_motion_state(pmTensor const& body_force, pmTensor const& body_torque, double const& time_step);
	public:
		pmRigid_body() : identifier{counter} {}
		void add_particle(size_t const& idx);
		void remove_particle(size_t const& idx);
		void update(std::shared_ptr<pmParticle_system> psys, std::shared_ptr<pmExpression> particle_force, std::shared_ptr<pmSymbol> particle_velocity, std::shared_ptr<pmSymbol> particle_mass, std::shared_ptr<pmExpression> particle_theta, std::shared_ptr<pmField> rmatrix, std::shared_ptr<pmField> imatrix, std::shared_ptr<pmField> rid, double const& time_step_size);
		std::vector<size_t> const& get_index();
		void print();
	};
}

#endif //_PM_RIGID_BODY_H_