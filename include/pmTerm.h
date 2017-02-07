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
    
#ifndef _TERM_H_
#define _TERM_H_

#include <string>
#include "pmExpression.h"

/** This interface forms the base for single and field expressions. It holds a name.
*/
class pmTerm : public pmExpression {
protected:
	std::string name = "";
public:
	virtual ~pmTerm() {}
	virtual std::string get_name() const;
	virtual pmTensor get_value(int const& i=0) const=0;
	virtual void print() const override;
	virtual void printv() const=0;
	virtual void double_steps(bool const&) override {}
	virtual int get_field_size() const override;
	std::shared_ptr<pmTerm> clone() const;
	virtual void set_value(pmTensor const& value, int const& i=0)=0;
	bool is_assigned() const override;
	virtual bool is_hidden() const;
	virtual std::string get_type() const=0;
};

inline std::ostream& operator<<(std::ostream& os, pmTerm const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_TERM_H_