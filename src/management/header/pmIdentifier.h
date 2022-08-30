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

#ifndef _PM_IDENTIFIER_H_
#define _PM_IDENTIFIER_H_

#include "pmID_manager.h"
#include "pmSort.h"
#include <vector>
#include <numeric>

namespace Nauticle {
	/** This class implements a container for unique identifiers of any type. Deleted id-s are stored
	// and reused when a new id is added. Uniqueness of the id-s is guaranteed only if by the renumber method.
	*/
	template <typename T>
	class pmIdentifier {
		std::vector<T> value;
		pmID_manager deleted_ids;
		using iterator = typename std::vector<T>::iterator;
		using const_iterator = typename std::vector<T>::const_iterator;
	public:
		void add_id(T const& i);
		void add_id(std::vector<T> list);
		std::vector<T> const& get_ids() const;
		T operator[](T const& i);
		T const& operator[](T const& i) const;
		void reset();
		void resize(int const& size);
		void delete_id(T const& i);
		void renumber();
		void merge(pmIdentifier const& other);
		void reorder(std::vector<T> const& sorted);
		int size() const;
		iterator begin();
	    const_iterator cbegin() const;
	    iterator end();
	    const_iterator cend() const;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Adds the given object to the list of id-s if, and only if there is no former deleted id.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	void pmIdentifier<T>::add_id(T const& i) {
		value.push_back(deleted_ids.get_last_id(i));
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Adds the given vector of objects to the list of id-s. Deleted id-s are used here as well.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	void pmIdentifier<T>::add_id(std::vector<T> list) {
		if(list.empty()) { return; }
		for(auto& it:list) {
			it = deleted_ids.get_last_id(it);
		}
		value.reserve(value.size()+list.size());
		value.insert(value.end(), list.begin(), list.end());
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns theith id.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	std::vector<T> const& pmIdentifier<T>::get_ids() const {
		return value;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Definition of the [] operator. Returns the requested id by value.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	T pmIdentifier<T>::operator[](T const& i) {
		return value[i];
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Definition of the [] operator. Returns the requested id by constant reference.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	T const& pmIdentifier<T>::operator[](T const& i) const {
		return value[i];
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Clears the list of id-s and the deleted id-s as well.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	void pmIdentifier<T>::reset() {
		value.clear();
		deleted_ids.reset();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Sets the number of id-s in the container and renumbers the new list.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	void pmIdentifier<T>::resize(int const& size) {
		value.resize(size);
		renumber();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Removes the ith id from the list. The id is getting stored in the list of deleted id-s.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	void pmIdentifier<T>::delete_id(T const& i) {
		deleted_ids.add_id(i);
		value.erase(value.begin()+i);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Renumbers the list of id-s. The list of deleted id-s is purged.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	void pmIdentifier<T>::renumber() {
		std::iota(value.begin(), value.end(), T{(T)0});
		deleted_ids.reset();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Merges the given object to current one. Renumber function is also called. 
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	void pmIdentifier<T>::merge(pmIdentifier<T> const& other) {
		this->add_id(other.value);
		this->renumber();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Reorders the list of id-s by the given vector.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	void pmIdentifier<T>::reorder(std::vector<T> const& sorted) {
		pmSort::reorder(value, sorted);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Reorders the number of id-s stored in the container.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	int pmIdentifier<T>::size() const {
		return value.size();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns iterator.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	typename pmIdentifier<T>::iterator pmIdentifier<T>::begin() {
		return value.begin();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns constant iterator.
	/////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    typename pmIdentifier<T>::const_iterator pmIdentifier<T>::cbegin() const {
    	return value.cbegin();
    }

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns iterator.
	/////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    typename pmIdentifier<T>::iterator pmIdentifier<T>::end() {
    	return value.end();
    }

	/////////////////////////////////////////////////////////////////////////////////////////
    /// Returns constant iterator.
	/////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    typename pmIdentifier<T>::const_iterator pmIdentifier<T>::cend() const {
    	return value.cend();
    }
}

#endif //_PM_IDENTIFIER_H_