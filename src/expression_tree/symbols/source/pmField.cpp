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

#include "pmField.h"
#include "commonutils/Common.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmField::pmField(std::string const& n, int const& size, pmTensor const& v/*=pmTensor{0}*/, bool const& sym/*=true*/) {
	name = n;
	value.push_back(std::vector<pmTensor>());
	value[0].resize(size, v);
	symmetric = sym;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmField::pmField(std::string const& n, std::vector<pmTensor> const& v, bool const& sym/*=true*/) {
	name = n;
	value.push_back(std::vector<pmTensor>());
	value[0] = v;
	symmetric = sym;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmField::pmField(pmField const& other) {
	this->name = other.name;
	this->value = other.value;
	this->symmetric = other.symmetric;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmField::pmField(pmField&& other) {
	this->name = std::move(other.name);
	this->value = std::move(other.value);
	this->symmetric = std::move(other.symmetric);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmField& pmField::operator=(pmField const& other) {
	if(this!=&other) {
		this->name = other.name;
		this->value = other.value;
		this->symmetric = other.symmetric;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmField& pmField::operator=(pmField&& other) {
	if(this!=&other) {
		this->name = std::move(other.name);
		this->value = std::move(other.value);
		this->symmetric = std::move(other.symmetric);
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implement identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmField::operator==(pmField const& rhs) const {
	if(this->name != rhs.name || this->value != rhs.value || this->symmetric != rhs.symmetric || this->depth!=rhs.depth) {
		return false;
	} else {
		return true;
	}
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
	ProLog::pLogger::logf<ProLog::LMA>("[%i by %i]", tensor.get_numrows(), tensor.get_numcols());
	if(!this->is_symmetric()) {
		ProLog::pLogger::logf<ProLog::LMA>(" *");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the size of the field.
/////////////////////////////////////////////////////////////////////////////////////////
int pmField::get_field_size() const {
	return value[0].size();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the depth of data storage.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::set_storage_depth(size_t const& d) {
	depth = d;
	value.resize(depth,value.back());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates the field.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmField::evaluate(int const& i, size_t const& level/*=0*/) const {
	return value[level][i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets the value of the ith node.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::set_value(pmTensor const& v, int const& i/*=0*/) {
	for(int level=0; level<depth-1; level++) {
		value[level+1][i] = value[level][i];
	}
	value[0][i] = v;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the ith node.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor const& pmField::get_value(int const& i) const {
	return value[0][i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the field type.
/////////////////////////////////////////////////////////////////////////////////////////
std::string pmField::get_type() const {
	if(value[0][0].is_scalar()) { return "SCALAR"; }
	if(value[0][0].is_vector()) { return "VECTOR"; }
	return "TENSOR";
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
/// Sorts the field based on the given sorted index vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::sort_field(std::vector<int> const& sorted_idx) {
	for(auto& it:value) {
		pmSort::reorder(it, sorted_idx);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Resizes the field. The last element is copied to the empty places if N>current size.
/// If N<current size, the elements above N are destroyed.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::set_number_of_nodes(size_t const& N) {
	if(N!=value[0].size()) {
		for(auto& it:value) {
			pmTensor ctensor = it.back();
			it.resize(N, ctensor);	
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Writes object to string.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::write_to_string(std::ostream& os) const {
	os << name;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return symmetric property of the field. 
/////////////////////////////////////////////////////////////////////////////////////////
bool pmField::is_symmetric() const {
	return symmetric;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets symmetric property of the field. 
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::set_symmetry(bool const& sym) {
	symmetric = sym;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Deletes the member of the field with the given index.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::delete_member(size_t const& i) {
	for(auto& level_it:value) {
		level_it[i] = level_it.back();
		level_it.pop_back();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Deletes the set of members of the fiels listed in the given delete_indices vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::delete_set(std::vector<size_t> const& delete_indices) {
	for(auto& level_it:value) {
		Common::delete_indices(level_it, delete_indices);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a member to the field initialized with the last member in the container.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::add_member(pmTensor const& v/*=pmTensor{}*/) {
	pmTensor tensor = v;
	if(tensor.numel()==0) {
		tensor = value[0].back();
	}
	for(auto& level_it:value) {
		level_it.push_back(tensor);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Duplicates the member with the given index.
/////////////////////////////////////////////////////////////////////////////////////////
void pmField::duplicate_member(size_t const& i) {
	for(auto& it:value) {
		it.push_back(it[i]);
	}
}
