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
    
#include "pmEquation.h"
#include "pmConstant.h"
#include "c2c/c2CPP_declaration.h"
#include <thread>
#include <algorithm>

using namespace Nauticle;
using namespace c2c;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmEquation::pmEquation(std::string n, std::shared_ptr<pmSymbol> ex1, std::shared_ptr<pmExpression> ex2, std::shared_ptr<pmExpression> cond) {
	name = n;
	lhs = ex1;
	rhs = ex2;
	condition = cond;
	rhs_interaction = rhs->is_interaction();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmEquation::pmEquation(pmEquation const& other) {
	this->name = other.name;
	lhs = other.lhs->clone();
	rhs = other.rhs->clone();
	condition = other.condition->clone();
	this->rhs_interaction = other.rhs_interaction;
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
		this->rhs_interaction = other.rhs_interaction;
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
	this->rhs_interaction = std::move(other.rhs_interaction);
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
		this->rhs_interaction = std::move(other.rhs_interaction);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implement identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmEquation::operator==(pmEquation const& rhs) const {
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements the non-identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmEquation::operator!=(pmEquation const& rhs) const {
    return !this->operator==(rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Print function. TODO: consider pmLogger namespace.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::print() const {
	ProLog::pLogger::logf("%s: ",name.c_str());
	lhs->print();
	ProLog::pLogger::logf(" = ");
	rhs->print();
	ProLog::pLogger::logf<ProLog::LMA>("  # ");
	condition->print();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Solves equation for all nodes included in the field inside the variables of the rhs.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::evaluate(int const& start, int const& end) {
	int p_end = end>lhs->get_field_size() ? lhs->get_field_size() : end;
	for(int i=start; i<p_end; i++) {
		if(condition->evaluate(i, 0)[0]) {
			pmTensor tensor = rhs->evaluate(i, 0);
			if(tensor.numel()==0) {
				lhs->set_value(lhs->get_value(i)*0.0, i);
			} else {
				lhs->set_value(tensor, i);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Solves equation for all nodes included in the field inside the variables of the rhs.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::solve(size_t const& num_threads) {
	if((lhs->get_field_size()!=rhs->get_field_size() && 1!=rhs->get_field_size()) || lhs->get_field_size()==-1 || rhs->get_field_size()==-1) {
		ProLog::pLogger::error_msgf("Inconsistent field sizes in equation %s\n", name.c_str());
	}
	int p_end = lhs->get_field_size();

	auto process = [&](int const& start, int const& end){
		this->evaluate(start, end);
	};
	std::vector<std::thread> th;
	int number_of_threads = std::min((int)num_threads,p_end);
	int ppt = p_end/number_of_threads; // particle per thread
	for(int i=0; i<p_end; i+=ppt) {
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
/// Sets the name of the equation to n.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::set_name(std::string const& n) {
	name = n;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Assigns the given particle system to the rhs of the function. It is necessary for interactions.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::assign_particle_system(std::shared_ptr<pmParticle_system> psys) {
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

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the LHS of the equation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmSymbol> pmEquation::get_lhs() const {
	return lhs;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the RHS of the equation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmEquation::get_rhs() const {
	return rhs;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the condition of the equation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmEquation::get_condition() const {
	return condition;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the LHS of the equation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::set_lhs(std::shared_ptr<pmSymbol> left) {
	lhs = left;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the RHS of the equation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::set_rhs(std::shared_ptr<pmExpression> right) {
	rhs = right;
	rhs_interaction = rhs->is_interaction();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the condition of the equation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::set_condition(std::shared_ptr<pmExpression> cond) {
	condition = cond;
}

bool const& pmEquation::is_interaction() const {
	return rhs_interaction;
}

#ifdef JELLYFISH
std::string pmEquation::generate_cpp_caller(std::string const& session_name) const {
	std::string eval = lhs->get_cpp_evaluation("i") + " = " + rhs->get_cpp_evaluation("i")+";";
	auto condition_is_constant = std::dynamic_pointer_cast<pmConstant>(this->condition).use_count();
	if(!(condition_is_constant && std::abs(condition->evaluate(0)[0])>NAUTICLE_EPS)) {
		eval = "if("+condition->get_cpp_evaluation("i")+") { "+eval+" }";
	}

	bool fld = std::dynamic_pointer_cast<pmField>(lhs).use_count()>0;
	std::string caller = "";
	if(fld) {
		caller += "\tparallel([this](int i){"+eval+"}, num_threads);\n";
	} else {
		caller += "\t" + eval + "\n";
	}
	return caller;
}
#endif // JELLYFISH 















