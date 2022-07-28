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

#ifndef _LONG_RANGE_H_
#define _LONG_RANGE_H_

#include <memory>
#include "pmInteraction.h"
#include "pmMesh.h"
#include "prolog/pLogger.h"
#include "pmKernel.h"

namespace Nauticle {
	/** This abstract class implements the conventianal Smoothed Particle Hydrodynamics
	//  through interactions between particles.
	*/
	template <size_t S>
	class pmLong_range : public pmInteraction<S>, public pmMesh {
	protected:
		using Func_delete_marker = std::function<bool(pmTensor const&, int const&, int const&)>;
		std::vector<pmMesh::pmPairs> pairs;
		size_t depth = 1;
	protected:
		void delete_pairs(Func_delete_marker condition, size_t const& level=0);
	public:
		pmLong_range();
		virtual ~pmLong_range() {}
		void set_storage_depth(size_t const& d) override;
		void update_numbering(std::vector<int> const& sorted_particle_idx);
		virtual pmPairs const& get_pairs(size_t const& level=0) const override;
		virtual pmPairs& get_pairs(size_t const& level=0) override;
	};
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	pmLong_range<S>::pmLong_range() {
		pairs.resize(depth);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Return the list of pairs in the mesh.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	typename pmMesh::pmPairs const& pmLong_range<S>::get_pairs(size_t const& level/*=0*/) const {
		return pairs[level];
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Return the list of pairs in the mesh.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	typename pmMesh::pmPairs& pmLong_range<S>::get_pairs(size_t const& level/*=0*/) {
		return pairs[level];
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Set the number of former values to be stored.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	void pmLong_range<S>::set_storage_depth(size_t const& d) {
		pmOperator<S>::set_storage_depth(d);
		pairs.resize(depth);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Update the mesh according to the sorted particle list.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	void pmLong_range<S>::update_numbering(std::vector<int> const& sorted_particle_idx) {
		for(auto& it:pairs) {
			it.renumber_pairs(sorted_particle_idx);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Delete marked pairs from the mesh.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	void pmLong_range<S>::delete_pairs(Func_delete_marker condition, size_t const& level/*=0*/) {
		auto first = pairs[level].get_first();
		auto second = pairs[level].get_second();
		for(int pi=0; pi<pairs[level].size(); pi++) {
			int i = first[pi];
			int j = second[pi];
			pmTensor pos_i = this->psys->get_value(i);
			pmTensor pos_j = this->psys->get_value(j);
			pmTensor rel_pos = pos_j-pos_i;
			if(condition(rel_pos,i,j)) {
				pairs[level].mark_to_delete(pi);
			}
		}
		pairs[level].delete_marked_pairs();
	}
}

#include "Color_undefine.h"

#endif //_LONG_RANGE_H_
