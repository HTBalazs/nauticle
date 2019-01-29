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

#ifndef _COUNTER_H_
#define _COUNTER_H_

namespace Nauticle {
	template <typename T>
	class pmCounter {
	protected:
		static T counter;
		pmCounter();
	};

	template <typename T>
	T pmCounter<T>::counter = T{0};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Constructor
	/////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	pmCounter<T>::pmCounter() {
		counter++;
	}
}

#endif //_COUNTER_H_