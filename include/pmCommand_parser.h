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
    
#ifndef _COMMAND_PARSER_H_
#define _COMMAND_PARSER_H_

#include <map>
#include <vector>
#include <algorithm>
#include "prolog/pLogger.h"
#include "pmVersion.h"

/** This class analyses the input commands on execution.
*/
class pmCommand_parser {
private:
    int argc;
    std::vector<std::string> argv;
    bool switches_on;
    std::map<std::string,std::string> switch_map;
public:
    pmCommand_parser(int argc_, char* argv_[], bool switches_on_=false);
    ~pmCommand_parser() {}
    std::string get_arg(int i);
    std::string get_arg(std::string s);
    static void print_command_list();
    static void print_header();
    static void print_version();
};

#endif //_COMMAND_PARSER_H_