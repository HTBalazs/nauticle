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

#include <memory>
#include <mutex>
#include <functional>
#include "pmOperator.h"
#include "pmParticle_system.h"
#include "pmCounter.h"

namespace Nauticle {
	/** This interface forms the base for the interaction models. Since interactions
	//  are interpreted between particles it is inevitable to assign a particle system
	//  to the interaction model. The assignment is stored inside this interface.
	*/
	template <size_t S>
	class pmInteraction : public pmOperator<S>, public pmCounter<uint> {
		std::string declaration_type;
		using Func_ith = std::function<pmTensor(pmTensor const&, int const&, int const&, pmTensor const&, pmTensor const& guide)>;
	protected:
		std::string op_name;
		std::shared_ptr<pmParticle_system> psys;
		bool assigned=false;
	protected:
		pmInteraction();
		virtual ~pmInteraction() {}
		bool is_assigned() const override;
		int get_field_size() const override;
		bool cutoff_cell(pmTensor const& beta, pmTensor const& delta, size_t const& dimensions) const;
		pmTensor interact(int const& i, Func_ith contribute) const;
	public:
		void assign(std::shared_ptr<pmParticle_system> ps) override;
		virtual void write_to_string(std::ostream& os) const override;
		void set_declaration_type(std::string const& decl_type);
		std::string const& get_declaration_type() const;
		virtual std::string generate_evaluator_code(std::string const& i, std::string const& level) const override;
		virtual bool is_interaction() const override;
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
	/// Assigns SPH object to the given particle system.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	void pmInteraction<S>::assign(std::shared_ptr<pmParticle_system> ps) {
		this->psys = ps;
		if(this->psys.use_count()!=0) {
			this->assigned = true;
		}
		pmOperator<S>::assign(ps);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Checks if the SPH object is assigned to a particle system.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	bool pmInteraction<S>::is_assigned() const {
		return this->assigned;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the size of the particle system assigned to the interaction.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <size_t S>
	int pmInteraction<S>::get_field_size() const {
		if(psys.use_count()==0) {
			return 1;
		}
		return psys->get_field_size();
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
		pmTensor result;

		auto cell_iterator = psys->get_cell_iterator();
		pmTensor cell_size = psys->get_cell_size();
		pmTensor domain_minimum = psys->get_minimum();
		pmTensor domain_maximum = psys->get_maximum();
		pmTensor domain_cells = domain_maximum-domain_minimum;
		pmTensor domain_physical_minimum = psys->get_physical_minimum();
		pmTensor domain_physical_maximum = psys->get_physical_maximum();
		pmTensor domain_physical_size = psys->get_physical_size();
		pmTensor const beta = psys->get_boundary();
		pmTensor const ones = pmTensor::make_tensor(beta,1);
		size_t dimensions = psys->get_dimensions();
		std::vector<int> nbs;
		pmTensor pos_i = psys->get_value(i);
		pmTensor grid_pos_i = psys->grid_coordinates(pos_i);
		
		for(auto const& it:cell_iterator) {
			pmTensor grid_pos_j = grid_pos_i+it;
			pmTensor delta = -floor((grid_pos_j).divide_term_by_term(domain_cells));
			if(cutoff_cell(beta, delta, dimensions)) {
				continue;
			}
			pmTensor guide = delta.multiply_term_by_term(beta);
			grid_pos_j += delta.multiply_term_by_term(domain_cells-beta.multiply_term_by_term(domain_cells)+beta);
			int j = -1;
			std::vector<int> const& pidx = psys->get_cell_content(grid_pos_j,j);
			for(;j!=-1;j=pidx[j]) {
				pmTensor pos_j = psys->get_value(j);
				for(int k=0; k<beta.numel(); k++) {
					if(beta[k]==1) {
						pos_j[k] += delta[k]*(delta[k]-1)*(domain_physical_maximum[k]-pos_j[k]) + delta[k]*(delta[k]+1)*(domain_physical_minimum[k]-pos_j[k]);
					} else {
						pos_j[k] -= delta[k]*domain_physical_size[k];
					}
				}
				pmTensor rel_pos = pos_j-pos_i;
				result += contribute(rel_pos, i, j, cell_size, guide);
			}
		}
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
}

#endif //_INTERACTION_H_