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

#include "pmVTK_reader.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the vector of tensors for the given array name.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<pmTensor> pmVTK_reader::pop_array_from_polydata(int const& i, size_t const& domain_dim) const {
	size_t size = polydata->GetNumberOfPoints();
	std::vector<pmTensor> field;
	field.reserve(size);
	vtkSmartPointer<vtkDoubleArray> array = vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetArray(i));
	int array_size = array->GetSize();
	int dim = array_size/size;
	array_size = dim*size;
	array->SetNumberOfComponents(dim);
	int num_tuples = array->GetNumberOfTuples();
	size_t real_dim;
	switch(dim) {
		default:
		case 1 : real_dim = 1; break;
		case 3 : real_dim = domain_dim; break;
		case 9 : real_dim = domain_dim*domain_dim; break;
	}
	for(int a=0; a<array_size; a+=dim) {
		pmTensor tensor = pmTensor::Tensor(real_dim);
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
	vtkSmartPointer<vtkDoubleArray> array = vtkDoubleArray::SafeDownCast(polydata->GetFieldData()->GetArray(i));
	int dim = array->GetNumberOfComponents();
	pmTensor field_data = pmTensor::Tensor(dim);
	for(int a=0; a<dim; a++) {
		field_data[a] = array->GetValue(a);
	}
	return field_data;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the equations stored in the polydata.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmEquation>> pmVTK_reader::pop_equations_from_polydata(std::shared_ptr<pmWorkspace> workspace) const {
	std::vector<std::shared_ptr<pmEquation>> equations;
	vtkSmartPointer<vtkFieldData> equation_data = polydata->GetFieldData();
	int i=0;
	vtkSmartPointer<vtkStringArray> array = vtkStringArray::SafeDownCast(equation_data->GetAbstractArray("equations",i));
	if(!array) { return equations; }
	int num_values = array->GetNumberOfValues();
	for(int j=0; j<num_values; j++) {
		std::string vtkequation = array->GetValue(j).c_str();
		// set separators for equation and condition
		size_t sp_f = vtkequation.find(":");
		size_t sp_c = vtkequation.find("#");
		// pull name, equation and condition
		std::string equation_name = vtkequation.substr(0, sp_f);
		std::string equation_value = vtkequation.substr(sp_f+1, sp_c-1-sp_f);
		std::string equation_condition = vtkequation.substr(sp_c+1);

		std::unique_ptr<pmEquation_parser> fp{new pmEquation_parser};
		equations.push_back(fp->analyse_equation(equation_name, equation_value, equation_condition, workspace));
	}
	return equations;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the domain stored in the polydata.
/////////////////////////////////////////////////////////////////////////////////////////
pmDomain pmVTK_reader::pop_domain_from_polydata(std::shared_ptr<pmWorkspace> workspace) const {
	pmDomain domain;
	vtkSmartPointer<vtkFieldData> field_data = polydata->GetFieldData();
	for(int i=0; i<field_data->GetNumberOfArrays() ; i++) {
		std::string name = field_data->GetArrayName(i);
		if(name=="domain") {
			vtkSmartPointer<vtkStringArray> array = vtkStringArray::SafeDownCast(field_data->GetAbstractArray(i));
			if(!array) { ProLog::pLogger::error_msgf("Domain not defined!\n"); }
			for(int j=0; j<array->GetSize(); j++) {
				std::string vtk_inst = array->GetValue(j).c_str();
				std::string inst_name = vtk_inst.substr(0, vtk_inst.find(":"));
				std::string inst_value = vtk_inst.substr(vtk_inst.find(":")+1);
				std::unique_ptr<pmTensor_parser> tp{new pmTensor_parser};
				if(inst_name=="domain_min") { domain.set_minimum(tp->string_to_tensor(inst_value,workspace)); }
				if(inst_name=="domain_max") { domain.set_maximum(tp->string_to_tensor(inst_value,workspace)); }
				if(inst_name=="cell_size") { domain.set_cell_size(tp->string_to_tensor(inst_value,workspace)); }
				if(inst_name=="boundary") { domain.set_boundary(tp->string_to_tensor(inst_value,workspace)); }
			}
		}
	}
	return domain;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds the singles (pmVariable and pmConstant) stored in the polydata to the given workspace.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_reader::pop_singles_from_polydata(std::string const& type, std::shared_ptr<pmWorkspace> workspace) const {
	vtkSmartPointer<vtkFieldData> field_data = polydata->GetFieldData();
	int i=0;
	vtkSmartPointer<vtkStringArray> array = vtkStringArray::SafeDownCast(field_data->GetAbstractArray(type.c_str(),i));
	if(!array) { return; }
	int num_values = array->GetNumberOfValues();
	for(int j=0; j<num_values; j++) {
		std::string vtksingle = array->GetValue(j).c_str();
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
/// Returns the asymmetric field names stored in the polydata.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> pmVTK_reader::pop_asymmetric_field_names_from_polydata() const {
	std::vector<std::string> asymmetric_fields;
	vtkSmartPointer<vtkFieldData> field_data = polydata->GetFieldData();
	int i=0;
	vtkSmartPointer<vtkStringArray> array = vtkStringArray::SafeDownCast(field_data->GetAbstractArray("asymmetric",i));
	if(!array) { return asymmetric_fields; }
	int num_values = array->GetNumberOfValues();
	for(int j=0; j<num_values; j++) {
		std::string vtksingle = array->GetValue(j).c_str();
		asymmetric_fields.push_back(vtksingle);
	}
	return asymmetric_fields;
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
	workspace->delete_instance("id");
	workspace->set_number_of_nodes(polydata->GetNumberOfPoints());
	// Read constants and variables
	pop_singles_from_polydata("constants", workspace);
	pop_singles_from_polydata("variables", workspace);
	std::vector<std::string> asymmetric_fields = pop_asymmetric_field_names_from_polydata();
	// Read domain data
	pmDomain domain = pop_domain_from_polydata(workspace);

	// Read array data
	std::vector<pmTensor> psys_data;
	for(int i=0; i<polydata->GetPointData()->GetNumberOfArrays(); i++) {
		std::string name = polydata->GetPointData()->GetArrayName(i);
		if(name=="r") {
	 	 	psys_data = pop_array_from_polydata(i, domain.get_dimensions());
		} else {
			bool sym = true;
			for(auto const& it:asymmetric_fields) {
				if(name==it) {
					sym = false;
					break;
				}
			}
			workspace->add_field(name.c_str(), pop_array_from_polydata(i, domain.get_dimensions()), sym);
		}
	}

	workspace->add_particle_system(psys_data, domain);
	// Read equation data
	std::vector<std::shared_ptr<pmEquation>> equations = pop_equations_from_polydata(workspace);
	cas = std::make_shared<pmCase>();
	cas->add_workspace(workspace);
	cas->add_equation(equations);
	cas->assign_particle_system_to_equations();
}