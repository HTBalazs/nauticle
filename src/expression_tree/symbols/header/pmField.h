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

#include "pmCollection.h"
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
	class pmField final : public pmCollection<pmHTensor> {
	private:
		int reflect_parallel = 1;
		int reflect_perpendicular = 1;
		bool printable = true;
	protected:
		virtual std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmField()=delete;
		pmField(std::string const& n, int const& size, pmTensor const& value=pmTensor{0}, int const& prl=1, int const& ppd=-1, bool const& pr=true);
		pmField(std::string const& n, std::vector<pmTensor> const& value, int const& prl=1, int const& ppd=-1, bool const& pr=true);
		virtual ~pmField() override = default;
		void printv() const override;
		std::shared_ptr<pmField> clone() const;
		virtual void write_to_string(std::ostream& os) const override;
		void duplicate_member(size_t const& i, pmTensor const& guide=pmTensor{}) override;
		void set_printable(bool const& p);
		virtual bool is_printable() const override;
		void set_parallel_reflection(int const& rfl);
		void set_perpendicular_reflection(int const& rfl);
		int get_parallel_reflection() const;
		int get_perpendicular_reflection() const;
		bool operator==(pmField const& rhs) const;
		bool operator!=(pmField const& rhs) const;
		void initialize(pmTensor const& v);
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