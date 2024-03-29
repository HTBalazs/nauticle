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
    
#ifndef _PM_NONCOPYABLE_H_
#define _PM_NONCOPYABLE_H_

namespace Nauticle {
    /** This class has all its copy functions (constructor and assignment)
    //  deleted which makes any object of its derived classes Noncopyable.
    */
    class pmNoncopyable {
    public:
    	pmNoncopyable() {}
    	pmNoncopyable(pmNoncopyable const&)=delete;
    	pmNoncopyable& operator=(pmNoncopyable const&)=delete;
    	virtual ~pmNoncopyable() {}
    };
}

#endif //_PM_NONCOPYABLE_H_