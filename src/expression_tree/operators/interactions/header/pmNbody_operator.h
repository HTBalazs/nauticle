/*
    Copyright 2016-2018 Balazs Toth
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

#ifndef _NBODY_H_
#define _NBODY_H_

#include <iostream>
#include <memory>
#include <array>
#include "pmInteraction.h"
#include "pmTensor.h"

namespace Nauticle {
	/** This class implements an N-body interaction between particles defined in the assigned
	//  pmParticle_system. This interaction has N^2 complexity.
	*/
	class pmNbody_operator : public pmInteraction<2> {
	private:
		std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmNbody_operator() {}
		pmNbody_operator(std::array<std::shared_ptr<pmExpression>,2> op);
		pmNbody_operator(pmNbody_operator const& other);
		pmNbody_operator(pmNbody_operator&& other);
		pmNbody_operator& operator=(pmNbody_operator const& other);
		pmNbody_operator& operator=(pmNbody_operator&& other);
		virtual ~pmNbody_operator() {}
		void print() const override;
		pmTensor process(pmTensor const& A_i, pmTensor const& A_j, double const& rho_i, double const& rho_j, double const& m_i, double const& m_j, pmTensor const& r_ji, double const& d_ji, double const& W_ij) const;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		std::shared_ptr<pmNbody_operator> clone() const;
		int get_field_size() const override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmNbody_operator const* obj) {
		obj->write_to_string(os);
		return os;
	}
}

#endif // _NBODY_H_