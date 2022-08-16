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
    
#ifndef _RIGID_BODY_SYSTEM_H_
#define _RIGID_BODY_SYSTEM_H_

#include "pmRigid_body.h"
#include "pmParticle_system.h"
#include <string>

namespace Nauticle {
	class pmRigid_body_system {
		std::vector<std::shared_ptr<pmRigid_body>> rigid_body;
        std::shared_ptr<pmParticle_system> psys;
        std::shared_ptr<pmExpression> particle_force;
        std::shared_ptr<pmSymbol> particle_velocity;
        std::shared_ptr<pmSymbol> particle_mass;
        std::shared_ptr<pmField> rotation_matrix;
        std::string file_name;
	public:
		void initialize(std::string const& fn, std::shared_ptr<pmParticle_system> ps, std::shared_ptr<pmExpression> force, std::shared_ptr<pmSymbol> velocity, std::shared_ptr<pmField> rmatrix, std::shared_ptr<pmSymbol> mass);
        void print() const;
        void update(double const& time_step);
	};
}

#endif //_RIGID_BODY_SYSTEM_H_