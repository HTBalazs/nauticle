/////////////////////////////////////////////////////////////////////////////
// Name:        pmTensor_parser.h
// Purpose:     pmTensor_parser class declaration
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _TENSOR_PARSER_H_
#define _TENSOR_PARSER_H_

#include <string>
#include "prolog/pLogger.h"
#include "pmMath_parser.h"
#include "pmExpression_parser.h"
#include "pmTensor.h"
#include "pmNoncopyable.h"


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

#endif //_TENSOR_PARSER_H_