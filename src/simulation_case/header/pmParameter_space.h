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
    
#ifndef _PM_PARAMETERS_H_
#define _PM_PARAMETERS_H_

#include "pmTensor.h"
#include "pmExpression.h"
#include "prolog/pLogger.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <string>

namespace Nauticle {
	/** This class contains parameters for the calculations.
	*/
	class pmParameter_space final {
		/** This class holds the name and value of a parameter.
		*/
		class pmParameter {
			std::string name = "";
			std::shared_ptr<pmExpression> value;
		public:
			pmParameter()=delete;
			pmParameter(std::string const& n, std::shared_ptr<pmExpression> const& expr);
			std::string get_name() const;
			pmTensor get_value() const;
			void print() const;
		};
		std::vector<std::shared_ptr<pmParameter>> parameters;
	public:
		~pmParameter_space() {}
		pmParameter_space() {}
		pmParameter_space(pmParameter_space const& other);
		pmParameter_space(pmParameter_space&& other);
		pmParameter_space& operator=(pmParameter_space const& other);
		pmParameter_space& operator=(pmParameter_space&& other);
		void add_parameter(std::string const& n, std::shared_ptr<pmExpression> const& expr);
		void delete_parameter(std::string const& n);
		pmTensor get_parameter_value(std::string const& n) const;
		void print() const;
	};
}

#endif //_PM_PARAMETERS_H_