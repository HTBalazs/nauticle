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

#ifndef _FENICS_OPERATOR_POISSON_H_
#define _FENICS_OPERATOR_POISSON_H_

#include <iostream>
#include <memory>
#include <array>
#include "pmInteraction.h"
#include "pmTensor.h"

//*/#include "0_dam_break_poisson.h"/*
#include "1_lid_driven_cavity_poisson.h"//*/

namespace Nauticle {

	class pmFenics_operator_poisson : public pmInteraction<6> {
	private:
		std::shared_ptr<Problem_poisson> problem;
		std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmFenics_operator_poisson() {}
		pmFenics_operator_poisson(std::array<std::shared_ptr<pmExpression>,6> op);
		pmFenics_operator_poisson(pmFenics_operator_poisson const& other);
		pmFenics_operator_poisson(pmFenics_operator_poisson&& other);
		pmFenics_operator_poisson& operator=(pmFenics_operator_poisson const& other);
		pmFenics_operator_poisson& operator=(pmFenics_operator_poisson&& other);
		virtual ~pmFenics_operator_poisson() {}
		void print() const override;
		pmTensor process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		std::shared_ptr<pmFenics_operator_poisson> clone() const;
		int get_field_size() const override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmFenics_operator_poisson const* obj) {
		obj->write_to_string(os);
		return os;
	}
}

#endif // _FENICS_OPERATOR_POISSON_H_