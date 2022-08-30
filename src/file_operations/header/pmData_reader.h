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
    
#ifndef _PM_DATA_READER_H_
#define _PM_DATA_READER_H_

#include "prolog/pLogger.h"
#include "pmTensor.h"
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <iostream>
#include <string>
#include <memory>

namespace Nauticle {
	/** This class represents the background function, which can be used to 
	//  interpolate data to the particle system.
	*/
	class pmData_reader {
	protected:
		std::string file_name;
		std::vector<pmTensor> data;
		virtual std::shared_ptr<pmData_reader> clone_impl() const;
	public:
		void set_file_name(std::string const& fn);
		virtual void read_file(size_t const& dims);
		std::vector<pmTensor> get_data() const;
		std::shared_ptr<pmData_reader> clone() const;
	};
}

#endif //_PM_DATA_READER_H_