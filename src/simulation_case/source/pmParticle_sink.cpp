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

#include "pmParticle_sink.h"
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <thread>
#include <mutex>

using namespace Nauticle;
using namespace ProLog;

std::shared_ptr<pmParticle_modifier> pmParticle_sink::clone_impl() const {
	return std::make_shared<pmParticle_sink>(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the content of the pmParticle_sink object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_sink::print() const {
	pmParticle_modifier::print();
	pLogger::titlef<LMA>("Particle sink");
	pLogger::logf<YEL>("        condition: "); condition->print(); pLogger::line_feed(1);
	pLogger::footerf<LBL>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads input if not read yet, and performs interpolation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmParticle_sink::update(size_t const& num_threads) {
	std::mutex mtx;
	int p_end = workspace->get_number_of_nodes();
	std::vector<size_t> del;
	auto process = [&](int const& start, int const& end){
		for(size_t i=start; i<end; i++) {
			if(condition->evaluate(i)[0]) {
				mtx.lock();
				del.push_back(i);
				mtx.unlock();
			}
		}
	};
	std::vector<std::thread> th;
	int number_of_threads = std::min((int)num_threads,p_end);
	int ppt = p_end/number_of_threads; // particle per thread
	for(int i=0; i<p_end; i+=ppt) {
		th.push_back(std::thread{process, i, i+ppt<p_end?i+ppt:p_end});
	}
	for(auto& it:th) {
		it.join();
	}
	workspace->delete_particle_set(del);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the deep copy of the current object.
/////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<pmParticle_sink> pmParticle_sink::clone() const {
    return std::static_pointer_cast<pmParticle_sink, pmParticle_modifier>(clone_impl());
}

