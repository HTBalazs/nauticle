/////////////////////////////////////////////////////////////////////////////
// Name:        pmXML_processor.h
// Purpose:     pmXML_processor class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

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