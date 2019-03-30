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

#include "pmLong_range.h"
#include "Color_define.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmLong_range::pmPairs::pmPairs(std::vector<int> const& fst, std::vector<int> const& snd) : first{fst}, second{snd} {
    initial_length.resize(first.size());
    ID.resize(first.size());
    hysteron.resize(first.size());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sorts a 0..N vector by pair_vec and copies the result into sorted_pair_idx.
/// The content of sorted_pair_idx (if there's any) is destroyed.
/////////////////////////////////////////////////////////////////////////////////////////
void pmLong_range::pmPairs::sort_lists(std::vector<int>& sorted_pair_idx, std::vector<int> const& pair_vec)  const {
    sorted_pair_idx.resize(pair_vec.size());
    std::iota(sorted_pair_idx.begin(), sorted_pair_idx.end(), 0);
    std::vector<int> copy = pair_vec;
    pmSort::sort_by_vector(sorted_pair_idx, copy, pmSort::ascending);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Fills the helper start and end vectors. Needs to be called for first and second 
/// elements of the pairs separately.
/////////////////////////////////////////////////////////////////////////////////////////
void pmLong_range::pmPairs::update_helper_vectors(std::vector<int>& start, std::vector<int>& end, std::vector<int> const& sorted_pair_idx, std::vector<int> const& pair_vec, int const& N)  const {
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
void pmLong_range::pmPairs::update_pairs(std::vector<int>& pair_vec, std::vector<int> const& sorted_pair_idx, std::vector<int> const& start, std::vector<int> const& end, std::vector<int> const& sorted_particle_idx)  const {
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
/// Adds a pair given by two particle indices to the mesh.
/////////////////////////////////////////////////////////////////////////////////////////
void pmLong_range::pmPairs::add_pair(int const& i1, int const& i2, double const& l0) {
        first.push_back(i1);
        second.push_back(i2);
        initial_length.push_back(l0);
        ID.add_id(ID.size()+1);
        hysteron.push_back(pmHysteron{});
}

int pmLong_range::pmPairs::get_pair_index(int const& i1, int const& i2) const {
    for(int i=0; i<first.size(); i++) {
        if((first[i]==i1 && second[i]==i2) || (first[i]==i2 && second[i]==i1)) {
            return i;
        }
    }
    return -1;
}

double pmLong_range::pmPairs::get_initial_length(int const& i) const {
    return initial_length[i];
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the ith pair.
/////////////////////////////////////////////////////////////////////////////////////////
void pmLong_range::pmPairs::delete_pair(int const& i) {
    first.erase(first.begin()+i);
    second.erase(second.begin()+i);
    initial_length.erase(initial_length.begin()+i);
    ID.delete_id(i);
    hysteron.erase(hysteron.begin()+i);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Performs the sorting of the list of pairs based on the given sorted_particle_idx.
/////////////////////////////////////////////////////////////////////////////////////////
void pmLong_range::pmPairs::sort_pairs(std::vector<int> const& sorted_particle_idx) {
    sort_lists(sorted_first, first);
    sort_lists(sorted_second, second);
    update_helper_vectors(start_first, end_first, sorted_first, first, sorted_particle_idx.size());
    update_helper_vectors(start_second, end_second, sorted_second, second, sorted_particle_idx.size());
    update_pairs(first, sorted_first, start_first, end_first, sorted_particle_idx);
    update_pairs(second, sorted_second, start_second, end_second, sorted_particle_idx);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Clears all data.
/////////////////////////////////////////////////////////////////////////////////////////
void pmLong_range::pmPairs::reset() {
    first.clear();
    second.clear();
    sorted_first.clear();
    sorted_second.clear();
    start_first.clear();
    end_first.clear();
    start_second.clear();
    end_second.clear();
    initial_length.clear();
    ID.reset();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the number of pairs in the mesh.
/////////////////////////////////////////////////////////////////////////////////////////
int pmLong_range::pmPairs::size() const {
    return first.size();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the ith id.
/////////////////////////////////////////////////////////////////////////////////////////
int pmLong_range::pmPairs::get_pair_id(int const& i1, int const& i2) const {
    for(int i=0; i<first.size(); i++) {
        if((first[i]==i1 && second[i]==i2) || (first[i]==i2 && second[i]==i1)) {
            return ID[i];
        }
    }
    return -1;
}


void pmLong_range::pmPairs::print() const {
    if(first.size()!=0) {
        ProLog::pLogger::logf<ProLog::LYW>("\n            Particle pairs: ");
        ProLog::pLogger::logf<NAUTICLE_COLOR>("%i", first.size());
    }
}

std::vector<int> const& pmLong_range::pmPairs::get_first() const {
    return first;
}

std::vector<int> const& pmLong_range::pmPairs::get_second() const {
    return second;
}

pmIdentifier<int> const& pmLong_range::pmPairs::get_id() const {
    return ID;
}

pmHysteron& pmLong_range::pmPairs::get_hysteron(int const& i) {
    return hysteron[i];
}

pmHysteron const& pmLong_range::pmPairs::get_hysteron(int const& i) const {
    return hysteron[i];
}

typename pmLong_range::pmPairs const& pmLong_range::get_pairs() const {
    return pairs;
}

typename pmLong_range::pmPairs& pmLong_range::get_pairs() {
    return pairs;
}

void pmLong_range::sort_pairs(std::vector<int> const& sorted_idx) {
    pairs.sort_pairs(sorted_idx);
}

std::vector<pmHysteron> const& pmLong_range::pmPairs::get_hysteron() const {
    return hysteron;
}
std::vector<double> const& pmLong_range::pmPairs::get_initial_length() const {
    return initial_length;
}