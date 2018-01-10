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
    
#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#include <string>
#include "pmSingle.h"

namespace Nauticle {
	/** This class implements the constant expression in the expression tree. 
	//  A constant can be named or anonymous. The value of a constant cannot be changed
	//  after definition so the set_value function does not do anything.
	*/
	class pmConstant final : public pmSingle {
		bool hidden=false;
	protected:
		virtual std::shared_ptr<pmExpression> clone_impl() const override;
	public:
		pmConstant()=delete;
		pmConstant(std::string n, pmTensor const& value, bool const& hh=false);
		pmConstant(pmTensor const& value);
		pmConstant(pmConstant const&)=default;
		pmConstant& operator=(pmConstant const&)=default;
		virtual ~pmConstant() override {}
		void print() const override;
		std::shared_ptr<pmConstant> clone() const;
		bool is_hidden() const override;
		virtual void write_to_string(std::ostream& os) const override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementaton of << operator.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmConstant const* obj) {
		obj->write_to_string(os);
		return os;
	}
}

#endif //_CONSTANT_H_