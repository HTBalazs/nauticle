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
    
#ifndef _DOMAIN_H_
#define _DOMAIN_H_

#include "pmTensor.h"
#include "pmSort.h"
#include <vector>

namespace Nauticle {
	/** This class represents the domain in which a particle system can be
	//  interpreted. The domain can be of any dimensions (1..3). The domain is axis-aligned 
	//  in the Cartesian coordinate system. The domain boundaries can be periodic, symmetric or cutoff.
	*/
	class pmDomain {
		pmTensor minimum;
		pmTensor maximum;
		pmTensor cell_size;
		pmTensor boundary;
		pmTensor shift;
		std::vector<int> hash_key;
		std::vector<unsigned int> cell_start;
		std::vector<unsigned int> cell_end;
		std::vector<pmTensor> cell_iterator;
		bool up_to_date=false;
	private:
		void build_cell_arrays();
		void combinations_recursive(const std::vector<int> &elems, size_t comb_len, std::vector<size_t> &pos, size_t depth);
		void combinations(const std::vector<int> &elems, size_t comb_len);
		void build_cell_iterator();
	public:
		pmDomain() {}
		pmDomain(pmTensor const& dmin, pmTensor const& dmax, pmTensor const& csize, pmTensor const& bnd, pmTensor const& shft);
		bool operator==(pmDomain const& rhs) const;
		bool operator!=(pmDomain const& rhs) const;
		pmTensor get_grid_position(pmTensor const& point) const;
		double flatten(pmTensor const& cells, pmTensor const& grid_pos, size_t i) const;
		int calculate_hash_key_from_grid_position(pmTensor const& grid_position) const;
		int calculate_hash_key_from_position(pmTensor const& position) const;
		void expire();
		bool is_up_to_date() const;
		void restrict_particles(std::vector<std::vector<pmTensor>>& value, std::vector<size_t>& del) const;
		bool update_neighbour_list(std::vector<pmTensor> const& current_value, std::vector<int>& idx);
		std::vector<unsigned int> const& get_start() const;
		std::vector<unsigned int> const& get_end() const;
		int const& get_hash_key(int const& i) const;
		std::vector<pmTensor> const& get_cell_iterator() const;
		std::shared_ptr<pmDomain> clone() const;
		void set_number_of_nodes(size_t const& N);
		size_t get_number_of_nodes() const;
		pmTensor const& get_minimum() const;
		pmTensor const& get_maximum() const;
		pmTensor const& get_cell_size() const;
		size_t get_num_cells() const;
		size_t get_dimensions() const;
		pmTensor get_physical_minimum() const;
		pmTensor get_physical_maximum() const;
		pmTensor get_physical_size() const;
		pmTensor const& get_boundary() const;
		pmTensor const& get_shift() const;
		void set_minimum(pmTensor const& mn);
		void set_maximum(pmTensor const& mx);
		void set_cell_size(pmTensor const& csize);
		void set_boundary(pmTensor const& bnd);
		void set_shift(pmTensor const& shft);
		void print() const;
		void get_neighbors(pmTensor const& cell, std::vector<int>& neibs) const;
	};
}

#endif //_DOMAIN_H_