/////////////////////////////////////////////////////////////////////////////
// Name:        pmFunction_parser.h
// Purpose:     pmFunction_parser class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

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