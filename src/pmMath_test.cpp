/*
    Copyright 2017 Balazs Toth
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

    For more information please visit: https://BalazsToth@bitbucket.org/BalazsToth/lemps
*/

#include "pmMath_test.h"

std::string const pmMath_test::list_of_functions[73] = {"abs", "acos", "acot", "and", "asin", "atan", "cos", "cosh", "cot", "coth", "dem", "div", "elem", "exp", "floor", "fmax", "fmean", "fmin", "grad", "gt", "if", "log", "lt", "magnitude", "max", "min", "mod", "not", "or", "rand", "sgn", "sin", "sinh", "sph_D00", "sph_D01", "sph_D10", "sph_D11", "sph_G00", "sph_G01", "sph_G10", "sph_G11", "sph_L00", "sph_S00", "sph_X00", "sqrt", "tan", "tanh", "trace", "transpose", "trunc", "xor", "identity", "neighbours", "simulation", "workspace", "function_space", "variables", "constants", "fields", "particle_system", "parameter_space", "domain", "cell_size", "grid", "functions", "nbody", "cross", "inverse", "determinant", "eq", "euler", "predictor", "corrector"};
std::string const pmMath_test::list_of_operators[] = {	"+", "-", "*", "/", "^", "#", ":"};

/////////////////////////////////////////////////////////////////////////////////////////
/// Check if a character is digit.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_digit(char const& c) const {
	return (c>='0' && c<='9');
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given std::string object is an operator.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_operator(std::string const& s) const {
	for(auto const& it : list_of_operators) {
		if(is_match(s, it)) {
			return true;
		}
	}
	return false;
}
bool pmMath_test::is_operator(char const& s) const {
	for(auto const& it : list_of_operators) {
		if(s==it[0]) {
			return true;
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given character is a letter.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_letter(char const& c) const {
	return (c>='a' && c<='z') || ((c>='A' && c<='Z') || c=='_');
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given character is a letter or digit.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_letter_or_digit(char const& c) const {
	return is_digit(c) || is_letter(c);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given std::string object is a number.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_number(std::string const& s) const {
	for(auto const& it : s) {
		if(!is_digit(it) && it!='.' && it!='e') {
			return false;
		}
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given std::string object is beginning with a left bracket character.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_left_bracket(char const& c) const {
	return c=='(';
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given std::string object is beginning with a right bracket character.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_right_bracket(char const& c) const {
	return c==')';
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given std::string object is beginning with a bracket character.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_bracket(char const& c) const {
	return is_left_bracket(c) || is_right_bracket(c);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given std::string object is beginning with a left bracket character.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_left_bracket(std::string const& s) const {
	return *s.begin()=='(';
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given std::string object is beginning with a right bracket character.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_right_bracket(std::string const& s) const {
	return *s.begin()==')';
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given std::string object is beginning with a bracket character.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_bracket(std::string const& s) const {
	return is_left_bracket(s) || is_right_bracket(s);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given std::string object is a logical.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_logical(char const& c) const {
	return c=='<' || c=='>';
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given std::string object is a function.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_function(std::string const& s) const {
	for(auto const& it : list_of_functions) {
		if(is_match(s, it)) {
			return true;
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given std::string object is a word (not operator and not number).
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_word(std::string const& s) const {
	if(is_digit(*s.begin()) || is_function(s)) {
		return false;
	}
	for(auto const& it : s) {
		if(!is_letter(it) && !is_digit(it)) {
			return false;
		}
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks the match of the given std::string objects.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmMath_test::is_match(std::string const& s1, std::string const& s2) const {
	if(s1.size() == s2.size()) {
		for(std::size_t i = 0; i < s1.size(); ++i) {
			if(std::tolower(s1[i]) != std::tolower(s2[i])) {
				return false;
			}
		}
		return true;
	}
	return false;
}