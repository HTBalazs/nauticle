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

#include "pmVTK_reader.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the vector of tensors for the given array name.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<pmTensor> pmVTK_reader::pop_array_from_polydata(int const& i, size_t const& domain_dim) const {
	size_t size = polydata->GetNumberOfPoints();
	std::vector<pmTensor> field;
	field.reserve(size);
	vtkSmartPointer<vtkFloatArray> array = vtkFloatArray::SafeDownCast(polydata->GetPointData()->GetArray(i));
	int array_size = array->GetSize();
	int dim = array_size/size;
	array->SetNumberOfComponents(dim);
	int num_tuples = array->GetNumberOfTuples();
	size_t real_dim;
	switch(dim) {
		default:
		case 1 : real_dim = 1; break;
		case 3 : real_dim = domain_dim; break;
		case 9 : real_dim = domain_dim*domain_dim; break;
	}
	bool scalar = dim==1 ? true : false;
	for(int a=0; a<array_size; a+=dim) {
		pmTensor tensor = pmTensor::Tensor(real_dim);
		tensor.set_scalar(scalar);
		for(int t=0; t<real_dim; t++) {
			tensor[t] = array->GetValue(a+t);
		}
		field.push_back(tensor);
	}
	return field;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a tensor for the given field name.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmVTK_reader::pop_single_from_polydata(vtkSmartPointer<vtkPolyData> polydata, std::string name, int const& i) const {
	vtkSmartPointer<vtkFloatArray> array = vtkFloatArray::SafeDownCast(polydata->GetFieldData()->GetArray(i));
	int dim = array->GetNumberOfComponents();
	pmTensor field_data = pmTensor::Tensor(dim);
	for(int a=0; a<dim; a++) {
		field_data[a] = array->GetValue(a);
	}
	return field_data;
}
std::vector<std::shared_ptr<pmFunction>> pmVTK_reader::pop_functions_from_polydata(std::shared_ptr<pmWorkspace> workspace) const {
	std::vector<std::shared_ptr<pmFunction>> functions;
	vtkSmartPointer<vtkFieldData> function_data = polydata->GetFieldData();
	int i=0;
	vtkSmartPointer<vtkStringArray> array = vtkStringArray::SafeDownCast(function_data->GetAbstractArray("functions",i));
	if(!array) { return functions; }
	int num_values = array->GetNumberOfValues();
	for(int j=0; j<num_values; j++) {
		std::string vtkfunction = array->GetValue(j);
		std::string function_name = vtkfunction.substr(0, vtkfunction.find(":"));
		std::string function_value = vtkfunction.substr(vtkfunction.find(":")+1);
		std::unique_ptr<pmFunction_parser> fp{new pmFunction_parser};
		functions.push_back(fp->analyse_function(function_name, function_value, workspace));
	}
	return functions;
}

pmDomain pmVTK_reader::pop_domain_from_polydata(std::shared_ptr<pmWorkspace> workspace) const {
	pmDomain domain;
	vtkSmartPointer<vtkFieldData> field_data = polydata->GetFieldData();
	for(int i=0; i<field_data->GetNumberOfArrays() ; i++) {
		std::string name = field_data->GetArrayName(i);
		if(name=="domain") {
			vtkSmartPointer<vtkStringArray> array = vtkStringArray::SafeDownCast(field_data->GetAbstractArray(i));
			if(!array) { pLogger::error_msgf("Domain not defined!\n"); }
			for(int j=0; j<array->GetSize(); j++) {
				std::string vtk_inst = array->GetValue(j);
				std::string inst_name = vtk_inst.substr(0, vtk_inst.find(":"));
				std::string inst_value = vtk_inst.substr(vtk_inst.find(":")+1);
				std::unique_ptr<pmTensor_parser> tp{new pmTensor_parser};
				if(inst_name=="domain_min") { domain.set_minimum(tp->string_to_tensor(inst_value,workspace)); }
				if(inst_name=="domain_max") { domain.set_maximum(tp->string_to_tensor(inst_value,workspace)); }
				if(inst_name=="cell_size") { domain.set_cell_size(tp->string_to_tensor(inst_value,workspace)[0]); }
				if(inst_name=="boundary") { domain.set_boundary(tp->string_to_tensor(inst_value,workspace)); }
			}
		}
	}
	return domain;
}

void pmVTK_reader::pop_singles_from_polydata(std::string const& type, std::shared_ptr<pmWorkspace> workspace) const {
	vtkSmartPointer<vtkFieldData> field_data = polydata->GetFieldData();
	int i=0;
	vtkSmartPointer<vtkStringArray> array = vtkStringArray::SafeDownCast(field_data->GetAbstractArray(type.c_str(),i));
	if(!array) { return; }
	int num_values = array->GetNumberOfValues();
	for(int j=0; j<num_values; j++) {
		std::string vtksingle = array->GetValue(j);
		std::string single_name = vtksingle.substr(0, vtksingle.find(":"));
		std::string single_value = vtksingle.substr(vtksingle.find(":")+1);
		std::unique_ptr<pmTensor_parser> tp{new pmTensor_parser};
		if(type=="constants") {
			workspace->add_constant(single_name, tp->string_to_tensor(single_value, workspace),false);
		} else if(type=="variables") {
			workspace->add_variable(single_name, tp->string_to_tensor(single_value, workspace));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Reads the file with the given name. All previously stored data are destroyed.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_reader::update() {
	vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
	reader->ReadAllScalarsOn();
	reader->SetFileName(file_name.c_str());
	reader->Update();
	polydata = reader->GetOutput(); // read everything to polydata
	std::shared_ptr<pmWorkspace> workspace = std::make_shared<pmWorkspace>();
	workspace->set_number_of_nodes(polydata->GetNumberOfPoints());

	// Read constants and variables
	pop_singles_from_polydata("constants", workspace);
	pop_singles_from_polydata("variables", workspace);
	// Read domain data
	pmDomain domain = pop_domain_from_polydata(workspace);

	// Read array data
	std::vector<pmTensor> psys_data;
	for(int i=0; i<polydata->GetPointData()->GetNumberOfArrays(); i++) {
		std::string name = polydata->GetPointData()->GetArrayName(i);
		if(name=="id") { continue; }
		if(name=="r") {
	 	 	psys_data = pop_array_from_polydata(i, domain.get_dimensions());
		} else {
			workspace->add_field(name.c_str(), pop_array_from_polydata(i, domain.get_dimensions()));
		}
	}

	workspace->add_particle_system(psys_data, domain);
	// Read function data
	std::vector<std::shared_ptr<pmFunction>> functions = pop_functions_from_polydata(workspace);
	function_space = std::make_shared<pmFunction_space>();
	function_space->add_workspace(workspace);
	function_space->add_function(functions);
	function_space->assign_particle_system_to_functions();
}