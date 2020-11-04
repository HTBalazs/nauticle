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

#ifndef _INTERACTION_H_
#define _INTERACTION_H_

#include "pmOperator.h"
#include "pmInteraction_root.h"
#include "pmWorkspace.h"
#include "pmCounter.h"
#include "pmParticle.h"
#include <memory>
#include <mutex>
#include <vector>
#include <functional>

namespace Nauticle {
	template <size_t S>
	class pmInteraction : public pmOperator<S>, public pmCounter<uint>, public pmInteraction_root {
		std::string declaration_type;
		using Func_ith = std::function<pmTensor(pmTensor const&, int const&, int const&, pmTensor const&)>;
		using Func_pos = std::function<pmTensor(pmTensor const&, int const&, pmTensor const&)>;
	protected:
		std::string op_name;
	protected:
		pmInteraction();
		virtual ~pmInteraction() {}
		bool is_assigned() const override;
		bool cutoff_cell(pmTensor const& beta, pmTensor const& delta, size_t const& dimensions) const;
		pmTensor interact(int const& i, Func_ith contribute) const;
		pmTensor interact(pmTensor const& pos_i, Func_pos contribute) const;
	public:
		virtual void write_to_string(std::ostream& os) const override;
		void set_declaration_type(std::string const& decl_type);
		std::string const& get_declaration_type() const;
		virtual std::string generate_evaluator_code(std::string const& i, std::string const& level) const override;
		virtual bool is_interaction() const override;
		std::string const& get_name() const override;
		int get_field_size() const override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	pmInteraction<S>::pmInteraction() {
		this->name = "INTERACTION_";
	    char ch[5];
	    sprintf(&ch[0], "%04i", counter);
	    this->name += ch;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if the SPH object is assigned to a particle system.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	bool pmInteraction<S>::is_assigned() const {
		return workspace.use_count();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if query cell is cutoff.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	bool pmInteraction<S>::cutoff_cell(pmTensor const& beta, pmTensor const& delta, size_t const& dimensions) const {
		bool cutoff_cell = false;
		for(int k=0; k<dimensions; k++) {
			if(std::abs(beta[k])>=2.0-NAUTICLE_EPS && std::abs(delta[k])>NAUTICLE_EPS) {
				cutoff_cell = true;
				break;
			}
		}
		return cutoff_cell;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Calculates the interaction between adjacent particles using the given contribution 
	/// lambda-function.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	pmTensor pmInteraction<S>::interact(int const& i, Func_ith contribute) const {
		// std::vector<unsigned int> const& start = domain->get_particle_system()->get_domain()->get_start();
		// std::vector<unsigned int> const& end = domain->get_particle_system()->get_domain()->get_end();
		// pmTensor result;
		// std::vector<pmTensor> const& cell_iterator = domain->get_particle_system()->get_domain()->get_cell_iterator();
		// std::shared_ptr<pmDomain> domain = domain->get_particle_system()->get_domain();
		// pmTensor cell_size = domain->get_cell_size();
		// pmTensor domain_minimum = domain->get_minimum();
		// pmTensor domain_maximum = domain->get_maximum();
		// pmTensor domain_cells = domain_maximum-domain_minimum;
		// pmTensor domain_physical_minimum = domain->get_physical_minimum();
		// pmTensor domain_physical_maximum = domain->get_physical_maximum();
		// pmTensor domain_physical_size = domain->get_physical_size();
		// pmTensor domain_shift = domain->get_shift();
		// pmTensor const beta = domain->get_boundary();
		// pmTensor const ones = pmTensor::make_tensor(beta,1);
		// size_t dimensions = domain->get_dimensions();
		// pmTensor pos_i = domain->get_particle_system()->evaluate(i);
		// pmTensor grid_pos_i = domain->get_particle_system()->get_domain()->get_grid_position(pos_i);
		// for(auto const& it:cell_iterator) {
		// 	pmTensor grid_pos_j{grid_pos_i+it};
		// 	pmTensor delta = -floor((grid_pos_j).divide_term_by_term(domain_cells));
		// 	// Ignore cells through cutoff boundary
		// 	if(cutoff_cell(beta, delta, dimensions)) {
		// 		continue;
		// 	}
		// 	pmTensor guide = delta.multiply_term_by_term(beta);
		// 	// look for periodic & symmetric neighbour cells (ensemble formula)
		// 	grid_pos_j += delta.multiply_term_by_term(domain_cells-beta.multiply_term_by_term(domain_cells)+beta);
		// 	int hash_j = domain->get_particle_system()->get_domain()->calculate_hash_key_from_grid_position(grid_pos_j);
		// 	if(start[hash_j]!=0xFFFFFFFF) {
		// 		for(int j=start[hash_j]; j<=end[hash_j]; j++) {
		// 			pmTensor pos_j = domain->get_particle_system()->evaluate(j);
		// 			for(int k=0; k<beta.numel(); k++) {
		// 				if(beta[k]==1) {
		// 					pos_j[k] += delta[k]*(delta[k]-1)*(domain_physical_maximum[k]-pos_j[k]) + delta[k]*(delta[k]+1)*(domain_physical_minimum[k]-pos_j[k]);
		// 				} else {
		// 					pos_j[k] -= delta[k]*domain_physical_size[k];
		// 				}
		// 			}
		// 			pmTensor rel_pos = pos_j-pos_i;

		// 			result += contribute(rel_pos, i, j, cell_size, guide);
		// 		}
		// 	}
		// }
		// return result;

		pmTensor result;
		std::vector<pmParticle*> neibs_i;
		std::shared_ptr<pmParticle_system> psys = this->workspace->get_particle_system();
		pmParticle* p0 = &(psys->get_particle(0));
		pmTensor pos_i = psys->evaluate(i);
		workspace->get_neighbors(pos_i, neibs_i);
		for(auto const& it:neibs_i) {
			for(pmParticle* pj=it; pj!=nullptr; pj=pj->next) {
				int j = pj-p0;
				pmTensor pos_j = psys->evaluate(j);
				pmTensor rel_pos = pos_j-pos_i;
				result += contribute(rel_pos, i, j, workspace->get_cell_size());
			}
		}
		return result;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Calculates the interaction between adjacent particles using the given contribution lambda-function.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	pmTensor pmInteraction<S>::interact(pmTensor const& pos_i, Func_pos contribute) const {
		// std::vector<unsigned int> const& start = domain->get_particle_system()->get_domain()->get_start();
		// std::vector<unsigned int> const& end = domain->get_particle_system()->get_domain()->get_end();
		// pmTensor cell_size = domain->get_particle_system()->get_domain()->get_cell_size();
		// pmTensor grid_pos_i = domain->get_particle_system()->get_domain()->get_grid_position(pos_i);
		pmTensor result;
		// std::vector<pmTensor> const& cell_iterator = domain->get_particle_system()->get_domain()->get_cell_iterator();
		// for(auto const& it:cell_iterator) {
		// 	pmTensor grid_pos_j{grid_pos_i+it};
		// 	int hash_j = domain->get_particle_system()->get_domain()->calculate_hash_key_from_grid_position(grid_pos_j);
		// 	if(start[hash_j]!=0xFFFFFFFF) {
		// 		for(int j=start[hash_j]; j<=end[hash_j]; j++) {
		// 			pmTensor pos_j = domain->get_particle_system()->evaluate(j);
		// 			result += contribute(pos_j, j, cell_size);
		// 		}
		// 	}
		// }
		return result;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Writes object to string.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	void pmInteraction<S>::write_to_string(std::ostream& os) const {
		os << op_name << "(";
		for(int i=0; i<S; i++) {
			os << this->operand[i];
			if(i!=S-1) {
				os << ",";
			}
		}
		os << ")";
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Set the declaration type for the interaction.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	void pmInteraction<S>::set_declaration_type(std::string const& decl_type) {
		declaration_type = decl_type;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Get the declaration type for the interaction.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	std::string const& pmInteraction<S>::get_declaration_type() const {
		return declaration_type;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Generate the c++ code for the evaluation of the interaction.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	std::string pmInteraction<S>::generate_evaluator_code(std::string const& i, std::string const& level) const {
		return this->name + "->evaluate(" + i + "," + level + ")";
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Check if the current object requires nearest neighbor search.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	bool pmInteraction<S>::is_interaction() const {
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the interaction name.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	std::string const& pmInteraction<S>::get_name() const {
		return name;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the number of particles.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	int pmInteraction<S>::get_field_size() const {
	    return this->workspace->get_particle_system()->get_field_size();
	}
}

#endif //_INTERACTION_H_