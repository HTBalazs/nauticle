/////////////////////////////////////////////////////////////////////////////
// Name:        pmCommand_parser.h
// Purpose:     pmCommand_parser class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

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
};

#endif //_COMMAND_PARSER_H_