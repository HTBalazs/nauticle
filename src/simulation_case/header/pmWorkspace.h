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
    
#ifndef _WORKSPACE_H_
#define _WORKSPACE_H_

#include <iomanip>
#include <memory>
#include <stack>
#include <string>
#include "prolog/pLogger.h"
// #include "pmField.h"
#include "pmParticle_system.h"
#include "pmMath_test.h"
#include "c2c/c2CPP_declaration.h"

namespace Nauticle {
	/** This class contains all the definitions of variables and constants. It also
	//	stores the size of the fields enclosed in the variables. Since no variable
	//	or constant can exist without a proprietary workspace, the destructor destroys
	//  all definitions.
	*/
	class pmWorkspace final : public pmMath_test {
	private:
		static std::string const reserved_names[];
		std::vector<std::shared_ptr<pmSymbol>> definitions;
		size_t num_nodes=1;
		std::stack<int> deleted_ids;
		size_t num_constants;
		size_t num_variables;
		std::vector<std::shared_ptr<pmExpression>> interactions;
	private:
		bool verify_name(std::string const& name) const;
		bool is_constant(std::shared_ptr<pmSymbol> term) const;
		bool is_variable(std::shared_ptr<pmSymbol> term) const;
		bool is_constant_or_variable(std::shared_ptr<pmSymbol> term) const;
		void define_bases();
	public:
		pmWorkspace();
		pmWorkspace(pmWorkspace const& other);
		pmWorkspace(pmWorkspace&& other);
		pmWorkspace& operator=(pmWorkspace const& other);
		pmWorkspace& operator=(pmWorkspace&& other);
		bool operator==(pmWorkspace const& rhs) const;
		bool operator!=(pmWorkspace const& rhs) const;
		virtual ~pmWorkspace() override {}
		bool is_existing(std::string const& name) const;
		void merge(std::shared_ptr<pmWorkspace> other);
		void add_constant(std::string const& name, pmTensor const& value, bool const& hidden=false);
		void add_variable(std::string const& name, pmTensor const& value);
		void add_field(std::string const& name, pmTensor const& value=pmTensor{0}, bool const& sym=true, bool const& printable=true, std::string const& fname="");
		void add_field(std::string const& name, std::vector<pmTensor> const& values, bool const& sym=true, bool const& printable=true);
		void add_particle_system(std::vector<pmTensor> const& values, pmDomain const& domain);
		void delete_instance(std::string const& name);
		pmTensor get_value(std::string const& name, int const& i=0) const;
		std::weak_ptr<pmSymbol> get_instance(std::string const& name, bool const& safe=true) const;
		std::weak_ptr<pmParticle_system> get_particle_system() const;
		template <typename T> void print_content(std::string const& title) const;
		void print() const;
		bool sort_all_by_position();
		std::vector<std::shared_ptr<pmSymbol>> get_definitions();
		std::shared_ptr<pmWorkspace> clone() const;
		size_t get_number_of_nodes() const;
		size_t get_number_of_variables() const;
		size_t get_number_of_constants() const;
		static void print_reserved_names();
		static bool is_reserved(std::string const& name);
		template <typename T> std::vector<std::shared_ptr<T>> get(bool const& forced=false) const;
		void set_number_of_nodes(size_t const& N);
		std::vector<c2c::c2CPP_declaration> generate_declarations(std::string& init_code) const;
		void add_interaction(std::shared_ptr<pmExpression> ia);
		std::vector<std::shared_ptr<pmExpression>> const& get_interactions() const;
		void delete_particle(size_t const& i);
		void delete_particle_set(std::vector<size_t> const& delete_indices);
		void duplicate_particle(size_t const& i);
		bool number_of_particles_changed() const;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Prints the workspace content.
	/////////////////////////////////////////////////////////////////////////////////////////
	template<typename T> void pmWorkspace::print_content(std::string const& title) const {
		ProLog::pLogger::titlef<ProLog::LMA>(title.c_str());
		int count=0;
		for(auto const& it:get<T>()) {
			if(it->is_hidden()) { continue; }
			count++;
			ProLog::pLogger::logf<ProLog::YEL>("         %i) ",count);
			it->printv();
			ProLog::pLogger::logf("\n");
		}
		if(count==0) {
			ProLog::pLogger::logf<ProLog::WHT>("            < empty >\n");
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the instances of type T (stored in pmWorkspace) in an std::vector<T>.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T> std::vector<std::shared_ptr<T>> pmWorkspace::get(bool const& forced/*=false*/) const {
		std::vector<std::shared_ptr<T>> vecT;
		for(auto const& it:definitions) {
			if(it->is_hidden() && !forced) { continue; }
			std::shared_ptr<T> type = std::dynamic_pointer_cast<T>(it);
			if(type.use_count()>0) {
				vecT.push_back(type);
			}
		}
		return vecT;
	}
}

#endif //_WORKSPACE_H_