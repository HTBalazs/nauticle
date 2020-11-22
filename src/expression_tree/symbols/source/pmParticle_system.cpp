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
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle_system::pmParticle_system(std::vector<pmTensor> const& val) {
	value.reserve(val.size());
	for(auto const& it:val) {
		value.emplace_back(it);
	}
	size_t dimensions = val[0].numel();
	up_to_date = false;
	pmFirst_virtual_index::first_virtual_index = value.size();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints particle system content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>("{%s}", "r");
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
/// 
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::set_value(pmTensor const& val, int const& i/*=0*/) {
	pmCollection<pmParticle>::set_value(val,i);
	this->expire();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the ith particle's reference.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle& pmParticle_system::get_particle(size_t const& i) {
	return value[i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns "r".
/////////////////////////////////////////////////////////////////////////////////////////
std::string pmParticle_system::get_name() const {
	return "r";
}

bool pmParticle_system::is_up_to_date() const {
	return up_to_date;
}

void pmParticle_system::expire() {
	up_to_date = false;
}

void pmParticle_system::set_up_to_date() {
	up_to_date = true;
}

void pmParticle_system::destroy_virtual_members() {
	pmCollection<pmParticle>::destroy_virtual_members();
	this->expire();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes object to string.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::write_to_string(std::ostream& os) const {
	os << name;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Duplicates the member with the given index.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::duplicate_member(size_t const& i, pmTensor const& guide/*=pmTensor{}*/) {
	if(pmFirst_virtual_index::first_virtual_index <= i) {
		ProLog::pLogger::error_msgf("Duplication of virtual particles is not allowed.\n");
	}
	if(pmFirst_virtual_index::first_virtual_index<value.size()) {
		value.push_back(value[i]);
		pmFirst_virtual_index::first_virtual_index = value.size();
	}
	this->expire();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the whole content of the field.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_system::printv() const {
	this->print();
	for(int i=0; i<20-name.size(); i++) {
		ProLog::pLogger::logf<ProLog::LMA>(" ");
	}
	pmTensor tensor = this->evaluate(0);
	ProLog::pLogger::logf<ProLog::LMA>("[%i by %i]", tensor.get_numrows(), tensor.get_numcols());
}

bool pmParticle_system::operator==(pmParticle_system const& rhs) const {
	return this->value == rhs.value;
}

bool pmParticle_system::operator!=(pmParticle_system const& rhs) const {
	return !this->operator==(rhs);
}

void pmParticle_system::insert_virtual_particles(std::vector<pmParticle> const& particles) {
	value.insert(value.end(),particles.begin(),particles.end());
}








