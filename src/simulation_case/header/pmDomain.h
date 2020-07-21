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
	};
}

#endif //_DOMAIN_H_