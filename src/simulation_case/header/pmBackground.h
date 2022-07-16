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
    
#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include <iostream>
#include <string>
#include <memory>
#include "prolog/pLogger.h"
#include "pmParticle_system.h"
#include "pmField.h"
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

namespace Nauticle {
	/** This class represents the background function, which can be used to 
	//  interpolate data to the particle system.
	*/
	class pmBackground {
		std::string file_name;
		std::shared_ptr<pmField> field;
		std::shared_ptr<pmParticle_system> psys;
		vtkSmartPointer<vtkUnstructuredGrid> unstructured_grid;
		std::shared_ptr<pmExpression> condition;
		std::shared_ptr<pmExpression> particle_condition;
		bool import = true;
	public:
		pmBackground() {}
		pmBackground(pmBackground const& other);
		pmBackground(pmBackground&& other);
		~pmBackground() {}
		pmBackground& operator=(pmBackground const& rhs);
		pmBackground& operator=(pmBackground&& rhs);
		void print() const;
		void set_file_name(std::string const& fn);
		void read_file();
		void interpolate();
		void set_particle_system(std::shared_ptr<pmParticle_system> ps);
		void set_field(std::shared_ptr<pmField> fld);
		void set_condition(std::shared_ptr<pmExpression> cond);
		void set_particle_condition(std::shared_ptr<pmExpression> cond);
		void update();
		std::shared_ptr<pmBackground> clone() const;
	};
}

#endif //_BACKGROUND_H_