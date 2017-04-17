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

    For more information please visit: https://bitbucket.org/lempsproject/
*/

#include "pmNeighbours.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmNeighbours::pmNeighbours(std::array<std::shared_ptr<pmExpression>,1> op) {
	operand = std::move(op);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmNeighbours::pmNeighbours(pmNeighbours const& other) {
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmNeighbours::pmNeighbours(pmNeighbours&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmNeighbours& pmNeighbours::operator=(pmNeighbours const& other) {
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
pmNeighbours& pmNeighbours::operator=(pmNeighbours&& other) {
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
std::shared_ptr<pmExpression> pmNeighbours::clone_impl() const {
	return std::make_shared<pmNeighbours>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmNeighbours> pmNeighbours::clone() const {
	return std::static_pointer_cast<pmNeighbours, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints DEM content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmNeighbours::print() const {
	pLogger::logf<COLOR>("neighbours");
	print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmNeighbours::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!assigned) { pLogger::error_msgf("Neighbour counter is not assigned to any particle system.\n"); }

	double rad = this->operand[0]->evaluate(i,level)[0];
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, double const& cell_size, pmTensor const& guide)->pmTensor{
		pmTensor num_neighbours{1,1,0};
		double distance = rel_pos.norm();
		if(distance < rad) {
			num_neighbours[0]++;
		}
		return num_neighbours;
	};
	return interact(i, contribute);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the size of the field.
/////////////////////////////////////////////////////////////////////////////////////////
int pmNeighbours::get_field_size() const {
	return psys.lock()->get_field_size();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes object to string.
/////////////////////////////////////////////////////////////////////////////////////////
void pmNeighbours::write_to_string(std::ostream& os) const {
	os << "neighbours()" << std::endl;
}


