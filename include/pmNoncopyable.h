/*
    Copyright 2017 Balazs Toth
    This file is part of LEMPS.

    LEMPS is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LEMPS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LEMPS.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://BalazsToth@bitbucket.org/BalazsToth/lemps
*/
    
#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

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

#endif //_NONCOPYABLE_H_