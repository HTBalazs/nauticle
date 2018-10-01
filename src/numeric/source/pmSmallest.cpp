/*
    Copyright 2016-2018 Balazs Toth
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

#include "pmSmallest.h" 

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
template <typename T, size_t N>
pmSmallest<T,N>::pmSmallest() {
    values.reserve(N);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Pushes the given value to the container if it is smaller than the max element.
/////////////////////////////////////////////////////////////////////////////////////////
template <typename T, size_t N>
void pmSmallest<T,N>::push_value(T const& v, int const& idx) {
    if(values.size()<N) {
        values.push_back(v);
        indices.push_back(idx);
        return;
    }
    auto it = max_element(std::begin(values), std::end(values));
    if(*it>v) {
        *it = v;
        indices[it-values.begin()] = idx;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the list of minimum elements.
/////////////////////////////////////////////////////////////////////////////////////////
template <typename T, size_t N>
std::vector<T> const& pmSmallest<T,N>::get_values() const {
    return values;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the list of minimum elements.
/////////////////////////////////////////////////////////////////////////////////////////
template <typename T, size_t N>
std::vector<int> const& pmSmallest<T,N>::get_indices() const {
    return indices;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the ith element of the list.
/////////////////////////////////////////////////////////////////////////////////////////
template <typename T, size_t N>
std::pair<T,int> pmSmallest<T,N>::operator[](size_t const& i) const {
    return std::pair<T,int> {values[i],indices[i]};
}










