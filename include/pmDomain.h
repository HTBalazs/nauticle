/////////////////////////////////////////////////////////////////////////////
// Name:        pmDomain.h
// Purpose:     Domain for particles.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _DOMAIN_H_
#define _DOMAIN_H_

#include "pmTensor.h"

/** An object of this class represents the domain in which a particle system can be
//  interpreted. The domain can be of any dimensions (1..3). The domain is axis-aligned 
//  in the Cartesian coordinate system.
*/
class pmDomain {
	pmTensor minimum;
	pmTensor maximum;
	float cell_size;
public:
	pmDomain() {}
	pmDomain(pmTensor const& dmin, pmTensor const& dmax, float const& csize);
	pmTensor get_minimum() const;
	pmTensor get_maximum() const;
	float get_cell_size() const;
	size_t get_num_cells() const;
	size_t get_dimensions() const;
	void set_minimum(pmTensor const& mn);
	void set_maximum(pmTensor const& mx);
	void set_cell_size(float const& csize);
	void print() const;
};

#endif //_DOMAIN_H_