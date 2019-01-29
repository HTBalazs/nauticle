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

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "pmCase.h"
#include "c2c/c2Interface.h"
#include <memory>

namespace Nauticle {
    /** This class implements the interface class for the dynamic linking.
    */
    class pmInterface : public c2Interface {
    public:
        ~pmInterface() override {}
        virtual void initialize(std::shared_ptr<pmCase> c)=0;
    	virtual void solve(size_t const& num_threads)=0;
    };
}

#endif // _INTERFACE_H_