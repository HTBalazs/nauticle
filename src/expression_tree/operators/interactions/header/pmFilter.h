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
    
#ifndef _FILTER_H_
#define _FILTER_H_

#include <memory>
#include "pmInteraction.h"
#include "prolog/pLogger.h"
#include "pmKernel.h"

namespace Nauticle {
    /** This abstract class implements the conventianal Smoothed Particle Hydrodynamics
    //  through interactions between particles.
    */
    template <size_t S>
    class pmFilter : public pmInteraction<S> {
    protected:
    	std::shared_ptr<pmKernel> kernel;
    public:
    	pmFilter() {}
    	virtual ~pmFilter() {}
    };
}

#endif //_FILTER_H_
