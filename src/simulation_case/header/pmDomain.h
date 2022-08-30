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
    
#ifndef _PM_DOMAIN_H_
#define _PM_DOMAIN_H_

#include "pmTensor.h"
#include <vector>

namespace Nauticle {
	/** This class represents the domain in which a particle system can be
	//  interpreted. The domain can be of any dimensions (1..3). The domain is axis-aligned 
	//  in the Cartesian coordinate system. The domain boundaries can be periodic, symmetric or cutoff.
	*/
	class pmDomain {
	public:
		pmTensor minimum;
		pmTensor maximum;
		pmTensor cell_size;
		pmTensor boundary;
	protected:
		std::vector<pmTensor> cell_iterator;
		std::vector<int> cidx;
	protected:
		double flatten(pmTensor const& cells, pmTensor const& grid_pos, size_t i) const;
		void combinations_recursive(std::vector<int> const& elems, size_t comb_len, std::vector<size_t> &pos, size_t depth);
		void combinations(std::vector<int> const& elems, size_t comb_len);
		int hash_key(pmTensor const& grid_pos) const;
		void build_cell_iterator();
	public:
		pmDomain()=default;
		pmDomain(pmTensor const& dmin, pmTensor const& dmax, pmTensor const& csize, pmTensor const& bnd);
		bool operator==(pmDomain const& other) const;
		bool operator!=(pmDomain const& other) const;
		pmTensor const& get_minimum() const;
		pmTensor const& get_maximum() const;
		pmTensor const& get_cell_size() const;
		pmTensor const& get_boundary() const;
		size_t get_num_cells() const;
		size_t get_dimensions() const;
		pmTensor get_physical_minimum() const;
		pmTensor get_physical_maximum() const;
		pmTensor get_physical_size() const;
		std::vector<pmTensor> const& get_cell_iterator();
		void set_minimum(pmTensor const& mn);
		void set_maximum(pmTensor const& mx);
		void set_cell_size(pmTensor const& csize);
		void set_boundary(pmTensor const& bnd);
		pmTensor grid_coordinates(pmTensor const& point) const;
		virtual void printv() const;
	};
}

#endif //_PM_DOMAIN_H_