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

    For more information please visit: https://BalazsToth@bitbucket.org/BalazsToth/lemps
*/

#include "pmMath_parser.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Removes white spaces.
/////////////////////////////////////////////////////////////////////////////////////////
void pmMath_parser::remove_spaces(std::string& str) const {
	std::string::iterator it = str.begin();
	while(it!=str.end()) {
		if(*it==' ') {
			str.erase(it, it+1);
		} else {
			it++;
		}
	}
}