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
    
#ifndef _CELL_ITERATOR_H_
#define _CELL_ITERATOR_H_

#include "pmTensor.h"
#include "pmSort.h"
#include <vector>

namespace Nauticle {
	class pmCell_iterator final {
		std::vector<pmTensor> list;
	private:
		void combinations_recursive(const std::vector<int> &elems, size_t const& dims, std::vector<size_t> &pos, size_t const& depth);
		void combinations(const std::vector<int> &elems, size_t const& dims);
	public:
		void build_cell_iterator(size_t const& dims);
		std::vector<pmTensor> const& get_list() const;
	};
}

#endif // _CELL_ITERATOR_H_