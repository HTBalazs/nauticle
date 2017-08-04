/*
    Copyright 2016-2017 Balazs Toth
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

#include "pmNeighbors.h"
#include "nauticle_constants.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmNeighbors::pmNeighbors(std::array<std::shared_ptr<pmExpression>,1> op) {
	operand = std::move(op);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmNeighbors::pmNeighbors(pmNeighbors const& other) {
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmNeighbors::pmNeighbors(pmNeighbors&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmNeighbors& pmNeighbors::operator=(pmNeighbors const& other) {
	if(this!=&other) {
		this->assigned = false;
		for(int i=0; i<this->operand.size(); i++) {
			this->operand[i] = other.operand[i]->clone();
		}
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmNeighbors& pmNeighbors::operator=(pmNeighbors&& other) {
	if(this!=&other) {
		this->psys = std::move(other.psys);
		this->assigned = std::move(other.assigned);
		this->operand = std::move(other.operand);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmNeighbors::clone_impl() const {
	return std::make_shared<pmNeighbors>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmNeighbors> pmNeighbors::clone() const {
	return std::static_pointer_cast<pmNeighbors, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints DEM content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmNeighbors::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>("neighbors");
	print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmNeighbors::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!assigned) { ProLog::pLogger::error_msgf("Neighbour counter is not assigned to any particle system.\n"); }

	double rad = this->operand[0]->evaluate(i,level)[0];
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, double const& cell_size, pmTensor const& guide)->pmTensor{
		pmTensor num_neighbors{1,1,0};
		double distance = rel_pos.norm();
		if(distance < rad + NAUTICLE_EPS) {
			num_neighbors[0]++;
		}
		return num_neighbors;
	};
	return interact(i, contribute);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the size of the field.
/////////////////////////////////////////////////////////////////////////////////////////
int pmNeighbors::get_field_size() const {
	return psys.lock()->get_field_size();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes object to string.
/////////////////////////////////////////////////////////////////////////////////////////
void pmNeighbors::write_to_string(std::ostream& os) const {
	os << "neighbors()" << std::endl;
}


