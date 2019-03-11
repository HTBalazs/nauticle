/*
    Copyright 2016-2019 Balazs Toth
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
    
#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "pmInteraction.h"
#include "prolog/pLogger.h"
#include "nauticle_constants.h"
#include "Color_define.h"

namespace Nauticle {
	/** This class implements the conventianal Discrete element method as 
	//  through interactions between particles. 
	*/
	class pmCollision_counter : public pmInteraction<3> {
	private:
		std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmCollision_counter() {}
		pmCollision_counter(std::array<std::shared_ptr<pmExpression>,3> op);
		pmCollision_counter(pmCollision_counter const& other);
		pmCollision_counter(pmCollision_counter&& other);
		pmCollision_counter& operator=(pmCollision_counter const& other);
		pmCollision_counter& operator=(pmCollision_counter&& other);
		virtual ~pmCollision_counter() {}
		void print() const override;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		std::shared_ptr<pmCollision_counter> clone() const;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmCollision_counter const* obj) {
		obj->write_to_string(os);
		return os;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmCollision_counter::pmCollision_counter(std::array<std::shared_ptr<pmExpression>,3> op) {
		this->operand = std::move(op);
		this->op_name = "count_collisions";
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmCollision_counter::pmCollision_counter(pmCollision_counter const& other) {
		this->assigned = false;
		for(int i=0; i<this->operand.size(); i++) {
			this->operand[i] = other.operand[i]->clone();
		}
		this->op_name = other.op_name;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Move constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmCollision_counter::pmCollision_counter(pmCollision_counter&& other) {
		this->psys = std::move(other.psys);
		this->assigned = std::move(other.assigned);
		this->operand = std::move(other.operand);
		this->op_name = std::move(other.op_name);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Copy assignment operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmCollision_counter& pmCollision_counter::operator=(pmCollision_counter const& other) {
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
	inline pmCollision_counter& pmCollision_counter::operator=(pmCollision_counter&& other) {
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
	inline std::shared_ptr<pmExpression> pmCollision_counter::clone_impl() const {
		return std::make_shared<pmCollision_counter>(*this);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the copy of the object.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::shared_ptr<pmCollision_counter> pmCollision_counter::clone() const {
		return std::static_pointer_cast<pmCollision_counter, pmExpression>(clone_impl());
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Prints DEM content.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline void pmCollision_counter::print() const {
		ProLog::pLogger::logf<NAUTICLE_COLOR>(this->op_name.c_str());
		this->print_operands();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the interaction.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pmCollision_counter::evaluate(int const& i, size_t const& level/*=0*/) const {
		if(!this->assigned) { ProLog::pLogger::error_msgf("Collision counter is not assigned to any particle system.\n"); }
		size_t dimension = this->psys.lock()->get_particle_space()->get_domain().get_dimensions();
		double Ri = this->operand[0]->evaluate(i,level)[0];
		auto contribute = [&](pmTensor const& rel_pos, int const& i, int const& j, pmTensor const& cell_size, pmTensor const& guide)->pmTensor{
			pmTensor force{(int)dimension,1,0.0};
			double d_ji = rel_pos.norm();
			if(d_ji > NAUTICLE_EPS) {
				double Rj = this->operand[0]->evaluate(j,level)[0];
				double min_dist = Ri + Rj;
				pmParticle_system::pmMesh& links = this->psys.lock()->get_links();
				int link_id = links.get_link_id(i,j);
				if(d_ji < min_dist) {
					if(link_id<0 && i<j) {
						links.add_link(i,j,d_ji);
					}
				} else if(link_id!=-1) {
					links.delete_link(link_id);
				}
			}
			return force;
		};
		return this->interact(i, contribute);	
	}
}

#include "Color_undefine.h"


#endif //_COLLISION_H_