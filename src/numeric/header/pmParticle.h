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
    
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "pmField.h"
#include "pmHistory.h"
#include <string>

namespace Nauticle {
	class pmParticle {
		pmHistory<pmTensor> position;
		pmParticle* next;
	public:
		pmParticle() = delete;
		pmParticle(pmTensor const& pos) : position{pos} {};
		pmParticle(Particle const& other);
		pmParticle(Particle&& other);
		pmParticle& operator=(Particle const& other);
		pmParticle& operator=(Particle&& other);
		pmParticle& operator=(pmTensor const& t);
		~pmParticle() {}
		void break_link();
		void set_position(pmTensor const& pos);
		void set_next(pmParticle* p);
		pmTensor const& get_position(size_t const& level=0) const;
		pmParticle* get_next() const;
		void set_storage_depth(size_t const& d);
	};
}

#endif //_PARTICLE_H_