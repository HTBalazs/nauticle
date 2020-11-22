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

#include "pmParticle.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle::pmParticle(pmHTensor const& pos) {
	this->position = pos;
	this->next = nullptr;
	this->parent_index = -1;
	this->guide = position(0)*0.0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle::pmParticle(pmParticle const& other) {
	this->position = other.position;
	this->next = nullptr;
	this->parent_index = other.parent_index;
	this->guide = other.guide;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle& pmParticle::operator=(pmParticle const& other) {
	if(this!=&other) {
		this->position = other.position;
		this->next = nullptr;
		this->parent_index = other.parent_index;
		this->guide = other.guide;
	}
	return *this;
}

pmParticle::pmParticle(pmParticle&& other) {
	this->position = other.position;
	this->next = other.next;
	this->parent_index = other.parent_index;
	this->guide = other.guide;
	other.next = nullptr;
}

pmParticle& pmParticle::operator=(pmParticle&& other) {
	if(this!=&other) {
		this->position = other.position;
		this->next = other.next;
		this->parent_index = other.parent_index;
		this->guide = other.guide;
		other.next = nullptr;
	}
	return *this;
}

pmTensor const& pmParticle::operator()(size_t const& level) const {
	return position(level);
}

pmTensor& pmParticle::operator()(size_t const& level){
	return position(level);
}

bool pmParticle::operator==(pmParticle const& other) const {
	return this->position==other.position;
}

bool pmParticle::operator!=(pmParticle const& other) const {
	return !this->operator==(other);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Overloading the = operator for tensor values.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle& pmParticle::operator=(pmTensor const& pos) {
	next = nullptr;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the particle position.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle::set_position(pmTensor const& pos) {
	position = pos;
	next = nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the position of the particle.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor const& pmParticle::get_position(size_t const& level/*=0*/) const {
	return position(level);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set the storage depth for the particle system.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle::set_storage_depth(size_t const& d) {
	position.set_storage_depth(d);
}

size_t pmParticle::get_storage_depth() const {
	return position.get_storage_depth();
}

void pmParticle::shift(pmTensor const& distance) {
	for(auto& it:position) {
		it += distance;
	}
}

void pmParticle::initialize(pmTensor const& val) {
	position.initialize(val);
}

void pmParticle::print() const {
	position(0).print();
}