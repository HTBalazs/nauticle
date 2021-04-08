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

#include "pmData_reader.h"
#include <vtkCellArray.h>
#include <vtkDelaunay3D.h>
#include <vtkPoints.h>
#include <vtkTriangle.h>
#include <vtkPointData.h>
#include <vtkLine.h>
#include <vtkImageData.h>
#include <vtkProbeFilter.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkMath.h>
#include <vtkCellLocator.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkSmartPointer.h>
#include <vtkSimplePointsReader.h>
#include <vtkPolyData.h>

using namespace Nauticle;
using namespace ProLog;

std::shared_ptr<pmData_reader> pmData_reader::clone_impl() const {
	return std::make_shared<pmData_reader>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the file name of the input file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmData_reader::set_file_name(std::string const& fn) {
	file_name = fn;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads the input file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmData_reader::read_file(size_t const& dims) {
	// Read the file
	vtkSmartPointer<vtkSimplePointsReader> reader = vtkSmartPointer<vtkSimplePointsReader>::New();
	reader->SetFileName(file_name.c_str());
	reader->Update();
	vtkSmartPointer<vtkPolyData> poly_data = vtkSmartPointer<vtkPolyData>::New();
	poly_data = reader->GetOutput();
	size_t data_length = poly_data->GetNumberOfPoints();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points = poly_data->GetPoints();
	data.resize(data_length);
	for(int i=0; i<data_length; i++) {
		double* p = points->GetPoint(i);
		pmTensor tensor{(int)dims,1,0.0};
		for(int j=0; j<dims; j++) {
			tensor[j] = p[j];
		}
		data[i] = tensor;
	}
}

std::vector<pmTensor> pmData_reader::get_data() const {
	return data;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the current object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmData_reader> pmData_reader::clone() const {
    return clone_impl();
}

