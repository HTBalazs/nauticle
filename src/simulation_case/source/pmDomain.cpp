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
#include "pmConstant.h"
#include "Color_define.h"

using namespace Nauticle;


/////////////////////////////////////////////////////////////////////////////////////////
/// Definition of a pure virtual destructor. It is used to make pmDomain abstract.
/////////////////////////////////////////////////////////////////////////////////////////
pmDomain::~pmDomain() {}

/////////////////////////////////////////////////////////////////////////////////////////
/// Setter for the domain.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::set_domain_parameters(pmTensor const& dmin, pmTensor const& dmax, pmTensor const& csize, pmTensor const& bnd, pmTensor const& shft) {
	if(dmin.numel()!=dmax.numel() || dmax.numel()!=csize.numel() || csize.numel()!=bnd.numel() || csize.numel()<1 || csize.numel()>3) {
		ProLog::pLogger::error_msgf("Domain requires vectors of identical sizes and all of them need to be 1, 2 or 3 dimensional.\n");
	}
	if(!dmin.is_integer() || !dmax.is_integer()) {
		dmin.print();
		dmax.print();
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
	cell_iterator.build_cell_iterator(cell_size.numel());
	grid.resize(this->get_num_cells());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of nodes registered in the workspace. For more information see e.g.
/// pmField and pmVariable classes.
/////////////////////////////////////////////////////////////////////////////////////////
size_t pmDomain::get_number_of_nodes() const {
	if(psys.use_count()==0) {
		return 1;
	}
	return psys->get_field_size();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new particle system to the workspace with an initialization tensor field. If a 
/// particle system is already defined, it will be replaced with the new one.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::add_particle_system(std::vector<pmTensor> const& values) {
	psys = std::make_shared<pmParticle_system>(values);
	psys->expire();
	this->update();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the particle system of the domain object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_system> pmDomain::get_particle_system() const {
	return psys;
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
/// Returns the grid position of of the given point.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmDomain::get_grid_position(pmTensor const& point) const {
	return round(floor(point.divide_term_by_term(cell_size))-minimum)+1.0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Restrict particles to the domain.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::restrict_particles(std::vector<std::vector<pmTensor>>& value, std::vector<size_t>& del) const {
	if(psys->is_up_to_date()) { return; }
	pmTensor domain_cell_number = maximum-minimum;
	for(int i=0; i<value[0].size(); i++) {
		pmTensor g = get_grid_position(value[0][i])-1.0;
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

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmDomain> pmDomain::clone() const {
	return clone_impl();
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
	ProLog::pLogger::logf<ProLog::LYW>("         Domain:");
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n            min: ");
	minimum.print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n            max: ");
	maximum.print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n            boundary: ");
	boundary.print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n            cell size: ");
	cell_size.print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>("\n            shift: ");
	shift.print();
	ProLog::pLogger::line_feed(1);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Builds the linked list of particles for each cell.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmDomain::build_cell_list() {
	for(int i=0; i<psys->get_field_size(); ++i) {
		pmParticle& pi = psys->get_particle(i);
		int hash_key = calculate_hash_key_from_position(pi.get_position());
		if(hash_key<0) {
			return false;
		}
		pi.set_next(grid[hash_key]);
		grid[hash_key] = &pi;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Update the domain (reinitialization of the grid and linked list and rebuild)
/////////////////////////////////////////////////////////////////////////////////////////
bool pmDomain::update() {
	if(psys.use_count()==0) {
		return false;
	}
	if(psys->is_up_to_date()) {
		return true;
	}
	memset(&grid[0], 0, this->get_num_cells()*sizeof(pmParticle*));
	if(this->build_cell_list()) {
		psys->set_up_to_date();
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the linked list of the cell with the given position.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle* pmDomain::get_linked_list(pmTensor const& pos) {
	int hash_key = calculate_hash_key_from_position(pos);
	return grid[hash_key];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the vector of linked list for all neighboring cells.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::get_neighbors(pmTensor const& pos, std::vector<pmParticle*>& nb) {
	std::vector<pmTensor> const& cell_list = cell_iterator.get_list();
	nb.reserve(cell_list.size());
	for(auto const& it:cell_list) {
		pmParticle* llist = get_linked_list(pos+cell_size.multiply_term_by_term(it));
		if(llist!=nullptr) {
			nb.push_back(llist);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the flatten index in the domain grid.
/////////////////////////////////////////////////////////////////////////////////////////
double pmDomain::flatten(pmTensor const& cells, pmTensor const& grid_pos, size_t i) const {
	if(i>=cells.numel()) { return 0.0; }
	return std::round((cells[i])*flatten(cells, grid_pos, i+1)+grid_pos[i]);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns hash key for the given grid cell.
/////////////////////////////////////////////////////////////////////////////////////////
int pmDomain::calculate_hash_key_from_grid_position(pmTensor const& grid_pos) const {
	pmTensor cells = maximum-minimum+2.0;
	return flatten(cells, grid_pos, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns hash key for particle i.
/////////////////////////////////////////////////////////////////////////////////////////
int pmDomain::calculate_hash_key_from_position(pmTensor const& position) const {
	pmTensor grid_pos = get_grid_position(position);
	return calculate_hash_key_from_grid_position(grid_pos);
}

bool pmDomain::is_stationary_domain() const {
	return true;
}


