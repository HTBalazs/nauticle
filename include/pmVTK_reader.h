/*
    Copyright 2016 Balazs Toth
    This file is part of LEMPS.

    LEMPS is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LEMPS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LEMPS.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://BalazsToth@bitbucket.org/BalazsToth/lemps
*/
    
#ifndef _VTK_READER_H_
#define _VTK_READER_H_

#include "pmVTK_manager.h"
#include <string>
#include "prolog/pLogger.h"
#include "pmTensor_parser.h"


class pmVTK_reader : public pmVTK_manager {
private:
	std::vector<pmTensor> pop_array_from_polydata(int const& i, size_t const& domain_dim) const;
	pmTensor pop_single_from_polydata(vtkSmartPointer<vtkPolyData> poly_data, std::string name, int const& i) const;
	std::vector<std::shared_ptr<pmFunction>> pop_functions_from_polydata(std::shared_ptr<pmWorkspace> workspace) const;
	
	pmDomain pop_domain_from_polydata(std::shared_ptr<pmWorkspace> workspace) const;
	void pop_singles_from_polydata(std::string const& TYPE, std::shared_ptr<pmWorkspace> workspace) const;
public:
	virtual ~pmVTK_reader() {}
	void update() override;
};

#endif //_VTK_READER_H_