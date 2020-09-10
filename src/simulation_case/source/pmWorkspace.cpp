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

#include "pmWorkspace.h"
#include <numeric>
#include "pmConstant.h"
#include "pmVariable.h"
#include "pmLong_range.h"

using namespace Nauticle;

std::string const pmWorkspace::reserved_names[] = {"id", "true", "false", "pi", "Wp01110", "Wp01120", "Wp01130", "Wp11110", "Wp11120", "Wp11130", "Wp22210", "Wp22220", "Wp22230", "Wp32210", "Wp32220", "Wp32230", "Wp52210", "Wp52220", "Wp52230", "We21010", "We21020", "We21030", "domain_min", "domain_max", "cell_size", "ASCII", "BINARY", "periodic", "symmetric", "cutoff", "e_i", "e_j", "e_k", "simulation", "workspace", "case", "variables", "constants", "fields", "particle_system", "parameter_space", "domain", "grid", "equations", "condition", "write_step", "substeps", "all_steps"};

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmWorkspace::pmWorkspace() {
	std::vector<pmTensor> id(1);
	std::iota(id.begin(), id.end(), pmTensor{1,1,0});
	this->add_field("id", id);
	this->add_constant("true", pmTensor{1,1,1}, true);
	this->add_constant("false", pmTensor{1,1,0}, true);
	this->add_constant("pi", pmTensor{1,1,NAUTICLE_PI}, true);
	this->add_constant("Wp01110", pmTensor{1,1,0}, true);
	this->add_constant("Wp01120", pmTensor{1,1,1}, true);
	this->add_constant("Wp01130", pmTensor{1,1,2}, true);
	this->add_constant("Wp11110", pmTensor{1,1,3}, true);
	this->add_constant("Wp11120", pmTensor{1,1,4}, true);
	this->add_constant("Wp11130", pmTensor{1,1,5}, true);
	this->add_constant("Wp22210", pmTensor{1,1,6}, true);
	this->add_constant("Wp22220", pmTensor{1,1,7}, true);
	this->add_constant("Wp22230", pmTensor{1,1,8}, true);
	this->add_constant("Wp32210", pmTensor{1,1,9}, true);
	this->add_constant("Wp32220", pmTensor{1,1,10}, true);
	this->add_constant("Wp32230", pmTensor{1,1,11}, true);
	this->add_constant("Wp52210", pmTensor{1,1,12}, true);
	this->add_constant("Wp52220", pmTensor{1,1,13}, true);
	this->add_constant("Wp52230", pmTensor{1,1,14}, true);
	this->add_constant("We21010", pmTensor{1,1,15}, true);
	this->add_constant("We21020", pmTensor{1,1,16}, true);
	this->add_constant("We21030", pmTensor{1,1,17}, true);
	this->add_constant("ASCII", pmTensor{1,1,0}, true);
	this->add_constant("BINARY", pmTensor{1,1,1}, true);
	this->add_constant("periodic", pmTensor{1,1,0}, true);
	this->add_constant("symmetric", pmTensor{1,1,1}, true);
	this->add_constant("cutoff", pmTensor{1,1,2}, true);
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

bool pmWorkspace::operator==(pmWorkspace const& rhs) const {
	if(this->num_nodes != rhs.num_nodes) {
		return false;
	}
	for(auto const& it:this->get<pmConstant>()) {
		std::string name = it->get_name();
		auto rhs_instance = std::dynamic_pointer_cast<pmConstant>(rhs.get_instance(name,false).lock());
		if(rhs_instance.use_count()!=0) {
			if(*it != *rhs_instance) {
				return false;
			}
		} else {
			return false;
		}
	}
	for(auto const& it:this->get<pmVariable>()) {
		std::string name = it->get_name();
		auto rhs_instance = std::dynamic_pointer_cast<pmVariable>(rhs.get_instance(name,false).lock());
		if(rhs_instance.use_count()!=0) {
			if(*it != *rhs_instance) {
				return false;
			}
		} else {
			return false;
		}
	}
	for(auto const& it:this->get<pmField>()) {
		std::string name = it->get_name();
		auto rhs_instance = std::dynamic_pointer_cast<pmField>(rhs.get_instance(name,false).lock());
		if(rhs_instance.use_count()!=0) {
			if(*it != *rhs_instance) {
				return false;
			}
		} else {
			return false;
		}
	}
	if(*this->get_particle_system().lock() != *rhs.get_particle_system().lock()) {
		return false;
	}
	
	for(auto const& it:rhs.get<pmConstant>()) {
		std::string name = it->get_name();
		auto this_instance = std::dynamic_pointer_cast<pmConstant>(this->get_instance(name,false).lock());
		if(this_instance.use_count()!=0) {
			if(*it != *this_instance) {
				return false;
			}
		} else {
			return false;
		}
	}
	for(auto const& it:rhs.get<pmVariable>()) {
		std::string name = it->get_name();
		auto this_instance = std::dynamic_pointer_cast<pmVariable>(this->get_instance(name,false).lock());
		if(this_instance.use_count()!=0) {
			if(*it != *this_instance) {
				return false;
			}
		} else {
			return false;
		}
	}
	for(auto const& it:rhs.get<pmField>()) {
		std::string name = it->get_name();
		auto this_instance = std::dynamic_pointer_cast<pmField>(this->get_instance(name,false).lock());
		if(this_instance.use_count()!=0) {
			if(*it != *this_instance) {
				return false;
			}
		} else {
			return false;
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Implements the non-identity check.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmWorkspace::operator!=(pmWorkspace const& rhs) const {
	return !this->operator==(rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the term is a constant.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmWorkspace::is_constant(std::shared_ptr<pmSymbol> term) const {
	return std::dynamic_pointer_cast<pmConstant>(term).use_count()!=0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the term is a variable.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmWorkspace::is_variable(std::shared_ptr<pmSymbol> term) const {
	return std::dynamic_pointer_cast<pmVariable>(term).use_count()!=0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Checks if the term is a constant or variable.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmWorkspace::is_constant_or_variable(std::shared_ptr<pmSymbol> term) const {
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
		ProLog::pLogger::warning_msgf("\"%s\" is reserved or already existing.\n", name.c_str());
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
	if(name=="dt") {
		ProLog::pLogger::warning_msgf("Constant time step may cause problems. Use it as variable instead.");
	}
	definitions.push_back(std::static_pointer_cast<pmSymbol>(std::make_shared<pmConstant>(name, value, hidden)));
	num_constants++;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new variable to the workspace with an optional initialization value. If an 
/// instance is already existing with the same name it does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::add_variable(std::string const& name, pmTensor const& value/*=pmTensor{0}*/) {
	if(!verify_name(name)) return;
	definitions.push_back(std::static_pointer_cast<pmSymbol>(std::make_shared<pmVariable>(name, value)));
	num_variables++;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new field to the workspace with an optional initialization value. If an 
/// instance is already existing with the same name it does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::add_field(std::string const& name, pmTensor const& value/*=pmTensor{0}*/, bool const& sym/*=true*/, bool const& printable/*=true*/) {
	if(!verify_name(name)) return;
	definitions.push_back(std::static_pointer_cast<pmSymbol>(std::make_shared<pmField>(name, num_nodes, value, sym, printable)));
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a new field to the workspace with an initialization tensor field. If an 
/// instance is already existing with the same name it does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::add_field(std::string const& name, std::vector<pmTensor> const& values, bool const& sym/*=true*/, bool const& printable/*=true*/) {
	if(!verify_name(name) && name!="id") return;
	if(values.size()!=num_nodes) {
		ProLog::pLogger::error_msgf("Inconsistent size of field \"%s\".\n",name.c_str());
	}
	definitions.push_back(std::static_pointer_cast<pmSymbol>(std::make_shared<pmField>(name, values, sym, printable)));
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Defines the bases unit vectors for the domain.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::define_bases() {
	double dims = this->get_particle_system().lock()->get_particle_space()->get_domain().get_dimensions();
	std::string bases[] = {"e_i", "e_j", "e_k"};
	for(int i=0; i<dims; i++) {
		this->add_constant(bases[i], pmTensor::make_identity(dims).sub_tensor(0,dims-1,i,i), true);
	}
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
	definitions.push_back(std::static_pointer_cast<pmSymbol>(std::make_shared<pmParticle_system>("r", values, domain)));
	define_bases();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Deletes an instance with the given name. If no instance is existing with the name it
/// does nothing.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::delete_instance(std::string const& name) {
	std::vector<std::shared_ptr<pmSymbol>>::iterator it = definitions.begin();
	for(;it!=definitions.end();it++) {
		if((*it)->get_name()==name) {
			if((*it).use_count()>1) {
				ProLog::pLogger::warning_msgf("\"%s\" is in use and cannot be deleted.\n", name.c_str());
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
	ProLog::pLogger::warning_msgf("No such instance: \"%s\"\n", name.c_str());
	return pmTensor{};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns an instance with the given name. Returns an empty pointer if no such instance
//  found.
/////////////////////////////////////////////////////////////////////////////////////////
std::weak_ptr<pmSymbol> pmWorkspace::get_instance(std::string const& name, bool const& safe/*=true*/) const {
	for(auto const& it:definitions) {
		if(it->get_name()==name) {
			return std::weak_ptr<pmSymbol>{it};
		}
	}
	if(safe) {
		ProLog::pLogger::warning_msgf("No such instance: \"%s\"\n", name.c_str());
	}
	return std::weak_ptr<pmSymbol>{};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the particle system of the workspace object.
/////////////////////////////////////////////////////////////////////////////////////////
std::weak_ptr<pmParticle_system> pmWorkspace::get_particle_system() const {
	std::shared_ptr<pmSymbol> psys = get_instance("r").lock();
	return std::static_pointer_cast<pmParticle_system>(psys);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the content of the workspace.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::print() const {
	ProLog::pLogger::headerf<ProLog::LGY>(" Workspace (number of nodes: %i)", num_nodes);
	print_content<pmConstant>("Constants");
	print_content<pmVariable>("Variables");
	print_content<pmField>("Fields");
	ProLog::pLogger::footerf<ProLog::LGY>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sorts all fields and particle systems in the workspace based on the positions of the 
/// particles.
/////////////////////////////////////////////////////////////////////////////////////////
bool pmWorkspace::sort_all_by_position() {
	std::shared_ptr<pmParticle_system> psys = this->get<pmParticle_system>()[0];
	if(psys->is_sorted()) { return true; }
	std::vector<size_t> del;
	psys->restrict_particles(del);
	this->delete_particle_set(del);
	if(num_nodes==0) {
		ProLog::pLogger::error_msgf("Workspace size cannot be set to zero.\n");
	}
	bool success = psys->sort_field();
	if(success) {
		std::vector<int> sorted_idx = psys->get_sorted_idx();
		for(auto const& it:definitions) {
			std::shared_ptr<pmField> term = std::dynamic_pointer_cast<pmField>(it);
			if(term.use_count()>0 && term->get_name()!="r") {
				success &= term->sort_field(sorted_idx);
			}
		}
		for(auto& it:interactions) {
			it->update();
		}
	}
	return success;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the vector of definitions
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<pmSymbol>> pmWorkspace::get_definitions() {
	return definitions;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints reserved names.
/////////////////////////////////////////////////////////////////////////////////////////
/*static*/ void pmWorkspace::print_reserved_names() {
	ProLog::pLogger::log<ProLog::WHT>("Reserved names in workspace:\n");
	size_t i=1;
	for(auto const& it:reserved_names) {
		ProLog::pLogger::log<ProLog::WHT>("%i) %s\n", i, it.c_str());
		i++;
	}
	for(auto const& it:list_of_functions) {
		ProLog::pLogger::log<ProLog::WHT>("%i) %s\n", i, it.c_str());
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
	if(N==0) { ProLog::pLogger::error_msgf("Workspace size cannot be set to zero.\n"); }
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
							field->set_value(pmTensor{1,1,(double)i},i);	
						} else {
							field->set_value(pmTensor{1,1,(double)i},deleted_ids.top());
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

std::vector<c2c::c2CPP_declaration> pmWorkspace::generate_declarations(std::string& init_code) const {
	std::vector<c2c::c2CPP_declaration> declaration;
	for(auto const& it:this->get<pmConstant>(true)) {
		declaration.push_back(c2c::c2CPP_declaration{"std::shared_ptr<pmConstant>", "ws_" + it->get_name(), false, "", ""});
        init_code += "\t\tws_" + it->get_name() + " = std::dynamic_pointer_cast<pmConstant>(ws->get_instance(\"" + it->get_name() + "\").lock());\n"; 
    }
    for(auto const& it:this->get<pmVariable>()) {
        declaration.push_back(c2c::c2CPP_declaration{"std::shared_ptr<pmVariable>", "ws_" + it->get_name(), false, "", ""});
        init_code += "\t\tws_" + it->get_name() + " = std::dynamic_pointer_cast<pmVariable>(ws->get_instance(\"" + it->get_name() + "\").lock());\n";
    }
    for(auto const& it:this->get<pmField>()) {
        std::string type;
        if(it->get_name()=="r") {
            type = "pmParticle_system";
        } else {
            type = "pmField";
        }
		declaration.push_back(c2c::c2CPP_declaration{"std::shared_ptr<" + type + ">", "ws_" + it->get_name(), false, "", ""});
        init_code += "\t\tws_" + it->get_name() + " = std::dynamic_pointer_cast<" + type + ">(ws->get_instance(\"" + it->get_name() + "\").lock());\n";
    }
    size_t i = 0;
    for(auto const& it:interactions) {
        declaration.push_back(c2c::c2CPP_declaration{"std::shared_ptr<pmExpression>", it->get_name(), false, "", ""});
        init_code += "\t\t" + it->get_name() + " = ws->get_interactions()[" + std::to_string(i) + "];\n";
        i++;
    }
    return declaration;
}


void pmWorkspace::add_interaction(std::shared_ptr<pmExpression> ia) {
	interactions.push_back(ia);
}

std::vector<std::shared_ptr<pmExpression>> const& pmWorkspace::get_interactions() const {
	return interactions;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Removes the ith particle together with its field values from the workspace. 
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::delete_particle(size_t const& i) {
	if(num_nodes==1) {
		ProLog::pLogger::warning_msgf("Cannot delete the last particle.");
	}
	for(auto& it:this->get<pmField>()) {
		if(it->get_name()=="id") {
			deleted_ids.push(it->evaluate(i,0)[0]);
		}
		it->delete_member(i);
	}
	num_nodes--;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Removes the particles and field values with the indices given in the delete_indices vector.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::delete_particle_set(std::vector<size_t> const& delete_indices) {
	if(delete_indices.empty()) {
		return;
	}
	for(auto& it:this->get<pmField>()) {
		if(it->get_name()=="id") {
			for(auto const& idx_it:delete_indices) {
				deleted_ids.push(it->evaluate(idx_it,0)[0]);
			}
		}
		it->delete_set(delete_indices);
	}
	num_nodes -= delete_indices.size();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Duplicates particle with the given index. Field values are also duplicated.
/////////////////////////////////////////////////////////////////////////////////////////
void pmWorkspace::duplicate_particle(size_t const& i) {
	for(auto& it:this->get<pmField>()) {
		it->duplicate_member(i);
		if(it->get_name()=="id") {
			if(deleted_ids.empty()) {
				it->set_value(num_nodes,num_nodes);
			} else {
				it->set_value(deleted_ids.top(),num_nodes);
				deleted_ids.pop();
			}
		}
	}
	num_nodes++;
}


