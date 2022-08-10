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

#include "pmPairs.h"
#include "pmSort.h"
#include "commonutils/Common.h"
#include <numeric>

using namespace Nauticle;

void pmPairs::update_pair_idx() {
	for(int i=0; i<first.size(); i++) {
		pair_index[first[i]].push_back(i);
		pair_index[second[i]].push_back(i);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Fills the list of particles with the corresponding pair indices.
/////////////////////////////////////////////////////////////////////////////////////////
void pmPairs::set_number_of_nodes(int const& num_particles) {
	pair_index.resize(num_particles);
	for(int i=0; i<first.size(); i++) {
		pair_index[first[i]].push_back(i);
		pair_index[second[i]].push_back(i);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a pair given by two particle indices to the mesh.
/////////////////////////////////////////////////////////////////////////////////////////
void pmPairs::add_pair(int const& i1, int const& i2, std::vector<double> const& new_values_ordered) {
	if(i1==i2) { return; }
	int const& p1 = i1<i2?i1:i2;
	int const& p2 = i1>i2?i1:i2;
	first.push_back(p1);
	second.push_back(p2);
	for(int i=0; i<pair_data.size(); i++) {
		pair_data[i].second.push_back(new_values_ordered[i]);
	}
	pair_index[p1].push_back(first.size()-1); 
	pair_index[p2].push_back(second.size()-1);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the ith pair.
/////////////////////////////////////////////////////////////////////////////////////////
void pmPairs::delete_marked_pairs() {
	if(delete_marker.empty()) {
		return;
	}
	Common::delete_indices(first, delete_marker);
	Common::delete_indices(second, delete_marker);
	for(auto& it:pair_data) {
		Common::delete_indices(it.second, delete_marker);
	}
	delete_marker.clear();
	std::fill(pair_index.begin(), pair_index.end(), std::vector<size_t>{});
	update_pair_idx();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clears all data.
/////////////////////////////////////////////////////////////////////////////////////////
void pmPairs::reset() {
	first.clear();
	second.clear();
	pair_data.clear();
	pair_index.clear();
	delete_marker.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of pairs in the mesh.
/////////////////////////////////////////////////////////////////////////////////////////
int pmPairs::get_number_of_pairs() const {
	return first.size();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the list of the first particles' indices.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<int> const& pmPairs::get_first() const {
	return first;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the list of the second particles' indices.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<int> const& pmPairs::get_second() const {
	return second;
}

void pmPairs::add_data(std::string const& name, double const& initial_value/*=0.0*/) {
	std::vector<double> vec;
	vec.resize(first.size());
	std::fill(vec.begin(),vec.end(),initial_value);
	pair_data.push_back(pmPair_data{name,vec});
}

std::vector<pmPair_data>& pmPairs::get_data() {
	return pair_data;
}

std::vector<pmPair_data> const& pmPairs::get_data() const {
	return pair_data;
}

std::vector<double>& pmPairs::get_data(std::string const& name) {
	for(auto& it:pair_data) {
		if(it.first==name) {
			return it.second;
		}
	}
	return pair_data.back().second;
}

std::vector<double> const& pmPairs::get_data(std::string const& name) const {
	for(auto const& it:pair_data) {
		if(it.first==name) {
			return it.second;
		}
	}
	return pair_data.back().second;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the list of pair indices relating to the ith particle.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<size_t> const& pmPairs::get_pair_index(size_t const& i) const {
	return pair_index[i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the list of pair indices of all particles.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::vector<size_t>> const& pmPairs::get_pair_index() const {
	return pair_index;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Mark the ith particle for deletion.
/////////////////////////////////////////////////////////////////////////////////////////
void pmPairs::mark_to_delete(size_t const& i) {
	delete_marker.push_back(i);
}