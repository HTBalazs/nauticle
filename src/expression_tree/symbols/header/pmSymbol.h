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
    
#ifndef _PM_TERM_H_
#define _PM_TERM_H_

#include "pmExpression.h"
#include "pmHistory.h"
#include "c2c/c2CPP_declaration.h"
#include <string>

namespace Nauticle {
	/** This interface forms the base for single and field expressions. It holds a name.
	*/
	class pmSymbol : public pmExpression {
	public:
#ifdef JELLYFISH
        static std::string const prefix;
#endif // JELLYFISH
	public:
		virtual ~pmSymbol() {}
		virtual std::string const& get_name() const override;
		virtual pmTensor const& get_value(int const& i=0) const=0;
		virtual void print() const override;
		virtual void printv() const=0;
		virtual void set_storage_depth(size_t const& d) override {}
		virtual int get_field_size() const override;
		std::shared_ptr<pmSymbol> clone() const;
		virtual void set_value(pmTensor const& value, int const& i=0, bool const& forced=false) {}
		bool is_assigned() const override;
		virtual bool is_hidden() const;
		virtual std::string get_type() const=0;
		virtual void set_lock(size_t const& idx, bool const& lck=true) {}
#ifdef JELLYFISH
		virtual std::string get_cpp_name() const=0;
		virtual c2c::c2CPP_declaration generate_cpp_declaration() const=0;
#endif // JELLYFISH
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of << operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmSymbol const* obj) {
		obj->write_to_string(os);
		return os;
	}
}

#endif //_PM_TERM_H_