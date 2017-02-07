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

#include "pmWorkspace.h"
#include <numeric>

std::string const pmWorkspace::reserved_names[] = {"id", "true", "false", "pi", "Wp22210", "Wp22220", "Wp22230", "Wp32210", "Wp32220", "Wp32230", "Wp52210", "Wp52220", "Wp52230", "domain_min", "domain_max", "cell_size", "ASCII", "BINARY"};

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmWorkspace::pmWorkspace() {
	std::vector<pmTensor> id(1);
	std::iota(id.begin(), id.end(), pmTensor{1,1,0});
	this->add_field("id", id);
	this->add_constant("true", pmTensor{1,1,1}, true);
	this->add_constant("false", pmTensor{1,1,0}, true);
	this->add_constant("pi", pmTensor{1,1,3.14159265359}, true);
	this->add_constant("Wp22210", pmTensor{1,1,0}, true);
	this->add_constant("Wp22220", pmTensor{1,1,1}, true);
	this->add_constant("Wp22230", pmTensor{1,1,2}, true);
	this->add_constant("Wp32210", pmTensor{1,1,3}, true);
	this->add_constant("Wp32220", pmTensor{1,1,4}, true);
	this->add_constant("Wp32230", pmTensor{1,1,5}, true);
	this->add_constant("Wp52210", pmTensor{1,1,6}, true);
	this->add_constant("Wp52220", pmTensor{1,1,7}, true);
	this->add_constant("Wp52230", pmTensor{1,1,8}, true);
	this->add_constant("ASCII", pmTensor{1,1,0}, true);
	this->add_constant("BINARY", pmTensor{1,1,1}, true);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmWorkspace::pmWorkspace(pmWorkspace const& other) {
	this->num_nodes = other.num_nodes;
	for(auto const& it:other.definitions) {
		this->definitions.push_back(it->clone());
	}
	this->num_constants = other.num_constants;
	this->num_variables = other.num_variables;
	this->deleted_ids = other.deleted_ids;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmWorkspace& pmWorkspace::operator=(pmWorkspace const& other) {
	if(this!=&other) {
		this->num_nodes = other.num_nodes;
		for(auto const& it:other.definitions) {
			this->definitions.push_back(it->clone());
		}
		this->num_constants = other.num_constants;
		this->num_variables = other.num_variables;
		this->deleted_ids = other.deleted_ids;
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmWorkspace::pmWorkspace(pmWorkspace&& other) {
	this->num_nodes = other.num_nodes;
	this->definitions = std::move(other.definitions);
	this->num_constants = other.num_constants;
	this->num_variables = other.num_variables;
	this->deleted_ids = std::move(other.deleted_ids);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Move assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmWorkspace& pmWorkspace::operator=(pmWorkspace&& other) {
	if(this!=&other) {
		this->num_nodes = other.num_nodes;
		this->definitions = std::move(other.definitions);
		this->num_constants = other.num_constants;
		this->num_variables = other.num_variables;
		this->deleted_ids = std::move(other.deleted_ids);
	}
	return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the term is a constant.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmWorkspace::is_constant(std::shared_ptr<pmTerm> term) const {
	return std::dynamic_pointer_cast<pmConstant>(term).use_count()!=0;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the term is a variable.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmWorkspace::is_variable(std::shared_ptr<pmTerm> term) const {
	return std::dynamic_pointer_cast<pmVariable>(term).use_count()!=0;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the term is a constant or variable.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmWorkspace::is_constant_or_variable(std::shared_ptr<pmTerm> term) const {
	return is_constant(term) || is_variable(term);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Merges the given workspace to the object. Conflicting instances are skipped.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::merge(std::shared_ptr<pmWorkspace> other) {
	if(other.use_count()==0) { return; }
	if(this->num_nodes!=other->num_nodes) {
		this->set_number_of_nodes(other->get_number_of_nodes());
	}
	for(auto const& it:other->definitions) {
		std::string name = it->get_name();
		if(!is_reserved(name)) {
			if(this->is_existing(name)) {
				continue;
			}
			if(is_constant(it)) {
				add_constant(name, it->evaluate(0));
			} else if(is_variable(it)) {
				add_variable(name, it->evaluate(0));
			} else {
				this->definitions.push_back(it->clone());
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Verifies the given instance name.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmWorkspace::verify_name(std::string const& name) const {
	if(!is_word(name) || is_function(name) || is_number(name) || is_existing(name)) {
		pLogger::warning_msgf("\"%s\" is reserved or already existing.\n", name.c_str());
		return false;
	} else {
		return true;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the given name is existing in the workspace.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmWorkspace::is_existing(std::string const& name) const {
	for(auto const& it:definitions) {
		if(it->get_name()==name) {
			return true;
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new constant to the workspace with a value. If an instance is
/// already existing with the same name it does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::add_constant(std::string const& name, pmTensor const& value, bool const& hidden/*=false*/) {
	if(!verify_name(name)) return;
	definitions.push_back(std::static_pointer_cast<pmTerm>(std::make_shared<pmConstant>(name, value, hidden)));
	num_constants++;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new variable to the workspace with an optional initialization value. If an 
/// instance is already existing with the same name it does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::add_variable(std::string const& name, pmTensor const& value/*=pmTensor{0}*/) {
	if(!verify_name(name)) return;
	definitions.push_back(std::static_pointer_cast<pmTerm>(std::make_shared<pmVariable>(name, value)));
	num_variables++;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new field to the workspace with an optional initialization value. If an 
/// instance is already existing with the same name it does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::add_field(std::string const& name, pmTensor const& value/*=pmTensor{0}*/) {
	if(!verify_name(name)) return;
	definitions.push_back(std::static_pointer_cast<pmTerm>(std::make_shared<pmField>(name, num_nodes, value)));
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new field to the workspace with an initialization tensor field. If an 
/// instance is already existing with the same name it does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::add_field(std::string const& name, std::vector<pmTensor> const& values) {
	if(!verify_name(name)) return;
	if(values.size()!=num_nodes) {
		pLogger::error_msgf("Inconsistent size of field \"%s\".\n",name.c_str());
	}
	definitions.push_back(std::static_pointer_cast<pmTerm>(std::make_shared<pmField>(name, values)));
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new particle system to the workspace with an initialization tensor field. If an 
/// instance is already existing with the same name it does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::add_particle_system(std::vector<pmTensor> const& values, pmDomain const& domain) {
	if(is_existing("r")) { return; }
	if(values.size()!=num_nodes) {
		set_number_of_nodes(values.size());
	}
	definitions.push_back(std::static_pointer_cast<pmTerm>(std::make_shared<pmParticle_system>("r", values, domain)));
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Deletes an instance with the given name. If no instance is existing with the name it
/// does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::delete_instance(std::string const& name) {
	std::vector<std::shared_ptr<pmTerm>>::iterator it = definitions.begin();
	for(;it!=definitions.end();it++) {
		if((*it)->get_name()==name) {
			if((*it).use_count()>1) {
				pLogger::warning_msgf("\"%s\" is in use and cannot be deleted.\n", name.c_str());
			} else {
				if(is_constant(*it)) {
					num_constants--;
				} else if(is_variable(*it)) {
					num_variables--;
				}
				definitions.erase(it);
			}
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of nodes registered in the workspace. For more information see e.g.
/// pmField and pmVariable classes.
/////////////////////////////////////////////////////////////////////////////////////////
size_t pmWorkspace::get_number_of_nodes() const {
	return num_nodes;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of an instance with the given name. The optional index argument 
/// helps to refer to the desired element of a field. Returns an empty tensor if no 
/// instance found with the given name.
/////////////////////////////////////////////////////////////////////////////////////////
pmTensor pmWorkspace::get_value(std::string const& name, int const& i/*=0*/) const {
	for(auto const& it:definitions) {
		if(it->get_name()==name) {
			return it->evaluate(i);
		}
	}
	pLogger::warning_msgf("No such instance: \"%s\"\n", name.c_str());
	return pmTensor{};
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns an instance with the given name. Returns an empty pointer if no such instance
//  found.
/////////////////////////////////////////////////////////////////////////////////////////
std::weak_ptr<pmTerm> pmWorkspace::get_instance(std::string const& name) const {
	for(auto const& it:definitions) {
		if(it->get_name()==name) {
			return std::weak_ptr<pmTerm>{it};
		}
	}
	pLogger::warning_msgf("No such instance: \"%s\"\n", name.c_str());
	return std::weak_ptr<pmTerm>{};
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the particle system of the workspace object.
/////////////////////////////////////////////////////////////////////////////////////////
std::weak_ptr<pmParticle_system> pmWorkspace::get_particle_system() const {
	std::shared_ptr<pmTerm> psys = get_instance("r").lock();
	return std::static_pointer_cast<pmParticle_system>(psys);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the content of the workspace.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::print() const {
	pLogger::headerf<LGY>(" Workspace (number of nodes: %i)", num_nodes);
	print_content<pmConstant>("Constants");
	print_content<pmVariable>("Variables");
	print_content<pmField>("Fields");
	pLogger::footerf<LGY>();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Sorts all fields and particle systems in the workspace based on the positions of the 
/// particles.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::sort_all_by_position() {
	std::vector<int> sorted_idx(num_nodes,0);
	std::iota(sorted_idx.begin(), sorted_idx.end(), 0);
	for(auto const& it:definitions) {
		if(it->get_name()=="r") {
			std::shared_ptr<pmParticle_system> psys = std::dynamic_pointer_cast<pmParticle_system>(it);
			psys->sort_field(sorted_idx);
			break;
		}
	}
	for(auto const& it:definitions) {
		std::shared_ptr<pmField> term = std::dynamic_pointer_cast<pmField>(it);
		if(term.use_count()>0 && term->get_name()!="r") {
			term->sort_field(sorted_idx);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the vector of definitions
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmTerm>> pmWorkspace::get_definitions() {
	return definitions;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Prints reserved names.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ void pmWorkspace::print_reserved_names() {
	pLogger::log<WHT>("Reserved names in workspace:\n");
	size_t i=1;
	for(auto const& it:reserved_names) {
		pLogger::log<WHT>("%i) %s\n", i, it.c_str());
		i++;
	}
	for(auto const& it:list_of_functions) {
		pLogger::log<WHT>("%i) %s\n", i, it.c_str());
		i++;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns deep copy of the object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmWorkspace> pmWorkspace::clone() const {
	return std::make_shared<pmWorkspace>(*this);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of variables stored in the workspace.
/////////////////////////////////////////////////////////////////////////////////////////
size_t pmWorkspace::get_number_of_variables() const {
	return num_variables;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of constants stored in the workspace.
/////////////////////////////////////////////////////////////////////////////////////////
size_t pmWorkspace::get_number_of_constants() const {
	return num_constants;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if any new instance can get the given name.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ bool pmWorkspace::is_reserved(std::string const& name) {
	for(auto const& it:reserved_names) {
		if(it==name) {
			return true;
		}
	}
	for(auto const& it:list_of_functions) {
		if(it==name) {
			return true;
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Resets the number of nodes. If new size is smaller, the remaining nodes are destroyed.
/// If new size is larger, the formerly deleted id-s are re-used first.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::set_number_of_nodes(size_t const& N) {
	if(N==num_nodes) { return; }
	if(N==0) { pLogger::error_msgf("Workspace size cannot be set to zero.\n"); }
	for(auto const& it:definitions) {
		std::shared_ptr<pmField> field = std::dynamic_pointer_cast<pmField>(it);
		if(field.use_count()>0) {
			if(field->get_name()=="id"){
				if(N<num_nodes) {
					for(int i=num_nodes; i>=N; i--) {
						deleted_ids.push((int)field->evaluate(i)[0]);
					}
				} else {
					field->set_number_of_nodes(N);
					for(int i=num_nodes; i<N; i++) {
						if(deleted_ids.empty()) {
							field->set_value(pmTensor{1,1,(float)i},i);	
						} else {
							field->set_value(pmTensor{1,1,(float)i},deleted_ids.top());
							deleted_ids.pop();
						}
					}
				}
			} else {
				field->set_number_of_nodes(N);				
			}
		}
	}
	num_nodes = N;
}