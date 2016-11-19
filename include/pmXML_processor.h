/*
    Copyright 2016 Balazs Toth
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

#ifndef _XML_PROCESSOR_H_
#define _XML_PROCESSOR_H_

#include <string>
#include <vector>
#include <memory>
#include "handyxml/hdyXML_parser.h"
#include "pmWorkspace.h"
#include "pmExpression_parser.h"
#include "pmFunction_parser.h"
#include "pmTensor_parser.h"
#include "pmFunction.h"
#include "pmFunction_space.h"
#include "pmGrid_space.h"
#include "pmParameter_space.h"
#include "pmVTK_reader.h"
#include "pmVTK_writer.h"
#include "prolog/pLogger.h"

/**	This class extends the hdyXML_parser class with further functions to process
//	the XML tree.
*/
class pmXML_processor final : public hdyXML_parser {
	std::shared_ptr<pmWorkspace> get_workspace() const;
	std::vector<std::shared_ptr<pmFunction>> get_functions(std::shared_ptr<pmWorkspace> workspace) const;
	std::shared_ptr<pmFunction_space> get_initial_condition_function_space() const;
public:
	virtual ~pmXML_processor() override {}
	std::shared_ptr<pmGrid_space> get_grid_space(std::shared_ptr<pmBlock> particle_system, std::shared_ptr<pmWorkspace> workspace) const;
	std::shared_ptr<pmFunction_space> get_function_space() const;
	std::shared_ptr<pmParameter_space> get_parameter_space(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
	std::shared_ptr<pmWorkspace> get_initial_condition() const;
};

#endif //_XML_PROCESSOR_H_