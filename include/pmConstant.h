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
    
#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#include <string>
#include "pmSingle.h"

/** This class implements the constant expression in the expression tree. 
//  A constant can be named or anonymous. The value of a constant cannot be changed
//  after definition so the set_value function does not do anything.
*/

class pmConstant final : public pmSingle {
	bool hidden=false;
protected:
	virtual std::shared_ptr<pmExpression> clone_impl() const override;
public:
	pmConstant()=delete;
	pmConstant(std::string n, pmTensor const& value, bool const& hh=false);
	pmConstant(pmTensor const& value);
	pmConstant(pmConstant const&)=default;
	pmConstant& operator=(pmConstant const&)=default;
	virtual ~pmConstant() override {}
	void print() const override;
	std::shared_ptr<pmConstant> clone() const;
	void set_value(pmTensor const& value, int const& i=0) override;
	bool is_hidden() const override;
	virtual void write_to_string(std::ostream& os) const override;
};

inline std::ostream& operator<<(std::ostream& os, pmConstant const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_CONSTANT_H_