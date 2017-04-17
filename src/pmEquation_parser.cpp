/*
    Copyright 2016-2017 Balazs Toth
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
    
#include "pmEquation_parser.h"
#include "commonutils/Common.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the data is an equation.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmEquation_parser::verify_sides(std::string const& infix) const {
	if(Common::find_word(infix,"=").size()!=1) {
		pLogger::warning_msgf("\"%s\" is not a valid equation.\n", infix.c_str());
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the lhs of the infix equation.
/////////////////////////////////////////////////////////////////////////////////////////
std::string pmEquation_parser::get_lhs_infix(std::string const& data) const {
	int pos = data.find("=");
	return data.substr(0, pos);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the rhs of the infix equation.
/////////////////////////////////////////////////////////////////////////////////////////
std::string pmEquation_parser::get_rhs_infix(std::string const& data) const {
	int pos = data.find("=");
	return data.substr(pos+1);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Analyses the equation given by the infix.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmEquation> pmEquation_parser::analyse_equation(std::string const& name, std::string const& equation_data, std::string const& condition_data, std::shared_ptr<pmWorkspace> workspace) {
	if(!verify_sides(equation_data)) return std::shared_ptr<pmEquation>(nullptr);
	std::string lhs_infix = get_lhs_infix(equation_data);
	std::string rhs_infix = get_rhs_infix(equation_data);
	std::shared_ptr<pmTerm> lhs = analyse_expression<pmTerm>(lhs_infix, workspace);
	if(!lhs) { 
		pLogger::warning_msgf("\"%s\" is not a function and ignored.\n", equation_data.c_str());
		return std::shared_ptr<pmEquation>(nullptr);
	}
	std::shared_ptr<pmExpression> rhs = analyse_expression<pmExpression>(rhs_infix, workspace);
	std::shared_ptr<pmExpression> condition;
	if(condition_data.empty()) {
		condition = std::make_shared<pmConstant>(pmTensor{1});
	} else {
		condition = analyse_expression<pmExpression>(condition_data, workspace);
	}
	return std::make_shared<pmEquation>(name, lhs, rhs, condition);
}

























