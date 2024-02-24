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

#include "pmFsearch.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// This function performs the type-specific process (pmFmax, pmFmin, pmFmean).
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmFsearch::evaluate(int const& i, size_t const& level/*=0*/) const {
	static pmTensor tensor{1,1};
	if(i==0) {
		process(tensor, level);
	}
	return tensor;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the size of the resulting field. It returns 1. 
/////////////////////////////////////////////////////////////////////////////////////////
int pmFsearch::get_field_size() const {
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the object is assigned to a pmParticle_system object.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmFsearch::is_assigned() const {
	return true;
}

#ifdef JELLYFISH
c2c::c2CPP_type pmFsearch::get_cpp_return_type() const {
    return this->operand[0]->evaluate(0).get_cpp_type().get_type();
}

c2c::c2CPP_class_member_function pmFsearch::generate_cpp_evaluator_function() const {
    std::string content = generate_cpp_evaluator_function_content();
    c2c::c2CPP_class_member_function cmf = c2c::c2CPP_class_member_function{this->return_type, this->get_name(), false, "", std::vector<c2c::c2CPP_declaration>{}, c2c::PRIVATE, content};
    return cmf;
}

std::string pmFsearch::get_cpp_evaluation(std::string const& idx, size_t const& level/*=0*/) const {
    return this->get_name()+"()";
}
#endif // JELLYFISH