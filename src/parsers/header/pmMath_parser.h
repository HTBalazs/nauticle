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
    
#ifndef _PM_MATH_PARSER_H_
#define _PM_MATH_PARSER_H_

#include <string>
#include <vector>
#include "prolog/pLogger.h"
#include "pmMath_test.h"

namespace Nauticle {
    /** This interface realizes the most important functions for the mathematical
    //  parsing classes.
    */
    class pmMath_parser : public pmMath_test {
    public:
    	pmMath_parser() {}
    protected:
    	virtual ~pmMath_parser() override {}
    	virtual bool verify_infix(std::string const& infix) const=0;
    	void remove_spaces(std::string& str) const;
    	virtual std::vector<std::string> build_table(std::string const&)=0;
    };
}

#endif //_PM_MATH_PARSER_H_