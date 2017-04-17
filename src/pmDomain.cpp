/*
    Copyright 2016-2017 Balazs Toth
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

#include "pmDomain.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmDomain::pmDomain(pmTensor const& dmin, pmTensor const& dmax, double const& csize, pmTensor const& bnd) {
	if(dmin.numel()!=dmax.numel()) {
		pLogger::error_msgf("Domain requires vectors of identical sizes.\n");
	}
	minimum = dmin;
	maximum = dmax;
	cell_size = csize;
	boundary = bnd;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the minimum coordinate of the domain.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDomain::get_minimum() const {
	return minimum;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the maximum coordinate of the domain.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDomain::get_maximum() const {
	return maximum;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the cell size of the domain.
/////////////////////////////////////////////////////////////////////////////////////////
double pmDomain::get_cell_size() const {
	return cell_size;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of cells.
/////////////////////////////////////////////////////////////////////////////////////////
size_t pmDomain::get_num_cells() const {
	return (int)(maximum-minimum).productum();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the dimensions of the domain.
/////////////////////////////////////////////////////////////////////////////////////////
size_t pmDomain::get_dimensions() const {
	return minimum.numel();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns minimum*cellsize. Minimum stores the number of cells from the origin to the 
/// minimum corner of the domain.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDomain::get_physical_minimum() const {
	return minimum*cell_size;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns maximum*cellsize. Maximum stores the number of cells from the origin to the 
/// maximum corner of the domain.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDomain::get_physical_maximum() const {
	return maximum*cell_size;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the physical size of the domain.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDomain::get_physical_size() const {
	return (maximum-minimum)*cell_size;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the boundary type of the domain (periodic or symmetric).
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDomain::get_boundary() const {
	return boundary;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the domain object content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::print() const {
	pLogger::logf<LYW>("            Domain:");
	pLogger::logf<COLOR>("\n               min: ");
	minimum.print();
	pLogger::logf<COLOR>("\n               max: ");
	maximum.print();
	pLogger::logf<COLOR>("\n               boundary: ");
	boundary.print();
	pLogger::logf<COLOR>("\n               cell size: %g", cell_size);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Setter for domain_min.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::set_minimum(pmTensor const& mn) {
	minimum = mn;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Setter for domain_max.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::set_maximum(pmTensor const& mx) {
	maximum = mx;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Setter for cell_size.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::set_cell_size(double const& csize) {
	cell_size = csize;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Setter for boundary.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::set_boundary(pmTensor const& bnd) {
	boundary = bnd;
}

