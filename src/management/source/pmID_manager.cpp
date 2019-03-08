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

#include "pmID_manager.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Stores the given id in the top of stack.
/////////////////////////////////////////////////////////////////////////////////////////
void pmID_manager::add_id(int const& id) {
	deleted_ids.push(id);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the top element of the stack. If the stack is empty, it returns i.
/////////////////////////////////////////////////////////////////////////////////////////
int pmID_manager::get_last_id(int const& i) {
	int id = i;
	if(!deleted_ids.empty()) {
		id = deleted_ids.top();
		deleted_ids.pop();
	}
	return id;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Destroys the content of the stack.
/////////////////////////////////////////////////////////////////////////////////////////
void pmID_manager::reset() {
	deleted_ids = std::stack<int>{};
}