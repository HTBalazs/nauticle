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

#include "pmTime_series.h"
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


/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTime_series::pmTime_series() {
	this->name = "TS_";
    char ch[5];
    snprintf(ch, 5, "%04i", counter);
    this->name += ch;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the file name of the input file.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmData_reader> pmTime_series::clone_impl() const {
	return std::make_shared<pmTime_series>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the content of the pmTime_series object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmTime_series::print() const {
	pLogger::headerf<LBL>("Time series");
	pLogger::titlef<LMA>("Data");
	pLogger::logf<YEL>("        file: ");
	pLogger::logf<NRM>("%s\n", file_name.c_str());
	pLogger::logf<YEL>("        interpolate_to: ");
	pLogger::logf<NRM>("%s\n", variable->get_name().c_str());
	pLogger::logf<YEL>("        condition: "); condition->print(); pLogger::line_feed(1);
	pLogger::footerf<LBL>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads the input file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmTime_series::read_file(size_t const& dims) {
	// Read the file
	vtkSmartPointer<vtkSimplePointsReader> reader = vtkSmartPointer<vtkSimplePointsReader>::New();
	reader->SetFileName(file_name.c_str());
	reader->Update();
	vtkSmartPointer<vtkPolyData> poly_data = vtkSmartPointer<vtkPolyData>::New();
	poly_data = reader->GetOutput();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points = poly_data->GetPoints();
	int num_points = poly_data->GetNumberOfPoints();
	data.resize(num_points);
	time.resize(num_points);
	for(int i=0; i<num_points; i++) {
		double* p = points->GetPoint(i);
		pmTensor tensor{(int)dims,1,0.0};
		time[i] = p[0];
		for(int j=1; j<dims+1; j++) {
			tensor[j-1] = p[j];
		}
		data[i] = tensor;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Performs interpolation using the given field and particle system.
/////////////////////////////////////////////////////////////////////////////////////////
void pmTime_series::interpolate(double const& t) {
	if(variable.use_count()==0 || condition->evaluate(0)[0]==0 || data.empty()) {
		return;
	}
	pmTensor result = data[0];
	for(int i=0; i<data.size()-1; i++) {
		if(time[i]<=t && time[i+1]>t) {
			result = (data[i+1]-data[i])/(time[i+1]-time[i])*(t-time[i])+data[i];
			variable->set_value(result);
			return;
		}
	}
	result = data.back();
	variable->set_value(result);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets variable for the interpolation results to be stored in.
/////////////////////////////////////////////////////////////////////////////////////////
void pmTime_series::set_particle_system(std::shared_ptr<pmParticle_system> ps) {
	psys = ps;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets variable for the interpolation results to be stored in.
/////////////////////////////////////////////////////////////////////////////////////////
void pmTime_series::set_variable(std::shared_ptr<pmVariable> var) {
	variable = var;
}

void pmTime_series::set_condition(std::shared_ptr<pmExpression> cond) {
	condition = cond;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads input if not read yet, and performs interpolation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmTime_series::update(double const& t) {
	if(import) {
		this->read_file(variable->evaluate(0).numel());
		import = false;
	}
	interpolate(t);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the current object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmTime_series> pmTime_series::clone() const {
    return std::static_pointer_cast<pmTime_series, pmData_reader>(clone_impl());
}

