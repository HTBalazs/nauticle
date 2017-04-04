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

template <DEM_TYPE TYPE>
class pmDem : public pmInteraction<6> {
private:
	std::shared_ptr<pmExpression> clone_impl() const override;
public:
	pmDem() {}
	pmDem(std::array<std::shared_ptr<pmExpression>,6> op);
	pmDem(pmDem const& other);
	pmDem(pmDem&& other);
	pmDem& operator=(pmDem const& other);
	pmDem& operator=(pmDem&& other);
	virtual ~pmDem() {}
	void print() const override;
	pmTensor evaluate(int const& i, Eval_type eval_type/*=current*/) const override;
	std::shared_ptr<pmDem> clone() const;
	virtual void write_to_string(std::ostream& os) const override;
};

/////////////////////////////////////////////////////////////////////////////////////////
/// Implementaton of << operator.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE>
inline std::ostream& operator<<(std::ostream& os, pmDem<TYPE> const* obj) {
	obj->write_to_string(os);
	return os;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE>
pmDem<TYPE>::pmDem(std::array<std::shared_ptr<pmExpression>,6> op) {
	operand = std::move(op);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE>
pmDem<TYPE>::pmDem(pmDem<TYPE> const& other) {
	this->assigned = false;
	for(int i=0; i<this->operand.size(); i++) {
		this->operand[i] = other.operand[i]->clone();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE>
pmDem<TYPE>::pmDem(pmDem<TYPE>&& other) {
	this->psys = std::move(other.psys);
	this->assigned = std::move(other.assigned);
	this->operand = std::move(other.operand);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE>
pmDem<TYPE>& pmDem<TYPE>::operator=(pmDem<TYPE> const& other) {
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
template <DEM_TYPE TYPE>
pmDem<TYPE>& pmDem<TYPE>::operator=(pmDem<TYPE>&& other) {
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
template <DEM_TYPE TYPE>
std::shared_ptr<pmExpression> pmDem<TYPE>::clone_impl() const {
	return std::make_shared<pmDem<TYPE>>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE>
std::shared_ptr<pmDem<TYPE>> pmDem<TYPE>::clone() const {
	return std::static_pointer_cast<pmDem<TYPE>, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints DEM content.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE>
void pmDem<TYPE>::print() const {
	pLogger::logf<COLOR>("dem");
	print_operands();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the interaction.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE>
pmTensor pmDem<TYPE>::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	if(!assigned) { pLogger::error_msgf("DEM model is not assigned to any particle system.\n"); }

	size_t dimension = psys.lock()->get_particle_space()->get_domain().get_dimensions();

	if(TYPE==LINEAR) {
		pmTensor vi = operand[0]->evaluate(i,eval_type);
		pmTensor omi = operand[1]->evaluate(i,eval_type);
		float Ri = operand[2]->evaluate(i,eval_type)[0];
		float ks = operand[3]->evaluate(i,eval_type)[0];
		float kd = operand[4]->evaluate(i,eval_type)[0];
		float kf = -operand[5]->evaluate(i,eval_type)[0];

		auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, float const& cell_size, pmTensor const& guide)->pmTensor{
			pmTensor force;
			float d_ji = rel_pos.norm();
			if(d_ji > 1e-6f) {
				float Rj = operand[2]->evaluate(j,eval_type)[0];
				float min_dist = Ri + Rj;
				if(d_ji < min_dist) {
					pmTensor e_ji = rel_pos / d_ji;
					// overlap
					float delta = min_dist-d_ji;
					pmTensor vj = operand[0]->evaluate(j,eval_type).reflect(guide);
					pmTensor omj = operand[1]->evaluate(j,eval_type).reflect(guide);
					pmTensor rel_vel = vj-vi;
					// spring force
					// force += -ks*rel_pos*(min_dist/d_ji-1.0);
					force += -ks*delta*e_ji;
					// damping force
					force += kd*(rel_vel.transpose()*rel_pos)*e_ji;
					
					// // relative tangential velocity
					pmTensor tan_vel = rel_vel - (rel_vel.transpose()*e_ji) * e_ji;

					// relative tangential velocity
					float rci = Ri-delta/2.0;
					float rcj = Rj-delta/2.0;
					pmTensor wi = omi;
					pmTensor wj = omj;
					if(dimension==2) {
						wi = pmTensor{3,1,0};
						wi[2] = omi[0];
						wj = pmTensor{3,1,0};
						wj[2] = omj[0];
						tan_vel += (rci*cross(wi,e_ji.append(3,1)) + rcj*cross(wj,e_ji.append(3,1))).sub_tensor(0,1,0,0);
					} else if(dimension==3) {
						tan_vel += rci*cross(wi,e_ji) + rcj*cross(wj,e_ji);
					}
					// tangential shear force
					force += kf*tan_vel;

				}
			}
			return force;
		};
		return interact(i, contribute);
	} else {
		pmTensor vi = operand[0]->evaluate(i,eval_type);
		pmTensor omi = operand[1]->evaluate(i,eval_type);
		float Ri = operand[2]->evaluate(i,eval_type)[0];
		float kf = operand[3]->evaluate(i,eval_type)[0];

		auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, float const& cell_size, pmTensor const& guide)->pmTensor{
			pmTensor torque;
			torque.set_scalar(false);
			float d_ji = rel_pos.norm();
			if(d_ji > 1e-6f) {
				float Rj = operand[2]->evaluate(j,eval_type)[0];
				float min_dist = Ri + Rj;
				if(d_ji < min_dist) {
					pmTensor e_ji = rel_pos / d_ji;
					// overlap
					float delta = min_dist-d_ji;
					pmTensor vj = operand[0]->evaluate(j,eval_type).reflect(guide);
					pmTensor omj = operand[1]->evaluate(j,eval_type);
					pmTensor rel_vel = vj-vi;

					pmTensor tan_vel = rel_vel - (rel_vel.transpose()*e_ji) * e_ji;
					// relative tangential velocity
					float rci = Ri-delta/2.0;
					float rcj = Rj-delta/2.0;
					pmTensor wi = omi;
					pmTensor wj = omj;
					if(dimension==2) {
						wi = pmTensor{3,1,0};
						wi[2] = omi[0];
						wj = pmTensor{3,1,0};
						wj[2] = omj[0];
						tan_vel += (rci*cross(wi,e_ji.append(3,1)) + rcj*cross(wj,e_ji.append(3,1))).sub_tensor(0,1,0,0);
						// tangential shear force
						pmTensor force = kf*tan_vel;
						// torque
						torque += cross(e_ji.append(3,1)*rci,force.append(3,1)).sub_tensor(2,2,0,0);
					} else if(dimension==3) {
						tan_vel += rci*cross(wi,e_ji) + rcj*cross(wj,e_ji);
						// tangential shear force
						pmTensor force = kf*tan_vel;
						// torque
						torque += cross(e_ji*Ri,force);
					}
				}
			}
			return torque;
		};
		return interact(i, contribute);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes operator to string.
/////////////////////////////////////////////////////////////////////////////////////////
template <DEM_TYPE TYPE>
void pmDem<TYPE>::write_to_string(std::ostream& os) const {
	if(TYPE==LINEAR) {
		os << "dem_l(";
	} else {
		os << "dem_a(";
	}
	for(auto const& it:operand) {
		os << it;
	}
	os << ")";
}

#endif //_DEM_H_