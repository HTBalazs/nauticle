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

#include "pmMesh.h"
#include "pmSort.h"
#include "Color_define.h"
#include "commonutils/Common.h"
#include <numeric>

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmMesh::pmPairs::pmPairs(int const& num_particles) {
	pair_index.resize(num_particles);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmMesh::pmPairs::pmPairs(std::vector<int> const& fst, std::vector<int> const& snd, int const& num_particles) : first{fst}, second{snd} {
	pair_index.resize(num_particles);
	update_pair_idx();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set the number of particles.
/////////////////////////////////////////////////////////////////////////////////////////
void pmMesh::pmPairs::set_number_of_particles(size_t const& num_particles) {
	pair_index.resize(num_particles);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Fills the list of particles with the corresponding pair indices.
/////////////////////////////////////////////////////////////////////////////////////////
void pmMesh::pmPairs::update_pair_idx() {
	for(int i=0; i<first.size(); i++) {
		pair_index[first[i]].push_back(i);
		pair_index[second[i]].push_back(i);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sorts a 0..N vector by pair_vec and copies the result into sorted_pair_idx.
/// The content of sorted_pair_idx (if there's any) is destroyed.
/////////////////////////////////////////////////////////////////////////////////////////
void pmMesh::pmPairs::sort_lists(std::vector<int>& sorted_pair_idx, std::vector<int> const& pair_vec)  const {
	sorted_pair_idx.resize(pair_vec.size());
	std::iota(sorted_pair_idx.begin(), sorted_pair_idx.end(), 0);
	std::vector<int> copy = pair_vec;
	pmSort::sort_by_vector(sorted_pair_idx, copy, pmSort::ascending);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Fills the helper start and end vectors. Needs to be called for first and second 
/// elements of the pairs separately.
/////////////////////////////////////////////////////////////////////////////////////////
void pmMesh::pmPairs::update_helper_vectors(std::vector<int>& start, std::vector<int>& end, std::vector<int> const& sorted_pair_idx, std::vector<int> const& pair_vec, int const& N)  const {
	start.resize(N);
	end.resize(N);
	std::fill(start.begin(),start.end(),0xFFFFFFFF);
	std::fill(end.begin(),end.end(),0xFFFFFFFF);
	int count = 0;
	for(int i=0; i<N; i++) {
		int count_start = count;
		for(int j=count; j<sorted_pair_idx.size(); j++) {
			if(pair_vec[sorted_pair_idx[j]]==i) {
				count++;
			}
		}
		int count_end = count-1;
		if(count_start<=count_end) {
			start[i] = count_start;
			end[i] = count_end;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Updates the list of pairs according to the sorted_particle_idx. Needs to be called 
///for first and second elements of the pairs separately.
/////////////////////////////////////////////////////////////////////////////////////////
void pmMesh::pmPairs::update_pairs(std::vector<int>& pair_vec, std::vector<int> const& sorted_pair_idx, std::vector<int> const& start, std::vector<int> const& end, std::vector<int> const& sorted_particle_idx)  const {
	for(int i=0; i<sorted_particle_idx.size(); i++) {
		int old_id = sorted_particle_idx[i];
		int new_id = i;
		int start_id = start[old_id];
		int end_id = end[old_id];
		if(start_id!=0xFFFFFFFF) {
			for(int j=start_id; j<=end_id; j++) {
				pair_vec[sorted_pair_idx[j]] = new_id;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Performs the sorting of the list of pairs based on the given sorted_particle_idx.
/////////////////////////////////////////////////////////////////////////////////////////
void pmMesh::pmPairs::renumber_pairs(std::vector<int> const& sorted_particle_idx) {
	set_number_of_particles(sorted_particle_idx.size());
	update_pair_idx();
	pmSort::reorder(pair_index, sorted_particle_idx);
	sort_lists(sorted_first, first);
	sort_lists(sorted_second, second);
	update_helper_vectors(start_first, end_first, sorted_first, first, sorted_particle_idx.size());
	update_helper_vectors(start_second, end_second, sorted_second, second, sorted_particle_idx.size());
	update_pairs(first, sorted_first, start_first, end_first, sorted_particle_idx);
	update_pairs(second, sorted_second, start_second, end_second, sorted_particle_idx);
	delete_marker.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Adds a pair given by two particle indices to the mesh.
/////////////////////////////////////////////////////////////////////////////////////////
void pmMesh::pmPairs::add_pair(int const& i1, int const& i2, std::vector<pmTensor> const& new_values_ordered) {
	first.push_back(i1);
	second.push_back(i2);
	for(int i=0; i<pair_data.size(); i++) {
		pair_data[i].second.push_back(new_values_ordered[i]);
	}
	pair_index[i1].push_back(first.size()-1);
	pair_index[i2].push_back(second.size()-1);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the ith pair.
/////////////////////////////////////////////////////////////////////////////////////////
void pmMesh::pmPairs::delete_marked_pairs() {
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
void pmMesh::pmPairs::reset() {
	first.clear();
	second.clear();
	sorted_first.clear();
	sorted_second.clear();
	start_first.clear();
	end_first.clear();
	start_second.clear();
	end_second.clear();
	pair_data.clear();
	pair_index.clear();
	delete_marker.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of pairs in the mesh.
/////////////////////////////////////////////////////////////////////////////////////////
int pmMesh::pmPairs::size() const {
	return first.size();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the size of the mesh.
/////////////////////////////////////////////////////////////////////////////////////////
void pmMesh::pmPairs::print() const {
	if(first.size()!=0) {
		ProLog::pLogger::logf<ProLog::LYW>("\n            Particle pairs: ");
		ProLog::pLogger::logf<NAUTICLE_COLOR>("%i", first.size());
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the list of the first particles' indices.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<int> const& pmMesh::pmPairs::get_first() const {
	return first;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the list of the second particles' indices.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<int> const& pmMesh::pmPairs::get_second() const {
	return second;
}

void pmMesh::pmPairs::add_data(std::string const& name, pmTensor const& initial_value/*=pmTensor{1,1,0.0}*/) {
	std::vector<pmTensor> vec{first.size(),initial_value};
	pair_data.push_back(pmPair_data{name,vec});
}

std::vector<pmPair_data>& pmMesh::pmPairs::get_data() {
	return pair_data;
}

std::vector<pmPair_data> const& pmMesh::pmPairs::get_data() const {
	return pair_data;
}

std::vector<pmTensor>& pmMesh::pmPairs::get_data(std::string const& name) {
	for(auto& it:pair_data) {
		if(it.first==name) {
			return it.second;
		}
	}
	return pair_data.back().second;
}

std::vector<pmTensor> const& pmMesh::pmPairs::get_data(std::string const& name) const {
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
std::vector<size_t> const& pmMesh::pmPairs::get_pair_index(size_t const& i) const {
	return pair_index[i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Return the list of pair indices of all particles.
/////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::vector<size_t>> const& pmMesh::pmPairs::get_pair_index() const {
	return pair_index;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Mark the ith particle for deletion.
/////////////////////////////////////////////////////////////////////////////////////////
void pmMesh::pmPairs::mark_to_delete(size_t const& i) {
	delete_marker.push_back(i);
}