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
    
#include "pmEquation.h"
#include <thread>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmEquation::pmEquation(std::string n, std::shared_ptr<pmTerm> ex1, std::shared_ptr<pmExpression> ex2, std::shared_ptr<pmExpression> cond) {
	name = n;
	lhs = ex1;
	rhs = ex2;
	condition = cond;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmEquation::pmEquation(pmEquation const& other) {
	this->name = other.name;
	lhs = other.lhs->clone();
	rhs = other.rhs->clone();
	condition = other.condition->clone();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmEquation& pmEquation::operator=(pmEquation const& other) {
	if(this!=&other) {
		this->name = other.name;
		lhs = other.lhs->clone();
		rhs = other.rhs->clone();
		condition = other.condition->clone();
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmEquation::pmEquation(pmEquation&& other) {
	this->name = other.name;
	lhs = std::move(other.lhs);
	rhs = std::move(other.rhs);
	condition = other.condition->clone();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmEquation& pmEquation::operator=(pmEquation&& other) {
	if(this!=&other) {
		this->name = other.name;
		lhs = std::move(other.lhs);
		rhs = std::move(other.rhs);
		condition = other.condition->clone();
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Print function. TODO: consider pmLogger namespace.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::print() const {
	pLogger::logf("%s: ",name.c_str());
	lhs->print();
	pLogger::logf(" = ");
	rhs->print();
	pLogger::logf<LMA>("  # ");
	condition->print();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Solves equation for all nodes included in the field inside the variables of the rhs.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::evaluate(int const& p_begin, int const& p_end) {
	int end = p_end>rhs->get_field_size() ? rhs->get_field_size() : p_end;
	for(int i=p_begin; i<end; i++) {
		if(condition->evaluate(i, 0)[0]) {
			lhs->set_value(rhs->evaluate(i, 0), i);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Solves equation for all nodes included in the field inside the variables of the rhs.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::solve(size_t const& num_threads) {
	if(lhs->get_field_size()!=rhs->get_field_size() || lhs->get_field_size()==-1 || rhs->get_field_size()==-1) {
		pLogger::error_msgf("Inconsistent function fields.\n");
	}
	int p_end = lhs->get_field_size();
	if(p_end==1) {
		this->evaluate(0, 1);
		return;
	}
	auto process = [&](int const& p_start, int const& p_end){
		this->evaluate(p_start, p_end);
	};
	std::vector<std::thread> th;
	int number_of_threads = std::min((int)num_threads,p_end);
	int ppt = p_end/number_of_threads;
	int p_start = 0;
	for(int i=p_start; i<p_end; i+=ppt) {
		th.push_back(std::thread{process, i, i+ppt});
	}
	for(auto& it:th) {
		it.join();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the name of the function.
/////////////////////////////////////////////////////////////////////////////////////////
std::string pmEquation::get_name() const {
	return name;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Assigns the given particle system to the rhs of the function. It is necessary for interactions.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::assign_particle_system(std::weak_ptr<pmParticle_system> psys) {
	rhs->assign(psys);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmEquation> pmEquation::clone() const {
	return std::make_shared<pmEquation>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes the function to the stream.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::write_to_string(std::ostream& os) const {
	os << lhs << "=" << rhs << "#" << condition;
}




















