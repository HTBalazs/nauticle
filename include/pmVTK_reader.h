/////////////////////////////////////////////////////////////////////////////
// Name:        pmVTK_manager.h
// Purpose:     pmVTK_manager class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

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