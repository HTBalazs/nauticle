#include "pmSolid.h"
#include <vtkTransformFilter.h>
#include <vtkTransform.h>
#include <vtkProbeFilter.h>
#include <vtkPointSet.h>
#include <Eigen/Eigen>
#include <set>

using namespace Nauticle;
using namespace ProLog;

size_t pmSolid::counter = 0;

void pmSolid::print() const {
	pLogger::headerf<LBL>("Solid surface:");
	pLogger::titlef<LMA>("Geometry");
	pLogger::logf<YEL>("        file: ");
	pLogger::logf<NRM>("%s\n", file_name.c_str());
	pLogger::logf<YEL>("        thickness: "); thickness->print(); pLogger::line_feed(1);
	pLogger::logf<YEL>("        normal field: ");
	pLogger::logf<NRM>("%s\n", normal_field->get_name().c_str());
	pLogger::logf<YEL>("        potential field: ");
	pLogger::logf<NRM>("%s\n", potential_field->get_name().c_str());
	pLogger::logf<YEL>("        condition: "); condition->print(); pLogger::line_feed(1);
	pLogger::logf<YEL>("        particle condition: "); particle_condition->print(); pLogger::line_feed(1);
	if(center.use_count()) {
		pLogger::logf<YEL>("        center: "); center->print(); pLogger::line_feed(1);
	}
	if(rotation.use_count()) {
		pLogger::logf<YEL>("        rotation: "); rotation->print(); pLogger::line_feed(1);
	}
	if(wall_velocity.use_count()) {
		pLogger::logf<YEL>("        wall velocity: "); wall_velocity->print(); pLogger::line_feed(1);
	}
	pLogger::footerf<LBL>();
}

void pmSolid::set_thickness(std::shared_ptr<pmExpression> thk) {
	thickness = thk;
}

void pmSolid::read_file() {
	reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName(file_name.c_str());
	reader->Update();
	this->solidify();
}

void pmSolid::set_normal_field(std::shared_ptr<pmField> nrm) {
	normal_field = nrm;
}

void pmSolid::set_potential_field(std::shared_ptr<pmField> pot) {
	potential_field = pot;
}

void pmSolid::set_wall_velocity(std::shared_ptr<pmField> wall_vel) {
	wall_velocity = wall_vel;
}

void pmSolid::set_center(std::shared_ptr<pmExpression> ctr) {
	center = ctr;
}

void pmSolid::set_rotation(std::shared_ptr<pmExpression> rot) {
	rotation = rot;
}

template<class T>
void pmSolid::remove_duplicates(std::vector<T>& values) const {
	auto b = values.begin();
	auto e = values.end();
	std::vector<T> seen{};
	e = std::remove_if(b, e, [&seen](T x)->bool {
		for(auto const& it:seen) {
			if(it==x) {
				return true;
			}
		}
		seen.push_back(x);
		return false;
	});
	values.erase(e, values.end());
}

pmTensor pmSolid::get_vertex_normal(pmTensor const& vertex, std::vector<pmTensor> const& normals, int i1, int i2, int i3) const {
	std::vector<pmTensor> new_vertex{vertex+normals[i1],vertex+normals[i2],vertex+normals[i3]};
	/*
	Eigen::Matrix3d A;
	Eigen::Vector3d b;
	A << normals[i1][0], normals[i1][1], normals[i1][2], normals[i2][0], normals[i2][1], normals[i2][2], normals[i3][0], normals[i3][1], normals[i3][2];
	b << (new_vertex[0].transpose()*normals[i1])[0], (new_vertex[1].transpose()*normals[i2])[0], (new_vertex[2].transpose()*normals[i3])[0];
	Eigen::Vector3d x = A.colPivHouseholderQr().solve(b);
	pmTensor result{3,1,0};
	result[0] = x(0);
	result[1] = x(1);
	result[2] = x(2);
	result -= vertex;
	return -result/result.norm();
	/*/
	pmTensor A{3,3,0};
	A[0] = normals[i1][0];
	A[1] = normals[i1][1];
	A[2] = normals[i1][2];
	A[3] = normals[i2][0];
	A[4] = normals[i2][1];
	A[5] = normals[i2][2];
	A[6] = normals[i3][0];
	A[7] = normals[i3][1];
	A[8] = normals[i3][2];
	pmTensor b{3,1,0};
	b[0] = (new_vertex[0].transpose()*normals[i1])[0];
	b[1] = (new_vertex[1].transpose()*normals[i2])[0];
	b[2] = (new_vertex[2].transpose()*normals[i3])[0];
	pmTensor result{3,1,0};
	result = A.inverse()*b - vertex;
	return -result/result.norm();//*/
}

