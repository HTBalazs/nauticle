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
    
#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

#include "pmField.h"
#include "pmParticle.h"
#include <string>
#include <algorithm>

namespace Nauticle {
	/** This class manages a cloud of nodes (particles) and forms a spatial domain 
	//  for the governing equations.
	//  Neighbour search and cloud/grid generation is integrated inside.
	*/
	class pmParticle_system final : public pmField {
		std::vector<pmParticle> value;
		bool up_to_date = false;
		std::vector<pmParticle>::iterator virtual_particle_begin;
		std::vector<pmParticle>::iterator virtual_particle_end;
	protected:
		virtual std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmParticle_system()=delete;
		pmParticle_system(std::vector<pmTensor> const& val);
		virtual ~pmParticle_system() {}
		void print() const override;
		std::shared_ptr<pmParticle_system> clone() const;
		bool is_position() const override;
		bool is_printable() const override;
		virtual void set_value(pmTensor const& val, int const& i=0) override;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		bool is_symmetric() const override;
		void set_storage_depth(size_t const& d) override;
		int get_field_size() const override;
		pmParticle& get_particle(size_t const& i);
		std::string get_name() const override;
		std::string get_type() const override;
		bool is_up_to_date() const;
		void expire();
		void set_up_to_date();
		void add_real_particle(pmParticle const& ptc);
		void add_virtual_particle(pmParticle const& ptc);
		std::vector<pmParticle>::iterator real_begin();
		std::vector<pmParticle>::iterator real_end();
		std::vector<pmParticle>::iterator virtual_begin();
		std::vector<pmParticle>::iterator virtual_end();
		void add_member(pmTensor const& v=pmTensor{}) override;
		void destroy_virtual_particles();
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmParticle_system const* obj) {
		obj->write_to_string(os);
		return os;
	}
}

#endif //_PARTICLE_SYSTEM_H_