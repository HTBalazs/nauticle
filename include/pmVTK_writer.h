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
    
#ifndef _VTK_WRITER_H_
#define _VTK_WRITER_H_

#include <string>
#include "pmVTK_manager.h"
#include "pmTensor.h"
#include "prolog/pLogger.h"

enum write_mode { ASCII, BINARY };

class pmVTK_writer : public pmVTK_manager {
	write_mode mode=ASCII;
private:
	void push_nodes_to_polydata();
	void push_fields_to_polydata();
	void push_single_to_polydata(vtkSmartPointer<vtkPolyData> polydata, std::string const& name, pmTensor const& field_data);
	void push_variables_to_polydata();
	void push_constants_to_polydata();
	void push_domain_to_polydata();
	void push_functions_to_polydata();
public:	
	virtual ~pmVTK_writer() {}
	void set_write_mode(write_mode mode);
	void update() override;
};

#endif //_VTK_WRITER_H_