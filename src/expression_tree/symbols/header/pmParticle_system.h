/*
    Copyright 2016-2018 Balazs Toth
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

namespace Nauticle {
	/** This class manages a cloud of nodes (particles) and forms a spatial domain 
	//  for the governing equations.
	//  Neighbour search and cloud/grid generation is integrated inside.
	*/
	class pmParticle_system final : public pmField {
		
		class pmParticle_space final {
			std::vector<int> hash_key;
			std::vector<unsigned int> cell_start;
			std::vector<unsigned int> cell_end;
			std::vector<pmTensor> cell_iterator;
			pmDomain domain;
			bool up_to_date=false;
		private:
			void build_cell_arrays();
			double flatten(pmTensor const& cells, pmTensor const& grid_pos, size_t i) const;
			int calculate_hash_key_from_position(pmTensor const& position) const;
			void combinations_recursive(const std::vector<int> &elems, size_t comb_len, std::vector<size_t> &pos, size_t depth);
			void combinations(const std::vector<int> &elems, size_t comb_len);
			void build_cell_iterator();
		public:
			int calculate_hash_key_from_grid_position(pmTensor const& grid_position) const;
			pmParticle_space()=delete;
			pmParticle_space(size_t const& num_particles, pmDomain const& dom);
			pmParticle_space(pmParticle_space const& other);
			pmParticle_space(pmParticle_space&& other);
			pmParticle_space& operator=(pmParticle_space const& other);
			pmParticle_space& operator=(pmParticle_space&& other);
			void expire();
			bool is_up_to_date() const;
			void restrict_particles(std::vector<std::vector<pmTensor>>& value) const;
			void update_neighbour_list(std::vector<pmTensor> const& current_value, std::vector<int>& idx);
			std::vector<unsigned int> const& get_start() const;
			std::vector<unsigned int> const& get_end() const;
			int const& get_hash_key(int const& i) const;
			pmTensor get_grid_position(pmTensor const& point) const;
			pmDomain get_domain() const;
			void print() const;
			std::vector<pmTensor> const& get_cell_iterator() const;
			std::shared_ptr<pmParticle_space> clone() const;
			void set_number_of_nodes(size_t const& N);
		};
		std::shared_ptr<pmParticle_space> particle_space;
		std::vector<int> sorted_idx;
	protected:
		virtual std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmParticle_system()=delete;
		pmParticle_system(std::string const& n, std::vector<pmTensor> const& value, pmDomain const& domain);
		pmParticle_system(pmParticle_system const& other);
		pmParticle_system(pmParticle_system&& other);
		pmParticle_system& operator=(pmParticle_system const& other);
		pmParticle_system& operator=(pmParticle_system&& other);
		virtual ~pmParticle_system() {}
		void sort_field();
		virtual void set_value(pmTensor const& value, int const& i=0) override;
		std::shared_ptr<pmParticle_space> get_particle_space();
		void print() const override;
		void printv() const override;
		std::shared_ptr<pmParticle_system> clone() const;
		void set_number_of_nodes(size_t const& N) override;
		bool is_sorted() const;
		std::vector<int> get_sorted_idx() const;
		bool is_position() const override;
		virtual void delete_member(size_t const& i) override;
		virtual void add_member(pmTensor const& v=pmTensor{}) override;
		virtual void duplicate_member(size_t const& i) override;
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