void pmSolid::solidify() {
	vtkSmartPointer<vtkPolyData> surface = vtkSmartPointer<vtkPolyData>(reader->GetOutput());
	int num_points = surface->GetNumberOfPoints();
	int num_cells = surface->GetNumberOfCells();

	std::vector<pmTensor> normal;
	normal.resize(num_points,pmTensor{3,1,0});

	// Pointwise for
	for(int pid=0; pid<num_points; pid++) {
		vtkIdList* face_ids = vtkIdList::New();
		// Get cells of the ith vertex
		surface->GetPointCells(pid, face_ids);
		std::vector<pmTensor> normals;
		size_t num_local_cells = face_ids->GetNumberOfIds();
		// Cellwise for
		for(int cid=0; cid<num_local_cells; cid++) {
			vtkIdList* point_ids = vtkIdList::New();
			// Get points of the cidth cell
			surface->GetCellPoints(face_ids->GetId(cid), point_ids);
			double v0[3];
			double v1[3];
			double v2[3];
			surface->GetPoint(point_ids->GetId(0), v0);
			surface->GetPoint(point_ids->GetId(1), v1);
			surface->GetPoint(point_ids->GetId(2), v2);
			// Compute normal for the cidth cell
			pmTensor n{3,1,0};
			n[0] = ((v0[1]-v1[1])*(v2[2]-v1[2]))-((v0[2]-v1[2])*(v2[1]-v1[1]));
			n[1] = ((v0[2]-v1[2])*(v2[0]-v1[0]))-((v0[0]-v1[0])*(v2[2]-v1[2]));
			n[2] = ((v0[0]-v1[0])*(v2[1]-v1[1]))-((v0[1]-v1[1])*(v2[0]-v1[0]));
			normals.push_back(n/n.norm());
		}
		remove_duplicates(normals);
		int num_normals = normals.size();
		double* p = surface->GetPoint(pid);
		pmTensor vertex{3,1,0};
		vertex[0] = p[0];
		vertex[1] = p[1];
		vertex[2] = p[2];
		if(num_normals==3) {
			normal[pid] = get_vertex_normal(vertex, normals, 0, 1, 2);
		} else if(num_normals==1) {
			normal[pid] = normals[0];
		} else {
			for(int i=0; i<num_normals; i++) {
				pmTensor next_normal = get_vertex_normal(vertex, normals, i, (i+1)%num_normals, (i+2)%num_normals);
				normal[pid] += next_normal/num_normals;
			}
		}
	}

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	points->SetNumberOfPoints(num_points*2);
	int old_size = surface->GetNumberOfPoints();
	for(int pid=0; pid<old_size; pid++) {
		double point[3];
		surface->GetPoint(pid, point);
		points->SetPoint(pid, point[0], point[1], point[2]);
		points->SetPoint(pid+old_size, point[0]-normal[pid][0]*thickness->evaluate(0)[0], point[1]-normal[pid][1]*thickness->evaluate(0)[0], point[2]-normal[pid][2]*thickness->evaluate(0)[0]);
	}

	unstructured_grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	unstructured_grid->SetPoints(points);

	vtkIdTypeArray* connectivity = vtkIdTypeArray::New();
	connectivity->SetNumberOfComponents(7);
	connectivity->SetNumberOfTuples(num_cells);
	for(int cid=0; cid<num_cells; cid++) {
		vtkIdList* point_ids = vtkIdList::New();
		point_ids->SetNumberOfIds(6);
		surface->GetCellPoints(cid, point_ids);

		point_ids->SetId(3,point_ids->GetId(0)+num_points);
		point_ids->SetId(4,point_ids->GetId(1)+num_points);
		point_ids->SetId(5,point_ids->GetId(2)+num_points);

		double list[7];
		list[0] = 6;
		list[1] = point_ids->GetId(0);
		list[2] = point_ids->GetId(1);
		list[3] = point_ids->GetId(2);
		list[4] = point_ids->GetId(3);
		list[5] = point_ids->GetId(4);
		list[6] = point_ids->GetId(5);
		connectivity->SetTuple(cid,list);
	}

	vtkCellArray* cell_array = vtkCellArray::New();
	cell_array->SetCells(num_cells, connectivity);
	unstructured_grid->SetCells(13, cell_array);

	vtkFloatArray* delta = vtkFloatArray::New();
	delta->SetNumberOfComponents(1);
	delta->SetNumberOfTuples(num_points*2);
	delta->SetName("delta");
	for(int i=0; i<num_points*2; i++) {
		delta->SetTuple1(i, (double)(i>=num_points)*thickness->evaluate(0)[0]);
	}

	vtkFloatArray* normal_vector = vtkFloatArray::New();
	normal_vector->SetNumberOfComponents(3);
	normal_vector->SetNumberOfTuples(num_points*2);
	normal_vector->SetName("normal");
	for(int i=0; i<num_points; i++) {
		normal_vector->SetTuple3(i, normal[i][0],normal[i][1],normal[i][2]);
		normal_vector->SetTuple3(i+num_points, normal[i][0],normal[i][1],normal[i][2]);
	}

	unstructured_grid->GetPointData()->SetScalars(delta);
	unstructured_grid->GetPointData()->SetVectors(normal_vector);
}

