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
	public:
		virtual ~pmLong_range() {}
		void set_storage_depth(size_t const& d) override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Set the number of former values to be stored.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S, typename Derived>
	void pmLong_range<S,Derived>::set_storage_depth(size_t const& d) {
		pmOperator<S>::set_storage_depth(d);
		pmConnectivity<Derived>::pairs.resize(d);
	}
}

#include "Color_undefine.h"

#endif //_LONG_RANGE_H_
