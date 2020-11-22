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
    
#ifndef _COLLECTION_H_
#define _COLLECTION_H_

#include "pmSymbol.h"
#include "pmFirst_virtual_index.h"
#include <vector>
#include <memory>

namespace Nauticle {
	/** This class manages a cloud of nodes (particles) and forms a spatial domain 
	//  for the governing equations.
	//  Neighbour search and cloud/grid generation is integrated inside.
	*/
	template <class T>
	class pmCollection : public pmSymbol, public pmFirst_virtual_index {
	protected:
		std::vector<T> value;
	protected:
		// virtual std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmCollection() {}
		virtual ~pmCollection() override = default;
		// std::shared_ptr<pmCollection> clone() const;
		int get_field_size() const override;
		int get_field_total_size() const;
		virtual std::string get_type() const override;
		void set_storage_depth(size_t const& d) override;
		virtual void set_number_of_nodes(size_t const& N);
		virtual void delete_member(size_t const& i);
		virtual void delete_set(std::vector<size_t> const& indices);
		virtual void duplicate_member(size_t const& i, pmTensor const& guide=pmTensor{})=0;
		virtual void set_value(pmTensor const& v, int const& i=0) override;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		virtual void destroy_virtual_members();
	};

	template <class T>
	int pmCollection<T>::get_field_size() const {
		return pmFirst_virtual_index::first_virtual_index;
	}

	template <class T>
	int pmCollection<T>::get_field_total_size() const {
		return value.size();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the field type.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	std::string pmCollection<T>::get_type() const {
		if(value[0](0).is_scalar()) { return "SCALAR"; }
		if(value[0](0).is_vector()) { return "VECTOR"; }
		return "TENSOR";
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Set the depth of the particle system.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	void pmCollection<T>::set_storage_depth(size_t const& d) {
		for(auto& it:value) {
			it.set_storage_depth(d);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Resizes the field. The last element is copied to the empty places if N>current size.
	/// If N<current size, the elements above N are destroyed.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	void pmCollection<T>::set_number_of_nodes(size_t const& N) {
		if(N!=value.size()) {
			value.resize(N, value.back());
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Deletes the member of the field with the given index.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	void pmCollection<T>::delete_member(size_t const& i) {
		value[i] = value.back();
		value.pop_back();
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Deletes the set of members of the fiels listed in the given delete_indices vector.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	void pmCollection<T>::delete_set(std::vector<size_t> const& delete_indices) {
		for(auto const& it:delete_indices) {
			this->delete_member(it);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Sets the value of the ith node.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	void pmCollection<T>::set_value(pmTensor const& v, int const& i/*=0*/) {
		value[i] = T{v};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Evaluates the field.
	/////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	pmTensor pmCollection<T>::evaluate(int const& i, size_t const& level/*=0*/) const {
		return value[i](level);
	}

	template <class T>
	void pmCollection<T>::destroy_virtual_members() {
		value.resize(pmFirst_virtual_index::first_virtual_index);
	}

	// /////////////////////////////////////////////////////////////////////////////////////////
	// /// Clone implementation.
	// /////////////////////////////////////////////////////////////////////////////////////////
	// template <class T>
	// std::shared_ptr<pmExpression> pmCollection<T>::clone_impl() const {
	// 	return std::make_shared<pmCollection>(*this);
	// }

	// /////////////////////////////////////////////////////////////////////////////////////////
	// /// Returns a copy of the field.
	// /////////////////////////////////////////////////////////////////////////////////////////
	// template <class T>
	// std::shared_ptr<pmCollection<T>> pmCollection<T>::clone() const {
	// 	return std::static_pointer_cast<pmCollection, pmExpression>(clone_impl());
	// }
}

#endif //_COLLECTION_H_