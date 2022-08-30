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
    
#ifndef _PM_PARTICLE_EMITTER_H_
#define _PM_PARTICLE_EMITTER_H_

#include "prolog/pLogger.h"
#include "pmParticle_modifier.h"
#include "pmExpression.h"
#include "pmGrid_space.h"
#include <memory>
#include <string>
#include <vector>
#include <utility>

namespace Nauticle {
	/** This class represents the background function, which can be used to 
	//  interpolate data to the particle system.
	*/
	struct pmInitializer {
		std::shared_ptr<pmField> field;
		std::shared_ptr<pmExpression> expression;
	};
	class pmParticle_emitter : public pmParticle_modifier {
		std::shared_ptr<pmGrid_space> grid;
		std::vector<pmInitializer> initializer;
	protected:
		virtual std::shared_ptr<pmParticle_modifier> clone_impl() const override;
	public:
		void print() const override;
		void update(size_t const& num_threads) override;
		std::shared_ptr<pmParticle_emitter> clone() const;
		void add_grid(std::shared_ptr<pmGrid_space> new_grid);
		void add_initializer(pmInitializer const& init);
	};
}

#endif //_PM_PARTICLE_EMITTER_H_