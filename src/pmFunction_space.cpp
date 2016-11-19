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

#include "pmFunction_space.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmFunction_space::pmFunction_space(pmFunction_space const& other) {
	this->workspace = other.workspace->clone();
	for(auto const& it:other.functions) {
		this->functions.push_back(it->clone());
	}
	assign_particle_system_to_functions();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmFunction_space& pmFunction_space::operator=(pmFunction_space const& rhs) {
	if(this!=&rhs) {
		this->workspace = rhs.workspace->clone();
		for(auto const& it:rhs.functions) {
			this->functions.push_back(it->clone());
		}
		assign_particle_system_to_functions();
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmFunction_space::pmFunction_space(pmFunction_space&& other) {
	this->workspace = std::move(other.workspace);
	this->functions = std::move(other.functions);
	assign_particle_system_to_functions();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmFunction_space& pmFunction_space::operator=(pmFunction_space&& rhs) {
	if(this!=&rhs) {
		this->workspace = std::move(rhs.workspace);
		this->functions = std::move(rhs.functions);
		assign_particle_system_to_functions();
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Adds the given workspace to the function space. Previously stored workspace is destroyed.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFunction_space::add_workspace(std::shared_ptr<pmWorkspace> ws) {
	workspace = ws;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Recives a function and pushes it to the functions vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFunction_space::add_function(std::shared_ptr<pmFunction> func) {
	for(auto const& it:functions) {
		if(it->get_name()==func->get_name()){
			pLogger::warning_msg("Function \"%s\" is already existing in the function space.\n",func->get_name().c_str());
			return;
		}
	}
	functions.push_back(func);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Recieves a vector of functions and pushes it to the end of the functions vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFunction_space::add_function(std::vector<std::shared_ptr<pmFunction>> func) {
	for(auto const& it:func) {
		this->add_function(it);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the content of the pmFunction_space object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFunction_space::print() const {
	pLogger::headerf<LBL>("Function space");
	workspace->print();
	int f=0;
	pLogger::titlef<LMA>("Functions");
	for(auto const& it:functions) {
		f++;
		pLogger::logf<YEL>("         %i) ", f);
		it->print();
		pLogger::logf("\n");
	}
	if(f==0) {
		pLogger::logf<WHT>("            < empty >\n");
	}
	pLogger::footerf<LBL>();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the workspace. 
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmWorkspace> pmFunction_space::get_workspace() const {
	return workspace;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the functions.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmFunction>> pmFunction_space::get_functions() const {
	return functions;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Solves all function in order.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFunction_space::solve(size_t const& num_threads, std::string const& name/*=""*/) {
	workspace->sort_all_by_position();
	if(name=="") {
		for(auto const& it:functions) {
			it->solve(num_threads);
		}
	} else {
		for(auto const& it:functions) {
			if(it->get_name()==name) {
				it->solve(num_threads);
				return;
			}
		}
		pLogger::warning_msg("No function found with name \"%s\"\n.", name.c_str());
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Assigns the particle system of the workspace to all functions.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFunction_space::assign_particle_system_to_functions() {
	std::weak_ptr<pmParticle_system> psys = std::dynamic_pointer_cast<pmParticle_system>(workspace->get_instance("r").lock());
	for(auto const& it:functions) {
		it->assign_particle_system(psys);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy tof the object. 
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmFunction_space> pmFunction_space::clone() const {
	return std::make_shared<pmFunction_space>(*this);
}

void pmFunction_space::merge(std::shared_ptr<pmFunction_space> const& other) {
	this->workspace->merge(other->workspace);
	size_t i=0;
	for(auto const& it:other->functions) {
		this->add_function(it);
	}
}

void pmFunction_space::initialize() {
	this->assign_particle_system_to_functions();
}