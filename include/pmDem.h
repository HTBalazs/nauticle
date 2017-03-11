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
    
#ifndef _DEM_H_
#define _DEM_H_

#include "pmInteraction.h"
#include "prolog/pLogger.h"

/** This class implements the conventianal Discrete element method as 
//  through interactions between particles. 
*/
class pmDem : public pmInteraction<6> {
private:
	std::shared_ptr<pmExpression> clone_impl() const override;
public:
	pmDem() {}
	pmDem(std::array<std::shared_ptr<pmExpression>,6> op);
	pmDem(pmDem const& other);
	pmDem(pmDem&& other);
	pmDem& operator=(pmDem const& other);
	pmDem& operator=(pmDem&& other);
	virtual ~pmDem() {}
	void print() const override;
	pmTensor evaluate(int const& i, Eval_type eval_type/*=current*/) const override;
	std::shared_ptr<pmDem> clone() const;
	virtual void write_to_string(std::ostream& os) const override;
};

/////////////////////////////////////////////////////////////////////////////////////////
/// Implementaton of << operator.
/////////////////////////////////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& os, pmDem const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_DEM_H_