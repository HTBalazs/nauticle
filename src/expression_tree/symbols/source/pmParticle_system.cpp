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
pmParticle_system::pmParticle_system(std::string const& n, std::vector<pmTensor> const& value, std::shared_ptr<pmDomain> const& dm) : pmField{n,value} {
	domain = dm;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_system(pmParticle_system const& other) : pmField{other} {
	this->domain = other.domain;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_system(pmParticle_system&& other) : pmField{other} {
	this->domain = std::move(other.domain);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system& pmParticle_system::operator=(pmParticle_system const& other) {
	if(this!=&other) {
		pmField::operator=(other);
		this->domain = other.domain;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system& pmParticle_system::operator=(pmParticle_system&& other) {
	if(this!=&other) {
		pmField::operator=(other);
		this->domain = std::move(other.domain);
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
	if(this->domain != rhs.domain) {
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
	domain->set_expired();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the neighbour list for the particle system.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_system::pmDomain> pmParticle_system::get_domain() {
	return domain;
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
	domain->print();
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
		domain->set_number_of_nodes(N);
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
pmParticle_system::pmDomain::pmDomain(pmTensor const& dmin, pmTensor const& dmax, pmTensor const& csize, pmTensor const& bnd) {
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

/////////////////////////////////////////////////////////////////////////////////////////
/// Implement identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::pmDomain::operator==(pmDomain const& rhs) const {
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
bool pmParticle_system::pmDomain::operator!=(pmDomain const& rhs) const {
    return !this->operator==(rhs);
}

void pmParticle_system::pmDomain::restrict_particles(std::vector<std::vector<pmTensor>>& value, std::vector<size_t>& del) const {
	if(up_to_date) { return; }
	pmTensor cell_number = maximum-minimum;
	for(int i=0; i<value[0].size(); i++) {
		pmTensor g = grid_coordinates(value[0][i]);
		pmTensor shift = (g-mod(g,cell_number)).multiply_term_by_term(cell_size);
		for(auto& it:value) {
			size_t deletable = 0;
			for(int j=0; j<it[i].numel(); j++) {
				it[i][j] = it[i][j] - shift[j]*(boundary[j]!=2);
				deletable += (shift[j]!=0 && boundary[j]==2);
			}
			if(deletable>0) {
				del.push_back(i);
			}
		}
	}
}

void pmParticle_system::restrict_particles(std::vector<size_t>& del) {
	domain->restrict_particles(value, del);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the neighbour list expired.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmDomain::set_expired() {
	up_to_date = false;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the neighbour list validity.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::pmDomain::is_up_to_date() const {
	return up_to_date;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Flattens the d-dimensional space to 1D.
/////////////////////////////////////////////////////////////////////////////////////////
double pmParticle_system::pmDomain::flatten(pmTensor const& cells, pmTensor const& grid_pos, size_t i) const {
	if(i>=cells.numel()) { return 0.0; }
	return std::round((cells[i])*flatten(cells, grid_pos, i+1)+grid_pos[i]);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns hash key for the given grid cell.
/////////////////////////////////////////////////////////////////////////////////////////
int pmParticle_system::pmDomain::hash_key(pmTensor const& grid_pos) const {
	return flatten(maximum-minimum, grid_pos, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates the whole neighbour list.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::pmDomain::update(std::vector<pmTensor> const& current_value, std::vector<size_t>& del) {
	this->restrict_particles(del);
	size_t num_particles = current_value.size();
	pidx.resize(num_particles);
	std::fill(pidx.begin(), pidx.end(), -1);
	cidx.resize(std::round((maximum-minimum).productum()));
	std::fill(cidx.begin(), cidx.end(), -1);
	for(int i=0; i<num_particles; i++) {
        pmTensor grid_crd = grid_coordinates(current_value[i]);
        int hkey = hash_key(grid_crd);
        if(hkey>cidx.size() || hkey<0) {
        	return false;
        }
        if(cidx[hkey]!=-1) {
        	pidx[i] = cidx[hkey];
        }
        cidx[hkey] = i;
	}
	this->up_to_date = false;
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the grid coordingate of point.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmParticle_system::pmDomain::grid_coordinates(pmTensor const& point) const {
	return round(floor(point.divide_term_by_term(cell_size))-minimum);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints particle space content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmDomain::print() const {
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
/// Generates the component of the cell_iterator.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmDomain::combinations_recursive(std::vector<int> const& elems, size_t comb_len, std::vector<size_t> &pos, size_t depth) {
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
void pmParticle_system::pmDomain::combinations(std::vector<int> const& elems, size_t comb_len) {
	std::vector<size_t> positions(comb_len, 0);
	combinations_recursive(elems, comb_len, positions, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates the cell iterator.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::pmDomain::build_cell_iterator() {
	if(!cell_iterator.empty()) { return; }
	std::vector<int> elements = {-1, 0, 1};
	combinations(elements, this->get_dimensions());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the cell iterator.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<pmTensor> const& pmParticle_system::pmDomain::get_cell_iterator() const {
	this->build_cell_iterator();
	return cell_iterator;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_system::pmDomain> pmParticle_system::pmDomain::clone() const {
	return std::make_shared<pmDomain>(*this);
}

void pmParticle_system::pmDomain::set_number_of_nodes(size_t const& N) {
	pidx.resize(N);
}

size_t pmParticle_system::pmDomain::get_number_of_nodes() const {
	return pidx.size();
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
	domain->set_number_of_nodes(this->get_field_size());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Delete particles indexed in the indices vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::delete_set(std::vector<size_t> const& indices) {
	pmField::delete_set(indices);
	domain->set_number_of_nodes(this->get_field_size());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new particle to the system. The position is initialized based on the one in 
// 	the end of the vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::add_member(pmTensor const& v/*=pmTensor{}*/) {
	pmField::add_member(v);
	domain->set_number_of_nodes(this->get_field_size());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Duplicates particle with the given index.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::duplicate_member(size_t const& i) {
	pmField::duplicate_member(i);
	particle_space->set_number_of_nodes(this->get_field_size());
}

bool pmParticle_system::is_up_to_date() const {
	return domain->is_up_to_date();
}

bool pmParticle_system::update(std::vector<size_t>& del) {
	return domain->update(value[0], del);
}








