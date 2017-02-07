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
    
#ifndef _FMAX_H_
#define _FMAX_H_

#include "pmFsearch.h"

class pmFmax : public pmFsearch {
	std::shared_ptr<pmExpression> clone_impl() const override;
private:
	void process(pmTensor& value, Eval_type eval_type=current) const override;
public:
	pmFmax(std::shared_ptr<pmExpression>);
	pmFmax(pmFmax const& other);
	pmFmax(pmFmax&& other);
	pmFmax& operator=(pmFmax const& other);
	pmFmax& operator=(pmFmax&& other);
	std::shared_ptr<pmFmax> clone() const;
	void print() const override;
	virtual void write_to_string(std::ostream& os) const override;
};

inline std::ostream& operator<<(std::ostream& os, pmFmax const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_FMAX_H_