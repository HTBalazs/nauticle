/*
    Copyright 2016-2017 Balazs Toth
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
    
#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <string>
#include "pmSingle.h"

/** This class represents a single-valued variable.
*/
class pmVariable final : public pmSingle {
protected:
	virtual std::shared_ptr<pmExpression> clone_impl() const override;
	size_t depth = 1;
public:
	pmVariable()=delete;
	pmVariable(std::string const& n, pmTensor const& v=pmTensor{0});
	pmVariable(pmVariable const&)=default;
	pmVariable(pmVariable&&)=default;
	pmVariable& operator=(pmVariable const&)=default;
	pmVariable& operator=(pmVariable&&)=default;
	virtual ~pmVariable() override {}
	void set_storage_depth(size_t const& d) override;
	pmTensor evaluate(int const&, size_t const& level=0) const override;
	void set_value(pmTensor const& value, int const& i=0) override;
	std::shared_ptr<pmVariable> clone() const;
	virtual void write_to_string(std::ostream& os) const override;
};

/////////////////////////////////////////////////////////////////////////////////////////
/// Implementation of << operator for pmTensor.
/////////////////////////////////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& os, pmVariable const* obj) {														\
	obj->write_to_string(os);																					\
	return os;																									\
}


#endif //_VARIABLE_H_