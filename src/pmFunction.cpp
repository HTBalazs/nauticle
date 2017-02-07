/*
    Copyright 2017 Balazs Toth
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

    For more information please visit: https://bitbucket.org/lempsproject/
*/
    
#include "pmFunction.h"
#include <thread>

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmFunction::pmFunction(std::string n, std::shared_ptr<pmTerm> ex1, std::shared_ptr<pmExpression> ex2) {
	name = n;
	lhs = ex1;
	rhs = ex2;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmFunction::pmFunction(pmFunction const& other) {
	this->name = other.name;
	lhs = other.lhs->clone();
	rhs = other.rhs->clone();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmFunction& pmFunction::operator=(pmFunction const& other) {
	if(this!=&other) {
		this->name = other.name;
		lhs = other.lhs->clone();
		rhs = other.rhs->clone();
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmFunction::pmFunction(pmFunction&& other) {
	this->name = other.name;
	lhs = std::move(other.lhs);
	rhs = std::move(other.rhs);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator
/////////////////////////////////////////////////////////////////////////////////////////
pmFunction& pmFunction::operator=(pmFunction&& other) {
	if(this!=&other) {
		this->name = other.name;
		lhs = std::move(other.lhs);
		rhs = std::move(other.rhs);
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Print function. TODO: consider pmLogger namespace.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFunction::print() const {
	pLogger::logf("%s: ",name.c_str());
	lhs->print();
	pLogger::logf(" = ");
	rhs->print();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Solves equation for all nodes included in the field inside the variables of the rhs.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFunction::evaluate(int const& p_begin, int const& p_end) {
	int end = p_end>rhs->get_field_size() ? rhs->get_field_size() : p_end;
	for(int i=p_begin; i<end; i++) {
		lhs->set_value(rhs->evaluate(i, current), i);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Solves equation for all nodes included in the field inside the variables of the rhs.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFunction::solve(size_t const& num_threads) {
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
	int ppt = p_end/num_threads;
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
std::string pmFunction::get_name() const {
	return name;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Assigns the given particle system to the rhs of the function. It is necessary for interactions.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFunction::assign_particle_system(std::weak_ptr<pmParticle_system> psys) {
	rhs->assign(psys);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmFunction> pmFunction::clone() const {
	return std::make_shared<pmFunction>(*this);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Writes the function to the stream.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFunction::write_to_string(std::ostream& os) const {
	os << lhs << "=" << rhs;
}




















