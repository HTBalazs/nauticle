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
    
#ifndef _FIELD_H_
#define _FIELD_H_

#include <string>
#include "pmSymbol.h"
#include "pmSort.h"

namespace Nauticle {
	/** An object of this class can hold a field of scalar, vector or tensor above any particle
	//  cloud. No assignment to any particle system is required but sorting is always performed when
	//  the particle system sorting in the same workspace is triggered. The field optionally stores
	//  a copy of the field data in the previous step. Current and previous data is managed automatically
	//  when the two_step option is on.
	*/
	class pmField : public pmSymbol {
	protected:
		std::vector<std::vector<pmTensor>> value;
		bool symmetric = true;
		size_t depth = 1;
	protected:
		virtual std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmField()=delete;
		pmField(std::string const& n, int const& size, pmTensor const& value=pmTensor{0}, bool const& sym=true);
		pmField(std::string const& n, std::vector<pmTensor> const& value, bool const& sym=true);
		pmField(pmField const&);
		pmField(pmField&&);
		pmField& operator=(pmField const&);
		pmField& operator=(pmField&&);
		virtual ~pmField() override {}
		void printv() const override;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		virtual void set_value(pmTensor const& value, int const& i=0) override;
		pmTensor const& get_value(int const& i) const override;
		int get_field_size() const override;
		std::string get_type() const override;
		void set_storage_depth(size_t const& d) override;
		void push_back(pmTensor const& obj);
		bool is_double_steps() const;
		std::shared_ptr<pmField> clone() const;
		void sort_field(std::vector<int> const& sorted_idx);
		virtual void set_number_of_nodes(size_t const& N);
		virtual void write_to_string(std::ostream& os) const override;
		bool is_symmetric() const override;
		void set_symmetry(bool const& sym);
		virtual void delete_member(size_t const& i);
		void delete_set(std::vector<size_t> const& indexes);
		virtual void add_member(pmTensor const& v=pmTensor{});
		virtual void duplicate_member(size_t const& i);
		void deleter(std::vector<pmTensor>& data, std::vector<size_t> const& delete_indices);
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmField const* obj) {
		obj->write_to_string(os);
		return os;
	}
}

#endif //_FIELD_H_