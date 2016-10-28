/////////////////////////////////////////////////////////////////////////////
// Name:        pmMath_parser.h
// Purpose:     pmMath_parser class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _MATH_PARSER_H_
#define _MATH_PARSER_H_

#include <string>
#include <vector>
#include "prolog/pLogger.h"
#include "pmMath_test.h"

/** This interface realizes the most important functions for the mathematical
//  parsing classes.
*/

class pmMath_parser : public pmMath_test {
public:
	pmMath_parser() {}
protected:
	virtual ~pmMath_parser() override {}
	virtual bool verify_infix(std::string const& infix) const=0;
	void remove_spaces(std::string& str) const;
	virtual std::vector<std::string> build_table(std::string const&)=0;
};

#endif //_MATH_PARSER_H_