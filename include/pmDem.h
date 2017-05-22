/*
    Copyright 2016-2017 Balazs Toth
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

    For more information please visit: https://bitbucket.org/lempsproject/
*/
    
#ifndef _DEM_H_
#define _DEM_H_

#include "pmInteraction.h"
#include "prolog/pLogger.h"

/** This class implements the conventianal Discrete element method as 
//  through interactions between particles. 
*/

enum DEM_TYPE {
	LINEAR,
	ANGULAR
};

template <DEM_TYPE TYPE, size_t NOPS>
class pmDem : public pmInteraction<NOPS> {
private:
	std::shared_ptr<pmExpression> clone_impl() const override;
public:
	pmDem() {}
	pmDem(std::array<std::shared_ptr<pmExpression>,NOPS> op);
	pmDem(pmDem const& other);
	pmDem(pmDem&& other);
	pmDem& operator=(pmDem const& other);
	pmDem& operator=(pmDem&& other);
	virtual ~pmDem() {}
	void print() const override;
	pmTensor evaluate(int const& i, size_t const& level=0) const override;
	std::shared_ptr<pmDem> clone() const;
	virtual void write_to_string(std::ostream& os) const override;
};

/////////////////////////////////////////////////////////////////////////////////////////
/// Implementaton of << operator.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE, size_t NOPS>
inline std::ostream& operator<<(std::ostream& os, pmDem<TYPE, NOPS> const* obj) {
	obj->write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE, size_t NOPS>
pmDem<TYPE, NOPS>::pmDem(std::array<std::shared_ptr<pmExpression>,NOPS> op) {
	this->operand = std::move(op);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE, size_t NOPS>
pmDem<TYPE, NOPS>::pmDem(pmDem<TYPE, NOPS> const& other) {
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE, size_t NOPS>
pmDem<TYPE, NOPS>::pmDem(pmDem<TYPE, NOPS>&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE, size_t NOPS>
pmDem<TYPE, NOPS>& pmDem<TYPE, NOPS>::operator=(pmDem<TYPE, NOPS> const& other) {
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
template <DEM_TYPE TYPE, size_t NOPS>
pmDem<TYPE, NOPS>& pmDem<TYPE, NOPS>::operator=(pmDem<TYPE, NOPS>&& other) {
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
template <DEM_TYPE TYPE, size_t NOPS>
std::shared_ptr<pmExpression> pmDem<TYPE, NOPS>::clone_impl() const {
	return std::make_shared<pmDem<TYPE, NOPS>>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE, size_t NOPS>
std::shared_ptr<pmDem<TYPE, NOPS>> pmDem<TYPE, NOPS>::clone() const {
	return std::static_pointer_cast<pmDem<TYPE, NOPS>, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints DEM content.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE, size_t NOPS>
void pmDem<TYPE, NOPS>::print() const {
	if(TYPE==LINEAR) {
		pLogger::logf<COLOR>("dem_l");
	} else {
		pLogger::logf<COLOR>("dem_a");
	}
	this->print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE, size_t NOPS>
pmTensor pmDem<TYPE, NOPS>::evaluate(int const& i, size_t const& level/*=0*/) const {
	if(!this->assigned) { pLogger::error_msgf("DEM model is not assigned to any particle system.\n"); }

	size_t dimension = this->psys.lock()->get_particle_space()->get_domain().get_dimensions();

	if(TYPE==LINEAR) {
		pmTensor vi = this->operand[0]->evaluate(i,level);
		pmTensor omi = this->operand[1]->evaluate(i,level);
		double Ri = this->operand[2]->evaluate(i,level)[0];
		double ks = this->operand[3]->evaluate(i,level)[0];
		double kd = this->operand[4]->evaluate(i,level)[0];
		double kf = this->operand[5]->evaluate(i,level)[0];

		auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, double const& cell_size, pmTensor const& guide)->pmTensor{
			pmTensor force;
			double d_ji = rel_pos.norm();
			if(d_ji > 1e-6) {
				double Rj = this->operand[2]->evaluate(j,level)[0];
				double min_dist = Ri + Rj;
				if(d_ji < min_dist) {
					pmTensor e_ji = rel_pos / d_ji;
					// overlap
					double delta = min_dist-d_ji;
					pmTensor vj = this->operand[0]->evaluate(j,level).reflect(guide);
					pmTensor omj = this->operand[1]->evaluate(j,level).reflect(guide);
					pmTensor rel_vel = vj-vi;
					// spring force
					force += -ks*delta*e_ji;
					// damping force
					force += kd*(rel_vel.transpose()*rel_pos)*e_ji;
					// relative tangential velocity
					pmTensor tan_vel = rel_vel - (rel_vel.transpose()*e_ji) * e_ji;
					// relative tangential velocity
					double rci = Ri-delta/2.0;
					double rcj = Rj-delta/2.0;
					pmTensor wi = omi;
					pmTensor wj = omj;
					if(dimension==2) {
						wi = pmTensor{3,1,0};
						wi[2] = omi[0];
						wj = pmTensor{3,1,0};
						wj[2] = omj[0];
						tan_vel += (cross(wi,rci*e_ji.append(3,1)) + cross(wj,rcj*e_ji.append(3,1))).sub_tensor(0,1,0,0);
					} else if(dimension==3) {
						tan_vel += cross(wi,rci*e_ji) + cross(wj,rcj*e_ji);
					}
					// tangential shear force
					force += kf*tan_vel;
				}
			}
			return force;
		};
		return this->interact(i, contribute);
	} else {
		pmTensor vi = this->operand[0]->evaluate(i,level);
		pmTensor omi = this->operand[1]->evaluate(i,level);
		double Ri = this->operand[2]->evaluate(i,level)[0];
		double kf = this->operand[3]->evaluate(i,level)[0];

		auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, double const& cell_size, pmTensor const& guide)->pmTensor{
			pmTensor torque;
			torque.set_scalar(false);
			double d_ji = rel_pos.norm();
			if(d_ji > 1e-6) {
				double Rj = this->operand[2]->evaluate(j,level)[0];
				double min_dist = Ri + Rj;
				if(d_ji < min_dist) {
					pmTensor e_ji = rel_pos / d_ji;
					// overlap
					double delta = min_dist-d_ji;
					pmTensor vj = this->operand[0]->evaluate(j,level).reflect(guide);
					pmTensor omj = this->operand[1]->evaluate(j,level);
					pmTensor rel_vel = vj-vi;

					pmTensor tan_vel = rel_vel - (rel_vel.transpose()*e_ji) * e_ji;
					// relative tangential velocity
					double rci = Ri-delta/2.0;
					double rcj = Rj-delta/2.0;
					pmTensor wi = omi;
					pmTensor wj = omj;
					if(dimension==2) {
						wi = pmTensor{3,1,0};
						wi[2] = omi[0];
						wj = pmTensor{3,1,0};
						wj[2] = omj[0];
						tan_vel += (cross(wi,rci*e_ji.append(3,1)) + cross(wj,rcj*e_ji.append(3,1))).sub_tensor(0,1,0,0);
						// tangential shear force
						pmTensor force = kf*tan_vel;
						// torque
						torque += cross(force.append(3,1),e_ji.append(3,1)*rci).sub_tensor(2,2,0,0);
					} else if(dimension==3) {
						tan_vel += cross(wi,rci*e_ji) + cross(wj,rcj*e_ji);
						// tangential shear force
						pmTensor force = kf*tan_vel;
						// torque
						torque += cross(force,rci*e_ji);
					}
				}
			}
			return torque;
		};
		return this->interact(i, contribute);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes operator to string.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE, size_t NOPS>
void pmDem<TYPE, NOPS>::write_to_string(std::ostream& os) const {
	if(TYPE==LINEAR) {
		os<<"dem_l("<<this->operand[0]<<","<<this->operand[1]<<","<<this->operand[2]<<","<<this->operand[3]<<","<<this->operand[4]<<","<<this->operand[5]<<")";
	} else {
		os<<"dem_a("<<this->operand[0]<<","<<this->operand[1]<<","<<this->operand[2]<<","<<this->operand[3]<<")";
	}
}

#endif //_DEM_H_