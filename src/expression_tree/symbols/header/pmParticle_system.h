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
#include "pmSort.h"
#include "pmField.h"
#include "pmHysteron.h"
#include "pmIdentifier.h"

namespace Nauticle {
	/** This class manages a cloud of nodes (particles) and forms a spatial domain 
	//  for the governing equations.
	//  Neighbour search and cloud/grid generation is integrated inside.
	*/
	class pmParticle_system final : public pmField {
		class pmDomain {
			pmTensor minimum;
			pmTensor maximum;
			pmTensor cell_size;
			pmTensor boundary;
			std::vector<pmTensor> cell_iterator;
			std::vector<int> pidx;
			std::vector<int> cidx;
			bool up_to_date=false;
		private:
			double flatten(pmTensor const& cells, pmTensor const& grid_pos, size_t i) const;
			void combinations_recursive(std::vector<int> const& elems, size_t comb_len, std::vector<size_t> &pos, size_t depth);
			void combinations(std::vector<int> const& elems, size_t comb_len);
			int hash_key(pmTensor const& grid_pos) const;
			void build_cell_iterator();
			void restrict_particles(std::vector<size_t>& del) const;
		public:
			pmDomain() {}
			pmDomain(pmTensor const& dmin, pmTensor const& dmax, pmTensor const& csize, pmTensor const& bnd);
			bool operator==(pmDomain const& rhs) const;
			bool operator!=(pmDomain const& rhs) const;
			pmTensor const& get_minimum() const;
			pmTensor const& get_maximum() const;
			pmTensor const& get_cell_size() const;
			size_t get_num_cells() const;
			size_t get_dimensions() const;
			pmTensor get_physical_minimum() const;
			pmTensor get_physical_maximum() const;
			pmTensor get_physical_size() const;
			pmTensor const& get_boundary() const;
			void set_minimum(pmTensor const& mn);
			void set_maximum(pmTensor const& mx);
			void set_cell_size(pmTensor const& csize);
			void set_boundary(pmTensor const& bnd);
			void print() const;
			bool update(std::vector<pmTensor> const& current_value, std::vector<size_t>& del);
			std::vector<pmTensor> const& get_cell_iterator() const;
			pmTensor grid_coordinates(pmTensor const& point) const;
			std::vector<int> const& get_cell_content(pmTensor const& grid_crd, int& index) const;
			bool is_up_to_date() const;
			void set_expired();
			void set_number_of_nodes(size_t const& N);
			size_t get_number_of_nodes() const;
		};
		std::shared_ptr<pmDomain> domain;
	protected:
		virtual std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmParticle_system()=delete;
		pmParticle_system(std::string const& n, std::vector<pmTensor> const& value, std::shared_ptr<pmDomain> const& dm);
		pmParticle_system(pmParticle_system const& other);
		pmParticle_system(pmParticle_system&& other);
		pmParticle_system& operator=(pmParticle_system const& other);
		pmParticle_system& operator=(pmParticle_system&& other);
		bool operator==(pmParticle_system const& rhs) const;
		bool operator!=(pmParticle_system const& rhs) const;
		virtual ~pmParticle_system() {}
		virtual void set_value(pmTensor const& value, int const& i=0) override;
		std::shared_ptr<pmDomain> get_domain();
		void print() const override;
		void printv() const override;
		std::shared_ptr<pmParticle_system> clone() const;
		void set_number_of_nodes(size_t const& N) override;
		bool is_position() const override;
		virtual void delete_member(size_t const& i) override;
		virtual void delete_set(std::vector<size_t> const& indices) override;
		virtual void add_member(pmTensor const& v=pmTensor{}) override;
		virtual void duplicate_member(size_t const& i) override;
		void restrict_particles(std::vector<size_t>& del);
		bool is_up_to_date() const;
		bool update(std::vector<size_t>& del);
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