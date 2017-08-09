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

#ifndef _XML_PROCESSOR_H_
#define _XML_PROCESSOR_H_

#include <string>
#include <vector>
#include <memory>
#include "handyxml/hdyXML_parser.h"
#include "pmWorkspace.h"
#include "pmEquation.h"
#include "pmCase.h"
#include "pmGrid_space.h"
#include "pmParameter_space.h"

namespace Nauticle {
    /**	This class extends the hdyXML_parser class with further equations to process
    //	the XML tree.
    */
    class pmXML_processor final : public HandyXML::hdyXML_parser {
    	std::shared_ptr<pmWorkspace> get_workspace() const;
    	std::vector<std::shared_ptr<pmEquation>> get_equations(std::shared_ptr<pmWorkspace> workspace) const;
    	std::shared_ptr<pmCase> get_initial_condition_case() const;
    public:
    	virtual ~pmXML_processor() override {}
    	std::shared_ptr<pmGrid_space> get_grid_space(std::shared_ptr<pmBlock> particle_system, std::shared_ptr<pmWorkspace> workspace, pmDomain const& domain) const;
    	std::shared_ptr<pmCase> get_case() const;
    	std::shared_ptr<pmParameter_space> get_parameter_space(std::shared_ptr<pmWorkspace> workspace=std::make_shared<pmWorkspace>()) const;
    	std::shared_ptr<pmWorkspace> get_initial_condition() const;
    };
}

#endif //_XML_PROCESSOR_H_