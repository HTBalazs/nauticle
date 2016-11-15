/////////////////////////////////////////////////////////////////////////////
// Name:        pmMath_test.h
// Purpose:     pmMath_test class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _MATH_TEST_H_
#define _MATH_TEST_H_

#include <string>

class pmMath_test {
protected:
	// list of functions and operators
	static std::string const list_of_functions[69];
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