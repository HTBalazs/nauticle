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
	class pmCollision_handler : public pmLong_range<5,pmCollision_handler> {
		mutable std::vector<int> count;
	private:
		std::shared_ptr<pmExpression> clone_impl() const override;
		void create_pairs(int const& i, size_t const& level=0);
		void remove_unnecessary_pairs(size_t const& level=0);
		void evaluate_pairs(size_t const& level=0);
		void count_collisions(size_t const& level=0) const;
	public:
		pmCollision_handler(std::array<std::shared_ptr<pmExpression>,5> op);
		pmCollision_handler(pmCollision_handler const& other);
		pmCollision_handler(pmCollision_handler&& other);
		pmCollision_handler& operator=(pmCollision_handler const& other);
		pmCollision_handler& operator=(pmCollision_handler&& other);
		virtual ~pmCollision_handler() {}
		std::shared_ptr<pmCollision_handler> clone() const;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		virtual void update(size_t const& level=0) override;
		void print() const override;
	};
}

#endif //_COLLISION_H_