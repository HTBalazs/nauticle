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

#include "pmBackground.h"
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

using namespace Nauticle;
using namespace ProLog;

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the content of the pmBackground object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmBackground::print() const {
	pLogger::headerf<LBL>("Background functions");
	pLogger::titlef<LMA>("Background");
	pLogger::logf<YEL>("        file: ");
	pLogger::logf<NRM>("%s\n", file_name.c_str());
	pLogger::logf<YEL>("        interpolate_to: ");
	pLogger::logf<NRM>("%s\n", field->get_name().c_str());
	pLogger::logf<YEL>("        condition: "); condition->print(); pLogger::line_feed(1);
	pLogger::logf<YEL>("        particle condition: "); particle_condition->print(); pLogger::line_feed(1);
	pLogger::footerf<LBL>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the file name of the input file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmBackground::set_file_name(std::string const& fn) {
	file_name = fn;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads the input file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmBackground::read_file() {
	auto reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();
	reader->ReadAllScalarsOn();
	reader->SetFileName(file_name.c_str());
	reader->Update();
	unstructured_grid = reader->GetOutput();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Performs interpolation using the given field and particle system.
/////////////////////////////////////////////////////////////////////////////////////////
void pmBackground::interpolate() {
	if(position_field.use_count()==0 || field.use_count()==0 || unstructured_grid==NULL || condition->evaluate(0)[0]==0) {
		return;
	}

	auto points = vtkSmartPointer<vtkPoints>::New();
	for(int i=0; i<position_field->get_field_size(); i++) {
		if(particle_condition->evaluate(i)[0]==0) {
			continue;
		}
		pmTensor tensor = position_field->evaluate(i);
		size_t n = tensor.numel();
		points->InsertNextPoint(tensor[0], n>1?tensor[1]:0.0, n>2?tensor[2]:0.0);
	}

  	auto probe_unstructured_grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
  	probe_unstructured_grid->SetPoints(points);
	auto probe = vtkSmartPointer<vtkProbeFilter>::New();
	probe->SetSourceData(unstructured_grid);
	probe->SetInputData(probe_unstructured_grid);
	probe->Update();
 
 	{
		vtkDataArray* data = probe->GetOutput()->GetPointData()->GetScalars();
		if(data!=NULL) {
			vtkDoubleArray* doubleData = vtkDoubleArray::SafeDownCast(data);
			for(int i=0; i<doubleData->GetNumberOfTuples(); i++) {
				pmTensor tensor{1,1,doubleData->GetValue(i)};
				field->set_value(tensor,i);
		    }
			return;
		}
	}
	{
		vtkDataArray* data = probe->GetOutput()->GetPointData()->GetVectors();
		if(data!=NULL) {
			vtkDoubleArray* doubleData = vtkDoubleArray::SafeDownCast(data);
			for(int i=0; i<doubleData->GetNumberOfTuples(); i++) {
				pmTensor tensor{field->evaluate(0).numel(),1,0.0};
				doubleData->GetTuple(i,*tensor);
				field->set_value(tensor,i);
		    }
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets particle system for interpolation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmBackground::set_position_field(std::shared_ptr<pmExpression> ps) {
	position_field = ps;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets field for the interpolation results to be stored in.
/////////////////////////////////////////////////////////////////////////////////////////
void pmBackground::set_field(std::shared_ptr<pmField> fld) {
	field = fld;
}

void pmBackground::set_condition(std::shared_ptr<pmExpression> cond) {
	condition = cond;
}

void pmBackground::set_particle_condition(std::shared_ptr<pmExpression> pcond) {
	particle_condition = pcond;
}

void pmBackground::initialize() {
	this->read_file();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads input if not read yet, and performs interpolation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmBackground::update(double const& dt) {
	interpolate();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the current object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmBackground> pmBackground::clone() const {
    return std::make_shared<pmBackground>(*this);
}

