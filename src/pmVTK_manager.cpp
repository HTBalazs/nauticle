/*
    Copyright 2016-2017 Balazs Toth
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
std::shared_ptr<pmSolver> pmVTK_manager::get_solver() const {
	return solver;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Overwrites the existing solver with the given one.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_manager::set_solver(std::shared_ptr<pmSolver> sol) {
	solver = sol;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the name of the vtk-file.
/////////////////////////////////////////////////////////////////////////////////////////
void pmVTK_manager::set_file_name(std::string const& fn) {
	file_name = fn;
}




