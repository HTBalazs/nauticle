/*
	Copyright 2016-2019 Balazs Toth
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

#ifndef _MESH_H_
#define _MESH_H_

#include "pmHysteron.h"
#include <vector>

namespace Nauticle {
	/** This abstract class implements a common type for long range interactions so that they 
	//  can be casted to a non-template parent.
	*/
	class pmMesh {
	protected:
		class pmPairs {
			std::vector<std::vector<size_t>> pair_index;
			std::vector<size_t> delete_marker;
			std::vector<pmHysteron> hysteron;
			std::vector<double> initial_length;
			std::vector<int> first;
			std::vector<int> second;
			mutable std::vector<int> sorted_first;
			mutable std::vector<int> sorted_second;
			mutable std::vector<int> start_first;
			mutable std::vector<int> end_first;
			mutable std::vector<int> start_second;
			mutable std::vector<int> end_second;
		private:
			void sort_lists(std::vector<int>& sorted_pair_idx, std::vector<int> const& pair_vec) const;
			void update_helper_vectors(std::vector<int>& start, std::vector<int>& end, std::vector<int> const& sorted_pair_idx, std::vector<int> const& pair_vec, int const& N) const;
			void update_pairs(std::vector<int>& pair_vec, std::vector<int> const& sorted_pair_idx, std::vector<int> const& start, std::vector<int> const& end, std::vector<int> const& sorted_particle_idx) const;
			void update_pair_idx();
		public:
			pmPairs() = default;
			pmPairs(int const& num_particles);
			pmPairs(std::vector<int> const& fst, std::vector<int> const& snd, int const& num_particles);
			void set_number_of_particles(size_t const& num_particles);
			void renumber_pairs(std::vector<int> const& sorted_particle_idx);
			void add_pair(int const& i1, int const& i2, double const& l0, pmHysteron const& hys=pmHysteron{});
			void delete_marked_pairs();
			void reset();
			int size() const;
			void print() const;
			std::vector<int> const& get_first() const;
			std::vector<int> const& get_second() const;
			pmHysteron& get_hysteron(int const& i);
			pmHysteron const& get_hysteron(int const& i) const;
			std::vector<pmHysteron> const& get_hysteron() const;
			double get_initial_length(int const& idx) const;
			std::vector<double> const& get_initial_length() const;
			std::vector<size_t> const& get_pair_index(size_t const& i) const;
			std::vector<std::vector<size_t>> const& get_pair_index() const;
			void mark_to_delete(size_t const& i);
		};
	public:
		virtual pmPairs const& get_pairs(size_t const& level=0) const=0;
		virtual pmPairs& get_pairs(size_t const& level=0)=0;
	};
}

#endif // _MESH_H_