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

    For more information please visit: https://BalazsToth@bitbucket.org/BalazsToth/lemps
*/

#ifndef _INTERACTION_H_
#define _INTERACTION_H_

#include <memory>
#include <mutex>
#include <functional>
#include "pmOperator.h"
#include "pmParticle_system.h"

/** This interface forms the base for the interaction models. Since interactions
//  are interpreted between particles it is inevitable to assign a particle system
//  to the interaction model. The assignment is stored inside this interface.
*/
template <size_t S>
class pmInteraction : public pmOperator<S> {
	using Func_ith = std::function<pmTensor(pmTensor const&, int const&, int const&, float const&)>;
	using Func_pos = std::function<pmTensor(pmTensor const&, int const&, float const&)>;
protected:
	std::weak_ptr<pmParticle_system> psys;
	bool assigned=false;
protected:
	virtual ~pmInteraction() {}
	void assign(std::weak_ptr<pmParticle_system> ps) override;
	bool is_assigned() const override;
	int get_field_size() const override;
	pmTensor interact(int const& i, std::array<std::shared_ptr<pmExpression>, S> operand, Func_ith contribute) const;
	pmTensor interact(pmTensor const& pos_i, std::array<std::shared_ptr<pmExpression>, S> operand, Func_pos contribute) const;
};

/////////////////////////////////////////////////////////////////////////////////////////
/// Assigns SPH object to the given particle system.
/////////////////////////////////////////////////////////////////////////////////////////
template <size_t S>
void pmInteraction<S>::assign(std::weak_ptr<pmParticle_system> ps) {
	this->psys = ps;
	if(!this->psys.expired()) {
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
	if(psys.expired()) {
		return 1;
	}
	return psys.lock()->get_field_size();
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Calculates the interaction between adjacent particles using the given contribution 
/// lambda-function.
/////////////////////////////////////////////////////////////////////////////////////////
template <size_t S>
pmTensor pmInteraction<S>::interact(int const& i, std::array<std::shared_ptr<pmExpression>, S> operand, Func_ith contribute) const {
	std::shared_ptr<pmParticle_system> ps = psys.lock();
	std::vector<unsigned int> const& start = ps->get_particle_space()->get_start();
	std::vector<unsigned int> const& end = ps->get_particle_space()->get_end();
	float cell_size = ps->get_particle_space()->get_domain().get_cell_size();
	pmTensor pos_i = ps->get_value(i);
	pmTensor grid_pos_i = ps->get_particle_space()->get_grid_position(pos_i);
	pmTensor result;
	std::vector<pmTensor> const& cell_iterator = ps->get_particle_space()->get_cell_iterator();
		pmDomain domain = ps->get_particle_space()->get_domain();
		pmTensor domain_cells = domain.get_maximum()-domain.get_minimum();
		pmTensor domain_minimum = domain.get_minimum();
		pmTensor domain_physical_minimum = domain.get_physical_minimum();
		pmTensor domain_size = domain.get_physical_size();
	for(auto const& it:cell_iterator) {
		pmTensor grid_pos_j{grid_pos_i+it};
		grid_pos_j = grid_pos_j-floor((grid_pos_j-domain_minimum).divide_term_by_term(domain_cells)).multiply_term_by_term(domain_cells);
		int hash_j = ps->get_particle_space()->calculate_hash_key_from_grid_position(grid_pos_j);
		if(start[hash_j]!=0xFFFFFFFF) {
			for(int j=start[hash_j]; j<=end[hash_j]; j++) {
				pmTensor pos_j = ps->get_value(j);
				pmTensor rel_pos = pos_j-pos_i;
				rel_pos -= floor((rel_pos-domain_physical_minimum).divide_term_by_term(domain_size)).multiply_term_by_term(domain_size);
				result += contribute(rel_pos, i, j, cell_size);
			}
		}
	}
	return result;
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Calculates the interaction between adjacent particles using the given contribution lambda-function.
/////////////////////////////////////////////////////////////////////////////////////////
template <size_t S>
pmTensor pmInteraction<S>::interact(pmTensor const& pos_i, std::array<std::shared_ptr<pmExpression>, S> operand, Func_pos contribute) const {
	std::shared_ptr<pmParticle_system> ps = psys.lock();
	std::vector<unsigned int> const& start = ps->get_particle_space()->get_start();
	std::vector<unsigned int> const& end = ps->get_particle_space()->get_end();
	float cell_size = ps->get_particle_space()->get_domain().get_cell_size();
	pmTensor grid_pos_i = ps->get_particle_space()->get_grid_position(pos_i);
	pmTensor result;
	std::vector<pmTensor> const& cell_iterator = ps->get_particle_space()->get_cell_iterator();
	for(auto const& it:cell_iterator) {
		pmTensor grid_pos_j{grid_pos_i+it};
		int hash_j = ps->get_particle_space()->calculate_hash_key_from_grid_position(grid_pos_j);
		if(start[hash_j]!=0xFFFFFFFF) {
			for(int j=start[hash_j]; j<=end[hash_j]; j++) {
				pmTensor pos_j = ps->get_value(j);
				result += contribute(pos_j, j, cell_size);
			}
		}
	}
	return result;
}

#endif //_INTERACTION_H_