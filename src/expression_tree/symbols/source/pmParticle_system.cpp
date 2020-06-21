/*
    Copyright 2016-2019 Balazs Toth
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
    
#include "pmParticle_system.h"
#include <numeric>
#include "Color_define.h"

using namespace Nauticle;

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
	sorted_idx.resize(value.size());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_system(pmParticle_system const& other) : pmField{other} {
	this->particle_space = other.particle_space;
	this->sorted_idx = other.sorted_idx;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_system(pmParticle_system&& other) : pmField{other} {
	this->particle_space = std::move(other.particle_space);
	this->sorted_idx = std::move(other.sorted_idx);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system& pmParticle_system::operator=(pmParticle_system const& other) {
	if(this!=&other) {
		pmField::operator=(other);
		this->particle_space = other.particle_space;
		this->sorted_idx = other.sorted_idx;
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
		this->sorted_idx = std::move(other.sorted_idx);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implement identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::operator==(pmParticle_system const& rhs) const {
	if(!this->pmField::operator==(rhs)) {
		return false;
	}
	if(this->particle_space != rhs.particle_space) {
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements the non-identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::operator!=(pmParticle_system const& rhs) const {
    return !this->operator==(rhs);
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
bool pmParticle_system::sort_field() {
	// Check if already sorted
	if(particle_space->is_up_to_date()) { return true; }
	std::iota(sorted_idx.begin(), sorted_idx.end(), 0);
	bool success = particle_space->update_neighbour_list(value[0], sorted_idx);
	if(success) {
		// reorder particle positions due to sorted_idx
		success = pmField::sort_field(sorted_idx);
	}
	return success;
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
	ProLog::pLogger::logf<NAUTICLE_COLOR>("{%s}", name.c_str());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints particle system and particle space content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::printv() const {
	pmField::printv();
	ProLog::pLogger::line_feed(1);
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

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the number of particles in tha particle system.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::set_number_of_nodes(size_t const& N) {
	if(N!=value[0].size()) {
		pmField::set_number_of_nodes(N);
		particle_space->set_number_of_nodes(N);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the particle system is ordered.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::is_sorted() const {
	return particle_space->is_up_to_date();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the sorted index array.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<int> pmParticle_system::get_sorted_idx() const {
	return sorted_idx;
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
/// Implement identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::pmParticle_space::operator==(pmParticle_space const& rhs) const {
	return this->domain == rhs.domain;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements the non-identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::pmParticle_space::operator!=(pmParticle_space const& rhs) const {
    return !this->operator==(rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Restrict particles to the domain.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmParticle_space::restrict_particles(std::vector<std::vector<pmTensor>>& value, std::vector<size_t>& del) const {
	if(up_to_date) { return; }
	pmTensor domain_cell_size = domain.get_cell_size();
	pmTensor domain_cell_number = domain.get_maximum()-domain.get_minimum();
	for(int i=0; i<value[0].size(); i++) {
		pmTensor g = get_grid_position(value[0][i]);
		pmTensor shift = (g-mod(g,domain_cell_number)).multiply_term_by_term(domain_cell_size);
		pmTensor bound_type = domain.get_boundary();
		for(auto& it:value) {
			size_t deletable = 0;
			for(int j=0; j<it[i].numel(); j++) {
				it[i][j] = it[i][j] - shift[j]*(bound_type[j]!=2);
				deletable += (shift[j]!=0 && bound_type[j]==2);
			}
			if(deletable>0) {
				del.push_back(i);
			}
		}
	}
}

void pmParticle_system::restrict_particles(std::vector<size_t>& del) {
	particle_space->restrict_particles(value, del);
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
double pmParticle_system::pmParticle_space::flatten(pmTensor const& cells, pmTensor const& grid_pos, size_t i) const {
	if(i>=cells.numel()) { return 0.0; }
	return std::round((cells[i])*flatten(cells, grid_pos, i+1)+grid_pos[i]);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns hash key for the given grid cell.
/////////////////////////////////////////////////////////////////////////////////////////
int pmParticle_system::pmParticle_space::calculate_hash_key_from_grid_position(pmTensor const& grid_pos) const {
	pmTensor cells = domain.get_maximum()-domain.get_minimum();
	return flatten(cells, grid_pos, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns hash key for particle i.
/////////////////////////////////////////////////////////////////////////////////////////
int pmParticle_system::pmParticle_space::calculate_hash_key_from_position(pmTensor const& position) const {
	pmTensor grid_pos = get_grid_position(position);
	return calculate_hash_key_from_grid_position(grid_pos);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sorts the field of the particle system (position) and the given sorted_idx vector.
/// Implements a sorting based on the hash key of the particles.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::pmParticle_space::update_neighbour_list(std::vector<pmTensor> const& current_value, std::vector<int>& sorted_idx) {
	// perform neighbour search only when particle positions are changed
	if(up_to_date) { return true; }
	for(int i=0; i<current_value.size(); ++i) {
		hash_key[i] = calculate_hash_key_from_position(current_value[i]);
		if(hash_key[i]<0 || hash_key[i]>=domain.get_num_cells()) { 
			ProLog::pLogger::warning_msgf("Particle is out of domain.\n");
			return false;
		}
	}
	pmSort::sort_by_vector(sorted_idx, hash_key, pmSort::ascending);
	build_cell_arrays();
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Builds the arrays that store the particle position layout.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmParticle_space::build_cell_arrays() {
	std::fill(cell_start.begin(),cell_start.end(),0xFFFFFFFF);
	std::fill(cell_end.begin(),cell_end.end(),0xFFFFFFFF);
    cell_start[hash_key[0]] = 0;
    cell_end[hash_key[0]] = 0;
    for(int i=1; i<hash_key.size(); i++){
        if(hash_key[i] != hash_key[i-1]) {
            cell_start[hash_key[i]] = i;
            cell_end[hash_key[i-1]] = i-1;
        }
        if(hash_key.size() == i+1) {
            cell_end[hash_key[i]] = i;
        }
    }
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
int const& pmParticle_system::pmParticle_space::get_hash_key(int const& i) const {
	return hash_key[i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the grid position of of the given point.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmParticle_system::pmParticle_space::get_grid_position(pmTensor const& point) const {
	return round(floor(point.divide_term_by_term(domain.get_cell_size()))-domain.get_minimum());
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

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the number of particles managed in the neighbor search.
/////////////////////////////////////////////////////////////////////////////////////////
size_t pmParticle_system::pmParticle_space::get_number_of_nodes() const {
	return hash_key.size();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns if the object is the position.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::is_position() const {
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Delete particle from the system.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::delete_member(size_t const& i) {
	pmField::delete_member(i);
	size_t const new_size = sorted_idx.size()-1;
	sorted_idx.resize(new_size);
	particle_space->set_number_of_nodes(new_size);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Delete particles indexed in the indices vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::delete_set(std::vector<size_t> const& indices) {
	pmField::delete_set(indices);
	size_t const new_size = sorted_idx.size()-indices.size();
	particle_space->set_number_of_nodes(new_size);
	sorted_idx.resize(new_size);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new particle to the system. The position is initialized based on the one in 
// 	the end of the vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::add_member(pmTensor const& v/*=pmTensor{}*/) {
	pmField::add_member(v);
	sorted_idx.push_back(0);
	particle_space->set_number_of_nodes(sorted_idx.size());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Duplicates particle with the given index.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::duplicate_member(size_t const& i) {
	pmField::duplicate_member(i);
	sorted_idx.push_back(0);
	particle_space->set_number_of_nodes(sorted_idx.size());
}
