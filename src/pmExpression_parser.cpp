/////////////////////////////////////////////////////////////////////////////
// Name:        pmExpression_parser.cpp
// Purpose:     pmExpression_parser class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include "pmExpression_parser.h"
#include "commonutils/Common.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Verifies the table of the infix.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmExpression_parser::verify_table(std::vector<std::string> table) const {
	for(int i=0; i<table.size()-1; i++) {
		if(is_function(table[i]) && table[i+1]!="(") {
			pLogger::warning_msgf("Reserved word is used as variable or constant: \"%s\".\n", table[i].c_str());
			return false;
		}
		if(is_word(table[i]) && !is_function(table[i]) && table[i+1]=="(") {
			pLogger::warning_msgf("Unknown function: \"%s\".\n", table[i].c_str());
			return false;
		}
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Verifies the infix format.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmExpression_parser::verify_infix(std::string const& infix) const {
	for(int i=0; i<infix.size()-1;i++) {
		if(is_operator(infix[i]) && is_operator(infix[i+1])) {
			pLogger::warning_msgf("Invalid operators in \"%s\".\n", infix.c_str());
			return false;
		}
	}
	if(Common::find_word(infix,"(").size()!=Common::find_word(infix,")").size()) {
		pLogger::warning_msgf("Unbalanced brackets in \"%s\".\n", infix.c_str());
		return false;
	}
	if(!Common::find_word(infix,"#").empty()) {
		pLogger::warning_msgf("Illegal character \"#\".\n");
		return false;	
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the precedence of the given operator.
/////////////////////////////////////////////////////////////////////////////////////////
int pmExpression_parser::get_precedence(std::string const& s) const {
	if(s.empty()) return 0;
	char const op = *s.begin();
	switch(op) {
		case '+' : return 2;
		case '-' : return 2;
		case '#' : return 2;
		case '*' : return 3;
		case ':' : return 3;
		case '/' : return 3;
		case '^' : return 4;
		default : return 0;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the associativity of the given operator.
/////////////////////////////////////////////////////////////////////////////////////////
int pmExpression_parser::get_associativity(std::string const& s) const {
	if(s.empty()) return 0;
	char const op = *s.begin();
	switch(op) {
		case '^' : return 1;
		default : return 0;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Constructs a table of the infix std::string object.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> pmExpression_parser::build_table(std::string const& s) {
	std::string::const_iterator it = s.cbegin();
	std::vector<std::string> words;
	while(it!=s.cend()) {
		if(is_digit(*it)) {
			std::string word;
			while((is_digit(*it) || *it=='.') && it!=s.cend()) {
				word+=*it;
				it++;
			}
			words.push_back(word);
		}
		if(is_letter(*it)) {
			std::string word;
			while((is_letter(*it) || is_digit(*it)) && it!=s.cend()) {
				word+=*it;
				it++;
			}
			words.push_back(word);
		}
		if(it!=s.cend()) {
			if(*it=='-' && (*(it-1)=='(' || *(it-1)==',')) {
				words.push_back(std::string{'#'});
			} else {
				words.push_back(std::string{*it});
			}
			it++;
		}
	}
	return words;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Parsing.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> pmExpression_parser::parse(std::vector<std::string> const& words) {
	std::stack<std::string> operators;
	std::string result;
	for(auto const& it : words) {
		if(is_operator(it)) {
			if((operators.empty() || get_precedence(operators.top()) < get_precedence(it)) || (get_associativity(it) == 1 && get_associativity(operators.top())==1)) {
				operators.push(it);
			} else {
				while(get_precedence(operators.top()) >= get_precedence(it)) {
					result += operators.top();
					operators.pop();
					if(operators.empty()) break;
				}
				operators.push(it);
			}
		} else if(is_bracket(it) || it==",") {
			if(is_left_bracket(it)) {
				operators.push(it);
			} else {
				while(!is_left_bracket(operators.top())) {
					result += operators.top();
					operators.pop();
				}
				if(it!=",") {
					operators.pop();
				}
				if(!operators.empty()) {
					if(is_function(operators.top())) {
						result += operators.top();
						operators.pop();
					}
				}
			}
		} else if(is_function(it)) {
			operators.push(it);
		} else {
			result += it;
		}
		if(is_letter_or_digit(result.back())) {
			result += ' ';
		}
	}
	while(!operators.empty()) {
		result += operators.top();
		operators.pop();
	}
	return build_table(result);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Assigns a found variable or constant to a workspace instance.
/////////////////////////////////////////////////////////////////////////////////////////
std::weak_ptr<pmTerm> pmExpression_parser::assign(std::shared_ptr<pmWorkspace> workspace, std::string const& name) {
	return workspace->get_instance(name);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Orders the top "count" elements of the stack into array. The order is first out->last in array.
/// All "count" elements are popped from the stack.
/////////////////////////////////////////////////////////////////////////////////////////
template <size_t S>
void pmExpression_parser::stack_extract(std::stack<std::shared_ptr<pmExpression>>& e, std::array<std::shared_ptr<pmExpression>,S>& ops, size_t count/*=S*/) {
	count--;
	ops[count] = e.top(); e.pop();
	if(!count) {
		return;
	} else {
		stack_extract<S>(e, ops, count);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Constructs the expression tree.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmExpression_parser::build_expression_tree(std::vector<std::string> const& postfix, std::shared_ptr<pmWorkspace> workspace/*=std::make_shared<pmWorkspace>()*/) {
	std::stack<std::shared_ptr<pmExpression>> e;
	for(auto const& it : postfix) {
		if(is_word(it)) {
			if(workspace.use_count()>0) {
				std::weak_ptr<pmExpression> instance = assign(workspace, it);
				if(instance.expired()) {
					pLogger::error_msgf("Variable \"%s\" is undefined and cannot be assigned.\n", it.c_str());
					throw(false);
				}
				e.push(instance.lock());
			}
		} else if(is_operator(it)) {
			if(it=="+" && e.size()>1) {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_operator<'+',2>>(operands));
			} else if(it=="+" && e.size()==1) {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_operator<'+',1>>(operands));
			} else if(it=="-" && e.size()>1) {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_operator<'-',2>>(operands));
			} else if((it=="-" && e.size()==1) || it=="#") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_operator<'-',1>>(operands));
			} else if(it=="*") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_operator<'*',2>>(operands));
			} else if(it=="/") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_operator<'/',2>>(operands));
			} else if(it=="^") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_operator<'^',2>>(operands));
			}  else if(it==":") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_operator<':',2>>(operands));
			}
		} else if(is_function(it)) {
			if(it=="nbody") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmNbody>(operands));
			}
			if(it=="dem") {
				std::array<std::shared_ptr<pmExpression>,6> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmDem>(operands));
			}
			if(it=="sph_X00") {
				std::array<std::shared_ptr<pmExpression>,4> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmSph_operator<XSAMPLE,0,0>>(operands));
			}
			if(it=="sph_S00") {
				std::array<std::shared_ptr<pmExpression>,4> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmSph_operator<SAMPLE,0,0>>(operands));
			}
			if(it=="sph_G00") {
				std::array<std::shared_ptr<pmExpression>,4> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmSph_operator<GRADIENT,0,0>>(operands));
			}
			if(it=="sph_G01") {
				std::array<std::shared_ptr<pmExpression>,4> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmSph_operator<GRADIENT,0,1>>(operands));
			}
			if(it=="sph_G10") {
				std::array<std::shared_ptr<pmExpression>,4> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmSph_operator<GRADIENT,1,0>>(operands));
			}
			if(it=="sph_G11") {
				std::array<std::shared_ptr<pmExpression>,4> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmSph_operator<GRADIENT,1,1>>(operands));
			}
			if(it=="sph_D00") {
				std::array<std::shared_ptr<pmExpression>,4> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmSph_operator<DIVERGENCE,0,0>>(operands));
			}
			if(it=="sph_D01") {
				std::array<std::shared_ptr<pmExpression>,4> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmSph_operator<DIVERGENCE,0,1>>(operands));
			}
			if(it=="sph_D10") {
				std::array<std::shared_ptr<pmExpression>,4> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmSph_operator<DIVERGENCE,1,0>>(operands));
			}
			if(it=="sph_D11") {
				std::array<std::shared_ptr<pmExpression>,4> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmSph_operator<DIVERGENCE,1,1>>(operands));
			}
			if(it=="sph_L00") {
				std::array<std::shared_ptr<pmExpression>,4> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmSph_operator<LAPLACE,0,0>>(operands));
			}
			if(it=="transpose") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmTranspose>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<TRANSPOSE,1>>(operands));
			}
			if(it=="trace") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmTrace>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<TRACE,1>>(operands));
			}
			if(it=="sin") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmSin>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<SIN,1>>(operands));
			}
			if(it=="cos") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmCos>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<COS,1>>(operands));
			}
			if(it=="tan") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmTan>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<TAN,1>>(operands));
			}
			if(it=="cot") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmCot>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<COT,1>>(operands));
			}
			if(it=="asin") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmAsin>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ASIN,1>>(operands));
			}
			if(it=="acos") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmAcos>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ACOS,1>>(operands));
			}
			if(it=="atan") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmAtan>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ATAN,1>>(operands));
			}
			if(it=="acot") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmAcot>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ACOT,1>>(operands));
			}
			if(it=="exp") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmExp>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<EXP,1>>(operands));
			}
			if(it=="log") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmLog>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<LOG,1>>(operands));
			}
			if(it=="sinh") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmSinh>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<SINH,1>>(operands));
			}
			if(it=="cosh") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmCosh>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<COSH,1>>(operands));
			}
			if(it=="tanh") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmTanh>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<TANH,1>>(operands));
			}
			if(it=="coth") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmCoth>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<COTH,1>>(operands));
			}
			if(it=="sqrt") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmSqrt>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<SQRT,1>>(operands));
			}
			if(it=="sgn") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmSgn>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<SGN,1>>(operands));
			}
			if(it=="abs") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmAbs>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ABS,1>>(operands));
			}
			if(it=="floor") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmFloor>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<FLOOR,1>>(operands));
			}
			if(it=="trunc") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmTrunc>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<TRUNC,1>>(operands));
			}
			if(it=="lt") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// e.push(std::make_shared<pmLess>(o2,o1));
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<LT,2>>(operands));
			}
			if(it=="gt") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// e.push(std::make_shared<pmGreater>(o2,o1));
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<GT,2>>(operands));
			}
			if(it=="min") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// e.push(std::make_shared<pmMin>(o2,o1));
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<MIN,2>>(operands));
			}
			if(it=="max") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// e.push(std::make_shared<pmMax>(o2,o1));
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<MAX,2>>(operands));
			}
			if(it=="mod") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// e.push(std::make_shared<pmMod>(o2,o1));
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<MOD,2>>(operands));
			}
			if(it=="int1") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// o1->double_steps(false);
				// o2->double_steps(false);
				// e.push(std::make_shared<pmInt1>(o2,o1));
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				operands[0]->double_steps(false);
				operands[1]->double_steps(false);
				e.push(std::make_shared<pmArithmetic_function<INT1,2>>(operands));
			}
			if(it=="int2") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// o1->double_steps(true);
				// o2->double_steps(true);
				// e.push(std::make_shared<pmInt2>(o2,o1));
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				operands[0]->double_steps(true);
				operands[1]->double_steps(true);
				e.push(std::make_shared<pmArithmetic_function<INT2,2>>(operands));
			}
			if(it=="fmin") {
				std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				e.push(std::make_shared<pmFmin>(operand));
			}
			if(it=="fmax") {
				std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				e.push(std::make_shared<pmFmax>(operand));
			}
			if(it=="fmean") {
				std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				e.push(std::make_shared<pmFmean>(operand));
			}
			if(it=="not") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmNot>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<NOT,1>>(operands));
			}
			if(it=="magnitude") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmMagnitude>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<MAGNITUDE,1>>(operands));
			}
			if(it=="rand") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// e.push(std::make_shared<pmRand>(o2,o1));
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<RAND,2>>(operands));
			}
			if(it=="and") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// e.push(std::make_shared<pmAnd>(o2,o1));
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<AND,2>>(operands));
			}
			if(it=="or") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// e.push(std::make_shared<pmOr>(o2,o1));
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<OR,2>>(operands));
			}
			if(it=="xor") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// e.push(std::make_shared<pmXor>(o2,o1));
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<XOR,2>>(operands));
			}
			if(it=="if") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o3 = e.top(); e.pop();
				// e.push(std::make_shared<pmIf>(o3,o2,o1));
				std::array<std::shared_ptr<pmExpression>,3> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<IF,3>>(operands));
			}
			if(it=="elem") {
				// std::shared_ptr<pmExpression> o1 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o2 = e.top(); e.pop();
				// std::shared_ptr<pmExpression> o3 = e.top(); e.pop();
				// e.push(std::make_shared<pmElem>(o3,o2,o1));
				std::array<std::shared_ptr<pmExpression>,3> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ELEM,3>>(operands));
			}
			if(it=="identity") {
				// std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				// e.push(std::make_shared<pmIdentity>(operand));
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<IDENTITY,1>>(operands));

			}
			if(it=="neighbours") {
				e.push(std::make_shared<pmNeighbours>());
			}
		} else if(is_number(it)) {
			e.push(std::make_shared<pmConstant>(pmTensor{stof(it)}));
		}
	}
	return e.top();
}