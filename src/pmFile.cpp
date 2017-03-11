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

#include "pmFile.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFile::pmFile(std::string const& filename, std::string const& mode) {
	file = fopen(filename.c_str(), mode.c_str());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Destructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFile::~pmFile() {
	fclose(file);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements fprintf.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFile::fprint(char const* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(file, fmt, args);
	va_end(args);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements vfprintf.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFile::vfprint(char const* fmt, va_list args) {
	vfprintf(file,fmt,args);
}