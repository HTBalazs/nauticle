/*
    Copyright 2016 Balazs Toth
    This file is part of LEMPS.

    LEMPS is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LEMPS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LEMPS.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://bitbucket.org/lempsproject/
*/

#ifndef _FEM_INTERFACE_H_
#define _FEM_INTERFACE_H_

#include <iostream>
#include <memory>
#include <array>
#include "pmInteraction.h"
#include "pmTensor.h"

class pmFem_interface : public pmInteraction<1> {
private:
	std::shared_ptr<pmExpression> clone_impl() const;
public:
	pmFem_interface() {}
	pmFem_interface(std::array<std::shared_ptr<pmExpression>,1> op);
	pmFem_interface(pmFem_interface const& other);
	pmFem_interface(pmFem_interface&& other);
	pmFem_interface& operator=(pmFem_interface const& other);
	pmFem_interface& operator=(pmFem_interface&& other);
	virtual ~pmFem_interface() {}
	void print() const override;
	pmTensor process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const;
	pmTensor evaluate(int const& i, Eval_type eval_type/*=current*/) const override;
	std::shared_ptr<pmFem_interface> clone() const;
	virtual void write_to_string(std::ostream& os) const override;
	int get_field_size() const override;
};

#endif // _FEM_INTERFACE_H_