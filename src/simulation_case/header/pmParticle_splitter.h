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

#ifndef _PARTICLE_SPLITTER_H_
#define _PARTICLE_SPLITTER_H_

#include "pmWorkspace.h"
#include "pmParticle_modifier.h"
#include <memory>
#include <vector>

namespace Nauticle {
	class pmParticle_splitter : public pmParticle_modifier {
		double epsilon = 0.4;
		double alpha = 0.9;
		size_t num_new = 6;
	private:
    public:
		void update();
	};
}

#endif // _PARTICLE_SPLITTER_H_