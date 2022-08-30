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

#ifndef _PM_PAIRS_H_
#define _PM_PAIRS_H_

#include "Color_define.h"
#include <memory>
#include <vector>
#include <utility>

namespace Nauticle {
	using pmPair_data = std::pair<std::string,std::vector<double>>;
	class pmPairs {
		std::vector<pmPair_data> pair_data;
		std::vector<std::vector<size_t>> pair_index;
		std::vector<size_t> delete_marker;
		std::vector<int> first;
		std::vector<int> second;
	private:
		void update_pair_idx();
	public:
		pmPairs() = default;
		void set_number_of_nodes(int const& num_particles);
		void add_pair(int const& i1, int const& i2, std::vector<double> const& new_values_ordered);
		void delete_marked_pairs();
		void reset();
		int get_number_of_pairs() const;
		std::vector<int> const& get_first() const;
		std::vector<int> const& get_second() const;
		void add_data(std::string const& name, double const& initial_value=0.0);
		std::vector<pmPair_data>& get_data();
		std::vector<pmPair_data> const& get_data() const;
		std::vector<double>& get_data(std::string const& name);
		std::vector<double> const& get_data(std::string const& name) const;
		std::vector<size_t> const& get_pair_index(size_t const& i) const;
		std::vector<std::vector<size_t>> const& get_pair_index() const;
		void mark_to_delete(size_t const& i);
	};
}

#include "Color_undefine.h"

#endif //_PM_PAIRS_H_