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
    
#ifndef _FIELD_H_
#define _FIELD_H_

#include "pmSymbol.h"
#include "pmSort.h"
#include "pmHistory.h"
#include <string>

namespace Nauticle {
	/** An object of this class can hold a field of scalar, vector or tensor above any particle
	//  cloud. No assignment to any particle system is required but sorting is always performed when
	//  the particle system sorting in the same workspace is triggered. The field optionally stores
	//  a copy of the field data in the previous step. Current and previous data is managed automatically
	//  when the two_step option is on.
	*/
	class pmField : public pmSymbol {
	private:
		std::vector<pmHTensor> value;
		bool symmetric = true;
		bool printable = true;
	protected:
		virtual std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmField() {}
		pmField(std::string const& n, int const& size, pmTensor const& value=pmTensor{0}, bool const& sym=true, bool const& pr=true);
		pmField(std::string const& n, std::vector<pmTensor> const& value, bool const& sym=true, bool const& pr=true);
		pmField(pmField const&);
		pmField(pmField&&);
		pmField& operator=(pmField const&);
		pmField& operator=(pmField&&);
		bool operator==(pmField const& rhs) const;
		bool operator!=(pmField const& rhs) const;
		virtual ~pmField() override {}
		void printv() const override;
		pmTensor evaluate(int const& i, size_t const& level=0) const override;
		virtual void set_value(pmTensor const& value, int const& i=0) override;
		virtual int get_field_size() const override;
		virtual std::string get_type() const override;
		virtual void set_storage_depth(size_t const& d) override;
		std::shared_ptr<pmField> clone() const;
		virtual void set_number_of_nodes(size_t const& N);
		virtual void write_to_string(std::ostream& os) const override;
		virtual bool is_symmetric() const;
		void set_symmetry(bool const& sym);
		virtual void delete_member(size_t const& i);
		virtual void delete_set(std::vector<size_t> const& indices);
		virtual void add_member(pmTensor const& v=pmTensor{});
		virtual void duplicate_member(size_t const& i);
		void set_printable(bool const& p);
		virtual bool is_printable() const;
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