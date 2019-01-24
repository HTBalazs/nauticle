/*
    Copyright 2016-2019 Balazs Toth
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
    
#ifndef _EQUATION_PARSER_H_
#define _EQUATION_PARSER_H_

#include <iostream>
#include <stack>
#include <string>
#include "prolog/pLogger.h"
#include "pmExpression_parser.h"
#include "pmEquation.h"

namespace Nauticle {
    /** This class converts the given std::string formatted equation to a pmEquation object.
    */
    class pmEquation_parser final : public pmExpression_parser {
    private:
    	bool verify_sides(std::string const& raw_data) const;
    	std::string get_lhs_infix(std::string const& data) const;
    	std::string get_rhs_infix(std::string const& data) const;
    public:
    	pmEquation_parser() {}
    	virtual ~pmEquation_parser() override {}
    	std::shared_ptr<pmEquation> analyse_equation(std::string const& name, std::string const& equation_data, std::string const& condition_data, std::shared_ptr<pmWorkspace> workspace);
    };
}

#endif //_EQUATION_PARSER_H_