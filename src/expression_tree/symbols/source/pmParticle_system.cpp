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
#include "commonutils/Common.h"
#include <numeric>
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_system(std::vector<pmTensor> const& value, pmDomain const& dm) : pmField{"r",value}, pmDomain{dm} {
	pidx.resize(value.size());
	periodic_jump = std::make_shared<pmField>("periodic_jump", value.size(), pmTensor{(int)dm.get_dimensions(),1,0});
	periodic_jump->set_field_size(value.size());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implement identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::operator==(pmParticle_system const& rhs) const {
	if(!this->pmField::operator==(rhs)) {
		return false;
	}
	return pmDomain::operator==(rhs);
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
void pmParticle_system::set_value(pmTensor const& value, int const& i/*=0*/, bool const& forced/*=false*/) {
	pmField::set_value(value, i, forced);
	if(locked[i] && !forced) { return; }
	up_to_date = false;
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
	pmDomain::printv();
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
void pmParticle_system::set_field_size(size_t const& N) {
	if(N!=value[0].size()) {
		pmField::set_field_size(N);
		pidx.resize(N);
		periodic_jump->set_field_size(N);
		this->up_to_date = false;
	}
}

void pmParticle_system::restrict_particles(std::vector<size_t>& del) {
	if(this->up_to_date) { return; }
	this->build_cell_iterator();
	pmTensor cell_number = maximum-minimum;
	for(int i=0; i<value[0].size(); i++) {
		pmTensor g = grid_coordinates(value[0][i]);
		pmTensor shift = (g-mod(g,cell_number)).multiply_term_by_term(cell_size);
		periodic_jump->set_value(periodic_jump->get_value(i)+shift.divide_term_by_term(this->get_physical_size()),i);
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

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the neighbour list expired.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::set_expired() {
	up_to_date = false;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the neighbour list validity.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::is_up_to_date() const {
	return up_to_date;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates the whole neighbour list.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmParticle_system::update_neighbor_list() {
	std::fill(pidx.begin(), pidx.end(), -1);
	cidx.resize(std::round((maximum-minimum).productum()));
	std::fill(cidx.begin(), cidx.end(), -1);
	for(int i=0; i<this->get_field_size(); i++) {
        pmTensor grid_crd = grid_coordinates(this->get_value(i));
        int hkey = hash_key(grid_crd);
        if(hkey>cidx.size() || hkey<0) {
        	return false;
        }
        if(cidx[hkey]!=-1) {
        	pidx[i] = cidx[hkey];
        }
        cidx[hkey] = i;
	}
	this->up_to_date = true;
	return true;
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
	pidx.resize(this->get_field_size());
	this->up_to_date = false;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Delete particles indexed in the indices vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::delete_set(std::vector<size_t> const& indices) {
	pmField::delete_set(indices);
	pidx.resize(this->get_field_size());
	this->up_to_date = false;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new particle to the system. The position is initialized based on the one in 
// 	the end of the vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::add_member(pmTensor const& v/*=pmTensor{}*/) {
	pmField::add_member(v);
	pidx.resize(this->get_field_size());
	periodic_jump->add_member(pmTensor{(int)this->get_dimensions(),1,0});
	this->up_to_date = false;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Duplicates particle with the given index.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::duplicate_member(size_t const& i) {
	pmField::duplicate_member(i);
	pidx.resize(this->get_field_size());
	periodic_jump->add_member(pmTensor{(int)this->get_dimensions(),1,0});
	this->up_to_date = false;
}

std::vector<int> const& pmParticle_system::get_cell_content(pmTensor const& grid_crd, int& index) const {
	index = cidx[this->hash_key(grid_crd)];
    return pidx;
}

std::shared_ptr<pmField> pmParticle_system::get_periodic_jump() const {
	return periodic_jump;
}

pmTensor pmParticle_system::get_periodic_shift(size_t const& i) const {
	return periodic_jump->evaluate(i).multiply_term_by_term(this->get_physical_size());
}

#ifdef JELLYFISH
std::string pmParticle_system::get_cpp_name() const {
	return this->prefix + "psys_" + this->name;
}
#endif // JELLYFISH
