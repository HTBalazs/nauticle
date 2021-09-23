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

#include "pmParticle_emitter.h"
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <thread>
#include <mutex>

using namespace Nauticle;
using namespace ProLog;

std::shared_ptr<pmParticle_modifier> pmParticle_emitter::clone_impl() const {
	return std::make_shared<pmParticle_emitter>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the content of the pmParticle_emitter object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_emitter::print() const {
	pmParticle_modifier::print();
	pLogger::titlef<LMA>("Particle emitter");
	pLogger::logf<YEL>("        condition: "); condition->print(); pLogger::line_feed(1);
	pLogger::logf<YEL>("        initializers: ");
	pLogger::line_feed(1);
	for(auto const& it:initializer) {
		pLogger::logf<YEL>("          name: %s", it.field->get_name().c_str());
		pLogger::logf<YEL>(" | value: "); it.expression->print();
		pLogger::line_feed(1);
	}
	pLogger::footerf<LBL>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads input if not read yet, and performs interpolation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_emitter::update(size_t const& num_threads) {
	std::vector<pmTensor> positions = grid->get_merged_grid()->get_grid();
	for(auto const& jt:positions) {
		if(condition->evaluate(0)[0]) {
			workspace->duplicate_particle(0);
			size_t num_particles = workspace->get_number_of_nodes();
			workspace->get_particle_system().lock()->set_value(jt,num_particles-1);
			for(auto const& it:initializer) {
				it.field->set_value(it.expression->evaluate(0),num_particles-1);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the current object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_emitter> pmParticle_emitter::clone() const {
    return std::static_pointer_cast<pmParticle_emitter, pmParticle_modifier>(clone_impl());
}

void pmParticle_emitter::add_grid(std::shared_ptr<pmGrid_space> new_grid) {
	grid = new_grid;
}

void pmParticle_emitter::add_initializer(pmInitializer const& init) {
	initializer.push_back(init);
}