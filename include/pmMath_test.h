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

    For more information please visit: https://bitbucket.org/lempsproject/
*/

#ifndef _MATH_TEST_H_
#define _MATH_TEST_H_

#include <string>

/** This class checks std::string formatted objects of mathematical operators.
*/
class pmMath_test {
protected:
	std::string const one_op_minus = "#";
	// list of functions and operators
	static std::string const list_of_functions[73];
	static std::string const list_of_operators[];
protected:
	virtual ~pmMath_test()=default;
	bool is_digit(char const& c) const;
	bool is_operator(std::string const& s) const;
	bool is_operator(char const& s) const;
	bool is_letter(char const& c) const;
	bool is_letter_or_digit(char const& c) const;
	bool is_number(std::string const& s) const;
	bool is_left_bracket(char const& c) const;
	bool is_right_bracket(char const& c) const;
	bool is_bracket(char const& c) const;
	bool is_left_bracket(std::string const& c) const;
	bool is_right_bracket(std::string const& c) const;
	bool is_bracket(std::string const& c) const;
	bool is_logical(char const& c) const;
	bool is_function(std::string const& s) const;
	bool is_word(std::string const& s) const;
	bool is_match(std::string const& s1, std::string const& s2) const;
};

#endif //_MATH_TEST_H_