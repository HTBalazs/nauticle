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

#ifndef _SMALLEST_H_
#define _SMALLEST_H_

#include <vector>

namespace Nauticle {
	template <typename T, size_t N>
    class pmSmallest {
        std::vector<T> values;
        std::vector<int> indices;
    public:
        pmSmallest();
        void push_value(T const& v, int const& idx);
        std::vector<T> const& get_values() const;
        std::vector<int> const& get_indices() const;
        std::pair<T,int> operator[](size_t const& i) const;
    };
}

#endif //_SMALLEST_H_