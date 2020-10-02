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

#include <string>
#include "pmDomain.h"
#include "pmField.h"
#include "pmHysteron.h"
#include "pmIdentifier.h"

namespace Nauticle {
	/** This class manages a cloud of nodes (particles) and forms a spatial domain 
	//  for the governing equations.
	//  Neighbour search and cloud/grid generation is integrated inside.
	*/
	class pmParticle_system final : public pmField {
		std::shared_ptr<pmDomain> domain;
		std::vector<int> sorted_idx;
	protected:
		virtual std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmParticle_system()=delete;
		pmParticle_system(std::string const& n, std::vector<pmTensor> const& value, std::shared_ptr<pmDomain> domain);
		pmParticle_system(pmParticle_system const& other);
		pmParticle_system(pmParticle_system&& other);
		pmParticle_system& operator=(pmParticle_system const& other);
		pmParticle_system& operator=(pmParticle_system&& other);
		bool operator==(pmParticle_system const& rhs) const;
		bool operator!=(pmParticle_system const& rhs) const;
		virtual ~pmParticle_system() {}
		bool sort_field();
		virtual void set_value(pmTensor const& value, int const& i=0) override;
		std::shared_ptr<pmDomain> get_domain();
		void print() const override;
		void printv() const override;
		std::shared_ptr<pmParticle_system> clone() const;
		void set_number_of_nodes(size_t const& N) override;
		bool is_sorted() const;
		std::vector<int> get_sorted_idx() const;
		bool is_position() const override;
		virtual void delete_member(size_t const& i) override;
		virtual void delete_set(std::vector<size_t> const& indices) override;
		virtual void add_member(pmTensor const& v=pmTensor{}) override;
		virtual void duplicate_member(size_t const& i) override;
		void restrict_particles(std::vector<size_t>& del);
		void get_neighbors(size_t const i, std::vector<int>& neibs) const;
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