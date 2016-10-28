/////////////////////////////////////////////////////////////////////////////
// Name:        pmParameter_space.h
// Purpose:     pmParameter_space class declaration
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <string>
#include "pmTensor.h"
#include "prolog/pLogger.h"

/** This class contains parameters for the calculations.
*/

class pmParameter_space final {
	/** This class holds the name and value of a parameter.
	*/
	class pmParameter {
	public:
		std::string name = "";
		pmTensor value = pmTensor{};
	public:
		pmParameter()=delete;
		pmParameter(std::string const& n, pmTensor const& v);
		pmParameter(pmParameter const&)=default;
		pmParameter(pmParameter&&)=default;
		pmParameter& operator=(pmParameter const&)=default;
		pmParameter& operator=(pmParameter&&)=default;
		void print() const;
	};
	std::vector<std::shared_ptr<pmParameter>> parameters;
	// std::shared_ptr<pmDomain> domain;
public:
	~pmParameter_space() {}
	pmParameter_space() {}
	pmParameter_space(pmParameter_space const& other);
	pmParameter_space(pmParameter_space&& other);
	pmParameter_space& operator=(pmParameter_space const& other);
	pmParameter_space& operator=(pmParameter_space&& other);
	void add_parameter(std::string const& n, pmTensor const& v);
	void delete_parameter(std::string const& n);
	pmTensor get_parameter_value(std::string const& n) const;
	void print() const;
};

#endif //_PARAMETERS_H_