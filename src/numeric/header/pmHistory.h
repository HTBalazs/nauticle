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

#ifndef _PM_HISTORY_H_
#define _PM_HISTORY_H_

#include <deque>

namespace Nauticle {
	template <typename T>
	class pmHistory {
		std::deque<T> value;
		size_t depth = 1;
	public:
		pmHistory() {}
		pmHistory(T const& t);
		void set_storage_depth(size_t const& d);
		size_t get_storage_depth() const;
		pmHistory& operator=(T const& rhs);
		pmHistory& operator=(pmHistory const& rhs);
		bool operator==(pmHistory const& other) const;
		T const& operator[](size_t const& level) const;
		T& operator[](size_t const& level);
		operator const T&() const;
	};

	template <typename T>
	pmHistory<T>::pmHistory(T const& t) {
		value.push_front(t);
	}
	
	template <typename T>
	void pmHistory<T>::set_storage_depth(size_t const& d) {
		depth = d;
		while(value.size()>depth) {
			value.pop_back();
		}
	}
	
	template <typename T>
	size_t pmHistory<T>::get_storage_depth() const {
		return depth;
	}
	
	template <typename T>
	pmHistory<T>& pmHistory<T>::operator=(T const& rhs) {
		value.push_front(rhs);
		if(value.size()>depth) {
			value.pop_back();
		}
		return *this;
	}

	template <typename T>
	pmHistory<T>& pmHistory<T>::operator=(pmHistory<T> const& rhs) {
		return this->operator=(rhs[0]);
	}
	
	template <typename T>
	bool pmHistory<T>::operator==(pmHistory<T> const& other) const {
		if(this==&other) {
			return true;
		}
		if(this->depth!=other.depth) {
			return false;
		}
		return this->value==other.value;
	}
	
	template <typename T>
	T const& pmHistory<T>::operator[](size_t const& level) const {
		return value[level];
	}
	
	template <typename T>
	T& pmHistory<T>::operator[](size_t const& level) {
		return value[level];
	}

	template <typename T>
	pmHistory<T>::operator const T&() const {
		return value[0];
	}

}

#endif //_PM_HISTORY_H_






















