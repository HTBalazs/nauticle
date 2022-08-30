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

#ifndef _PM_GRID_SPACE_H
#define _PM_GRID_SPACE_H

#include <vector>
#include <memory>
#include "pmGrid.h"

namespace Nauticle {
    /** This class forms a group of grids used by pmParticle_system.
    */
    class pmGrid_space {
    	std::vector<std::shared_ptr<pmGrid>> grid;
    public:
    	pmGrid_space() {}
    	pmGrid_space(pmGrid_space const& other);
    	pmGrid_space(pmGrid_space&& other);
    	pmGrid_space& operator=(pmGrid_space const& other);
    	pmGrid_space& operator=(pmGrid_space&& other);
    	void add_grid(std::shared_ptr<pmGrid> new_grid);
    	std::vector<std::shared_ptr<pmGrid>> get_grids() const;
    	std::vector<pmTensor> get_grid_id_field() const;
    	std::shared_ptr<pmGrid> get_merged_grid() const;
    	size_t get_grid_space_size() const;
        bool is_empty() const;
    };
}

#endif //_PM_GRID_SPACE_H