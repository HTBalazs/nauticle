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

#include "pmVTK_manager.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a function space filled with tha data read from the VTK file.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmFunction_space> pmVTK_manager::get_function_space() const {
	return function_space;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Overwrites the existing function space with the given one.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_manager::set_function_space(std::shared_ptr<pmFunction_space> fs) {
	function_space=fs;
}

void pmVTK_manager::set_file_name(std::string const& fn) {
	file_name = fn;
}




