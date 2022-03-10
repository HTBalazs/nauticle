/*
    Copyright 2016-2020 Balazs Havasi-Toth
    This file is part of Nauticle.

    Nauticle is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Nauticle is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Nauticle.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://bitbucket.org/nauticleproject/
*/
    
#ifndef _COMMAND_PARSER_H_
#define _COMMAND_PARSER_H_

#include <map>
#include <vector>
#include <algorithm>
#include "prolog/pLogger.h"

namespace Nauticle {
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
        static void print_version(bool to_file=false);
    };
}

#endif //_COMMAND_PARSER_H_