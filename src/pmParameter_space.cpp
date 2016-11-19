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

#include "pmParameter_space.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParameter_space::pmParameter::pmParameter(std::string const& n, pmTensor const& v) {
	name = n;
	value = v;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParameter_space::pmParameter_space(pmParameter_space const& other) {
	for(auto const& it:other.parameters) {
		this->parameters.push_back(std::make_shared<pmParameter>(*it));
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmParameter_space::pmParameter_space(pmParameter_space&& other) {
	this->parameters = std::move(other.parameters);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignmnt operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmParameter_space& pmParameter_space::operator=(pmParameter_space const& other) {
	if(this!=&other) {
		for(auto const& it:other.parameters) {
			this->parameters.push_back(std::make_shared<pmParameter>(*it));
		}
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignmnt operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmParameter_space& pmParameter_space::operator=(pmParameter_space&& other) {
	if(this!=&other) {
		this->parameters = std::move(other.parameters);
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new parameter to the parameter space. A parameter with a name that already
/// stored in the parameter space is not added.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParameter_space::add_parameter(std::string const& n, pmTensor const& v) {
	for(auto const& it:parameters) {
		if(it->name==n) {
			pLogger::warning_msgf("Parameter is already defined.\n");
			return;
		}
	}
	parameters.push_back(std::make_shared<pmParameter>(n,v));
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Destroys the parameter with the given name.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParameter_space::delete_parameter(std::string const& n) {
	std::vector<std::shared_ptr<pmParameter>>::iterator it = parameters.begin();
	for(; it<parameters.end(); it++) {
		if((*it)->name==n) {
			parameters.erase(it);
		}
	}
	pLogger::warning_msgf("No such parameter: \"%s\"\n", n.c_str());
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the parameter with the given name. If there is not such parameter, it returns
/// an empty tensor.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmParameter_space::get_parameter_value(std::string const& n) const {
	for(int i=0; i<parameters.size(); i++) {
		if(parameters[i]->name==n) {
			return parameters[i]->value;
		}
	}
	pLogger::warning_msgf("No such parameter: \"%s\"\n", n.c_str());
	return pmTensor{1};
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the content of the parameter space.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParameter_space::print() const {
	pLogger::headerf<LBL>("Parameter space");
	pLogger::titlef<LMA>("Parameters");
	int p=0;
	for(auto const& it:parameters) {
		p++;
		pLogger::logf<YEL>("        %i) ", p);
		it->print();
	}
	if(p==0) {
		pLogger::logf<WHT>("            < empty >\n");
	}
	pLogger::footerf<LBL>();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the content of the parameter.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParameter_space::pmParameter::print() const {
	pLogger::logf<WHT>("%s = ", name.c_str());
	value.print();
	pLogger::logf<WHT>("\n");	
}