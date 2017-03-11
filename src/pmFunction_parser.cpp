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
    
#include "pmFunction_parser.h"
#include "commonutils/Common.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the data is an equation.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmFunction_parser::verify_sides(std::string const& infix) const {
	if(Common::find_word(infix,"=").size()!=1) {
		pLogger::warning_msgf("\"%s\" is not a valid equation.\n", infix.c_str());
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the lhs of the infix equation.
/////////////////////////////////////////////////////////////////////////////////////////
std::string pmFunction_parser::get_lhs_infix(std::string const& data) const {
	int pos = data.find("=");
	return data.substr(0, pos);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the rhs of the infix equation.
/////////////////////////////////////////////////////////////////////////////////////////
std::string pmFunction_parser::get_rhs_infix(std::string const& data) const {
	int pos = data.find("=");
	return data.substr(pos+1);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Analyses the equation given by the infix.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmFunction> pmFunction_parser::analyse_function(std::string const& name, std::string const& infix, std::shared_ptr<pmWorkspace> workspace) {
	if(!verify_sides(infix)) return std::shared_ptr<pmFunction>(nullptr);
	std::string lhs_infix = get_lhs_infix(infix);
	std::string rhs_infix = get_rhs_infix(infix);
	std::shared_ptr<pmTerm> lhs = analyse_expression<pmTerm>(lhs_infix, workspace);
	if(!lhs) { 
		pLogger::warning_msgf("\"%s\" is not a function and ignored.\n", infix.c_str());
		return std::shared_ptr<pmFunction>(nullptr);
	}
	std::shared_ptr<pmExpression> rhs = analyse_expression<pmExpression>(rhs_infix, workspace);
	return std::make_shared<pmFunction>(name, lhs, rhs);
}