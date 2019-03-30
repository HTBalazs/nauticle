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
#include "pmLong_range.h"

namespace Nauticle {
	/** This class implements the conventianal Discrete element method as 
	//  through interactions between particles. 
	*/
	class pmCollision_counter : public pmLong_range, public pmInteraction<3> {
		mutable std::vector<int> count;
	private:
		std::shared_ptr<pmExpression> clone_impl() const override;
		void update_collision_counter(int const& i, size_t const& level=0);
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
		virtual void update(size_t const& level=0) override;
		void count_collisions() const;
	};
}

#endif //_COLLISION_H_