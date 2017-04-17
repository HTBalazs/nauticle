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

    For more information please visit: https://bitbucket.org/lempsproject/
*/

#include "pmSolver.h"

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmSolver::pmSolver(pmSolver const& other) {
	this->workspace = other.workspace->clone();
	for(auto const& it:other.equations) {
		this->equations.push_back(it->clone());
	}
	assign_particle_system_to_equations();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmSolver& pmSolver::operator=(pmSolver const& rhs) {
	if(this!=&rhs) {
		this->workspace = rhs.workspace->clone();
		for(auto const& it:rhs.equations) {
			this->equations.push_back(it->clone());
		}
		assign_particle_system_to_equations();
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmSolver::pmSolver(pmSolver&& other) {
	this->workspace = std::move(other.workspace);
	this->equations = std::move(other.equations);
	assign_particle_system_to_equations();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmSolver& pmSolver::operator=(pmSolver&& rhs) {
	if(this!=&rhs) {
		this->workspace = std::move(rhs.workspace);
		this->equations = std::move(rhs.equations);
		assign_particle_system_to_equations();
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds the given workspace to the solver. Previously stored workspace is destroyed.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSolver::add_workspace(std::shared_ptr<pmWorkspace> ws) {
	workspace = ws;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Recives a equation and pushes it to the equations vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSolver::add_equation(std::shared_ptr<pmEquation> func) {
	for(auto const& it:equations) {
		if(it->get_name()==func->get_name()){
			pLogger::warning_msg("equation \"%s\" is already existing in the solver.\n",func->get_name().c_str());
			return;
		}
	}
	equations.push_back(func);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Recieves a vector of equations and pushes it to the end of the equations vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSolver::add_equation(std::vector<std::shared_ptr<pmEquation>> func) {
	for(auto const& it:func) {
		this->add_equation(it);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the content of the pmSolver object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSolver::print() const {
	pLogger::headerf<LBL>("solver");
	workspace->print();
	int f=0;
	pLogger::titlef<LMA>("equations");
	for(auto const& it:equations) {
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
std::shared_ptr<pmWorkspace> pmSolver::get_workspace() const {
	return workspace;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the equations.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmEquation>> pmSolver::get_equations() const {
	return equations;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Solves all equation in order.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSolver::solve(size_t const& num_threads, std::string const& name/*=""*/) {
	workspace->sort_all_by_position();
	if(name=="") {
		for(auto const& it:equations) {
			it->solve(num_threads);
		}
	} else {
		for(auto const& it:equations) {
			if(it->get_name()==name) {
				it->solve(num_threads);
				return;
			}
		}
		pLogger::warning_msg("No equation found with name \"%s\"\n.", name.c_str());
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Assigns the particle system of the workspace to all equations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSolver::assign_particle_system_to_equations() {
	std::weak_ptr<pmParticle_system> psys = std::dynamic_pointer_cast<pmParticle_system>(workspace->get_instance("r").lock());
	for(auto const& it:equations) {
		it->assign_particle_system(psys);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy tof the object. 
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmSolver> pmSolver::clone() const {
	return std::make_shared<pmSolver>(*this);
}

void pmSolver::merge(std::shared_ptr<pmSolver> const& other) {
	this->workspace->merge(other->workspace);
	size_t i=0;
	for(auto const& it:other->equations) {
		this->add_equation(it);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Assigns pmParticle_system object in the pmWorkspace to all equations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmSolver::initialize() {
	this->assign_particle_system_to_equations();
}