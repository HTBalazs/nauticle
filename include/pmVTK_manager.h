/////////////////////////////////////////////////////////////////////////////
// Name:        pmVTK_manager.h
// Purpose:     pmVTK_manager class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _VTK_PARSER_H_
#define _VTK_PARSER_H_

#include <memory>
#include <string>
#include <vtkGenericDataObjectReader.h>
#include <vtkStructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkFieldData.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkStringArray.h>
#include <vtkDoubleArray.h>
#include <vtkPolyDataReader.h>
#include <vtkVertex.h>
#include <vtkPolyDataWriter.h>
#include "prolog/pLogger.h"
#include "pmFunction_parser.h"
#include "pmFile_parser.h"
#include "pmFunction_space.h"
#include "pmNoncopyable.h"


class pmVTK_manager : public pmNoncopyable {
protected:
	std::string file_name;
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	std::shared_ptr<pmFunction_space> function_space;
public:
	pmVTK_manager() {}
	virtual ~pmVTK_manager() {}
	std::shared_ptr<pmFunction_space> get_function_space() const;
	void set_function_space(std::shared_ptr<pmFunction_space> fs);
	virtual void update()=0;
	void set_file_name(std::string const& file_name);
};

#endif //_VTK_PARSER_H_