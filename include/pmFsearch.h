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

#ifndef _FSEARCH_H_
#define _FSEARCH_H_

#include "pmInteraction.h"

class pmFsearch : public pmInteraction<1> {
private:
	virtual void process(pmTensor& value, Eval_type eval_type=current) const=0;
public:
	virtual ~pmFsearch() {}
	int get_field_size() const override;
	bool is_assigned() const override;
	pmTensor evaluate(int const& i, Eval_type eval_type=current) const override;
};

#endif //_FSEARCH_H_