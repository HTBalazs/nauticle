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
#include <deque>
#include <functional>

namespace Nauticle {
	template <class T>
	class pmHistory {
		std::deque<T> value;
		size_t depth = 1;
	public:
		pmHistory();
		pmHistory(T const& val);
		pmHistory(pmHistory const& val);
		pmHistory(pmHistory&& val);
		pmHistory& operator=(pmHistory const& val);
		pmHistory& operator=(pmHistory&& val);
		void set_storage_depth(size_t const& d);
		size_t get_storage_depth() const;
		pmHistory& operator=(T const& val);
		T const& operator()(size_t const& level) const;
		T& operator()(size_t const& level);
		void initialize(T const& val);
		bool operator==(pmHistory<T> const& rhs) const;
		bool operator!=(pmHistory<T> const& rhs) const;
		typename std::deque<T>::iterator begin();
		typename std::deque<T>::iterator end();
		typename std::deque<T>::iterator cbegin() const;
		typename std::deque<T>::iterator cend() const;
		typename std::deque<T>::iterator rbegin();
		typename std::deque<T>::iterator rend();
		typename std::deque<T>::iterator crbegin() const;
		typename std::deque<T>::iterator crend() const;
	};

	using pmHTensor = pmHistory<pmTensor>;

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	pmHistory<T>::pmHistory() {
		depth = 1;
		value.emplace_front(0.0);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	pmHistory<T>::pmHistory(T const& val) {
		depth = 1;
		value.push_front(val);
	}

	template <class T>
	pmHistory<T>::pmHistory(pmHistory<T> const& other) {
		this->value = other.value;
		this->depth = other.depth;
	}
	template <class T>
	pmHistory<T>::pmHistory(pmHistory<T>&& other) {
		this->value = other.value;
		this->depth = other.depth;
	}
	template <class T>
	pmHistory<T>& pmHistory<T>::operator=(pmHistory const& rhs) {
		if(this!=&rhs) {
			this->value = rhs.value;
			this->depth = rhs.depth;
		}
		return *this;
	}
	template <class T>
	pmHistory<T>& pmHistory<T>::operator=(pmHistory&& rhs) {
		if(this!=&rhs) {
			this->value = rhs.value;
			this->depth = rhs.depth;
		}
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Sets the depth of the history.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	void pmHistory<T>::set_storage_depth(size_t const& d) {
		depth = d;
		value.resize(depth, value.back());
	}

	template <class T>
	size_t pmHistory<T>::get_storage_depth() const {
		return depth;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Overloads the = operator allowing the user to directly add a new element.
	/// It pops the last element from the queue.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	pmHistory<T>& pmHistory<T>::operator=(T const& val) {
		value.push_front(val);
		value.pop_back();
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Overloads the [] operator to access the elements of the queue.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	T const& pmHistory<T>::operator()(size_t const& level) const {
		return value[level];
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Overloads the [] operator to access the elements of the queue.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	T& pmHistory<T>::operator()(size_t const& level) {
		return value[level];
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Resets all the elements of the queue to the given value.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	void pmHistory<T>::initialize(T const& val) {
		for(auto& it:value) {
			it = val;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Overloads the == operator to check of the two given histories are the same.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	bool pmHistory<T>::operator==(pmHistory<T> const& rhs) const {
		if(this->depth==rhs.depth) {
			for(int i=0; i<this->value.size(); i++) {
				if(!((*this)(i)==rhs(i))) {
					return false;
				}
			}
		} else {
			return false;
		}
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Overloads the != operator to check of the two given histories are different.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	bool pmHistory<T>::operator!=(pmHistory<T> const& rhs) const {
		return !(*this==rhs);
	}

	template <class T>
	typename std::deque<T>::iterator pmHistory<T>::begin() {
		return value.begin();
	}

	template <class T>
	typename std::deque<T>::iterator pmHistory<T>::end() {
		return value.end();
	}

	template <class T>
	typename std::deque<T>::iterator pmHistory<T>::cbegin() const {
		return value.cbegin();
	}

	template <class T>
	typename std::deque<T>::iterator pmHistory<T>::cend() const {
		return value.cend();
	}

	template <class T>
	typename std::deque<T>::iterator pmHistory<T>::rbegin() {
		return value.rbegin();
	}

	template <class T>
	typename std::deque<T>::iterator pmHistory<T>::rend() {
		return value.rend();
	}

	template <class T>
	typename std::deque<T>::iterator pmHistory<T>::crbegin() const {
		return value.crbegin();
	}

	template <class T>
	typename std::deque<T>::iterator pmHistory<T>::crend() const {
		return value.crend();
	}

}

#endif //_HISTORY_H_