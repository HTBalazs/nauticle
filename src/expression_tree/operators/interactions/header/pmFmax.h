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
    
#ifndef _PM_FMAX_H_
#define _PM_FMAX_H_

#include "pmFsearch.h"

namespace Nauticle {
    /** This class finds the maximum value of a pmField over a pmParticle_system.
    */
    class pmFmax : public pmFsearch {
    	std::shared_ptr<pmExpression> clone_impl() const override;
    private:
    	void process(pmTensor& value, size_t const& level=0) const override;
    public:
    	pmFmax(std::shared_ptr<pmExpression>);
    	pmFmax(pmFmax const& other);
    	pmFmax(pmFmax&& other);
    	pmFmax& operator=(pmFmax const& other);
    	pmFmax& operator=(pmFmax&& other);
    	std::shared_ptr<pmFmax> clone() const;
    	void print() const override;
    	virtual void write_to_string(std::ostream& os) const override;
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Implementaton of << operator.
    /////////////////////////////////////////////////////////////////////////////////////////
    inline std::ostream& operator<<(std::ostream& os, pmFmax const* obj) {
    	obj->write_to_string(os);
    	return os;
    }
}

#endif //_PM_FMAX_H_