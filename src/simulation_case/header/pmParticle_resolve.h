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

#ifndef _PM_PARTICLE_RESOLVE_H_
#define _PM_PARTICLE_RESOLVE_H_

#include "pmParticle_modifier.h"
#include <memory>
#include <vector>

namespace Nauticle {
    /** This class implements the base of the particle modifier classes for condition 
    // driven particle generation and deletion.
    */
	class pmParticle_resolve : public pmParticle_modifier {
	protected:
        std::shared_ptr<pmField> radius;
        std::shared_ptr<pmField> mass;
	protected:
		std::vector<size_t> get_candidates() const;
	public:
        void set_radius(std::shared_ptr<pmField> rad);
        void set_mass(std::shared_ptr<pmField> ms);
        std::shared_ptr<pmField> const& get_radius() const;
        std::shared_ptr<pmField> const& get_mass() const;
        std::shared_ptr<pmParticle_resolve> clone() const;
	};
}

#endif //_PM_PARTICLE_RESOLVE_H_
