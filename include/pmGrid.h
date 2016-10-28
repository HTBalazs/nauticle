/////////////////////////////////////////////////////////////////////////////
// Name:        pmGrid.h
// Purpose:     pmGrid class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _GRID_H_
#define _GRID_H_

#include <vector>
#include <memory>
#include "pmTensor.h"
#include "prolog/pLogger.h"
#include "pmNoncopyable.h"

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