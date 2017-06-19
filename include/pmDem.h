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
    
#ifndef _DEM_H_
#define _DEM_H_

#include "pmInteraction.h"
#include "prolog/pLogger.h"
#include "nauticle_constants.h"

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

	pmTensor vi = this->operand[0]->evaluate(i,level);
	pmTensor omi = this->operand[1]->evaluate(i,level);
	double Ri = this->operand[2]->evaluate(i,level)[0];
	double Ei = this->operand[3]->evaluate(i,level)[0];
	double nui = this->operand[4]->evaluate(i,level)[0];
	double sn = this->operand[5]->evaluate(i,level)[0];
	double dn = this->operand[6]->evaluate(i,level)[0];
	double dt = this->operand[7]->evaluate(i,level)[0];
	double ct = this->operand[8]->evaluate(i,level)[0];
	
	auto normal_force = [&](double const& delta, double const& delta_dot, double const& khz)->double {
			// spring+damping+Hertz
			return -sn*delta + sqrt(khz)*dn*delta_dot*std::pow(delta,0.25) - khz*std::pow(delta, 1.5);
	};
	auto tangential_force = [&](double const& tan_vel, double const& F_normal)->double {
			// spring+damping & Coulomb
			return std::max(dt*tan_vel, -F_normal*ct);
	};

	if(TYPE==LINEAR) {
		auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, double const& cell_size, pmTensor const& guide)->pmTensor{
			pmTensor force{dimension,1,0};
			double d_ji = rel_pos.norm();
			if(d_ji > NAUTICLE_EPS) {
				double Rj = this->operand[2]->evaluate(j,level)[0];
				double Ej = this->operand[3]->evaluate(j,level)[0];
				double nuj = this->operand[4]->evaluate(j,level)[0];
				double min_dist = Ri + Rj;
				if(d_ji < min_dist) {
					pmTensor n_ji = rel_pos / d_ji;
					pmTensor vj = this->operand[0]->evaluate(j,level).reflect(guide);
					pmTensor omj = this->operand[1]->evaluate(j,level).reflect(guide);
					pmTensor rel_vel = vj-vi;
					// overlap
					double delta = min_dist-d_ji;
					double delta_dot = (rel_vel.transpose()*n_ji)[0];
					double khz = Ei==0 && Ej==0 ? 0 : 4.0/3.0*sqrt(Ri*Rj/(Ri+Rj))*(Ei*Ej/(Ej*(1-nui*nui)+Ei*(1-nuj*nuj)));
					// normal_force
					double F_normal = normal_force(delta, delta_dot, khz);
					force = F_normal*n_ji;

					// relative tangential velocity
					pmTensor tan_vel = rel_vel - (rel_vel.transpose()*n_ji) * n_ji;
					double rci = Ri-delta/2.0;
					double rcj = Rj-delta/2.0;
					pmTensor wi = omi;
					pmTensor wj = omj;
					if(dimension==2) {
						wi = pmTensor{3,1,0};
						wi[2] = omi[0];
						wj = pmTensor{3,1,0};
						wj[2] = omj[0];
						tan_vel += (cross(wi,rci*n_ji.append(3,1)) + cross(wj,rcj*n_ji.append(3,1))).sub_tensor(0,1,0,0);
					} else if(dimension==3) {
						tan_vel += cross(wi,rci*n_ji) + cross(wj,rcj*n_ji);
					}
					// tangential shear and friction force
					double vt = tan_vel.norm();
					if(vt>NAUTICLE_EPS) {
						pmTensor t_ji = tan_vel/vt;
						double F_tangential = tangential_force(vt, F_normal);
						force += F_tangential*t_ji;
					}
				}
			}
			return force;
		};
		return this->interact(i, contribute);
	} else {
		auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, double const& cell_size, pmTensor const& guide)->pmTensor{
			pmTensor torque{dimension,1,0};
			torque.set_scalar(false);
			double d_ji = rel_pos.norm();
			if(d_ji > NAUTICLE_EPS) {
				double Rj = this->operand[2]->evaluate(j,level)[0];
				double Ej = this->operand[3]->evaluate(j,level)[0];
				double nuj = this->operand[4]->evaluate(j,level)[0];
				double min_dist = Ri + Rj;
				if(d_ji < min_dist) {
					pmTensor n_ji = rel_pos / d_ji;
					// overlap
					pmTensor vj = this->operand[0]->evaluate(j,level).reflect(guide);
					pmTensor omj = this->operand[1]->evaluate(j,level);
					pmTensor rel_vel = vj-vi;

					double delta = min_dist-d_ji;
					double delta_dot = (rel_vel.transpose()*n_ji)[0];
					double khz = Ei==0 && Ej==0 ? 0 : 4.0/3.0*sqrt(Ri*Rj/(Ri+Rj))*(Ei*Ej/(Ej*(1-nui*nui)+Ei*(1-nuj*nuj)));
					// normal_force
					double F_normal = normal_force(delta, delta_dot, khz);

					pmTensor tan_vel = rel_vel - (rel_vel.transpose()*n_ji) * n_ji;
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
						tan_vel += (cross(wi,rci*n_ji.append(3,1)) + cross(wj,rcj*n_ji.append(3,1))).sub_tensor(0,1,0,0);
						pmTensor force{2,1,0};
						// tangential shear and friction force
						double vt = tan_vel.norm();
						if(vt>NAUTICLE_EPS) {
							pmTensor t_ji = tan_vel/vt;
							double F_tangential = tangential_force(vt, F_normal);
							force = F_tangential*t_ji;
						}
						// torque
						torque += cross(force.append(3,1),n_ji.append(3,1)*rci).sub_tensor(2,2,0,0);
					} else if(dimension==3) {
						tan_vel += cross(wi,rci*n_ji) + cross(wj,rcj*n_ji);
						// tangential shear force
						pmTensor force{3,1,0};
						// tangential shear and friction force
						double vt = tan_vel.norm();
						if(vt>NAUTICLE_EPS) {
							pmTensor t_ji = tan_vel/vt;
							double F_tangential = tangential_force(vt, F_normal);
							force = F_tangential*t_ji;
						}
						// torque
						torque += cross(rci*n_ji,force);
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