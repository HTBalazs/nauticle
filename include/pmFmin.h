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

    For more information please visit: https://BalazsToth@bitbucket.org/BalazsToth/lemps
*/
    
#ifndef _FMIN_H_
#define _FMIN_H_

#include "pmFsearch.h"

class pmFmin : public pmFsearch {
	std::shared_ptr<pmExpression> clone_impl() const override;
private:
	void process(pmTensor& value, Eval_type eval_type=current) const override;
public:
	pmFmin(std::shared_ptr<pmExpression>);
	pmFmin(pmFmin const& other);
	pmFmin(pmFmin&& other);
	pmFmin& operator=(pmFmin const& other);
	pmFmin& operator=(pmFmin&& other);
	std::shared_ptr<pmFmin> clone() const;
	void print() const override;
	virtual void write_to_string(std::ostream& os) const override;
};

inline std::ostream& operator<<(std::ostream& os, pmFmin const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_FMIN_H_