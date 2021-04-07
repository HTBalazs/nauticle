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

#ifndef _PARTICLE_MODIFIER_H_
#define _PARTICLE_MODIFIER_H_

#include "pmWorkspace.h"
#include "pmExpression.h"
#include <memory>
#include <vector>

namespace Nauticle {
    /** This class implements the base of the particle modifier classes for condition 
    // driven particle generation and deletion.
    */
	class pmParticle_modifier {
	protected:
		std::shared_ptr<pmWorkspace> workspace;
		std::shared_ptr<pmExpression> condition;
	protected:
        virtual std::shared_ptr<pmParticle_modifier> clone_impl() const=0;
	public:
        void set_condition(std::shared_ptr<pmExpression> cdn);
        std::shared_ptr<pmExpression> const& get_condition() const;
		virtual void set_workspace(std::shared_ptr<pmWorkspace> ws);
        virtual void update(size_t const& num_threads)=0;
        virtual void print() const;
        std::shared_ptr<pmParticle_modifier> clone() const;
	};
}

#endif // _PARTICLE_MODIFIER_H_
