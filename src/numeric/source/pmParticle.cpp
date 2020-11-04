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
pmParticle::pmParticle(pmTensor const& pos, bool const virt/*=false*/) {
	this->position = pos;
	this->_virtual = virt;
	this->delta = position[0]*0.0;
	this->next = nullptr;
	this->parent = nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle::pmParticle(pmParticle const& other) {
	this->position = other.position;
	this->delta = other.delta;
	this->_virtual = other._virtual;
	this->next = nullptr;
	this->parent = nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle& pmParticle::operator=(pmParticle const& other) {
	if(this!=&other) {
		this->position = other.position;
		this->_virtual = other._virtual;
		this->delta = other.delta;
		this->next = nullptr;
		this->parent = nullptr;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Overloading the = operator for tensor values.
/////////////////////////////////////////////////////////////////////////////////////////
pmParticle& pmParticle::operator=(pmTensor const& pos) {
	position = pos;
	delta = position[0]*0.0;
	next = nullptr;
	parent = nullptr;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the particle position.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle::set_position(pmTensor const& pos) {
	position = pos;
	delta = position[0]*0.0;
	next = nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the position of the particle.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor const& pmParticle::get_position(size_t const& level/*=0*/) const {
	return position[level];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set the storage depth for the particle system.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle::set_storage_depth(size_t const& d) {
	position.set_storage_depth(d);
}

bool pmParticle_set_virtual::is_virtual() const {
	return _virtual;
}

void pmParticle_set_virtual::set_virtual(bool const& virt) const {
	_virtual = virt;
}