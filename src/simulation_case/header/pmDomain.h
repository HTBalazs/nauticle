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

#include "pmCell_iterator.h"
#include "pmParticle_system.h"
#include "pmTensor.h"
#include "pmSort.h"
#include <vector>
#include <memory>

namespace Nauticle {
	/** This class represents the domain in which a particle system can be
	//  interpreted. The domain can be of any dimensions (1..3). The domain is axis-aligned 
	//  in the Cartesian coordinate system. The domain boundaries can be periodic, symmetric or cutoff.
	*/
	class pmDomain {
	protected:
		std::shared_ptr<pmParticle_system> psys;
	private:
		pmTensor minimum;
		pmTensor maximum;
		pmTensor cell_size;
		pmTensor boundary;
		pmTensor shift;
		std::vector<pmParticle*> grid;
		pmCell_iterator cell_iterator;
	protected:
		virtual std::shared_ptr<pmDomain> clone_impl() const=0;
	private:
		bool shift_check() const;
		virtual void restrict_particles(std::vector<std::vector<pmTensor>>& value, std::vector<size_t>& del) const;
		double flatten(pmTensor const& cells, pmTensor const& grid_pos, size_t i) const;
		int calculate_hash_key_from_grid_position(pmTensor const& grid_pos) const;
		int calculate_hash_key_from_position(pmTensor const& position) const;
		bool build_cell_list();
		pmTensor get_grid_position(pmTensor const& point) const;
	public:
		pmDomain()=default;
		virtual ~pmDomain()=0;
		virtual void set_domain_parameters(pmTensor const& dmin, pmTensor const& dmax, pmTensor const& csize, pmTensor const& bnd, pmTensor const& shft);
		size_t get_number_of_nodes() const;
		virtual void add_particle_system(std::vector<pmTensor> const& values);
		std::shared_ptr<pmParticle_system> get_particle_system() const;
		std::shared_ptr<pmDomain> clone() const;
		pmTensor const& get_minimum() const;
		pmTensor const& get_maximum() const;
		pmTensor const& get_cell_size() const;
		pmTensor const& get_boundary() const;
		pmTensor const& get_shift() const;
		size_t get_num_cells() const;
		size_t get_dimensions() const;
		virtual void print() const;
		pmParticle* get_linked_list(pmTensor const& pos);
		void get_neighbors(pmTensor const& pos, std::vector<pmParticle*>& nb);
		bool is_stationary_domain() const;
		bool update();
	};
}

#endif //_DOMAIN_H_