/*
    Copyright 2016 Balazs Toth
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
    
#ifndef _SORT_H_
#define _SORT_H_

#include <vector>
#include <algorithm>
#include <utility>
#include "prolog/pLogger.h"

namespace pmSort {
/////////////////////////////////////////////////////////////////////////////////////////
/// Implements ascending.
/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T> bool ascending(std::pair<T,int> const& a, std::pair<T,int> const& b) {
		return a.second < b.second;
	}
/////////////////////////////////////////////////////////////////////////////////////////
/// Implements descending.
/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T> bool descending(std::pair<T,int> const& a, std::pair<T,int> const& b) {
		return a.second > b.second;
	}
/////////////////////////////////////////////////////////////////////////////////////////
/// Pairs the givan a and b vectors and stores the result in the packed vector.
/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T> void pack(std::vector<T> const& a, std::vector<int> const& b, std::vector<std::pair<T,int>>& packed) {
		for(size_t i=0; i<a.size(); ++i) {
	        packed.push_back(std::make_pair(a[i], b[i]));
	    }
	}
/////////////////////////////////////////////////////////////////////////////////////////
/// Separates the vectors stored in the given vector of pairs and stores them in the a and b vectors.
/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T> void unpack(std::vector<std::pair<T,int>>& packed, std::vector<T>& a, std::vector<int>& b) {
        for(size_t i=0; i<a.size(); ++i) {
	        a[i] = packed[i].first;
	        b[i] = packed[i].second;
	    }
	}
/////////////////////////////////////////////////////////////////////////////////////////
/// Sorts a vector based on another. The third parameter governs the order.
/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T> void sort_by_vector(std::vector<T>& to_sort, std::vector<int>& sort_by, bool(*compare)(std::pair<T,int> const&, std::pair<T,int> const&)) {
		using Pair = std::pair<T,int>;
	    std::vector<Pair> packed;
	    pack<T>(to_sort, sort_by, packed);
	    std::sort(std::begin(packed), std::end(packed), compare);
	    unpack<T>(packed, to_sort, sort_by);
	}
/////////////////////////////////////////////////////////////////////////////////////////
/// Reorders a vector based on another vector. Here no sorting is performed.
/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T> void reorder(std::vector<T>& to_reorder, std::vector<int> const& reorder_by) {
		if(to_reorder.size() != reorder_by.size()) {
			pLogger::warning_msgf("Reorder requires vectors of identical sizes.\n");
			return;
		}
		std::vector<T> copy = to_reorder;
	    for(int i=0; i<reorder_by.size(); ++i) {
	        to_reorder[i] = copy[reorder_by[i]];
	    }
	}
};

#endif // _SORT_H_