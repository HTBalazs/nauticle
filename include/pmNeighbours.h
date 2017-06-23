/*
    Copyright 2016-2017 Balazs Toth
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

#ifndef _NEIGHBOURS_H_
#define _NEIGHBOURS_H_

#include "pmInteraction.h"
#include "prolog/pLogger.h"

namespace Nauticle {
	/** This class calculates the number of neighbouring particles within the given range.
	//  It requires a pmParticle_system assigned to it.
	*/
	class pmNeighbours : public pmInteraction<1> {
		std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmNeighbours() {}
		pmNeighbours(std::array<std::shared_ptr<pmExpression>,1> op);
		pmNeighbours(pmNeighbours const& other);
		pmNeighbours(pmNeighbours&& other);
		pmNeighbours& operator=(pmNeighbours const& other);
		pmNeighbours& operator=(pmNeighbours&& other);
		virtual ~pmNeighbours() {}
		void print() const override;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		std::shared_ptr<pmNeighbours> clone() const;
		int get_field_size() const override;
		virtual void write_to_string(std::ostream& os) const override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmNeighbours const* obj) {
		obj->write_to_string(os);
		return os;
	}
}

#endif // _NEIGHBOURS_H_