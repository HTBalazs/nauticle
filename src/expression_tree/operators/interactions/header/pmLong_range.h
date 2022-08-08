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
#include "prolog/pLogger.h"
#include "pmKernel.h"
#include "pmConnectivity.h"

namespace Nauticle {
	/** This abstract class implements the conventianal Smoothed Particle Hydrodynamics
	//  through interactions between particles.
	*/
	template <size_t S, typename Derived>
	class pmLong_range : public pmInteraction<S>, public pmConnectivity<Derived> {
	protected:
		void delete_pairs(typename pmConnectivity<Derived>::Func_delete_marker condition, size_t const& level=0) override;
	public:
		virtual ~pmLong_range() {}
		void set_storage_depth(size_t const& d) override;
		void update(size_t const& level=0) override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Set the number of former values to be stored.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S, typename Derived>
	void pmLong_range<S,Derived>::set_storage_depth(size_t const& d) {
		pmOperator<S>::set_storage_depth(d);
		pmConnectivity<Derived>::pairs.resize(d);
	}

	template <size_t S, typename Derived>
	void pmLong_range<S,Derived>::update(size_t const& level/*=0*/) {
		this->set_number_of_nodes(this->psys->get_field_size());
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Delete marked pairs from the mesh.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S, typename Derived>
	void pmLong_range<S,Derived>::delete_pairs(typename pmConnectivity<Derived>::Func_delete_marker condition, size_t const& level/*=0*/) {
		auto first = pmConnectivity<Derived>::pairs[level].get_first();
		auto second = pmConnectivity<Derived>::pairs[level].get_second();
		for(int pi=0; pi<pmConnectivity<Derived>::pairs[level].get_number_of_pairs(); pi++) {
			int i = first[pi];
			int j = second[pi];
			pmTensor pos_i = this->psys->get_value(i);
			pmTensor pos_j = this->psys->get_value(j);
			pmTensor rel_pos = pos_j-pos_i;
			if(condition(rel_pos,i,j)) {
				pmConnectivity<Derived>::pairs[level].mark_to_delete(pi);
			}
		}
		pmConnectivity<Derived>::pairs[level].delete_marked_pairs();
	}
}

#include "Color_undefine.h"

#endif //_LONG_RANGE_H_
