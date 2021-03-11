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
    
#ifndef _PARTICLE_SINK_H_
#define _PARTICLE_SINK_H_

#include <iostream>
#include <string>
#include <memory>
#include "prolog/pLogger.h"
#include "pmWorkspace.h"
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

namespace Nauticle {
	/** This class represents the background function, which can be used to 
	//  interpolate data to the particle system.
	*/
	class pmParticle_sink {
		std::shared_ptr<pmWorkspace> workspace;
		std::shared_ptr<pmExpression> condition;
	public:
		pmParticle_sink() {}
		pmParticle_sink(pmParticle_sink const& other);
		pmParticle_sink(pmParticle_sink&& other);
		~pmParticle_sink() {}
		pmParticle_sink& operator=(pmParticle_sink const& rhs);
		pmParticle_sink& operator=(pmParticle_sink&& rhs);
		void print() const;
		void set_workspace(std::shared_ptr<pmWorkspace> ws);
		void set_condition(std::shared_ptr<pmExpression> cond);
		void update(size_t const& num_threads);
		std::shared_ptr<pmParticle_sink> clone() const;
	};
}

#endif //_PARTICLE_SINK_H_