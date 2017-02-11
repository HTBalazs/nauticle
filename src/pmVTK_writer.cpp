/*
    Copyright 2017 Balazs Toth
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

    For more information please visit: https://bitbucket.org/lempsproject/
*/
    
#include "pmVTK_writer.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Push nodes to polydata object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_nodes_to_polydata() {
	std::shared_ptr<pmWorkspace> workspace = function_space->get_workspace();
	size_t n = workspace->get_number_of_nodes();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetNumberOfPoints(n);
	vtkSmartPointer<vtkCellArray> vertices =vtkSmartPointer<vtkCellArray>::New();
	vertices->SetNumberOfCells(n);
	for(int i=0; i<n; i++) {
		pmTensor position = workspace->get_value("r", i);
		points->SetPoint(i, position(0), position(1), position(2));
		vtkSmartPointer<vtkVertex> vertex = vtkSmartPointer<vtkVertex>::New();
		vertex->GetPointIds()->SetId(0,i);
		vertices->InsertNextCell(vertex);
	}
	polydata->SetPoints(points);
	polydata->SetVerts(vertices);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Push field data to polydata object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_fields_to_polydata() {
	bool scalar_set = false;
	size_t n = function_space->get_workspace()->get_number_of_nodes();
	for(auto const& it:function_space->get_workspace()->get_definitions()) {
		if(it->get_field_size()==n && !it->is_hidden()) {
			vtkSmartPointer<vtkFloatArray> field = vtkSmartPointer<vtkFloatArray>::New();
			field->SetName(it->get_name().c_str());
			if(it->get_type()=="SCALAR") {
				field->SetNumberOfComponents(1);
				field->SetNumberOfTuples(n);
				for(int i=0; i<n; i++) {
					pmTensor t = it->evaluate(i);
					field->SetTuple(i, &t[0]);
				}
				if(!scalar_set) {
					polydata->GetPointData()->SetScalars(field);
					scalar_set = true;
				} else {
					polydata->GetPointData()->AddArray(field);
				}
			}
			bool vector_set = false;
			if(it->get_type()=="VECTOR") {
				field->SetNumberOfComponents(3);
				field->SetNumberOfTuples(n);
				for(int i=0; i<n; i++) {
					pmTensor t = it->evaluate(i);
					float data[3] = {0,0,0};
					for(int j=0; j<t.numel(); j++) {
						data[j] = t[j];
					}
					field->SetTuple(i, data);
				}
				polydata->GetPointData()->AddArray(field);
			}
			bool tensor_set = false;
			if(it->get_type()=="TENSOR") {
				field->SetNumberOfComponents(9);
				field->SetNumberOfTuples(n);
				for(int i=0; i<n; i++) {
					pmTensor t = it->evaluate(i);
					pmTensor data{9,1,0};
					for(int j=0; j<t.get_numcols(); j++) {
						for(int k=0; k<t.get_numrows(); k++) {
							data[k*t.get_numcols()+j] = t(k,j);
						}
					}
					field->SetTuple(i, &data[0]);
				}
				polydata->GetPointData()->AddArray(field);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Push single data to polydata object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_single_to_polydata(vtkSmartPointer<vtkPolyData> polydata, std::string const& name, pmTensor const& field_data) {
	vtkSmartPointer<vtkFloatArray> float_array = vtkSmartPointer<vtkFloatArray>::New();
	float_array->SetNumberOfComponents(field_data.numel());
	float_array->SetName(name.c_str());
	float_array->InsertNextTuple(&field_data[0]);
	polydata->GetFieldData()->AddArray(float_array);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Pushes variables into polydata.
/////////////////////////////////////////////////////////////////////////////////////////
// void pmVTK_writer::push_variables_to_polydata() {
// 	for(auto const& it:function_space->get_workspace()->get<pmVariable>()) {
// 		push_single_to_polydata(polydata, it->get_name(), it->evaluate(0));
// 	}
// }
/////////////////////////////////////////////////////////////////////////////////////////
/// Pushes domain into polydata.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_domain_to_polydata() {
	pmDomain domain = function_space->get_workspace()->get_particle_system().lock()->get_particle_space()->get_domain();
	std::stringstream smin;
	std::stringstream smax;
	std::stringstream scsize;
	std::stringstream sbnd;
	smin << "domain_min:" << domain.get_minimum();
	smax << "domain_max:" << domain.get_maximum();
	scsize << "cell_size:" << domain.get_cell_size();
	sbnd << "boundary:" << domain.get_boundary();
	vtkSmartPointer<vtkStringArray> string_array = vtkSmartPointer<vtkStringArray>::New();
	string_array->SetName("domain");
	string_array->SetNumberOfComponents(1);
	string_array->SetNumberOfValues(4);
	string_array->SetValue(0,smin.str().c_str());
	string_array->SetValue(1,smax.str().c_str());
	string_array->SetValue(2,scsize.str().c_str());
	string_array->SetValue(3,sbnd.str().c_str());
	polydata->GetFieldData()->AddArray(string_array);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Pushes functions into polydata.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_functions_to_polydata() {
	std::vector<std::shared_ptr<pmFunction>> functions = function_space->get_functions();
	vtkSmartPointer<vtkStringArray> string_array = vtkSmartPointer<vtkStringArray>::New();
	string_array->SetNumberOfComponents(1);
	string_array->SetNumberOfTuples(functions.size());
	size_t i=0;
	string_array->SetName("functions");
	for(auto const& it:functions) {
		std::stringstream ss;
		ss << it->get_name() << ":" << it;
		string_array->SetValue(i,ss.str());
		i++;
	}
	polydata->GetFieldData()->AddArray(string_array);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Writes function space into vtk file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::update() {
	if(function_space.use_count()<1) {
		pLogger::warning_msgf("No function space added to VTK writer.\n");
		return;
	}
	push_domain_to_polydata();
	push_variables_to_polydata();
	push_constants_to_polydata();
	push_functions_to_polydata();
	push_nodes_to_polydata();
	push_fields_to_polydata();
	
	// Write vtk file.
	vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
	writer->SetFileName(file_name.c_str());
	writer->SetInputData(polydata);
	switch(mode) {
		case ASCII : writer->SetFileTypeToASCII(); break;
		case BINARY : writer->SetFileTypeToBinary(); break;
	}
	writer->Write();
}

void pmVTK_writer::set_write_mode(write_mode wm) {
	mode = wm;
}

void pmVTK_writer::push_variables_to_polydata() {
	vtkSmartPointer<vtkStringArray> string_array = vtkSmartPointer<vtkStringArray>::New();
	std::vector<std::shared_ptr<pmVariable>> variables = function_space->get_workspace()->get<pmVariable>();
	string_array->SetName("variables");
	string_array->SetNumberOfComponents(1);
	string_array->SetNumberOfValues(variables.size());
	size_t i=0;
	for(auto const& it:variables) {
		std::stringstream ss;
		ss << it << ":" << it->evaluate(0);
		string_array->SetValue(i,ss.str().c_str());
		i++;
	}
	polydata->GetFieldData()->AddArray(string_array);
}

void pmVTK_writer::push_constants_to_polydata() {
	vtkSmartPointer<vtkStringArray> string_array = vtkSmartPointer<vtkStringArray>::New();
	std::vector<std::shared_ptr<pmConstant>> constants = function_space->get_workspace()->get<pmConstant>();
	string_array->SetName("constants");
	string_array->SetNumberOfComponents(1);
	string_array->SetNumberOfValues(constants.size());
	size_t i=0;
	for(auto const& it:constants) {
		std::stringstream ss;
		ss << it << ":" << it->evaluate(0);
		string_array->SetValue(i,ss.str().c_str());
		i++;
	}
	polydata->GetFieldData()->AddArray(string_array);
}