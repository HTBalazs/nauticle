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

#include "pmDomain.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmDomain::pmDomain(pmTensor const& dmin, pmTensor const& dmax, pmTensor const& csize, pmTensor const& bnd) {
	if(dmin.numel()!=dmax.numel() || dmax.numel()!=csize.numel() || csize.numel()!=bnd.numel()) {
		ProLog::pLogger::error_msgf("Domain requires vectors of identical sizes.\n");
	}
	if(!dmin.is_integer() || !dmax.is_integer()) {
		ProLog::pLogger::warning_msgf("At least one of the domain sizes is not integer multiple of the cell size.\n");
	}
	minimum = dmin;
	maximum = dmax;
	cell_size = csize;
	boundary = bnd;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implement identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmDomain::operator==(pmDomain const& rhs) const {
	if( 0!=(this->minimum != rhs.minimum).productum() || 
		0!=(this->maximum != rhs.maximum).productum() || 
		0!=(this->cell_size != rhs.cell_size).productum() || 
		0!=(this->boundary != rhs.boundary).productum()) {
		return false;
	} else {
		return true;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements the non-identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmDomain::operator!=(pmDomain const& rhs) const {
    return !this->operator==(rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the minimum coordinate of the domain.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor const& pmDomain::get_minimum() const {
	return minimum;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the maximum coordinate of the domain.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor const& pmDomain::get_maximum() const {
	return maximum;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the cell size of the domain.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor const& pmDomain::get_cell_size() const {
	return cell_size;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of cells.
/////////////////////////////////////////////////////////////////////////////////////////
size_t pmDomain::get_num_cells() const {
	return std::round((maximum-minimum).productum());
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
	return minimum.multiply_term_by_term(cell_size);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns maximum*cellsize. Maximum stores the number of cells from the origin to the 
/// maximum corner of the domain.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDomain::get_physical_maximum() const {
	return maximum.multiply_term_by_term(cell_size);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the physical size of the domain.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDomain::get_physical_size() const {
	return (maximum-minimum).multiply_term_by_term(cell_size);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the boundary type of the domain (periodic, symmetric or cutoff).
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor const& pmDomain::get_boundary() const {
	return boundary;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the domain object content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::print() const {
	ProLog::pLogger::logf<ProLog::LYW>("            Domain:");
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n               min: ");
	minimum.print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n               max: ");
	maximum.print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n               boundary: ");
	boundary.print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n               cell size: ");
	cell_size.print();
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
void pmDomain::set_cell_size(pmTensor const& csize) {
	cell_size = csize;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Setter for boundary.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::set_boundary(pmTensor const& bnd) {
	boundary = bnd;
}

