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

#include "pmSort.h"
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
		pmQueue()=default;
		pmQueue(T val);
		pmQueue& operator=(T const& new_value);
		T const& operator()(size_t i=0) const;
		void clear();
		size_t size() const;
		void print() const;
	};

	template<typename T, size_t L> pmQueue<T,L>::pmQueue(T val) {
		this->operator=(val);
	}

	template<typename T, size_t L> pmQueue<T,L>& pmQueue<T,L>::operator=(T const& new_value) {
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

	template<typename T, size_t L> T const& pmQueue<T,L>::operator()(size_t i/*=0*/) const {
		return value[i];
	}
	template<typename T, size_t L> void pmQueue<T,L>::clear() {
		value.clear();
	}
	template<typename T, size_t L> size_t pmQueue<T,L>::size() const {
		return value.size();
	}
	template<typename T, size_t L> void pmQueue<T,L>::print() const {
		for(auto const& it:value) {
			std::cout << it << std::endl;
		}
	}

	template<typename T, size_t L>
	std::ostream& operator<<(std::ostream& os, pmQueue<T,L> v) {
		os << v(0);
		return os;
	}

	template<typename T, size_t L>
	class pmQueue<std::vector<T>,L> {
		std::deque<std::vector<T>> value;
	public:
		pmQueue()=default;
		pmQueue(std::vector<T> const& val);
		pmQueue& operator=(std::vector<T> const& new_value);
		std::vector<T> const& operator()(size_t i=0) const;
		T& operator[](size_t i);
		void clear();
		size_t size() const;
		void print() const;
		void reorder(std::vector<size_t> const& reorder_by);
	};

	template<typename T, size_t L> pmQueue<std::vector<T>,L>::pmQueue(std::vector<T> const& val) {
		this->operator=(val);
	}

	template<typename T, size_t L> pmQueue<std::vector<T>,L>& pmQueue<std::vector<T>,L>::operator=(std::vector<T> const& new_value) {
		if(value.empty()) {
			for(int i=0; i<L; i++) {
				value.push_back(new_value);
			}
		} else {	
			for(auto& it:value) {
				size_t old_size = it.size();
				size_t new_size = new_value.size();
				it.resize(new_value.size());
				if(old_size<new_size) {
					std::copy(new_value.begin()+old_size,new_value.end(),it.begin()+old_size);
				}
			}
			value.push_front(new_value);
			value.pop_back();
		}
		return *this;
	}

	template<typename T, size_t L> std::vector<T> const& pmQueue<std::vector<T>,L>::operator()(size_t i/*=0*/) const {
		return value[i];
	}
	template<typename T, size_t L> T& pmQueue<std::vector<T>,L>::operator[](size_t i) {
		for(int j=L-1;j>0; j--) {
			value[j][i] = value[j-1][i];
		}
		return value[0][i];
	}
	template<typename T, size_t L> void pmQueue<std::vector<T>,L>::clear() {
		value.clear();
	}
	template<typename T, size_t L> size_t pmQueue<std::vector<T>,L>::size() const {
		return value.size();
	}
	template<typename T, size_t L> void pmQueue<std::vector<T>,L>::print() const {
		for(auto const& it:value) {
			for(auto const& jt:it) {
				std::cout << jt << std::endl;
			}
		}
	}
	template<typename T, size_t	L> void pmQueue<std::vector<T>,L>::reorder(std::vector<size_t> const& reorder_by) {
		for(auto& it:value) {
			std::cout << it.size() << " " << reorder_by.size() << std::endl;
			pmSort::reorder(it, reorder_by);
		}
	}
}

#endif //_QUEUE_H_