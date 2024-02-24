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
    
#include "pmFmax.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmax::pmFmax(std::shared_ptr<pmExpression> op0) {
	operand[0] = op0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmax::pmFmax(pmFmax const& other) {
	this->operand[0] = other.operand[0]->clone();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmax::pmFmax(pmFmax&& other) {
	this->operand = std::move(other.operand);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmax& pmFmax::operator=(pmFmax const& other) {
	if(this!=&other) {
		this->operand[0] = other.operand[0]->clone();
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmFmax& pmFmax::operator=(pmFmax&& other) {
	if(this!=&other) {
		this->operand = std::move(other.operand);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements clone.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmFmax::clone_impl() const {
	return std::make_shared<pmFmax>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a deep copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmFmax> pmFmax::clone() const {
	return std::static_pointer_cast<pmFmax, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFmax::print() const {
	ProLog::pLogger::logf<NAUTICLE_COLOR>("fmax(");
	operand[0]->print();
	ProLog::pLogger::logf<NAUTICLE_COLOR>(")");
}

/////////////////////////////////////////////////////////////////////////////////////////
/// This function performs the maximum search called by the member function evaluate(...)
/// inherited from pmFsearch.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFmax::process(pmTensor& value, size_t const& level/*=0*/) const {
	pmTensor tensor = operand[0]->evaluate(0, level);
	if(!tensor.is_scalar()) {
		ProLog::pLogger::error_msgf("Fmax can be evaluated only on scalar fields!\n");
	}
	value = operand[0]->evaluate(0, level);
	for(int j=1; j<operand[0]->get_field_size(); j++) {
		pmTensor tj = operand[0]->evaluate(j, level);
		if(value(0,0)<tj(0,0)) {
			value = tj;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints object to string.
/////////////////////////////////////////////////////////////////////////////////////////
void pmFmax::write_to_string(std::ostream& os) const {
	os << "fmax(" << operand[0] << ")";
}


#ifdef JELLYFISH
std::string pmFmax::generate_cpp_evaluator_function_content() const { 
	std::string content = "\t//  Operator name: "+this->op_name+"\n";
	content += "\t"+return_type+" result = 0;\n";
	content += "\tfor(size_t i=0; i<"+pmSymbol::prefix+"v_numparticles; i++) {\n";
	content += "\t\tdouble eval_i = "+this->operand[0]->get_cpp_evaluation("i")+";\n";
	content += "\t\tif(result<eval_i) {\n";
	content += "\t\t\tresult = eval_i;\n";
	content += "\t\t}\n";
	content += "\t}\n";
	content += "\treturn result;";
	return content;
}
#endif // JELLYFISH



