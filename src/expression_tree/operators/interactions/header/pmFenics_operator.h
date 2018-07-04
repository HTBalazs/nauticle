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

#ifndef _FENICS_OPERATOR_H_
#define _FENICS_OPERATOR_H_

#include <iostream>
#include <memory>
#include <array>
#include "pmInteraction.h"
#include "pmTensor.h"
// #include "fenics_include/Elastodynamics.h"

namespace Nauticle {

	class pmFenics_operator : public pmInteraction<5> {
	private:
		Problem problem;
		std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmFenics_operator() {}
		pmFenics_operator(std::array<std::shared_ptr<pmExpression>,5> op);
		pmFenics_operator(pmFenics_operator const& other);
		pmFenics_operator(pmFenics_operator&& other);
		pmFenics_operator& operator=(pmFenics_operator const& other);
		pmFenics_operator& operator=(pmFenics_operator&& other);
		virtual ~pmFenics_operator() {}
		void print() const override;
		pmTensor process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		std::shared_ptr<pmFenics_operator> clone() const;
		int get_field_size() const override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmFenics_operator const* obj) {
		obj->write_to_string(os);
		return os;
	}
}

#endif // _FENICS_OPERATOR_H_