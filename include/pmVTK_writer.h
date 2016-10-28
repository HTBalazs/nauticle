/////////////////////////////////////////////////////////////////////////////
// Name:        pmVTK_manager.h
// Purpose:     pmVTK_manager class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

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