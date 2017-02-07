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

#include "pmFsearch.h"

pmTensor pmFsearch::evaluate(int const& i, Eval_type eval_type/*=current*/) const {
	static pmTensor value{1,1};
	if(i==0) {
		process(value, eval_type);
	}
	return value;
}
int pmFsearch::get_field_size() const {
	return 1;
}
bool pmFsearch::is_assigned() const {
	return true;
}