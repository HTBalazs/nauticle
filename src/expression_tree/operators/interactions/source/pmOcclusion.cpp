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

#include "pmOcclusion.h"
#include "nauticle_constants.h"
#include "Color_define.h"
#include <cmath>

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmOcclusion::pmOcclusion(std::array<std::shared_ptr<pmExpression>,3> op) {
	this->operand = std::move(op);
	this->op_name = "occlusion";
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmOcclusion::pmOcclusion(pmOcclusion const& other) {
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
		this->op_name = "occlusion";
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmOcclusion::pmOcclusion(pmOcclusion&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
	this->op_name = "occlusion";
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmOcclusion& pmOcclusion::operator=(pmOcclusion const& other) {
	if(this!=&other) {
		this->assigned = false;
		for(int i=0; i<this->operand.size(); i++) {
			this->operand[i] = other.operand[i]->clone();
			this->op_name = "occlusion";
		}
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmOcclusion& pmOcclusion::operator=(pmOcclusion&& other) {
	if(this!=&other) {
		this->psys = std::move(other.psys);
		this->assigned = std::move(other.assigned);
		this->operand = std::move(other.operand);
		this->op_name = "occlusion";
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmOcclusion::clone_impl() const {
	return std::make_shared<pmOcclusion>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmOcclusion> pmOcclusion::clone() const {
	return std::static_pointer_cast<pmOcclusion, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints DEM content.
/////////////////////////////////////////////////////////////////////////////////////////
void pmOcclusion::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>("occlusion");
	print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmOcclusion::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!assigned) { ProLog::pLogger::error_msgf("Occlusion counter is not assigned to any particle system.\n"); }

	auto ps = this->psys.lock();
	pmTensor ri = ps->evaluate(i,level);
	pmTensor dir = this->operand[0]->evaluate(i,level);
	double ang = this->operand[1]->evaluate(i,level)[0];
	double rad = this->operand[2]->evaluate(i,level)[0];
	double occlusion = 0;
	auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
		double distance = rel_pos.norm();
		if(distance < rad + NAUTICLE_EPS && i!=j) {
			pmTensor vr = rel_pos;
			vr /= vr.norm();
			double phi = std::acos((dir.transpose()*vr)[0]);
			if(std::abs(phi)<ang) {
				double occl_ij = (dir.transpose()*vr)[0];
				if(occl_ij>occlusion) {
					occlusion = occl_ij;
				}
			}
		}
		return 0;
	};
	interact(i, contribute);
	return pmTensor{1,1,occlusion};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the size of the field.
/////////////////////////////////////////////////////////////////////////////////////////
int pmOcclusion::get_field_size() const {
	return psys.lock()->get_field_size();
}


