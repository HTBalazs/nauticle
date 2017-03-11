/*
    Copyright 2017 Balazs Toth
    This file is part of LEMPS.

    LEMPS is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LEMPS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LEMPS.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://bitbucket.org/lempsproject/
*/
    
#ifndef _DOMAIN_H_
#define _DOMAIN_H_

#include "pmTensor.h"

/** This class represents the domain in which a particle system can be
//  interpreted. The domain can be of any dimensions (1..3). The domain is axis-aligned 
//  in the Cartesian coordinate system. The domain boundaries can be periodic and symmetric.
*/
class pmDomain {
	pmTensor minimum;
	pmTensor maximum;
	float cell_size;
	pmTensor boundary;
public:
	pmDomain() {}
	pmDomain(pmTensor const& dmin, pmTensor const& dmax, float const& csize, pmTensor const& bnd);
	pmTensor get_minimum() const;
	pmTensor get_maximum() const;
	float get_cell_size() const;
	size_t get_num_cells() const;
	size_t get_dimensions() const;
	pmTensor get_physical_minimum() const;
	pmTensor get_physical_maximum() const;
	pmTensor get_physical_size() const;
	pmTensor get_boundary() const;
	void set_minimum(pmTensor const& mn);
	void set_maximum(pmTensor const& mx);
	void set_cell_size(float const& csize);
	void set_boundary(pmTensor const& bnd);
	void print() const;
};

#endif //_DOMAIN_H_