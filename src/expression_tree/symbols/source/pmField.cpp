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

#include "pmField.h"
#include "commonutils/Common.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmField::pmField(std::string const& n, int const& size, pmTensor const& v/*=pmTensor{0}*/, int const& prl/*=1*/, int const& ppd/*=-1*/, bool const& pr/*=true*/) {
	name = n;
	value.resize(size, pmHTensor{v});
	reflect_parallel = prl;
	reflect_perpendicular = ppd;
	printable = pr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmField::pmField(std::string const& n, std::vector<pmTensor> const& v, int const& prl/*=1*/, int const& ppd/*=-1*/, bool const& pr/*=true*/) {
	name = n;
	value.resize(v.size());
	for(int i=0; i<v.size(); i++) {
		value[i] = v[i];
	}
	reflect_parallel = prl;
	reflect_perpendicular = ppd;
	printable = pr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implement identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmField::operator==(pmField const& rhs) const {
	return !(this->name != rhs.name || this->value != rhs.value || this->reflect_parallel != rhs.reflect_parallel || this->reflect_perpendicular != rhs.reflect_perpendicular || this->printable!=rhs.printable);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements the non-identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmField::operator!=(pmField const& rhs) const {
	return !this->operator==(rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the whole content of the field.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::printv() const {
	this->print();
	for(int i=0; i<20-name.size(); i++) {
		ProLog::pLogger::logf<ProLog::LMA>(" ");
	}
	pmTensor tensor = this->evaluate(0);
	ProLog::pLogger::logf<ProLog::LMA>("[%i by %i] ", tensor.get_numrows(), tensor.get_numcols());
	ProLog::pLogger::logf<ProLog::LMA>("[prl: %i, ppd: %i]", reflect_parallel, reflect_perpendicular);
	if(!this->is_printable()) {
		ProLog::pLogger::logf<ProLog::LMA>(" not printed");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clone implementation.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmExpression> pmField::clone_impl() const {
	return std::make_shared<pmField>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns a copy of the field.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmField> pmField::clone() const {
	return std::static_pointer_cast<pmField, pmExpression>(clone_impl());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes object to string.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::write_to_string(std::ostream& os) const {
	os << name;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Duplicates the member with the given index.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::duplicate_member(size_t const& i, pmTensor const& guide/*=pmTensor{}*/) {
	if(pmFirst_virtual_index::first_virtual_index <= i) {
		ProLog::pLogger::error_msgf("Duplication of virtual particles is not allowed.\n");
	}
	value.push_back(value[i]);
	pmHTensor& tmp = value.back();
	if(!guide.is_empty() && !guide.is_zero()) {
		for(auto& it:tmp) {
			pmTensor perpendicular = (guide.transpose()*it)*guide/guide.norm();
			pmTensor parallel = it-perpendicular;
			it = parallel*reflect_parallel+perpendicular*reflect_perpendicular;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the printable variable.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::set_printable(bool const& p) {
	this->printable = p;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the printable variable.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmField::is_printable() const {
	return this->printable;
}

void pmField::initialize(pmTensor const& v) {
	for(auto& it:value) {
		it.initialize(v);
	}
}
