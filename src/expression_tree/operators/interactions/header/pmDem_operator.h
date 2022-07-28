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
    
#ifndef _DEM_H_
#define _DEM_H_

#include "pmInteraction.h"
#include "prolog/pLogger.h"
#include "nauticle_constants.h"
#include "Color_define.h"

namespace Nauticle {
	enum DEM_TYPE {
		LINEAR,
		ANGULAR
	};
	
	/** This class implements the conventianal Discrete element method as 
	//  through interactions between particles. 
	*/
	template <DEM_TYPE TYPE, size_t NOPS>
	class pmDem_operator : public pmInteraction<NOPS> {
	private:
		std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmDem_operator() {}
		pmDem_operator(std::array<std::shared_ptr<pmExpression>,NOPS> op);
		pmDem_operator(pmDem_operator const& other);
		pmDem_operator(pmDem_operator&& other);
		pmDem_operator& operator=(pmDem_operator const& other);
		pmDem_operator& operator=(pmDem_operator&& other);
		virtual ~pmDem_operator() {}
		void print() const override;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		std::shared_ptr<pmDem_operator> clone() const;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <DEM_TYPE TYPE, size_t NOPS>
	inline std::ostream& operator<<(std::ostream& os, pmDem_operator<TYPE, NOPS> const* obj) {
		obj->write_to_string(os);
		return os;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <DEM_TYPE TYPE, size_t NOPS>
	pmDem_operator<TYPE, NOPS>::pmDem_operator(std::array<std::shared_ptr<pmExpression>,NOPS> op) {
		this->operand = std::move(op);
		this->op_name = TYPE==LINEAR ? "dem_l" : "dem_a";
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <DEM_TYPE TYPE, size_t NOPS>
	pmDem_operator<TYPE, NOPS>::pmDem_operator(pmDem_operator<TYPE, NOPS> const& other) {
		this->assigned = false;
		for(int i=0; i<this->operand.size(); i++) {
			this->operand[i] = other.operand[i]->clone();
		}
		this->op_name = other.op_name;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Move constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <DEM_TYPE TYPE, size_t NOPS>
	pmDem_operator<TYPE, NOPS>::pmDem_operator(pmDem_operator<TYPE, NOPS>&& other) {
		this->psys = std::move(other.psys);
		this->assigned = std::move(other.assigned);
		this->operand = std::move(other.operand);
		this->op_name = std::move(other.op_name);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy assignment operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <DEM_TYPE TYPE, size_t NOPS>
	pmDem_operator<TYPE, NOPS>& pmDem_operator<TYPE, NOPS>::operator=(pmDem_operator<TYPE, NOPS> const& other) {
		if(this!=&other) {
			this->assigned = false;
			for(int i=0; i<this->operand.size(); i++) {
				this->operand[i] = other.operand[i]->clone();
			}
			this->op_name = other.op_name;
		}
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Move assignment operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <DEM_TYPE TYPE, size_t NOPS>
	pmDem_operator<TYPE, NOPS>& pmDem_operator<TYPE, NOPS>::operator=(pmDem_operator<TYPE, NOPS>&& other) {
		if(this!=&other) {
			this->psys = std::move(other.psys);
			this->assigned = std::move(other.assigned);
			this->operand = std::move(other.operand);
			this->op_name = std::move(other.op_name);
		}
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Clone implementation.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <DEM_TYPE TYPE, size_t NOPS>
	std::shared_ptr<pmExpression> pmDem_operator<TYPE, NOPS>::clone_impl() const {
		return std::make_shared<pmDem_operator<TYPE, NOPS>>(*this);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the copy of the object.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <DEM_TYPE TYPE, size_t NOPS>
	std::shared_ptr<pmDem_operator<TYPE, NOPS>> pmDem_operator<TYPE, NOPS>::clone() const {
		return std::static_pointer_cast<pmDem_operator<TYPE, NOPS>, pmExpression>(clone_impl());
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Prints DEM content.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <DEM_TYPE TYPE, size_t NOPS>
	void pmDem_operator<TYPE, NOPS>::print() const {
		ProLog::pLogger::logf<NAUTICLE_COLOR>(this->op_name.c_str());
		this->print_operands();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the interaction.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <DEM_TYPE TYPE, size_t NOPS>
	pmTensor pmDem_operator<TYPE, NOPS>::evaluate(int const& i, size_t const& level/*=0*/) const {
		if(!this->assigned) { ProLog::pLogger::error_msgf("DEM model is not assigned to any particle system.\n"); }
		size_t dimension = this->psys->get_dimensions();

		pmTensor vi = this->operand[0]->evaluate(i,level);
		pmTensor omi = this->operand[1]->evaluate(i,level);
		double Ri = this->operand[2]->evaluate(i,level)[0];
		double mi = this->operand[3]->evaluate(i,level)[0];
		double Ei = this->operand[4]->evaluate(i,level)[0];
		double nui = this->operand[5]->evaluate(i,level)[0];
		double ct = this->operand[6]->evaluate(i,level)[0];
		
		auto normal_force = [&](double const& delta, double const& delta_dot, double const& khz, double const& ck)->double {
				// damping+Hertz
				return ck*delta_dot*std::pow(delta,0.25) - khz*std::pow(delta, 1.5);
		};
		auto tangential_force = [&](double const& F_normal)->double {
				// damping & Coulomb
				return -F_normal*ct;
		};
		auto hertz_spring = [&](double const& Rj, double const& Ej, double const& nuj)->double {
			return Ei==0 && Ej==0 ? 0 : 4.0/3.0*std::sqrt(Ri*Rj/(Ri+Rj))*(Ei*Ej/(Ej*(1-nui*nui)+Ei*(1-nuj*nuj)));
		};
		auto hertz_damping = [&](double const& khz, double const& mj)->double {
			return std::sqrt(khz*(mi*mj)/(mi+mj)/2.0)/8.0;
		};

		if(TYPE==LINEAR) {
			auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
				pmTensor force{(int)dimension,1,0.0};
				double d_ji = rel_pos.norm();
				if(d_ji > NAUTICLE_EPS) {
					double Rj = this->operand[2]->evaluate(j,level)[0];
					double min_dist = Ri + Rj;
					if(d_ji < min_dist) {
						double mj = this->operand[3]->evaluate(j,level)[0];
						double Ej = this->operand[4]->evaluate(j,level)[0];
						double nuj = this->operand[5]->evaluate(j,level)[0];
						pmTensor n_ji = rel_pos / d_ji;
						pmTensor vj = this->operand[0]->evaluate(j,level).reflect_perpendicular(guide);
						pmTensor omj = this->operand[1]->evaluate(j,level).reflect_perpendicular(guide);
						// TODO: optimise
						if(!this->operand[1]->is_symmetric()) {
							pmTensor flip = pmTensor::make_tensor(guide, 1);
							for(int i=0; i<guide.numel(); i++) {
								if(guide[i]!=0) {
									flip = -1;
								}
							}
							omj *= flip.productum();
						}

						pmTensor rel_vel = vj-vi;
						// overlap
						double delta = min_dist-d_ji;
						double delta_dot = (rel_vel.transpose()*n_ji)[0];
						double khz = hertz_spring(Rj,Ej,nuj);
						double ck = hertz_damping(khz, mj);
						// normal_force
						double F_normal = normal_force(delta, delta_dot, khz, ck);
						force = F_normal*n_ji;
						// relative tangential velocity
						pmTensor tan_vel = rel_vel - (rel_vel.transpose()*n_ji) * n_ji;
						double rci = Ri-delta/2.0;
						double rcj = Rj-delta/2.0;
						pmTensor wi = omi;
						pmTensor wj = omj;
						if(dimension==2) {
							wi = pmTensor{3,1,0.0};
							wi[2] = omi[0];
							wj = pmTensor{3,1,0.0};
							wj[2] = omj[0];
							pmTensor nji = n_ji.append(3,1);
							tan_vel += (cross(wi,rci*nji) + cross(wj,rcj*nji)).sub_tensor(0,1,0,0);
						} else if(dimension==3) {
							tan_vel += cross(wi,rci*n_ji) + cross(wj,rcj*n_ji);
						}
						// tangential friction force
						double vt = tan_vel.norm();
						if(vt>NAUTICLE_EPS) {
							pmTensor t_ji = tan_vel/vt;
							double F_tangential = tangential_force(F_normal);
							force += F_tangential*t_ji;
						}
					}
				}
				return force;
			};
			return this->interact(i, contribute);
		} else {
			auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
				pmTensor torque;
				double d_ji = rel_pos.norm();
				if(d_ji > NAUTICLE_EPS) {
					double Rj = this->operand[2]->evaluate(j,level)[0];
					double min_dist = Ri + Rj;
					if(d_ji < min_dist) {
						double mj = this->operand[3]->evaluate(j,level)[0];
						double Ej = this->operand[4]->evaluate(j,level)[0];
						double nuj = this->operand[5]->evaluate(j,level)[0];
						pmTensor n_ji = rel_pos / d_ji;
						// overlap
						pmTensor vj = this->operand[0]->evaluate(j,level).reflect_perpendicular(guide);
						pmTensor omj = this->operand[1]->evaluate(j,level).reflect_perpendicular(guide);
						// TODO: optimise
						if(!this->operand[1]->is_symmetric()) {
							pmTensor flip = pmTensor::make_tensor(guide, 1);
							for(int i=0; i<guide.numel(); i++) {
								if(guide[i]!=0) {
									flip = -1;
								}
							}
							omj *= flip.productum();
						}
						pmTensor rel_vel = vj-vi;

						double delta = min_dist-d_ji;
						double delta_dot = (rel_vel.transpose()*n_ji)[0];
						double khz = hertz_spring(Rj,Ej,nuj);
						double ck = hertz_damping(khz,mj);
						// normal_force
						double F_normal = normal_force(delta, delta_dot, khz, ck);
						pmTensor tan_vel = rel_vel - (rel_vel.transpose()*n_ji) * n_ji;
						// relative tangential velocity
						double rci = Ri-delta/2.0;
						double rcj = Rj-delta/2.0;
						pmTensor wi = omi;
						pmTensor wj = omj;
						if(dimension==2) {
							wi = pmTensor{3,1,0.0};
							wi[2] = omi[0];
							wj = pmTensor{3,1,0.0};
							wj[2] = omj[0];
							pmTensor nji = n_ji.append(3,1);
							tan_vel += (cross(wi,rci*nji) + cross(wj,rcj*nji)).sub_tensor(0,1,0,0);
							pmTensor force{2,1,0};
							// tangential friction force
							double vt = tan_vel.norm();
							if(vt>NAUTICLE_EPS) {
								pmTensor t_ji = tan_vel/vt;
								double F_tangential = tangential_force(F_normal);
								force = F_tangential*t_ji;
							}
							// torque
							torque += cross(force.append(3,1),nji*rci).sub_tensor(2,2,0,0);
						} else if(dimension==3) {
							tan_vel += cross(wi,rci*n_ji) + cross(wj,rcj*n_ji);
							pmTensor force{3,1,0.0};
							// tangential friction force
							double vt = tan_vel.norm();
							if(vt>NAUTICLE_EPS) {
								pmTensor t_ji = tan_vel/vt;
								double F_tangential = tangential_force(F_normal);
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
}

#include "Color_undefine.h"


#endif //_DEM_H_