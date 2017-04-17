/*
    Copyright 2016-2017 Balazs Toth
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

    For more information please visit: https://bitbucket.org/lempsproject/
*/
    
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
#include <vtkDoubleArray.h>
#include <vtkStringArray.h>
#include <vtkDoubleArray.h>
#include <vtkPolyDataReader.h>
#include <vtkVertex.h>
#include <vtkPolyDataWriter.h>
#include "prolog/pLogger.h"
#include "pmEquation_parser.h"
#include "pmSolver.h"
#include "pmNoncopyable.h"

/** This abstract class stores vtk-formatted data of pmSolver.
*/
class pmVTK_manager : public pmNoncopyable {
protected:
	std::string file_name;
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	std::shared_ptr<pmSolver> solver;
public:
	pmVTK_manager() {}
	virtual ~pmVTK_manager() {}
	std::shared_ptr<pmSolver> get_solver() const;
	void set_solver(std::shared_ptr<pmSolver> sol);
	virtual void update()=0;
	void set_file_name(std::string const& file_name);
};

#endif //_VTK_PARSER_H_