/*
    Copyright 2016-2019 Balazs Toth
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
#include <thread>
#include <algorithm>

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor
/////////////////////////////////////////////////////////////////////////////////////////
pmEquation::pmEquation(std::string n, std::shared_ptr<pmSymbol> ex1, std::shared_ptr<pmExpression> ex2, std::shared_ptr<pmExpression> cond) {
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
			lhs->set_value(tensor, i);
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
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the condition of the equation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmEquation::set_condition(std::shared_ptr<pmExpression> cond) {
	condition = cond;
}

std::string pmEquation::generate_evaluator_code() const {
	std::stringstream os;
	this->write_to_string(os);
	std::string code = "\t// " + os.str() + "\n";
	code += "\t{\n";

	code += "\tint p_end = ws_" + lhs->get_name() + "->get_field_size();\n\tauto process = [&](int const& start, int const& end){\n";
	code += "\t\tint new_end = end>ws_" + lhs->get_name() + "->get_field_size() ? ws_" + lhs->get_name() + "->get_field_size() : end;\n";
	code += "\t\tfor(int i=start; i<new_end; i++) {\n";
	code += "\t\t\tif(tensor_cast<bool>(" + condition->generate_evaluator_code("i", "0") + ")) {\n";
	code += "\t\t\t\tws_" + lhs->get_name() + "->set_value(" + rhs->generate_evaluator_code("i","0") + ",i);\n";
	code += "\t\t\t}\n\t\t}\n";
	code += "\t};\n";
	code += "\tstd::vector<std::thread> th;\n	int number_of_threads = std::min((int)num_threads,p_end);\n	int ppt = p_end/number_of_threads; // particle per thread\n	for(int i=0; i<p_end; i+=ppt) {\n		th.push_back(std::thread{process, i, i+ppt});\n	}\n	for(auto& it:th) {\n		it.join();\n	}\n";
	if(lhs->get_name()=="r") {
		code += "\tws->sort_all_by_position();\n";
	}
	code += "\t}\n";
	return code;
}

















