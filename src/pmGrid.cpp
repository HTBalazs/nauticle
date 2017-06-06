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

    For more information please visit: https://bitbucket.org/nauticleproject/
*/
    
#include "pmGrid.h"
#include <vtkSmartPointer.h>
#include <vtkSimplePointsReader.h>
#include <vtkPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmGrid::pmGrid(pmGrid const& other) {
	this->position = other.position;
	this->size = other.size;
	this->offset = other.offset;
	this->distance = other.distance;
	this->grid_id = other.grid_id;
	this->grid = other.grid;
	this->file_name = other.file_name;
	this->dimensions = other.dimensions;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmGrid::pmGrid(pmGrid&& other) {
	this->position = std::move(other.position);
	this->size = std::move(other.size);
	this->offset = std::move(other.offset);
	this->distance = std::move(other.distance);
	this->grid_id = std::move(other.grid_id);
	this->grid = std::move(other.grid);
	this->file_name = std::move(other.file_name);
	this->dimensions = std::move(other.dimensions);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmGrid& pmGrid::operator=(pmGrid const& other) {
	if(this!=&other) {
		this->position = other.position;
		this->size = other.size;
		this->offset = other.offset;
		this->distance = other.distance;
		this->grid_id = other.grid_id;
		this->grid = other.grid;
		this->file_name = other.file_name;
		this->dimensions = other.dimensions;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmGrid& pmGrid::operator=(pmGrid&& other) {
	if(this!=&other) {
		this->position = std::move(other.position);
		this->size = std::move(other.size);
		this->offset = std::move(other.offset);
		this->distance = std::move(other.distance);
		this->grid_id = std::move(other.grid_id);
		this->grid = std::move(other.grid);
		this->file_name = std::move(other.file_name);
		this->dimensions = std::move(other.dimensions);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the file name of the xyz file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::set_file_name(std::string const& fn) {
	file_name = fn;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the domensions of the grid for xyz file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::set_dimensions(size_t const& d) {
	dimensions = d;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the grid position.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::set_position(pmTensor const& p) {
	position = p;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the grid size.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::set_size(pmTensor const& s) {
	size = s;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the grid offset.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::set_offset(pmTensor const& o) {
	offset = o;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the grid distance.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::set_distance(pmTensor const& d) {
	distance = d;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the grid grid id.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::set_grid_id(pmTensor const& t) {
	grid_id = t;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements n level nested loops recursively, n is the size of indexes and end_per_index vectors.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::n_level_loop(std::vector<int>& indexes, const std::vector<int>& end_per_index, int idx, Func process) {
	if (idx == indexes.size()) {
		process(indexes);
	} else {
		for (indexes[idx] = 0 ; (int)indexes[idx] != (int)end_per_index[idx] ; indexes[idx]++) {
			n_level_loop(indexes, end_per_index, idx+1, process);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Calculates the distance between nodes considering the offset value. It also calculates the number of nodes.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::initialize_direction(double const& ofs, double& s, double& d, double& n) const {
	size_t num = 0;
	if(std::abs(ofs)>d*NAUTICLE_EPS) { 
		num = 1;
		s -= 2*ofs;
	}
	n = (size_t)(s/d)+num;
	d = s/(n-num);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Calculates the distance between nodes considering the offset values.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::initialize_grid(pmTensor& S, pmTensor& D, pmTensor& N) const {
	for(int i=0; i<S.numel(); i++) {
		initialize_direction(offset[i], S[i], D[i], N[i]);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Generates grid. Previously existing grid is destroyed.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::generate() {
	destroy_grid();
	if(file_name=="") {
		pmTensor S = size;
		pmTensor D = distance;
		pmTensor N = pmTensor::make_tensor(S,0);
		initialize_grid(S,D,N);
		std::vector<int> indexes(position.numel(),0);
		std::vector<int> end_per_index;
		for(int i=0; i<N.numel(); i++) {
			end_per_index.push_back(N[i]);
		}
		auto process = [this,&D](std::vector<int>& indexes)->void {
			// Innermost loop content. indexes[i] contains the ith index value.
			pmTensor node{(int)indexes.size(),1,0};
			for(int i=0; i<indexes.size(); i++) {
				node[i] = indexes[i]*D[i]+position[i]+D[i]/2;
			}
			grid.push_back(node);
		};
		n_level_loop(indexes,end_per_index,0, process);
	} else {
		// Read the file
		vtkSmartPointer<vtkSimplePointsReader> reader = vtkSmartPointer<vtkSimplePointsReader>::New();
		reader->SetFileName(file_name.c_str());
		reader->Update();
		vtkSmartPointer<vtkPolyData> poly_data = vtkSmartPointer<vtkPolyData>::New();
		poly_data = reader->GetOutput();
		size_t size = poly_data->GetNumberOfPoints();
		grid.reserve(size);
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		points = poly_data->GetPoints();
		int num_points = (int)poly_data->GetNumberOfPoints();
		for(int i=0; i<num_points; i++) {
			double* p = points->GetPoint(i);
			pmTensor tensor{dimensions,1,0};
			for(int j=0; j<dimensions; j++) {
				tensor[j] = p[j];
			}
			grid.push_back(tensor);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the generated grid.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<pmTensor> pmGrid::get_grid() const {
	return grid;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the generated grid.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::destroy_grid() {
	grid.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the grid.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::print() const {
	for(auto const& it:grid) {
		it.print();
		std::cout << std::endl;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Merged the other grid to the object. Self-merging results in duplicated nodes.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid::merge(std::shared_ptr<pmGrid> other) {
	grid.reserve(this->grid.size() + other->grid.size());
	grid.insert(grid.end(), other->grid.begin(), other->grid.end());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the grid id.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmGrid::get_grid_id() const {
	return grid_id;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns size of the generated grid.
/////////////////////////////////////////////////////////////////////////////////////////
size_t pmGrid::get_grid_size() const {
	return grid.size();
}
