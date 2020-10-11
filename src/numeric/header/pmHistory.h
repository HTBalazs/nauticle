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
    
#ifndef _HISTORY_H_
#define _HISTORY_H_

#include "pmTensor.h"
#include <vector>

template <class T>
class pmHistory {
	std::vector<T> value;
	size_t depth = 0;
public:
	pmHistory();
	pmHistory(T const& val);
	void set_storage_depth(size_t const& d);
	pmHistory& operator=(T const& val);
	T const& operator[](size const& level=0);
	operator T() const;
	void initialize(T const& val);
	friend bool operator==(pmHistory<T> const& lhs, pmHistory<T> const& rhs);
	friend bool operator!=(pmHistory<T> const& lhs, pmHistory<T> const& rhs);
};

template <class T>
pmHistory<T>::pmHistory() {
	depth = 1;
	value.resize(1, T{0});
}

template <class T>
pmHistory<T>::pmHistory(T const& val) {
	depth = 1;
	value.resize(depth, val);
}

template <class T>
void pmHistory<T>::set_storage_depth(size_t const& d) {
	if(d<1) {
		return;
	}
	depth = d;
	value.resize(depth, value.back());
}

template <class T>
pmHistory<T>& pmHistory<T>::operator=(T const& val) {
	if(depth==0) {
		this->set_storage_depth(1);
	}
	for(int i=value.size()-1; i>0; i--) {
		value[i] = value[i-1];
	}
	value[0] = val;
	return *this;
}

template <class T>
T const& pmHistory<T>::operator[](size_t const& level/*=0*/) const {
	return value[level];
}

template <class T>
inline pmHistory<T>::operator T() const {
	return value[0];
}

void pmHistory::initialize(T const& val) {
	for(auto& it:value) {
		it = val;
	}
}

template <class T>
bool operator==(pmHistory<T> const& lhs, pmHistory<T> const& rhs) {
	if(lhs.depth==rhs.depth) {
		for(int i=0; i<lhs.value.size(); i++) {
			if(lhs[i]!=rhs[i]) {
				return false;
			}
		}
	} else {
		return false;
	}
	return true;
}

template <class T>
bool operator!=(pmHistory<T> const& lhs, pmHistory<T> const& rhs) {
	return !operator==(lhs,rhs);
}

#endif //_HISTORY_H_