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

#ifndef _PM_ID_MANAGER_H_
#define _PM_ID_MANAGER_H_

#include <stack>

namespace Nauticle {
	/** This class implements a stack for integer id-s that are deleted but later could be reused.
	*/
	class pmID_manager {
		std::stack<int> deleted_ids;
	public:
		void add_id(int const& id);
		int get_last_id(int const& i);
		void reset();
	};
}

#endif //_PM_ID_MANAGER_H_