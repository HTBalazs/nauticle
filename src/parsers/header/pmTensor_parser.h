/*
    Copyright 2016-2017 Balazs Toth
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
    
#ifndef _TENSOR_PARSER_H_
#define _TENSOR_PARSER_H_

#include <string>
#include "prolog/pLogger.h"
#include "pmMath_parser.h"
#include "pmTensor.h"
#include "pmNoncopyable.h"
#include "pmWorkspace.h"


namespace Nauticle {
    /** This class analyses a tensor of arbitrary size given in std::string format 
    //	and returns a pmTensor object by value. The tensor must be constructed 
    // 	by the one-line sequence of elements where ";"-s are separating 
    //  elements in a row and "|"-s are separating rows.
    //  Example for a 3 by 3 matrix: "1;2;3|4;5;6|7;8;9".
    //  If the format or tensor size is infeasible the class returns and empty
    //  tensor (0 by 0). The member functions of the class never throw exception.
    */
    class pmTensor_parser final : public pmMath_parser, public pmNoncopyable {
    	int numrow;
    	int numcol;
    private:
    	bool verify_infix(std::string const&) const override;
    	std::vector<std::string> build_table(std::string const&) override;
    	pmTensor build_tensor(std::vector<std::string> const&, std::shared_ptr<pmWorkspace> const& workspace, int const& i=0) const;
    public:
    	pmTensor_parser() {}
    	virtual ~pmTensor_parser() override {}
    	pmTensor string_to_tensor(std::string const&, std::shared_ptr<pmWorkspace> const& workspace=std::make_shared<pmWorkspace>());
    	std::vector<pmTensor> string_to_tensor_field(std::string const&, std::shared_ptr<pmWorkspace> const& workspace);
    };
}

#endif //_TENSOR_PARSER_H_