void pmSolid::initialize() {
	this->read_file();
	this->solidify();
}

void pmSolid::update(double const& dt) {
	if(previous_thickness != thickness->evaluate(0)[0]) {
		this->solidify();
		previous_thickness = thickness->evaluate(0)[0];
	}
	transform(dt);
	interpolate();
}

void pmSolid::transform(double const& dt) {
	if(!rotation.use_count() || !center.use_count() || !wall_velocity.use_count()) { return; }
	vtkSmartPointer<vtkTransformFilter> transform = vtkSmartPointer<vtkTransformFilter>::New();
	vtkSmartPointer<vtkTransform> tr = vtkSmartPointer<vtkTransform>::New();
	tr->Translate(center->evaluate(0)[0],center->evaluate(0)[1],center->evaluate(0)[2]);
	tr->RotateWXYZ(rotation->evaluate(0).norm(), rotation->evaluate(0)[0], rotation->evaluate(0)[1], rotation->evaluate(0)[2]);
	tr->Translate(-center->evaluate(0)[0],-center->evaluate(0)[1],-center->evaluate(0)[2]);
	transform->SetTransform(tr);
	transform->SetInputData(unstructured_grid);
	transform->Update();
	unstructured_grid = transform->GetUnstructuredGridOutput();
	exprt = true;
}

void pmSolid::interpolate() {
	if(position_field.use_count()==0 || normal_field.use_count()==0 || potential_field.use_count()==0 || unstructured_grid==NULL || condition->evaluate(0)[0]==0) {
		return;
	}

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	for(int i=0; i<position_field->get_field_size(); i++) {
		if(particle_condition->evaluate(i)[0]==0) {
			continue;
		}
		pmTensor tensor = position_field->evaluate(i);
		size_t n = tensor.numel();
		points->InsertNextPoint(tensor[0], n>1?tensor[1]:0.0, n>2?tensor[2]:0.0);
	}
  	auto probe_unstructured_grid = vtkSmartPointer<vtkPolyData>::New();
  	probe_unstructured_grid->SetPoints(points);
	auto probe = vtkSmartPointer<vtkProbeFilter>::New();
	probe->SetTolerance(1e-12f);
	probe->SetSourceData(unstructured_grid);
	probe->SetInputData(probe_unstructured_grid);
	probe->Update();
 
 	{
		vtkDataArray* data = probe->GetOutput()->GetPointData()->GetScalars();
		if(data!=NULL) {
			vtkFloatArray* doubleData = vtkFloatArray::SafeDownCast(data);
			for(int i=0; i<doubleData->GetNumberOfTuples(); i++) {
				pmTensor tensor{1,1,doubleData->GetValue(i)};
				potential_field->set_value(tensor,i);
		    }
		}
	}
	{
		vtkDataArray* data = probe->GetOutput()->GetPointData()->GetVectors();
		if(data!=NULL) {
			for(int i=0; i<data->GetNumberOfTuples(); i++) {
				pmTensor tensor{normal_field->evaluate(0).numel(),1,0.0};
				data->GetTuple(i,*tensor);
				normal_field->set_value(tensor,i);
		    }
		}
	}
	if(wall_velocity.use_count()>0) {
		vtkDataArray* data = probe->GetOutput()->GetPointData()->GetScalars();
		if(data!=NULL) {
			vtkFloatArray* doubleData = vtkFloatArray::SafeDownCast(data);
			for(int i=0; i<doubleData->GetNumberOfTuples(); i++) {
				double potential = doubleData->GetValue(i);
				potential_field->set_value(pmTensor{1,1,potential},i);
				if(wall_velocity.use_count()>0 && potential>0) {
					pmTensor vel = cross(rotation->evaluate(0),position_field->evaluate(i)-center->evaluate(0));
					wall_velocity->set_value(vel,i);
				} else {
					wall_velocity->set_value(pmTensor{3,1,0},i);
				}
		    }
		}
	}
}

void pmSolid::write_geometry(std::string const& fn) const {
	if(exprt) {
		size_t lastindex = fn.find_last_of("."); 
		std::string rawname_vtk = fn.substr(0, lastindex);
		lastindex = file_name.find_last_of(".");
		std::string rawname_stl = file_name.substr(0, lastindex);
		vtkSmartPointer<vtkUnstructuredGridWriter> writer = vtkSmartPointer<vtkUnstructuredGridWriter>::New();
		std::string vtk_name{rawname_vtk+"_"+rawname_stl+".vtk"};
		writer->SetFileName(vtk_name.c_str());
		writer->SetInputData(unstructured_grid);
		writer->Update();
		exprt = false;
	}
}








