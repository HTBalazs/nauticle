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
    
#ifndef _FUNCTION_PARSER_H_
#define _FUNCTION_PARSER_H_

#include <iostream>
#include <stack>
#include <string>
#include "prolog/pLogger.h"
#include "pmExpression_parser.h"
#include "pmFunction.h"
#include "pmNoncopyable.h"

/** This class converts the given std::string formatted equation to a pmFunction object.
*/
class pmFunction_parser final : public pmExpression_parser {
private:
	bool verify_sides(std::string const& raw_data) const;
	std::string get_lhs_infix(std::string const& data) const;
	std::string get_rhs_infix(std::string const& data) const;
public:
	pmFunction_parser() {}
	virtual ~pmFunction_parser() override {}
	std::shared_ptr<pmFunction> analyse_function(std::string const& name, std::string const& raw_data, std::shared_ptr<pmWorkspace> workspace);
};

#endif //_FUNCTION_PARSER_H_