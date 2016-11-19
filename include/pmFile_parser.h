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
    
#ifndef _FILE_PARSER_H_
#define _FILE_PARSER_H_

#include <string>
#include "pmNoncopyable.h"

class pmFile_parser : public pmNoncopyable {
protected:
	std::fstream file;
	std::string name;
	std::string filecontent;
public:
	virtual ~pmFile_parser() {}
	virtual void read_file(std::string const& filename)=0;
	virtual void print() const=0;
};

#endif //_FILE_PARSER_H_