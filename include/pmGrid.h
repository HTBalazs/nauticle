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
    
#ifndef _GRID_H_
#define _GRID_H_

#include <vector>
#include <memory>
#include "pmTensor.h"
#include "prolog/pLogger.h"
#include "pmNoncopyable.h"

/** This class implements the particle generation over a uniform spatial grid.
*/
class pmGrid {
	pmTensor position;
	pmTensor size;
	pmTensor offset;
	pmTensor distance;
	pmTensor grid_id;
	std::vector<pmTensor> grid;
	using Func = std::function<void(std::vector<int>&)>;
private:
	void n_level_loop(std::vector<int>& indexes, const std::vector<int>& endPerIndex, int idx, Func process);
	void initialize_direction(float const& ofs, float& s, float& dist, float& n) const;
	void initialize_grid(pmTensor& S, pmTensor& D, pmTensor& N) const;
public:
	pmGrid() {}
	pmGrid(pmGrid const& other);
	pmGrid(pmGrid&& other);
	pmGrid& operator=(pmGrid const& other);
	pmGrid& operator=(pmGrid&& other);
	~pmGrid() {}
	void set_position(pmTensor const& p);
	void set_size(pmTensor const& s);
	void set_offset(pmTensor const& o);
	void set_distance(pmTensor const& d);
	void set_grid_id(pmTensor const& t);
	void generate();
	void merge(std::shared_ptr<pmGrid> other);
	void print() const;
	void destroy_grid();
	std::vector<pmTensor> get_grid() const;
	pmTensor get_grid_id() const;
	size_t get_grid_size() const;
};

#endif // _GRID_H_