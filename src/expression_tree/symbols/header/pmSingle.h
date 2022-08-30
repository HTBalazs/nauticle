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
    
#ifndef _PM_SINGLE_H_
#define _PM_SINGLE_H_

#include "pmSymbol.h"
#include <vector>
#include <string>

namespace Nauticle {
    /** This inteface forms the base for expressions of constant and variables.
    */
    class pmSingle : public pmSymbol {
    protected:
    	pmHistory<pmTensor> value;
    public:
    	virtual ~pmSingle() override {}
    	pmTensor const& get_value(int const& i=0) const override;
    	virtual pmTensor evaluate(int const&, size_t const& level=0) const override;
    	void printv() const override;
    	std::shared_ptr<pmSingle> clone() const;
    	std::string get_type() const override;
        virtual size_t get_storage_depth() const override;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Implementation of << operator.
    /////////////////////////////////////////////////////////////////////////////////////////
    inline std::ostream& operator<<(std::ostream& os, pmSingle const* obj) {
    	obj->write_to_string(os);
    	return os;
    }
}

#endif //_PM_SINGLE_H_