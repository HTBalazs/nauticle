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
pmDomain::pmDomain(pmTensor const& dmin, pmTensor const& dmax, pmTensor const& csize, pmTensor const& bnd, pmTensor const& shft) {
	if(dmin.numel()!=dmax.numel() || dmax.numel()!=csize.numel() || csize.numel()!=bnd.numel() || csize.numel()<1 || csize.numel()>3) {
		ProLog::pLogger::error_msgf("Domain requires vectors of identical sizes and all of them need to be 1, 2 or 3 dimensional.\n");
	}
	if(!dmin.is_integer() || !dmax.is_integer()) {
		ProLog::pLogger::warning_msgf("At least one of the domain sizes is not integer multiple of the cell size.\n");
	}
	minimum = dmin;
	maximum = dmax;
	cell_size = csize;
	boundary = bnd;
	shift = shft;
	if(!shift_check()) {
		ProLog::pLogger::error_msgf("Shifting is infeasible.\n");
	}
	size_t num_cells = get_num_cells();
	cell_start.resize(num_cells,0);
	cell_end.resize(num_cells,0);
	cell_iterator.build_cell_iterator(cell_size.numel());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the shifting is feasible.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmDomain::shift_check() const {
	bool valid_size = true;
	bool valid_content = true;
	if(cell_size.numel()*cell_size.numel()!=shift.get_numcols()*shift.get_numrows()) {
		ProLog::pLogger::warning_msgf("Shifting needs to be 1x1 (in 1D), 2x2 (in 2D) or 3x3 (in 3D) matrix.\n");
		valid_size = false;
	}
	switch(shift.numel()) {
		case 1:
			if(std::abs(shift[0])>NAUTICLE_EPS) {
				valid_content = false;
			}
			break;
		case 4:
			valid_content = (std::abs(shift[0])<NAUTICLE_EPS && std::abs(shift[3])<NAUTICLE_EPS && shift.number_of_nonzero_elements()<2);
			break;
		case 9: 
			valid_content = (std::abs(shift[0])<NAUTICLE_EPS && std::abs(shift[5])<NAUTICLE_EPS && std::abs(shift[9])<NAUTICLE_EPS && shift.number_of_nonzero_elements()<3);
			break;
	}
	if(!valid_content) {
		ProLog::pLogger::warning_msgf("Shifting can have at most 0 (in 1D), 1 (in 2D) or 2 (in 3D) non-zero elements, and the main diagonal must contain only zeros.\n");
	}
	return (valid_size && valid_content);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implement identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmDomain::operator==(pmDomain const& rhs) const {
	if( 0!=(this->minimum != rhs.minimum).productum() || 
		0!=(this->maximum != rhs.maximum).productum() || 
		0!=(this->cell_size != rhs.cell_size).productum() || 
		0!=(this->boundary != rhs.boundary).productum() ||
		0!=(this->shift != rhs.shift).productum()) {
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
/// Returns the grid position of of the given point.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDomain::get_grid_position(pmTensor const& point) const {
	return round(floor(point.divide_term_by_term(cell_size))-minimum);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the neighbour list expired.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::expire() {
	up_to_date = false;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Add a particle system to the domain.
/////////////////////////////////////////////////////////////////////////////////////////
void add_particle_system(std::shared_ptr<pmParticle_system> ps) {
	if(ps.use_count()>0) {
		psys = ps;
		this->update();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the neighbour list validity.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmDomain::is_up_to_date() const {
	return up_to_date;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Restrict particles to the domain.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::restrict_particles(std::vector<std::vector<pmTensor>>& value, std::vector<size_t>& del) const {
	if(up_to_date) { return; }
	pmTensor domain_cell_number = maximum-minimum;
	for(int i=0; i<value[0].size(); i++) {
		pmTensor g = get_grid_position(value[0][i]);
		pmTensor periodic_shift = (g-mod(g,domain_cell_number)).multiply_term_by_term(cell_size);
		for(auto& it:value) {
			size_t deletable = 0;
			for(int j=0; j<it[i].numel(); j++) {
				it[i][j] -= periodic_shift[j]*(boundary[j]!=2);
				deletable += (periodic_shift[j]!=0 && boundary[j]==2);
			}
			if(deletable>0) {
				del.push_back(i);
			}
		}
	}
}

void pmDomain::calculate_grid_coords() {
	grid_coords.resize(psys->get_field_size());
	for(int i=0; i<psys->get_field_size(); i++) {
		grid_coords[i] = (psys->evaluate(i,0)+get_physical_minimum()).divide_term_by_term(cell_size);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the vector of the first indices for all cells.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<unsigned int> const& pmDomain::get_start() const {
	return cell_start;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the vector of the last indices for all cells.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<unsigned int> const& pmDomain::get_end() const {
	return cell_end;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the hash key of particle i.
/////////////////////////////////////////////////////////////////////////////////////////
int const& pmDomain::get_hash_key(int const& i) const {
	return hash_key[i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the cell iterator.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<pmTensor> const& pmDomain::get_cell_iterator() const {
	return cell_iterator.get_list();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmDomain> pmDomain::clone() const {
	return std::make_shared<pmDomain>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the number of nodes to the given N.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::set_number_of_nodes(size_t const& N) {
	hash_key.resize(N,0);
	this->expire();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the number of particles managed in the neighbor search.
/////////////////////////////////////////////////////////////////////////////////////////
size_t pmDomain::get_number_of_nodes() const {
	return hash_key.size();
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
/// Returns the shift of the periodic condition in each direction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor const& pmDomain::get_shift() const {
	return shift;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the domain object content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::print() const {
	ProLog::pLogger::logf<ProLog::LYW>("      Domain:");
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n         min: ");
	minimum.print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n         max: ");
	maximum.print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n         boundary: ");
	boundary.print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n         cell size: ");
	cell_size.print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n         shift: ");
	shift.print();
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

/////////////////////////////////////////////////////////////////////////////////////////
/// Setter for shifting.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::set_shift(pmTensor const& shft) {
	shift = shft;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Performs neighbor search and builds the list of neighbors for each particle.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmDomain::build_cell_list() {
	if(psys.use_count()==0 || up_to_date) {
		return false;
	}
	memset(grid, 0, this->get_num_cells()*sizeof(Particle*));
 
	for(int i=0; i<psys->get_field_size(); ++i) {
		Particle& pi = psys->evaluate(i,0);
		pmTensor grid_coords = floor((pi.get_position()-minimum.multiply_term_by_term(cell_size)).divide_term_by_term(cell_size));

		pi.set_next()grid[x+y*kGridWidth];
		grid[x+y*kGridWidth] = &pi;

		gridCoords[i*2] = x;
		gridCoords[i*2+1] = y;
	}
	return true;
}

void pmDomain::update() {
	// size_t depth = psys->get_storage_depth(); 
	// grid.resize(depth);
	// grid_coords.resize(depth);
	// for(int i=0; i<depth; i++) {
	// 	grid.resize(this->get_num_cells());
	// 	memset(grid[i], 0, this->get_num_cells()*sizeof(Particle*));
	// 	grid_coords.resize(psys->get_field_size());
	// 	memset(grid_coords[i], 0, psys->get_field_size()*sizeof(Particle*));
	// }
}






