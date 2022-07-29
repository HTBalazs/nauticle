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
    
#ifndef _SPRING_H_
#define _SPRING_H_

#include "pmInteraction.h"
#include "prolog/pLogger.h"
#include "nauticle_constants.h"
#include "pmLong_range.h"

namespace Nauticle {
	/** This class implements the conventianal Discrete element method as 
	//  through interactions between particles. 
	*/
	class pmSpring : public pmLong_range<1,pmSpring> {
		mutable std::vector<pmTensor> force;
	private:
		std::shared_ptr<pmExpression> clone_impl() const override;
	protected:
		void delete_pairs(Func_delete_marker condition, size_t const& level=0) override;
	public:
		pmSpring(std::array<std::shared_ptr<pmExpression>,1> op);
		pmSpring(pmSpring const& other);
		pmSpring(pmSpring&& other);
		pmSpring& operator=(pmSpring const& other);
		pmSpring& operator=(pmSpring&& other);
		virtual ~pmSpring() {}
		std::shared_ptr<pmSpring> clone() const;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		void print() const override;
		virtual void update(size_t const& level=0) override;
		virtual void set_storage_depth(size_t const& d) override;
	};
}

#endif //_SPRING_H_