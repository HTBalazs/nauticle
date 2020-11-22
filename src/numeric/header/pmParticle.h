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

#include "pmHistory.h"
#include <string>

namespace Nauticle {
	class pmParticle {
	private:
		pmHTensor position;
	public:
		pmParticle* next = nullptr;
		int parent_index = -1;
		pmTensor guide;
	public:
		pmParticle()=default;
		pmParticle(pmHTensor const& pos);
		pmParticle(pmParticle const& other);
		pmParticle(pmParticle&& other);
		~pmParticle() {}
		pmTensor const& operator()(size_t const& level) const;
		pmTensor& operator()(size_t const& level);
		pmParticle& operator=(pmParticle const& other);
		pmParticle& operator=(pmParticle&& other);
		pmParticle& operator=(pmTensor const& t);
		bool operator==(pmParticle const& other) const;
		bool operator!=(pmParticle const& other) const;
		void set_position(pmTensor const& pos);
		pmTensor const& get_position(size_t const& level=0) const;
		void set_storage_depth(size_t const& d);
		size_t get_storage_depth() const;
		void shift(pmTensor const& distance);
		void initialize(pmTensor const& val);
		void print() const;
	};
}

#endif //_PARTICLE_H_