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

    For more information please visit: https://BalazsToth@bitbucket.org/BalazsToth/lemps
*/
    
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <memory>
#include <vector>
#include "pmTensor.h"
#include "pmParticle_system.h"

enum Interaction_type {ONEWAY, YAWENO, TWOWAY};

class pmAABB {
	pmTensor minimum;
	pmTensor maximum;
public:
};

class pmObject {
protected:
	std::weak_ptr<pmParticle_system> psys;
	pmAABB aabb;
	float thickness;
protected:
	virtual std::shared_ptr<pmObject> clone_impl() const=0;
public:
	pmObject(pmObject const& other);
	pmObject(pmObject&& other);
	pmObject& operator=(pmObject const& other);
	pmObject& operator=(pmObject&& other);
	void assign_to_particle_system();
	std::shared_ptr<pmObject> clone() const;
	pmTensor sample(std::shared_ptr<pmField> field, std::shared_ptr<pmField> mass, std::shared_ptr<pmField> density) const;
};

pmObject(pmObject const& other) {
	this->psys = other.psys;
	this->aabb = other.aabb;
	this->thickness = other.thickness;
}
pmObject(pmObject&& other) {
	this->psys = std::move(other.psys);
	this->aabb = std::move(other.aabb);
	this->thickness = std::move(other.thickness);
}
pmObject& operator=(pmObject const& other) {
	if(this!=&other) {
		this->psys = other.psys;
		this->aabb = other.aabb;
		this->thickness = other.thickness;
	}
	return this;
}
pmObject& operator=(pmObject&& other) {
	if(this!=&other) {
		this->psys = std::move(other.psys);
		this->aabb = std::move(other.aabb);
		this->thickness = std::move(other.thickness);
	}
	return this;
}
std::shared_ptr<pmObject> pmObject::clone() const {
	return clone_impl();
}

class pmVertex : public pmObject {
	pmTensor position;
public:
};

class pmLink : public pmObject {
	std::shared_ptr<pmVertex> vertex1;
	std::shared_ptr<pmVertex> vertex2;
	std::vector<pmTensor> sampling_nodes;
protected:
	virtual std::shared_ptr<pmObject> clone_impl() const override;
public:
	pmLink() {}
	pmLink(pmLink const& other);
	pmLink(pmLink&& other);
	pmLink& operator=(pmLink const& other);
	pmLink& operator=(pmLink&& other);
	virtual ~pmLink() {}
	void set_vertex1(std::shared_ptr<pmVertex> vertex);
	void set_vertex2(std::shared_ptr<pmVertex> vertex);
	void get_vertex1() const;
	void get_vertex2() const;
	void update();
	std::shared_ptr<pmLink> clone() const;
};

pmLink(pmLink const& other) : pmObject{other} {
	this->vertex1 = other.vertex1;
	this->vertex2 = other.vertex2;
	this->sampling_nodes = other.sampling_nodes;
}
pmLink(pmLink&& other) : pmObject{other} {
	this->vertex1 = std::move(other.vertex1);
	this->vertex2 = std::move(other.vertex2);
	this->sampling_nodes = std::move(other.sampling_nodes);
}
pmLink& operator=(pmLink const& other) {
	if(this!=&other) {
		pmObject::operator=(other);
		this->vertex1 = other.vertex1;
		this->vertex2 = other.vertex2;
		this->sampling_nodes = other.sampling_nodes;
	}
	return *this;
}
pmLink& operator=(pmLink&& other) {
	if(this!=&other) {
		pmObject::operator=(other);
		this->vertex1 = std::move(other.vertex1);
		this->vertex2 = std::move(other.vertex2);
		this->sampling_nodes = std::move(other.sampling_nodes);
	}
	return *this;
}

void pmLink::set_vertex1(std::shared_ptr<pmVertex> vertex) {
	vertex1 = vertex;
}
void pmLink::set_vertex2(std::shared_ptr<pmVertex> vertex) {
	vertex2 = vertex;
}
void pmLink::get_vertex1() const {
	return vertex1;
}
void pmLink::get_vertex2() const {
	return vertex2;
}
void pmLink::update() {
	sampling_nodes.clear();
	if(psys.expired()) { return; }
	float distance = (vertex2->get_position()-vertex1.get_position()).norm();
	size_t num_samples = (size_t)(distance/thickness/2)+1;
	sampling_nodes.push_back(vertex->get_position());
	for(int i=1; i<num_samples-1; i++) {
		pmTensor sample = vertex1->get_position()+i*(vertex2->get_position()-vertex1.get_position())/(num_samples-1);
		sampling_nodes.push_back(sample);
	}
}

pmTensor pmLink::sample(std::shared_ptr<pmField> field, std::shared_ptr<pmField> mass, std::shared_ptr<pmField> density) const {
	std::array<std::shared_ptr<pmExpression>,4> op = {field, mass, density, std::make_shared<pmConstant>(field->evaluate(0).numel()-1)};
	std::shared_ptr<pmExpression> sph = std::make_shared<pmSph_operator<SAMPLE,0,0>>(op);
	return sph->evaluate(position);
}

std::shared_ptr<pmObject> pmLink::clone_impl() const {
	return std::make_shared<pmLink>(*this);
}

std::shared_ptr<pmLink> pmLink::clone() const {
	return std::static_pointer_cast<pmLink, pmObject>(clone_impl());
}


#endif //_OBJECT_H_