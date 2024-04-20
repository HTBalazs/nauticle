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
    
#ifndef _PM_BACKGROUND_H_
#define _PM_BACKGROUND_H_

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
	private:
		std::shared_ptr<pmField> field;
	protected:
		std::string file_name;
		std::shared_ptr<pmExpression> position_field;
		vtkSmartPointer<vtkUnstructuredGrid> unstructured_grid;
		std::shared_ptr<pmExpression> condition;
		std::shared_ptr<pmExpression> particle_condition;
	protected:
		virtual void read_file();
	public:
		virtual void initialize();
		virtual void print() const;
		void set_file_name(std::string const& fn);
		virtual void interpolate();
		void set_position_field(std::shared_ptr<pmExpression> ps);
		virtual void set_field(std::shared_ptr<pmField> fld);
		void set_condition(std::shared_ptr<pmExpression> cond);
		void set_particle_condition(std::shared_ptr<pmExpression> cond);
		virtual void update(double const& dt);
		std::shared_ptr<pmBackground> clone() const;
		virtual void write_geometry(std::string const& fn) const {}
	};
}

#endif //_PM_BACKGROUND_H_