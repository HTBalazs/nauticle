/*
    Copyright 2017 Balazs Toth
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

#ifndef _NEIGHBOURS_H_
#define _NEIGHBOURS_H_

#include "pmInteraction.h"
#include "prolog/pLogger.h"

class pmNeighbours : public pmInteraction<0> {
	std::shared_ptr<pmExpression> clone_impl() const override;
public:
	pmNeighbours() {}
	pmNeighbours(std::array<std::shared_ptr<pmExpression>,0> op);
	pmNeighbours(pmNeighbours const& other);
	pmNeighbours(pmNeighbours&& other);
	pmNeighbours& operator=(pmNeighbours const& other);
	pmNeighbours& operator=(pmNeighbours&& other);
	virtual ~pmNeighbours() {}
	void print() const override;
	pmTensor evaluate(int const& i, Eval_type eval_type/*=current*/) const override;
	std::shared_ptr<pmNeighbours> clone() const;
	int get_field_size() const override;
	virtual void write_to_string(std::ostream& os) const override;
};

inline std::ostream& operator<<(std::ostream& os, pmNeighbours const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif // _NEIGHBOURS_H_