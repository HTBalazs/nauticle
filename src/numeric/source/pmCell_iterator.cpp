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

#include "pmCell_iterator.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the dims length combinations of the numbers stored in elems.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCell_iterator::combinations_recursive(std::vector<int> const& elems, size_t const& dims, std::vector<size_t>& pos, size_t const& depth) {
	if(depth>=dims) {
		pmTensor tensor = pmTensor{(int)dims,1,0};
		for (size_t i = 0; i < pos.size(); ++i) {
			tensor[i] = elems[pos[i]];
		}
		list.push_back(tensor);
		return; 
	}
	for(size_t i=0;i<elems.size();i++) {
		pos[depth] = i;
		combinations_recursive(elems, dims, pos, depth + 1);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the dims length combinations of the numbers stored in elems.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCell_iterator::combinations(std::vector<int> const& elems, size_t const& dims) {
	std::vector<size_t> positions(dims, 0);
	combinations_recursive(elems, dims, positions, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Builds the array that iterates over the adjacent cells.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCell_iterator::build_cell_iterator(size_t const& dims) {
	if(!list.empty()) { return; }
	std::vector<int> elements = {-1, 0, 1};
	combinations(elements, dims);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the iterator list.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<pmTensor> const& pmCell_iterator::get_list() const {
	return list;
}