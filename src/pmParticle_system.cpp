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
    
#include "pmParticle_system.h"

/////////////////////////////////////////////////////////////////////////////////////////
//////////																		/////////
//////////							pmParticle_system							/////////
//////////																		/////////
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_system(std::string const& n, std::vector<pmTensor> const& value, pmDomain const& domain) : pmField{n,value} {
	particle_space = std::make_shared<pmParticle_space>(value.size(), domain);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_system(pmParticle_system const& other) : pmField{other} {
	this->particle_space = other.particle_space;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_system(pmParticle_system&& other) : pmField{other} {
	this->particle_space = std::move(other.particle_space);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system& pmParticle_system::operator=(pmParticle_system const& other) {
	if(this!=&other) {
		pmField::operator=(other);
		this->particle_space = other.particle_space;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system& pmParticle_system::operator=(pmParticle_system&& other) {
	if(this!=&other) {
		pmField::operator=(other);
		this->particle_space = std::move(other.particle_space);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the position value and makes the neighbour list expired.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::set_value(pmTensor const& value, int const& i/*=0*/) {
	pmField::set_value(value, i);
	particle_space->expire();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sorts the field of the particle system (position) and the given idx object.
/// Implements a sorting based on the hash key of the particles.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::sort_field(std::vector<int>& sorted_idx) {
	// shift particles due to the periodic domain
	particle_space->restrict_particles(current_value, previous_value, two_step);
	particle_space->update_neighbour_list(current_value, sorted_idx);
	// reorder particle positions due to sorted_idx
	pmField::sort_field(sorted_idx);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the neighbour list for the particle system.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_system::pmParticle_space> pmParticle_system::get_particle_space() {
	return particle_space;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints particle system content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::print() const {
	pLogger::logf<COLOR>("{%s}", name.c_str());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints particle system and particle space content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::printv() const {
	print();
	pLogger::logf("   ");
	pLogger::logf<LMA>("[%i by %i]   ", this->evaluate(0).get_numrows(), this->evaluate(0).get_numcols());
	particle_space->print();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmParticle_system::clone_impl() const {
	return std::make_shared<pmParticle_system>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_system> pmParticle_system::clone() const {
	return std::static_pointer_cast<pmParticle_system, pmExpression>(clone_impl());
}

void pmParticle_system::set_number_of_nodes(size_t const& N) {
	if(N!=current_value.size()) {
		pmField::set_number_of_nodes(N);
		particle_space->set_number_of_nodes(N);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////																		/////////
//////////							pmParticle_space							/////////
//////////																		/////////
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_space::pmParticle_space(size_t const& num_particles, pmDomain const& dom) {
	domain = dom;
	hash_key.resize(num_particles,0);
	size_t num_cells = domain.get_num_cells();
	cell_start.resize(num_cells,0);
	cell_end.resize(num_cells,0);
	build_cell_iterator();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_space::pmParticle_space(pmParticle_space const& other) {
	this->domain = other.domain;
	this->hash_key = other.hash_key;
	this->cell_start = other.cell_start;
	this->cell_end = other.cell_end;
	this->cell_iterator = other.cell_iterator;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_space::pmParticle_space(pmParticle_space&& other) {
	this->domain = std::move(other.domain);
	this->hash_key = std::move(other.hash_key);
	this->cell_start = std::move(other.cell_start);
	this->cell_end = std::move(other.cell_end);
	this->cell_iterator = std::move(other.cell_iterator);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_space& pmParticle_system::pmParticle_space::operator=(pmParticle_space const& other) {
	if(this!=&other) {
		this->domain = other.domain;
		this->hash_key = other.hash_key;
		this->cell_start = other.cell_start;
		this->cell_end = other.cell_end;
		this->cell_iterator = other.cell_iterator;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_space& pmParticle_system::pmParticle_space::operator=(pmParticle_space&& other) {
	if(this!=&other) {
		this->domain = std::move(other.domain);
		this->hash_key = std::move(other.hash_key);
		this->cell_start = std::move(other.cell_start);
		this->cell_end = std::move(other.cell_end);
		this->cell_iterator = std::move(other.cell_iterator);	
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Restrict particles to the domain.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmParticle_space::restrict_particles(std::vector<pmTensor>& current_value, std::vector<pmTensor>& previous_value, bool const& two_step) const {
	if(up_to_date) { return; }
	pmTensor domain_size = domain.get_physical_size();
	pmTensor domain_minimum = domain.get_physical_minimum();
	for(int i=0; i<current_value.size(); i++) {
		pmTensor shift = floor((current_value[i]-domain_minimum).divide_term_by_term(domain_size)).multiply_term_by_term(domain_size);
		current_value[i] = current_value[i] - shift;
		if(two_step) {
			previous_value[i] = previous_value[i] - shift;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the neighbour list expired.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmParticle_space::expire() {
	up_to_date = false;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the neighbour list validity.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::pmParticle_space::is_up_to_date() const {
	return up_to_date;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the flatten index in the domain grid.
/////////////////////////////////////////////////////////////////////////////////////////
float pmParticle_system::pmParticle_space::flatten(pmTensor const& cells, pmTensor const& grid_pos, size_t i) const {
	if(i>=cells.numel()) { return 0.0f; }
	return cells[i]*flatten(cells, grid_pos, i+1)+grid_pos[i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns hash key for the given grid cell.
/////////////////////////////////////////////////////////////////////////////////////////
int pmParticle_system::pmParticle_space::calculate_hash_key_from_grid_position(pmTensor const& grid_pos) const {
	pmTensor cells = domain.get_maximum()-domain.get_minimum();
	return flatten(cells, grid_pos-domain.get_minimum(), 0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns hash key for particle i.
/////////////////////////////////////////////////////////////////////////////////////////
int pmParticle_system::pmParticle_space::calculate_hash_key_from_position(pmTensor const& position) const {
	pmTensor grid_pos = floor(position/domain.get_cell_size());
	return calculate_hash_key_from_grid_position(grid_pos);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sorts the field of the particle system (position) and the given sorted_idx vector.
/// Implements a sorting based on the hash key of the particles.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmParticle_space::update_neighbour_list(std::vector<pmTensor> const& current_value, std::vector<int>& sorted_idx) {
	// perform neighbour search only when particle positions are changed
	if(up_to_date) { return; }
	for(int i=0; i<current_value.size(); ++i) {
		hash_key[i] = calculate_hash_key_from_position(current_value[i]);
		if(hash_key[i]<0 || hash_key[i]>=domain.get_num_cells()) { 
			pLogger::error_msgf("Particle is out of domain.\n");
		}
	}
	pmSort::sort_by_vector(sorted_idx, hash_key, pmSort::ascending);
	build_cell_arrays();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Builds the arrays that store the particle position layout.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmParticle_space::build_cell_arrays() {
	std::fill(cell_start.begin(),cell_start.end(),0xFFFFFFFF);
	std::fill(cell_end.begin(),cell_end.end(),0xFFFFFFFF);
    for(int i=1; i<hash_key.size(); i++){
        if(hash_key[i] != hash_key[i-1]) {
            cell_start[hash_key[i]] = i;
            cell_end[hash_key[i-1]] = i-1;
        }
        if(hash_key.size() == i+1) {
            cell_end[hash_key[i]] = i;
        }
    }
    cell_start[hash_key[0]] = 0;
    up_to_date = true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the vector of the first indices for all cells.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<unsigned int> const& pmParticle_system::pmParticle_space::get_start() const {
	return cell_start;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the vector of the last indices for all cells.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<unsigned int> const& pmParticle_system::pmParticle_space::get_end() const {
	return cell_end;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the hash key of particle i.
/////////////////////////////////////////////////////////////////////////////////////////
int pmParticle_system::pmParticle_space::get_hash_key(int const& i) const {
	return hash_key[i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the grid position of of the given point.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmParticle_system::pmParticle_space::get_grid_position(pmTensor const& point) const {
	return floor(point/domain.get_cell_size());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns domain relating to the particle system.
/////////////////////////////////////////////////////////////////////////////////////////
pmDomain pmParticle_system::pmParticle_space::get_domain() const {
	return domain;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints particle space content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmParticle_space::print() const {
	domain.print();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the comb_len length combinations of the numbers stored in elems.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmParticle_space::combinations_recursive(std::vector<int> const& elems, size_t comb_len, std::vector<size_t> &pos, size_t depth) {
	if(depth>=comb_len) {
		pmTensor tensor{domain.get_minimum()};
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
/// Returns the comb_len length combinations of the numbers stored in elems.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmParticle_space::combinations(std::vector<int> const& elems, size_t comb_len) {
	std::vector<size_t> positions(comb_len, 0);
	combinations_recursive(elems, comb_len, positions, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Builds the array that iterates over the adjacent cells.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmParticle_space::build_cell_iterator() {
	if(!cell_iterator.empty()) { return; }
	std::vector<int> elements = {-1, 0, 1};
	combinations(elements, domain.get_dimensions());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the cell iterator.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<pmTensor> const& pmParticle_system::pmParticle_space::get_cell_iterator() const {
	return cell_iterator;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_system::pmParticle_space> pmParticle_system::pmParticle_space::clone() const {
	return std::make_shared<pmParticle_space>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the number of nodes to the given N.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmParticle_space::set_number_of_nodes(size_t const& N) {
	hash_key.resize(N,0);
	this->expire();
}

