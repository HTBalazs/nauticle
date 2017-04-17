/////////////////////////////////////////////////////////////////////////////
// Name:        pmGrid_space.h
// Purpose:     pmGrid_space class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) Nauticle-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _GRID_SPACE_H
#define _GRID_SPACE_H

#include <vector>
#include <memory>
#include "pmGrid.h"

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
};

#endif // _GRID_SPACE_H