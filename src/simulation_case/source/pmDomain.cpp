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
	this->build_cell_iterator();
}

bool pmDomain::operator==(pmDomain const& other) const {
	if(this->minimum == other.minimum && this->maximum == other.maximum && this->cell_size == other.cell_size && this->boundary == other.boundary) {
		return true;
	}
	return false;
}

bool pmDomain::operator!=(pmDomain const& other) const {
	return !(pmDomain::operator==(other));
}

pmTensor const& pmDomain::get_minimum() const {
	return minimum;
}

pmTensor const& pmDomain::get_maximum() const {
	return maximum;
}

pmTensor const& pmDomain::get_cell_size() const {
	return cell_size;
}

pmTensor const& pmDomain::get_boundary() const {
	return boundary;
}

size_t pmDomain::get_num_cells() const {
	return std::round((maximum-minimum).productum());
}

size_t pmDomain::get_dimensions() const {
	return cell_size.numel();
}

pmTensor pmDomain::get_physical_minimum() const {
	return minimum.multiply_term_by_term(cell_size);
}

pmTensor pmDomain::get_physical_maximum() const {
	return maximum.multiply_term_by_term(cell_size);
}

pmTensor pmDomain::get_physical_size() const {
	return (maximum-minimum).multiply_term_by_term(cell_size);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the cell iterator.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<pmTensor> const& pmDomain::get_cell_iterator() {
	this->build_cell_iterator();
	return cell_iterator;
}

void pmDomain::set_minimum(pmTensor const& mn) {
	minimum = mn;
}

void pmDomain::set_maximum(pmTensor const& mx) {
	maximum = mx;
}

void pmDomain::set_cell_size(pmTensor const& csize) {
	cell_size = csize;
}

void pmDomain::set_boundary(pmTensor const& bnd) {
	boundary = bnd; 
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates the component of the cell_iterator.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::combinations_recursive(std::vector<int> const& elems, size_t comb_len, std::vector<size_t> &pos, size_t depth) {
	if(depth>=comb_len) {
		pmTensor tensor = minimum;
		for (size_t i = 0; i < pos.size(); ++i) {
			tensor[i] = elems[pos[i]];
		}
		cell_iterator.push_back(tensor);
		return;
	}
	for(size_t i=0;i<elems.size();i++) {
		pos[depth] = i;
		combinations_recursive(elems, comb_len, pos, depth + 1);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates the cell_iterator.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::combinations(std::vector<int> const& elems, size_t comb_len) {
	std::vector<size_t> positions(comb_len, 0);
	combinations_recursive(elems, comb_len, positions, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates the cell iterator.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::build_cell_iterator() {
	if(!cell_iterator.empty()) { return; }
	std::vector<int> elements = {-1, 0, 1};
	combinations(elements, this->get_dimensions());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Flattens the d-dimensional space to 1D.
/////////////////////////////////////////////////////////////////////////////////////////
double pmDomain::flatten(pmTensor const& cells, pmTensor const& grid_pos, size_t i) const {
	if(i>=cells.numel()) { return 0.0; }
	return std::round((cells[i])*flatten(cells, grid_pos, i+1)+grid_pos[i]);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns hash key for the given grid cell.
/////////////////////////////////////////////////////////////////////////////////////////
int pmDomain::hash_key(pmTensor const& grid_pos) const {
	return flatten(maximum-minimum, grid_pos, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the grid coordingate of point.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDomain::grid_coordinates(pmTensor const& point) const {
	return round(floor(point.divide_term_by_term(cell_size))-minimum);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints particle space content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::printv() const {
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