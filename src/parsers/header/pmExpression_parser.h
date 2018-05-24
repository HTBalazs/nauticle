/*
    Copyright 2016-2018 Balazs Toth
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
    
#ifndef _EXPRESSION_PARSER_H_
#define _EXPRESSION_PARSER_H_

#include <iostream>
#include <stack>
#include <string>
#include <memory>
#include "prolog/pLogger.h"
#include "pmMath_parser.h"
#include "pmEquation.h"
#include "pmNoncopyable.h"
#include "pmConstant.h"
#include "pmVariable.h"
#include "pmField.h"
#include "pmNbody_operator.h"
#include "pmDem_operator.h"
#include "pmSph_operator.h"
#include "pmDvm_operator.h"
#include "pmSfm_operator.h"
#include "pmDpd_operator.h"
#include "pmMd_operator.h"
#include "pmFmin.h"
#include "pmFmax.h"
#include "pmFmean.h"
#include "pmFsum.h"
#include "pmNeighbors.h"
#include "pmArithmetic_operator.h"
#include "pmArithmetic_function.h"

namespace Nauticle {
	/** This class converts the given std::string formatted equation to an pmExpression object.
	*/
	class pmExpression_parser : public pmMath_parser, public pmNoncopyable {
	private:
		bool verify_table(std::vector<std::string> table) const;
		int get_precedence(std::string const& s) const;
		int get_associativity(std::string const& s) const;
		std::vector<std::string> build_table(std::string const& s) override;
		std::vector<std::string> parse(std::vector<std::string> const& words);
		std::weak_ptr<pmSymbol> assign(std::shared_ptr<pmWorkspace> workspace, std::string const& name);
		std::shared_ptr<pmExpression> build_expression_tree(std::vector<std::string> const& postfix, std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>());
		bool verify_infix(std::string const& infix) const override;
		template <size_t S> void stack_extract(std::stack<std::shared_ptr<pmExpression>>& e, std::array<std::shared_ptr<pmExpression>,S>& ops, size_t count=S);
	public:
		pmExpression_parser() {}
		virtual ~pmExpression_parser() override {}
		template <typename T> std::shared_ptr<T> analyse_expression(std::string infix, std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>());
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Analyses the equation given by the infix.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T> std::shared_ptr<T> pmExpression_parser::analyse_expression(std::string infix, std::shared_ptr<pmWorkspace> workspace/*=std::make_shared<pmWorkspace>()*/) {
		remove_spaces(infix);
		if(!verify_infix(infix)) {
			return std::shared_ptr<T>(nullptr);
		}
		std::vector<std::string> infix_table = build_table(infix);
		if(!verify_table(infix_table)) {
			return std::shared_ptr<T>(nullptr);
		}
		std::vector<std::string> postfix = parse(infix_table);
		std::shared_ptr<pmExpression> expression;
		try {
			expression = build_expression_tree(postfix, workspace);
		} catch(...) {
			return std::shared_ptr<T>(nullptr);
		}
		return std::dynamic_pointer_cast<T>(expression);
	}
}

#endif //_EXPRESSION_PARSER_H_