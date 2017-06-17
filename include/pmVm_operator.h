/*
    Copyright 2017 Balazs Toth
    This file is part of Nauticle.

    Nauticle is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Nauticle is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Nauticle.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://bitbucket.org/nauticleproject/
*/
    
#ifndef _VORTEX_METHOD_OPERATOR_H_
#define _VORTEX_METHOD_OPERATOR_H_

#include <cstdlib>
#include <array>
#include <string>
#include "pmFilter.h"
#include "prolog/pLogger.h"
#include "commonutils/Common.h"
#include "nauticle_constants.h"

class pmVm_operator : public pmFilter<3> {
	std::string op_name;
private:
	std::shared_ptr<pmExpression> clone_impl() const override;
public:
	pmVm_operator() {}
	pmVm_operator(std::array<std::shared_ptr<pmExpression>,3> op);
	pmVm_operator(pmVm_operator const& other);
	pmVm_operator(pmVm_operator&& other);
	pmVm_operator& operator=(pmVm_operator const& other);
	pmVm_operator& operator=(pmVm_operator&& other);
	virtual ~pmVm_operator() {}
	void print() const override;
	pmTensor evaluate(int const& i, size_t const& level=0) const override;
	std::shared_ptr<pmVm_operator> clone() const;
	virtual void write_to_string(std::ostream& os) const override;
};

#endif // _VORTEX_METHOD_OPERATOR_H_