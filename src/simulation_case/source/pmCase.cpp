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

#include "pmCase.h"

using namespace Nauticle;
using namespace ProLog;

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmCase::pmCase(pmCase const& other) {
	this->workspace = other.workspace->clone();
	for(auto const& it:other.equations) {
		this->equations.push_back(it->clone());
	}
	assign_particle_system_to_equations();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmCase& pmCase::operator=(pmCase const& rhs) {
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
pmCase::pmCase(pmCase&& other) {
	this->workspace = std::move(other.workspace);
	this->equations = std::move(other.equations);
	assign_particle_system_to_equations();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmCase& pmCase::operator=(pmCase&& rhs) {
	if(this!=&rhs) {
		this->workspace = std::move(rhs.workspace);
		this->equations = std::move(rhs.equations);
		assign_particle_system_to_equations();
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements the identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmCase::operator==(pmCase const& rhs) const {
	bool identical = *(this->workspace)==*(rhs.workspace);
	if(!identical || this->equations.size()!=rhs.equations.size()) { return false; }
	for(int i=0; i<this->equations.size(); i++) {
		identical *= this->equations[i] == rhs.equations[i];
	}
	return identical;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements the non-identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmCase::operator!=(pmCase const& rhs) const {
	return !this->operator==(rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds the given workspace to the solver. Previously stored workspace is destroyed.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::add_workspace(std::shared_ptr<pmWorkspace> ws) {
	workspace = ws;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Recives a equation and pushes it to the equations vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::add_equation(std::shared_ptr<pmEquation> func) {
	for(auto const& it:equations) {
		if(it->get_name()==func->get_name()){
			pLogger::warning_msg("Equation \"%s\" is already existing.\n",func->get_name().c_str());
			return;
		}
	}
	equations.push_back(func);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Recieves a vector of equations and pushes it to the end of the equations vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::add_equation(std::vector<std::shared_ptr<pmEquation>> func) {
	for(auto const& it:func) {
		this->add_equation(it);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the content of the pmCase object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::print() const {
	pLogger::headerf<LBL>("Case");
	workspace->print();
	int f=0;
	pLogger::titlef<LMA>("Equations");
	for(auto const& it:equations) {
		f++;
		pLogger::logf<YEL>("         %i) ", f);
		it->print();
		pLogger::logf("\n");
	}
	if(f==0) {
		pLogger::logf<WHT>("            < empty >\n");
	}
	for(auto const& it:particle_modifier) {
		it->print();
	}
	for(auto const& it:background) {
		it->print();
	}
	for(auto const& it:time_series) {
		it->print();
	}
	if(rbsys.use_count()>0) {
		rbsys->print();
	}
	pLogger::footerf<LBL>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the workspace. 
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmWorkspace> pmCase::get_workspace() const {
	return workspace;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the equations.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmEquation>> pmCase::get_equations() const {
	return equations;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Solves the equation with the given name or all equations in order if name is empty.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmCase::solve(double const& current_time, size_t const& num_threads, std::string const& name/*=""*/) {
	this->update_particle_modifiers(num_threads);
	this->update_background_fields();
	this->update_time_series_variables(current_time);
	this->update_rigid_bodies(workspace->get_instance("dt").lock()->evaluate(0)[0]);
	bool success = workspace->update();
	if(!success) {
		return false;
	}
	if(name=="") {
		for(auto const& it:equations) {
			if(it->is_interaction()) {
				success = workspace->update();
				if(!success) { return false; }
			}
			it->solve(num_threads);
			if(it->get_lhs()->get_name()=="r") {
				success = workspace->update();
				if(!success) { return false; }
			}
		}
	} else {
		for(auto const& it:equations) {
			if(it->get_name()==name) {
				if(it->is_interaction() && it->get_lhs()->get_name()=="r") {
					workspace->update();
				}
				it->solve(num_threads);
			}
		}
		pLogger::warning_msg("No equation found with name \"%s\"\n.", name.c_str());
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Assigns the particle system of the workspace to all equations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::assign_particle_system_to_equations() {
	std::shared_ptr<pmParticle_system> psys = workspace->get_particle_system();
	for(auto const& it:equations) {
		it->assign_particle_system(psys);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy tof the object. 
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmCase> pmCase::clone() const {
	return std::make_shared<pmCase>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Merge cases.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::merge(std::shared_ptr<pmCase> const& other) {
	this->workspace->merge(other->workspace);
	size_t i=0;
	for(auto const& it:other->equations) {
		this->add_equation(it);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates particle splitters and mergers.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::update_particle_modifiers(size_t const& num_threads) {
	workspace->update();
	for(auto& it:particle_modifier) {
		it->update(num_threads);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates background interpolations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::update_background_fields() {
	for(auto& it:background) {
		it->update();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates background interpolations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::update_rigid_bodies(double const& time_step) {
	if(rbsys.use_count()>0) {
		rbsys->update(time_step);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates time sereies interpolations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::update_time_series_variables(double const& t) {
	for(auto& it:time_series) {
		it->update(t);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Assigns pmParticle_system object in the pmWorkspace to all equations.
/////////////////////////////////////////////////////////////////////////////////////////
void pmCase::initialize() {
	this->assign_particle_system_to_equations();
}

void pmCase::add_particle_modifier(std::shared_ptr<pmParticle_modifier> pmod) {
	particle_modifier.push_back(pmod);
}

void pmCase::add_background(std::shared_ptr<pmBackground> bckg) {
	background.push_back(bckg);
}

void pmCase::add_time_series(std::shared_ptr<pmTime_series> ts) {
	time_series.push_back(ts);
}

void pmCase::add_rigid_body_system(std::shared_ptr<pmRigid_body_system> rbs) {
	rbsys = rbs;
}