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

#ifndef _PM_CONNECTIVITY_H_
#define _PM_CONNECTIVITY_H_

#include <memory>
#include "pmInteraction.h"
#include "prolog/pLogger.h"
#include "pmKernel.h"
#include "Color_define.h"
#include "pmPairs.h"

namespace Nauticle {
	/** This abstract class implements the conventianal Smoothed Particle Hydrodynamics
	//  through interactions between particles.
	*/
	class pmNontemplate {};

	template <typename Derived>
	class pmConnectivity : public pmNontemplate {
	protected:
		static std::vector<pmPairs> pairs;
	protected:
		using Func_delete_marker = std::function<bool(pmTensor const&, int const&, int const&)>;
		virtual void delete_pairs(Func_delete_marker condition, size_t const& level=0);
		void set_number_of_nodes(int const& num_particles);
	public:
		virtual ~pmConnectivity() {}
		void add_pair(int const& i1, int const& i2, std::vector<double> const& new_values_ordered);
		pmPairs const& get_pairs(size_t const& level=0) const;
		pmPairs& get_pairs(size_t const& level=0);
	};

	template <typename Derived> std::vector<pmPairs> pmConnectivity<Derived>::pairs;
	
	template <typename Derived>
	void pmConnectivity<Derived>::add_pair(int const& i1, int const& i2, std::vector<double> const& new_values_ordered) {
		for(auto& it:pairs) {
			it.add_pair(i1,i2,new_values_ordered);
		}
	}

	template <typename Derived>
	pmPairs const& pmConnectivity<Derived>::get_pairs(size_t const& level/*=0*/) const {
		return pairs[level];
	}

	template <typename Derived>
	pmPairs& pmConnectivity<Derived>::get_pairs(size_t const& level/*=0*/) {
		return pairs[level];
	}

	template <typename Derived>
	void pmConnectivity<Derived>::set_number_of_nodes(int const& num_particles) {
		for(auto& it:pairs) {
			it.set_number_of_nodes(num_particles);
		}
	}
}

#include "Color_undefine.h"

#endif //_PM_CONNECTIVITY_H_
