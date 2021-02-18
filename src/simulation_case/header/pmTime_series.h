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
    
#ifndef _TIME_SERIES_H_
#define _TIME_SERIES_H_

#include <iostream>
#include <string>
#include <memory>
#include "prolog/pLogger.h"
#include "pmVariable.h"
#include "pmParticle_system.h"
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

namespace Nauticle {
	/** This class represents the background function, which can be used to 
	//  interpolate data to the particle system.
	*/
	class pmTime_series {
		std::string file_name;
		std::shared_ptr<pmVariable> variable;
		std::shared_ptr<pmParticle_system> psys;
		std::shared_ptr<pmExpression> condition;
		std::vector<double> time;
		std::vector<pmTensor> data;
		bool import = true;
	public:
		pmTime_series() {}
		pmTime_series(pmTime_series const& other);
		pmTime_series(pmTime_series&& other);
		~pmTime_series() {}
		pmTime_series& operator=(pmTime_series const& rhs);
		pmTime_series& operator=(pmTime_series&& rhs);
		void print() const;
		void set_file_name(std::string const& fn);
		void read_file();
		void interpolate(double const& t);
		void set_particle_system(std::shared_ptr<pmParticle_system> ps);
		void set_variable(std::shared_ptr<pmVariable> var);
		void set_condition(std::shared_ptr<pmExpression> cond);
		void update(double const& t);
		std::shared_ptr<pmTime_series> clone() const;
	};
}

#endif //_TIME_SERIES_H_