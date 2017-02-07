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
    
#include "pmGrid_space.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmGrid_space::pmGrid_space(pmGrid_space const& other) {
	this->grid = other.grid;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmGrid_space::pmGrid_space(pmGrid_space&& other) {
	this->grid = std::move(other.grid);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmGrid_space& pmGrid_space::operator=(pmGrid_space const& other) {
	if(this!=&other) {
		this->grid = other.grid;
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmGrid_space& pmGrid_space::operator=(pmGrid_space&& other) {
	if(this!=&other) {
		this->grid = std::move(other.grid);
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Adds the given grid to the grid space.
/////////////////////////////////////////////////////////////////////////////////////////
void pmGrid_space::add_grid(std::shared_ptr<pmGrid> new_grid) {
	grid.push_back(new_grid);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the vector of grids. 
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmGrid>> pmGrid_space::get_grids() const {
	return grid;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns all grids merged in one grid. 
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmGrid> pmGrid_space::get_merged_grid() const {
	std::shared_ptr<pmGrid> merged_grid = std::make_shared<pmGrid>();
	for(auto const& it:grid) {
		merged_grid->merge(it);
	}
	return merged_grid;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the field of grid id-s. The field size is identical to the sum of all grid sizes.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<pmTensor> pmGrid_space::get_grid_id_field() const {
	std::vector<pmTensor> grid_id_field;
	grid_id_field.reserve(get_grid_space_size());
	for(auto const& it:grid) {
		pmTensor grid_id = it->get_grid_id();
		for(int i=0;i<it->get_grid_size(); i++) {
			grid_id_field.push_back(grid_id);
		}
	}
	return grid_id_field;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of nodes in the grid space.
/////////////////////////////////////////////////////////////////////////////////////////
size_t pmGrid_space::get_grid_space_size() const {
	size_t size = 0;
	for(auto const& it:grid) {
		size+=it->get_grid_size();
	}
	return size;
}