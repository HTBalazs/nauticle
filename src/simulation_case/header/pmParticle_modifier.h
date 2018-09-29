/*
    Copyright 2016-2018 Balazs Toth
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
#include "pmField.h"
#include <memory>
#include <vector>

namespace Nauticle {
	class pmParticle_modifier {
	protected:
		std::shared_ptr<pmWorkspace> workspace;
		std::shared_ptr<pmExpression> condition;
        std::shared_ptr<pmField> radius;
        std::shared_ptr<pmField> mass;
		std::shared_ptr<pmExpression> period;
	protected:
		std::vector<size_t> get_candidates() const;
        virtual std::shared_ptr<pmParticle_modifier> clone_impl() const=0;
	public:
        void set_condition(std::shared_ptr<pmExpression> cdn);
		void set_period(std::shared_ptr<pmExpression> pr);
        void set_radius(std::shared_ptr<pmField> rad);
        void set_mass(std::shared_ptr<pmField> ms);
        std::shared_ptr<pmExpression> const& get_condition() const;
        std::shared_ptr<pmExpression> const& get_period() const;
        std::shared_ptr<pmField> const& get_radius() const;
        std::shared_ptr<pmField> const& get_mass() const;
		virtual void set_workspace(std::shared_ptr<pmWorkspace> ws);
        virtual void update()=0;
        virtual void print() const=0;
        std::shared_ptr<pmParticle_modifier> clone() const;
	};
}

#endif // _PARTICLE_MODIFIER_H_
