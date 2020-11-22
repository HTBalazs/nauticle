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

#include "pmSymbol.h"
#include "pmParticle.h"
#include "pmCollection.h"
#include <string>
#include <algorithm>
#include <vector>

namespace Nauticle {
	/** This class manages a cloud of nodes (particles) and forms a spatial domain 
	//  for the governing equations.
	//  Neighbour search and cloud/grid generation is integrated inside.
	*/
	class pmParticle_system final : public pmCollection<pmParticle> {
	private:
		bool up_to_date = false;
	protected:
		virtual std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmParticle_system()=delete;
		pmParticle_system(std::vector<pmTensor> const& val);
		virtual ~pmParticle_system() override = default;
		void print() const override;
		std::shared_ptr<pmParticle_system> clone() const;
		virtual void set_value(pmTensor const& val, int const& i=0) override;
		pmParticle& get_particle(size_t const& i);
		std::string get_name() const override;
		bool is_up_to_date() const;
		void expire();
		void set_up_to_date();
		void destroy_virtual_members() override;
		void write_to_string(std::ostream& os) const override;
		void printv() const override;
		void duplicate_member(size_t const& i, pmTensor const& guide=pmTensor{}) override;
		void insert_virtual_particles(std::vector<pmParticle> const& particles);
		bool operator==(pmParticle_system const& rhs) const;
		bool operator!=(pmParticle_system const& rhs) const;
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