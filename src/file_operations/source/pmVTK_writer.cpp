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
    
#include "pmVTK_writer.h"
#include "pmLong_range.h"

using namespace Nauticle;

bool pmVTK_writer::write_domain = true;

void pmVTK_writer::fill_scalar_vertices(vtkSmartPointer<vtkDoubleArray> scalar) const {
	size_t n = cas->get_workspace()->get_number_of_nodes();
	for(int i=0;i<n;i++) {
		double num = 0.0;
		scalar->InsertNextTuple(&num);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Push lines to polydata object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_pairs_to_polydata() {
	auto interactions = cas->get_workspace()->get_interactions();
	for(auto const& it:interactions) {
		{
			auto connectivity = std::dynamic_pointer_cast<pmConnectivity<pmSpring>>(it);
			if(connectivity.use_count()!=0) {
				auto const& pairs = connectivity->get_pairs();
				std::vector<int> const& first = pairs.get_first();
				std::vector<int> const& second = pairs.get_second();
				if(first.empty()) { continue; }
				vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
				vtkSmartPointer<vtkDoubleArray> line_id = vtkSmartPointer<vtkDoubleArray>::New();
				line_id->SetNumberOfComponents(1);
				fill_scalar_vertices(line_id);
				line_id->SetName("line_id");
				for(int i=0; i<first.size(); i++) {
					vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
					line->GetPointIds()->SetId(0, first[i]);
					line->GetPointIds()->SetId(1, second[i]);
					lines->InsertNextCell(line);
					double num = (double)i;
					line_id->InsertNextTuple(&num);
				}
				polydata->SetLines(lines);
				polydata->GetCellData()->SetScalars(line_id);
			}
		}
		{
			auto connectivity = std::dynamic_pointer_cast<pmConnectivity<pmCollision_handler>>(it);
			if(connectivity) {
				auto pairs = connectivity->get_pairs();
				std::vector<int> const& first = pairs.get_first();
				std::vector<int> const& second = pairs.get_second();
				if(first.empty()) { continue; }
				vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
				vtkSmartPointer<vtkDoubleArray> line_id = vtkSmartPointer<vtkDoubleArray>::New();
				line_id->SetNumberOfComponents(1);
				fill_scalar_vertices(line_id);
				line_id->SetName("line_id");
				for(int i=0; i<first.size(); i++) {
					vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
					line->GetPointIds()->SetId(0, first[i]);
					line->GetPointIds()->SetId(1, second[i]);
					lines->InsertNextCell(line);
					double num = (double)i;
					line_id->InsertNextTuple(&num);
				}
				polydata->SetLines(lines);
				polydata->GetCellData()->SetScalars(line_id);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Push nodes to polydata object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_nodes_to_polydata() {
	std::shared_ptr<pmWorkspace> workspace = cas->get_workspace();
	size_t n = workspace->get_number_of_nodes();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetNumberOfPoints(n);
	vtkSmartPointer<vtkCellArray> vertices =vtkSmartPointer<vtkCellArray>::New();
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
/// Push cell field data to polydata object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_pair_fields_to_polydata() {
	auto interactions = cas->get_workspace()->get_interactions();
	for(auto const& it:interactions) {
		{
			auto connectivity = std::dynamic_pointer_cast<pmConnectivity<pmSpring>>(it);
			if(connectivity) {
				auto const& pairs = connectivity->get_pairs();
				size_t n = pairs.get_number_of_pairs();
				if(n==0) { continue; }
				for(auto const& it:pairs.get_data()) {
					vtkSmartPointer<vtkDoubleArray> field = vtkSmartPointer<vtkDoubleArray>::New();
					field->SetName(it.first.c_str());
					field->SetNumberOfComponents(1);
					fill_scalar_vertices(field);
					for(int i=0; i<n; i++) {
						double data = it.second[i];
						field->InsertNextTuple(&data);
					}
					polydata->GetCellData()->AddArray(field);
				}
			}
		}
		{
			auto connectivity = std::dynamic_pointer_cast<pmConnectivity<pmCollision_handler>>(it);
			if(connectivity) {
				auto const& pairs = connectivity->get_pairs();
				size_t n = pairs.get_number_of_pairs();
				if(n==0) { continue; }
				for(auto const& it:pairs.get_data()) {
					vtkSmartPointer<vtkDoubleArray> field = vtkSmartPointer<vtkDoubleArray>::New();
					field->SetName(it.first.c_str());
					field->SetNumberOfComponents(1);
					fill_scalar_vertices(field);
					for(int i=0; i<n; i++) {
						double data = it.second[i];
						field->InsertNextTuple(&data);
					}
					polydata->GetCellData()->AddArray(field);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Push point field data to polydata object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_point_fields_to_polydata() {
	bool scalar_set = false;
	size_t n = cas->get_workspace()->get_number_of_nodes();
	for(auto const& it:cas->get_workspace()->get<pmField>()) {
		vtkSmartPointer<vtkDoubleArray> field = vtkSmartPointer<vtkDoubleArray>::New();
		if(it->is_printable()) {
			field->SetName(it->get_name().c_str());
			if(it->get_type()=="SCALAR") {
				field->SetNumberOfComponents(1);
				field->SetNumberOfTuples(n);
				for(int i=0; i<n; i++) {
					pmTensor t = it->evaluate(i);
					if(t.numel()==0){
						t = pmTensor{1,1,0.0};
					}
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
					double data[3] = {0,0,0};
					for(int j=0; j<t.numel(); j++) {
						if(std::abs(t[j])<NAUTICLE_EPS) {
							data[j] = 0.0;
						} else {
							data[j] = t[j];
						}
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
	vtkSmartPointer<vtkDoubleArray> double_array = vtkSmartPointer<vtkDoubleArray>::New();
	double_array->SetNumberOfComponents(field_data.numel());
	double_array->SetName(name.c_str());
	double_array->InsertNextTuple(&field_data[0]);
	polydata->GetFieldData()->AddArray(double_array);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Pushes domain into polydata.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_domain_to_polydata() {
	std::shared_ptr<pmParticle_system> psys = cas->get_workspace()->get_particle_system();
	std::stringstream smin;
	std::stringstream smax;
	std::stringstream scsize;
	std::stringstream sbnd;
	smin << "domain_min:" << psys->get_minimum();
	smax << "domain_max:" << psys->get_maximum();
	scsize << "cell_size:" << psys->get_cell_size();
	sbnd << "boundary:" << psys->get_boundary();
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
/// Pushes equations into polydata.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_equations_to_polydata() {
	std::vector<std::shared_ptr<pmEquation>> equations = cas->get_equations();
	vtkSmartPointer<vtkStringArray> string_array = vtkSmartPointer<vtkStringArray>::New();
	string_array->SetNumberOfComponents(1);
	string_array->SetNumberOfTuples(equations.size());
	size_t i=0;
	string_array->SetName("equations");
	for(auto const& it:equations) {
		std::stringstream ss;
		ss << it->get_name() << ":" << it;
		string_array->SetValue(i,ss.str().c_str());
		i++;
	}
	polydata->GetFieldData()->AddArray(string_array);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes case into vtk file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::update() {
	if(cas.use_count()<1) {
		ProLog::pLogger::warning_msgf("No cas added to VTK writer.\n");
		return;
	}
	push_domain_to_polydata();
	push_variables_to_polydata();
	push_constants_to_polydata();
	push_equations_to_polydata();
	push_nodes_to_polydata();
	push_pairs_to_polydata();
	push_pair_fields_to_polydata();
	push_point_fields_to_polydata();
	push_asymmetric_to_polydata();

	for(auto const& it:cas->get_background()) {
		it->write_geometry(file_name);
	}

	// Write vtk file.
	vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
	writer->SetFileName(file_name.c_str());
	writer->SetInputData(polydata);
	switch(mode) {
		case ASCII : writer->SetFileTypeToASCII(); break;
		case BINARY : writer->SetFileTypeToBinary(); break;
	}
	writer->Write();
	if(write_domain) {
		vtkSmartPointer<vtkRectilinearGridWriter> domain_writer = vtkSmartPointer<vtkRectilinearGridWriter>::New();
		domain_writer->SetFileName("domain.vtk");
		auto psys = cas->get_workspace()->get_particle_system();
		int dimensions = psys->get_dimensions();
		pmTensor minimum = psys->get_minimum();
		pmTensor maximum = psys->get_maximum();
		pmTensor num_cells = maximum-minimum;
		pmTensor cell_size = psys->get_cell_size();

		rectilinear_grid->SetDimensions(num_cells[0]+1, dimensions>1?num_cells[1]+1:1.0, dimensions>2?num_cells[2]+1:1.0);
		
		vtkSmartPointer<vtkDoubleArray> xArray = vtkSmartPointer<vtkDoubleArray>::New();
		vtkSmartPointer<vtkDoubleArray> yArray = vtkSmartPointer<vtkDoubleArray>::New();
		vtkSmartPointer<vtkDoubleArray> zArray = vtkSmartPointer<vtkDoubleArray>::New();
		for(int i=0; i<=num_cells[0]+NAUTICLE_EPS; i++) {
			xArray->InsertNextValue((minimum[0]+i)*cell_size[0]);
		}
		for(int i=0; i<=(dimensions>1?num_cells[1]+NAUTICLE_EPS:0); i++) {
			yArray->InsertNextValue(dimensions>1?(minimum[1]+i)*cell_size[1]:0.0);
		}		
		for(int i=0; i<=(dimensions>2?num_cells[2]+NAUTICLE_EPS:0); i++) {
			zArray->InsertNextValue(dimensions>2?(minimum[2]+i)*cell_size[2]:0.0);
		}
		rectilinear_grid->SetXCoordinates(xArray);
		rectilinear_grid->SetYCoordinates(yArray);
		rectilinear_grid->SetZCoordinates(zArray);
		domain_writer->SetInputData(rectilinear_grid);
		domain_writer->Write();
		write_domain = false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the write mode to ASCII or BINARY.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::set_write_mode(write_mode wm) {
	mode = wm;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Pushes the variables stored in the pmCase to the polydata.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_variables_to_polydata() {
	vtkSmartPointer<vtkStringArray> string_array = vtkSmartPointer<vtkStringArray>::New();
	std::vector<std::shared_ptr<pmVariable>> variables = cas->get_workspace()->get<pmVariable>();
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

/////////////////////////////////////////////////////////////////////////////////////////
/// Pushes the constants stored in the pmCase to the polydata.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_constants_to_polydata() {
	vtkSmartPointer<vtkStringArray> string_array = vtkSmartPointer<vtkStringArray>::New();
	std::vector<std::shared_ptr<pmConstant>> constants = cas->get_workspace()->get<pmConstant>();
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

/////////////////////////////////////////////////////////////////////////////////////////
/// Pushes the list of asymmetric fields to the polydata.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_writer::push_asymmetric_to_polydata() {
	vtkSmartPointer<vtkStringArray> string_array = vtkSmartPointer<vtkStringArray>::New();
	std::vector<std::shared_ptr<pmField>> fields = cas->get_workspace()->get<pmField>();
	size_t counter = 0;
	for(auto const& it:fields) {
		if(!it->is_symmetric()) {
			counter++;
		}
	}
	if(counter==0) { return; }
	string_array->SetName("asymmetric");
	string_array->SetNumberOfComponents(1);
	string_array->SetNumberOfValues(counter);
	size_t i=0;
	for(auto const& it:fields) {
		if(!it->is_symmetric()) {
			std::stringstream ss;
			ss << it->get_name().c_str();
			string_array->SetValue(i,ss.str().c_str());
			i++;
		}
	}
	polydata->GetFieldData()->AddArray(string_array);
}
