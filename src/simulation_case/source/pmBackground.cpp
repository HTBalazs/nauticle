/*
    Copyright 2016-2019 Balazs Toth
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
#include <vtkPoints.h>
#include <vtkTriangle.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkLine.h>
#include <vtkImageData.h>
#include <vtkProbeFilter.h>
#include <vtkDelaunay2D.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkDoubleArray.h>
#include <vtkMath.h>
#include <vtkCellLocator.h>

using namespace Nauticle;
using namespace ProLog;

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmBackground::pmBackground(pmBackground const& other) {
	this->field = other.field;
	this->psys = other.psys;
	this->file_name = other.file_name;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmBackground& pmBackground::operator=(pmBackground const& rhs) {
	if(this!=&rhs) {
		this->field = rhs.field;
		this->psys = rhs.psys;
		this->file_name = rhs.file_name;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmBackground::pmBackground(pmBackground&& other) {
	this->field = std::move(other.field);
	this->psys = std::move(other.psys);
	this->file_name = std::move(other.file_name);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmBackground& pmBackground::operator=(pmBackground&& rhs) {
	if(this!=&rhs) {
		this->field = std::move(rhs.field);
		this->psys = std::move(rhs.psys);
		this->file_name = std::move(rhs.file_name);
	}
	return *this;
}

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
	vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
	reader->ReadAllScalarsOn();
	reader->SetFileName(file_name.c_str());
	reader->Update();
	polydata = reader->GetOutput();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Performs interpolation using the given field and particle system.
/////////////////////////////////////////////////////////////////////////////////////////
void pmBackground::interpolate() {
	if(psys.use_count()==0 || field.use_count()==0 || polydata==NULL) {
		return;
	}
	auto points = psys->generate_vtkpoints();

  	auto probePolyData = vtkSmartPointer<vtkPolyData>::New();
  	probePolyData->SetPoints(points);
 
	auto probe = vtkSmartPointer<vtkProbeFilter>::New();
	probe->SetSourceData(polydata);
	probe->SetInputData(probePolyData);
	probe->Update();
 
	vtkDataArray* data = probe->GetOutput()->GetPointData()->GetScalars();
	vtkDoubleArray* doubleData = vtkDoubleArray::SafeDownCast(data);
	for(int i=0; i<doubleData->GetNumberOfTuples(); i++) {
		pmTensor tensor{1,1,doubleData->GetValue(i)};
		field->set_value(tensor,i);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets particle system for interpolation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmBackground::set_particle_system(std::shared_ptr<pmParticle_system> ps) {
	psys = ps;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets field for the interpolation results to be stored in.
/////////////////////////////////////////////////////////////////////////////////////////
void pmBackground::set_field(std::shared_ptr<pmField> fld) {
	field = fld;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads input if not read yet, and performs interpolation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmBackground::update() {
	static bool import = true;
	if(import) {
		this->read_file();
		import = false;
	}
	interpolate();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the current object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmBackground> pmBackground::clone() const {
    return std::make_shared<pmBackground>(*this);
}

