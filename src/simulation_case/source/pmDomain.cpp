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
pmDomain::~pmDomain() {
	grid.clear();
}

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
	grid.resize(this->get_num_cells<1>());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of nodes registered in the workspace. For more information see e.g.
/// pmField and pmVariable classes.
/////////////////////////////////////////////////////////////////////////////////////////
size_t pmDomain::get_number_of_nodes() const {
	if(psys.use_count()==0) {
		return 1;
	}
	return psys->get_field_total_size();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new particle system to the workspace with an initialization tensor field. If a 
/// particle system is already defined, it will be replaced with the new one.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::add_particle_system(std::vector<pmTensor> const& values) {
	psys = std::make_shared<pmParticle_system>(values);
	psys->expire();
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
/// Restrict particles to the domain.
/////////////////////////////////////////////////////////////////////////////////////////
void pmDomain::restrict_particles(std::vector<size_t>& del) {
	if(psys->is_up_to_date()) { return; }
	pmTensor num_cells = maximum-minimum;
	for(int i=0; i<psys->get_field_size(); i++) {
		pmParticle& pi = psys->get_particle(i);
		pmTensor pos_i = pi.get_position();
		pmTensor grid_coords = get_grid_coordinates<0>(pos_i);
		pmTensor periodic_shift = -(grid_coords-mod(grid_coords,num_cells)).multiply_term_by_term(cell_size);
		size_t deletable = 0;
		for(int j=0; j<periodic_shift.numel(); j++) {
			periodic_shift[j] *= boundary[j]==2 ? 0.0 : 1.0;
			deletable -= (periodic_shift[j]!=0 && boundary[j]==2);
		}
		if(deletable>0) {
			del.push_back(i);
			continue;
		}
		if(!periodic_shift.is_zero()) {
			pi.shift(periodic_shift);
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
	for(int i=0; i<psys->get_field_total_size(); ++i) {
		pmParticle& pi = psys->get_particle(i);
		int hash_key = calculate_hash_key_from_position<1>(pi.get_position());
		if(!this->is_inside<1>(pi.get_position())) {
			return false;
		}
		pi.next = grid[hash_key];
		grid[hash_key] = &pi;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Update the domain (reinitialization of the grid and linked list and rebuild)
/////////////////////////////////////////////////////////////////////////////////////////
bool pmDomain::update() {
	static pmTensor domain_physical_maximum = maximum.multiply_term_by_term(cell_size);
	static pmTensor domain_physical_minimum = minimum.multiply_term_by_term(cell_size);
	if(psys.use_count()==0) {
		return false;
	}
	if(psys->is_up_to_date()) {
		return true;
	}
	// Update linked list of grid cells
	std::fill(grid.begin(),grid.end(),nullptr);
	if(!this->build_cell_list()) {
		return false;
	}
	// Iterate through outer cells to generate boundary particles.
	pmTensor domain_cells = maximum-minimum;
	// cellak
	virtual_particles.resize(0);
	for(auto const& it:grid) {
		// amelyikben van reszecske
		if(it!=nullptr) {
			// akorul vegigiteralunk
			for(auto const& cit:cell_iterator.get_list()) {
				// szomszedos cellaban egy pont
				pmTensor pos = it->get_position()+cit.multiply_term_by_term(cell_size);
				// annak a cellakoordinataja
				pmTensor grid_coords1 = get_grid_coordinates<1>(pos);
				int hash_key_outside = this->calculate_hash_key_from_grid_coordinates<1>(grid_coords1);
				if(grid[hash_key_outside]!=nullptr) {
					continue;
				}
				pmTensor grid_coords0 = get_grid_coordinates<0>(pos);
				// ahhoz pedig a delta
				pmTensor delta = -floor((grid_coords0).divide_term_by_term(domain_cells));
				// ha nem nulla (azaz a domainen kivul esik)
				if(delta.is_zero()) {
					continue;
				}
				// akkor kiszamoljuk, hogy melyik cellanak kene ott lennie (periodikus vagy szimmetrikus feltetel eseten)
				grid_coords1 += delta.multiply_term_by_term(domain_cells-boundary.multiply_term_by_term(domain_cells)+boundary);
				// annak a hash kulcsa
				int hash_key = this->calculate_hash_key_from_grid_coordinates<1>(grid_coords1);
				// abban a cellaban levo reszecskekbol kell virtualisakat generalnunk
				pmTensor guide = delta.multiply_term_by_term(boundary);
				for(pmParticle* pb = grid[hash_key]; pb!=nullptr; pb=pb->next) {
					// reszecske a meglevo reszecskebol
					pmParticle vp = *pb;
					// legyen az apja az eredeti
					vp.parent_index = pb-&psys->get_particle(0);
					// kell hozza guide ami megmondja, hogy majd a mezoket hogyan kell modositani
					vp.guide = guide;
					// aktualis pozicio
					pmTensor vpos = vp.get_position();
					// amit el kell tolni a peremfeltetelnek megfeleloen
					for(int k=0; k<boundary.numel(); k++) {
						if(delta[k]!=0) {
							if(boundary[k]==1) {
								vpos[k] += delta[k]*(delta[k]-1)*(domain_physical_maximum[k]-vpos[k]) + delta[k]*(delta[k]+1)*(domain_physical_minimum[k]-vpos[k]);
							} else {
								vpos[k] -= delta[k]*(domain_physical_maximum[k]-domain_physical_minimum[k]);
							}
						}
					}
					vp.set_position(vpos);
					// hozzaadhatjuk a reszecskerendszerhez
					virtual_particles.push_back(vp);
				}
				grid[hash_key_outside] = grid[hash_key];
			}
		}
	}
	psys->insert_virtual_particles(virtual_particles);
	// TODO: optimize grid update. The second build is unnecessary, only passive particles should be checked.
	std::fill(grid.begin(),grid.end(),nullptr);
	if(!this->build_cell_list()) {
		return false;
	}
	psys->set_up_to_date();
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the linked list of the cell with the given position.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle* pmDomain::get_linked_list(pmTensor const& pos) {
	if(!is_inside<1>(pos)) {
 		return nullptr;
	}
	int hash_key = calculate_hash_key_from_position<1>(pos);
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

























