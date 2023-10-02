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
    
#ifndef _PM_OUTPUT_H_
#define _PM_OUTPUT_H_

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "prolog/pLogger.h"
#include "pmExpression.h"

namespace Nauticle {
	/** This class represents the background function, which can be used to 
	//  interpolate data to the particle system.
	*/
	class pmOutput {
	protected:
		std::string file_name;
		std::vector<std::shared_ptr<pmExpression>> data;
		std::shared_ptr<pmExpression> current_time;
		std::shared_ptr<pmExpression> condition;
	public:
		void print() const;
		void set_file_name(std::string const& fn);
		void add_data(std::shared_ptr<pmExpression> expr);
		void add_time(std::shared_ptr<pmExpression> tm);
		void set_condition(std::shared_ptr<pmExpression> cond);
		void update();
	};
}

#endif //_PM_OUTPUT_H_