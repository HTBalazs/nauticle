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

#ifndef _PM_OCCLUSION_H_
#define _PM_OCCLUSION_H_

#include "pmInteraction.h"
#include "prolog/pLogger.h"

namespace Nauticle {
	/** This class calculates the number of neighbouring particles within the given range.
	//  It requires a pmParticle_system assigned to it.
	*/
	class pmOcclusion : public pmInteraction<3> {
		std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmOcclusion() {}
		pmOcclusion(std::array<std::shared_ptr<pmExpression>,3> op);
		pmOcclusion(pmOcclusion const& other);
		pmOcclusion(pmOcclusion&& other);
		pmOcclusion& operator=(pmOcclusion const& other);
		pmOcclusion& operator=(pmOcclusion&& other);
		virtual ~pmOcclusion() {}
		void print() const override;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		std::shared_ptr<pmOcclusion> clone() const;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmOcclusion const* obj) {
		obj->write_to_string(os);
		return os;
	}
}

#endif //_PM_OCCLUSION_H_