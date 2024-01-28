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

#define Q(x) #x
#define QUOTE(x) Q(x)
#define ADD_INTERACTION(NUMBER_OF_OPERANDS, INTERACTION_NAME, INTERACTION_DECL)  												\
if(it==std::string{#INTERACTION_NAME}) { 																						\
	std::array<std::shared_ptr<pmExpression>,NUMBER_OF_OPERANDS> operands; 														\
	stack_extract(e, operands); 																								\
	auto interaction = std::make_shared<INTERACTION_NAME>(operands);							 								\
	interaction->set_declaration_type(INTERACTION_DECL);																		\
	e.push(interaction); 																										\
	workspace->add_interaction(interaction);																					\
}

#include <algorithm>
#include "pmExpression_parser.h"
#include "commonutils/Common.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Verifies the table of the infix.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmExpression_parser::verify_table(std::vector<std::string> table) const {
	for(int i=0; i<table.size()-1; i++) {
		if(is_function(table[i]) && table[i+1]!="(") {
			ProLog::pLogger::warning_msgf("Reserved word is used as variable or constant: \"%s\".\n", table[i].c_str());
			return false;
		}
		if(is_word(table[i]) && !is_function(table[i]) && table[i+1]=="(") {
			ProLog::pLogger::error_msgf("Unknown function: \"%s\".\n", table[i].c_str());
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
			ProLog::pLogger::warning_msgf("Invalid operators in \"%s\".\n", infix.c_str());
			return false;
		}
	}
	if(Common::find_word(infix,"(").size()!=Common::find_word(infix,")").size()) {
		ProLog::pLogger::warning_msgf("Unbalanced brackets in \"%s\".\n", infix.c_str());
		return false;
	}
	if(!Common::find_word(infix,"#").empty()) {
		ProLog::pLogger::warning_msgf("Illegal character \"#\".\n");
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
			while((is_digit(*it) || *it=='.' || *it=='e' || *it=='E') && it!=s.cend()) {
				if(*it=='e' || *it=='E') {
					it++;
					word+='e';
					if(*it=='+' || is_digit(*it)) {
						word+='+';
						if(*it=='+') it++;
					} else if(*it=='-') {
						word+='-';
						it++;
					}
				} else {
					word+=*it;
					it++;
				}
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
std::weak_ptr<pmSymbol> pmExpression_parser::assign(std::shared_ptr<pmWorkspace> workspace, std::string const& name) {
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
					ProLog::pLogger::error_msgf("Symbol \"%s\" is undefined and cannot be assigned.\n", it.c_str());
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
			} else if(it==":") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_operator<':',2>>(operands));
			} else if(it=="%") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_operator<'%',2>>(operands));
			}
		} else if(is_function(it)) {
			if(it=="cross") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<CROSS,2>>(operands));
			}
			using nbody = pmNbody_operator;
			ADD_INTERACTION(2, nbody, "pmNbody_operator")
			using dem_l = pmDem_operator<LINEAR,7>;
			ADD_INTERACTION(7, dem_l, "pmDem_operator<LINEAR,7>")
			using dem_a = pmDem_operator<ANGULAR,7>;
			ADD_INTERACTION(7, dem_a, "pmDem_operator<ANGULAR,7>")
			using sph_X = pmSph_operator<XSAMPLE,0,0,5>;
			ADD_INTERACTION(5, sph_X, "pmSph_operator<XSAMPLE,0,0,5>")
			using sph_S = pmSph_operator<SAMPLE,0,0,5>;
			ADD_INTERACTION(5, sph_S, "pmSph_operator<SAMPLE,0,0,5>")
			using sph_I = pmSph_operator<INERTIA,0,0,5>;
			ADD_INTERACTION(5, sph_I, "pmSph_operator<INERTIA,0,0,5>")
			using sph_G00 = pmSph_operator<GRADIENT,0,0,5>;
			ADD_INTERACTION(5, sph_G00, "pmSph_operator<GRADIENT,0,0,5>")
			using sph_G01 = pmSph_operator<GRADIENT,0,1,5>;
			ADD_INTERACTION(5, sph_G01, "pmSph_operator<GRADIENT,0,1,5>")
			using sph_G10 = pmSph_operator<GRADIENT,1,0,5>;
			ADD_INTERACTION(5, sph_G10, "pmSph_operator<GRADIENT,1,0,5>")
			using sph_G11 = pmSph_operator<GRADIENT,1,1,5>;
			ADD_INTERACTION(5, sph_G11, "pmSph_operator<GRADIENT,1,1,5>")
			using sph_G = pmSph_operator<GRADIENT,2,0,5>;
			ADD_INTERACTION(5, sph_G, "pmSph_operator<GRADIENT,2,0,5>")
			using sph_D00 = pmSph_operator<DIVERGENCE,0,0,5>;
			ADD_INTERACTION(5, sph_D00, "pmSph_operator<DIVERGENCE,0,0,5>")
			using sph_D01 = pmSph_operator<DIVERGENCE,0,1,5>;
			ADD_INTERACTION(5, sph_D01, "pmSph_operator<DIVERGENCE,0,1,5>")
			using sph_D10 = pmSph_operator<DIVERGENCE,1,0,5>;
			ADD_INTERACTION(5, sph_D10, "pmSph_operator<DIVERGENCE,1,0,5>")
			using sph_D11 = pmSph_operator<DIVERGENCE,1,1,5>;
			ADD_INTERACTION(5, sph_D11, "pmSph_operator<DIVERGENCE,1,1,5>")
			using sph_D = pmSph_operator<DIVERGENCE,2,0,5>;
			ADD_INTERACTION(5, sph_D, "pmSph_operator<DIVERGENCE,2,0,5>")
			using sph_L0 = pmSph_operator<LAPLACE,0,0,5>;
			ADD_INTERACTION(5, sph_L0, "pmSph_operator<LAPLACE,0,0,5>")
			using sph_L1 = pmSph_operator<LAPLACE,1,0,6>;
			ADD_INTERACTION(6, sph_L1, "pmSph_operator<LAPLACE,1,0,6>")
			using sph_L2 = pmSph_operator<LAPLACE,2,0,5>;
			ADD_INTERACTION(5, sph_L2, "pmSph_operator<LAPLACE,2,0,5>")
			using sph_T = pmSph_operator<TENSILE,1,1,6>;
			ADD_INTERACTION(6, sph_T, "pmSph_operator<TENSILE,1,1,6>")
			using sph_A = pmSph_operator<AVISC,0,0,5>;
			ADD_INTERACTION(5, sph_A, "pmSph_operator<AVISC,0,0,5>")
			using sph_A0 = pmSph_operator<AVISC,0,0,5>;
			ADD_INTERACTION(5, sph_A0, "pmSph_operator<AVISC,0,0,5>")
			using sph_A1 = pmSph_operator<AVISC,1,0,6>;
			ADD_INTERACTION(6, sph_A1, "pmSph_operator<AVISC,1,0,6>")
			using dvm = pmDvm_operator;
			ADD_INTERACTION(2, dvm, "pmDvm_operator")
			using sfm = pmSfm_operator;
			ADD_INTERACTION(10, sfm, "pmSfm_operator")
			using md = pmMd_operator;
			ADD_INTERACTION(3, md, "pmMd_operator")
			using neighbors = pmNeighbors;
			ADD_INTERACTION(3, neighbors, "pmNeighbors")
			using occlusion = pmOcclusion;
			ADD_INTERACTION(3, occlusion, "pmOcclusion")
			using kuramoto = pmKuramoto_operator;
			ADD_INTERACTION(3, kuramoto, "pmKuramoto_operator")
			using collision_handler = pmCollision_handler;
			ADD_INTERACTION(5, collision_handler, "pmCollision_handler")
			using spring = pmSpring;
			ADD_INTERACTION(1, spring, "pmSpring")
			if(it=="fmin") {
				std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				auto interaction = std::make_shared<pmFmin>(operand);
				interaction->set_declaration_type("pmFmin");
				workspace->add_interaction(interaction);
				e.push(interaction);
			}
			if(it=="fmax") {
				std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				auto interaction = std::make_shared<pmFmax>(operand);
				interaction->set_declaration_type("pmFmax");
				workspace->add_interaction(interaction);
				e.push(interaction);
			}
			if(it=="fmean") {
				std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				auto interaction = std::make_shared<pmFmean>(operand);
				interaction->set_declaration_type("pmFmean");
				workspace->add_interaction(interaction);
				e.push(interaction);
			}
			if(it=="fsum") {
				std::shared_ptr<pmExpression> operand = e.top(); e.pop();
				auto interaction = std::make_shared<pmFsum>(operand);
				interaction->set_declaration_type("pmFsum");
				workspace->add_interaction(interaction);
				e.push(interaction);
			}
			if(it=="transpose") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<TRANSPOSE,1>>(operands));
			}
			if(it=="trace") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<TRACE,1>>(operands));
			}
			if(it=="deQ") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<DEQ,1>>(operands));
			}
			if(it=="deR") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<DER,1>>(operands));
			}
			if(it=="eigsys") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<EIGSYS,1>>(operands));
			}
			if(it=="eigval") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<EIGVAL,1>>(operands));
			}
			if(it=="sin") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<SIN,1>>(operands));
			}
			if(it=="cos") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<COS,1>>(operands));
			}
			if(it=="tan") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<TAN,1>>(operands));
			}
			if(it=="cot") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<COT,1>>(operands));
			}
			if(it=="asin") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ASIN,1>>(operands));
			}
			if(it=="acos") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ACOS,1>>(operands));
			}
			if(it=="atan") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ATAN,1>>(operands));
			}
			if(it=="atan2") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ATAN2,2>>(operands));
			}
			if(it=="acot") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ACOT,1>>(operands));
			}
			if(it=="exp") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<EXP,1>>(operands));
			}
			if(it=="log") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<LOG,1>>(operands));
			}
			if(it=="logm") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<LOGM,1>>(operands));
			}
			if(it=="sinh") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<SINH,1>>(operands));
			}
			if(it=="cosh") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<COSH,1>>(operands));
			}
			if(it=="tanh") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<TANH,1>>(operands));
			}
			if(it=="coth") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<COTH,1>>(operands));
			}
			if(it=="sqrt") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<SQRT,1>>(operands));
			}
			if(it=="sgn") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<SGN,1>>(operands));
			}
			if(it=="abs") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ABS,1>>(operands));
			}
			if(it=="floor") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<FLOOR,1>>(operands));
			}
			if(it=="trunc") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<TRUNC,1>>(operands));
			}
			if(it=="lt") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<LT,2>>(operands));
			}
			if(it=="gt") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<GT,2>>(operands));
			}
			if(it=="lte") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<LTE,2>>(operands));
			}
			if(it=="gte") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<GTE,2>>(operands));
			}
			if(it=="eq") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<EQUAL,2>>(operands));
			}
			if(it=="neq") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<NOTEQUAL,2>>(operands));
			}
			if(it=="min") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<MIN,2>>(operands));
			}
			if(it=="max") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<MAX,2>>(operands));
			}
			if(it=="mod") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<MOD,2>>(operands));
			}
			if(it=="euler") {
				std::array<std::shared_ptr<pmExpression>,3> operands;
				stack_extract(e, operands);
				operands[0]->set_storage_depth(1);
				operands[1]->set_storage_depth(1);
				e.push(std::make_shared<pmArithmetic_function<EULER,3>>(operands));
			}
			if(it=="predictor") {
				std::array<std::shared_ptr<pmExpression>,3> operands;
				stack_extract(e, operands);
				operands[0]->set_storage_depth(2);
				operands[1]->set_storage_depth(2);
				operands[2]->set_storage_depth(2);
				e.push(std::make_shared<pmArithmetic_function<PREDICTOR,3>>(operands));
			}
			if(it=="corrector") {
				std::array<std::shared_ptr<pmExpression>,3> operands;
				stack_extract(e, operands);
				operands[0]->set_storage_depth(2);
				operands[1]->set_storage_depth(2);
				operands[2]->set_storage_depth(2);
				e.push(std::make_shared<pmArithmetic_function<CORRECTOR,3>>(operands));
			}
			if(it=="verlet_r") {
				std::array<std::shared_ptr<pmExpression>,4> operands;
				stack_extract(e, operands);
				operands[2]->set_storage_depth(2);
				e.push(std::make_shared<pmArithmetic_function<VERLET_R,4>>(operands));
			}
			if(it=="verlet_v") {
				std::array<std::shared_ptr<pmExpression>,3> operands;
				stack_extract(e, operands);
				operands[1]->set_storage_depth(2);
				e.push(std::make_shared<pmArithmetic_function<VERLET_V,3>>(operands));
			}
			if(it=="limit") {
				std::array<std::shared_ptr<pmExpression>,3> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<LIMIT,3>>(operands));
			}
			if(it=="not") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<NOT,1>>(operands));
			}
			if(it=="magnitude") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<MAGNITUDE,1>>(operands));
			}
			if(it=="rand") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<URAND,2>>(operands));
			}
			if(it=="urand") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<URAND,2>>(operands));
			}
			if(it=="nrand") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<NRAND,2>>(operands));
			}
			if(it=="lnrand") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<LNRAND,2>>(operands));
			}
			if(it=="and") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<AND,2>>(operands));
			}
			if(it=="or") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<OR,2>>(operands));
			}
			if(it=="xor") {
				std::array<std::shared_ptr<pmExpression>,2> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<XOR,2>>(operands));
			}
			if(it=="if") {
				std::array<std::shared_ptr<pmExpression>,3> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<IF,3>>(operands));
			}
			if(it=="elem") {
				std::array<std::shared_ptr<pmExpression>,3> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<ELEM,3>>(operands));
			}
			if(it=="identity") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<IDENTITY,1>>(operands));
			}
			if(it=="determinant") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<DETERMINANT,1>>(operands));
			}
			if(it=="inverse") {
				std::array<std::shared_ptr<pmExpression>,1> operands;
				stack_extract(e, operands);
				e.push(std::make_shared<pmArithmetic_function<INVERSE,1>>(operands));
			}
		} else if(is_number(it)) {
			e.push(std::make_shared<pmConstant>(pmTensor{stof(it)}));
		}
	}
	return e.top();
}