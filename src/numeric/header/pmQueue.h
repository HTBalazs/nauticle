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

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <deque>
#include <vector>
#include <iostream>
#include <type_traits>
#include <algorithm>

namespace Nauticle {
	template<typename T, size_t L=1>
	class pmQueue {
		std::deque<T> value;
	public:
		pmQueue& operator=(T const& new_value);
		T const& operator()(size_t i=0) const;
		void clear();
		size_t size() const;
		void print() const;
	};

	template<typename T> pmQueue<T>& pmQueue<T>::operator=(T const& new_value) {
		if(value.empty()) {
			for(int i=0; i<L; i++) {
				value.push_back(new_value);
			}
		} else {
			value.push_front(new_value);
			value.pop_back();
		}
		return *this;
	}
	template<> pmQueue<std::vector<double>>& pmQueue<std::vector<double>>::operator=(std::vector<double> const& new_value) {
		for(auto& it:value) {
			size_t old_size = it.size();
			size_t new_size = new_value.size();
			it.resize(new_value.size());
			if(old_size<new_size) {
				std::copy(new_value.begin()+old_size,new_value.end(),it.begin()+old_size);
			}
		}
		value.push_front(new_value);
		if(value.size()>L) {
			value.pop_back();
		}
		return *this;
	}
	template<typename T> T const& pmQueue<T>::operator()(size_t i/*=0*/) const {
		if(i<this->size()) {
			return value[i];
		} else {
			return value.back();
		}
	}
	template<typename T> void pmQueue<T>::clear() {
		value.clear();
	}
	template<typename T> size_t pmQueue<T>::size() const {
		return value.size();
	}
	template<typename T> void pmQueue<T>::print() const {
		for(auto const& it:value) {
			std::cout << it << std::endl;
		}
	}
	template<> void pmQueue<std::vector<double>>::print() const {
		for(auto const& it:value) {
			for(auto const& jt:it) {
				std::cout << jt << std::endl;
			}
		}
	}

	template<typename T=double>
	std::ostream& operator<<(std::ostream& os, pmQueue<T> v) {
		os << v(0);
		return os;
	}
}

#endif //_QUEUE_H_