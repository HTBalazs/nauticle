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

#include "pmScript.h"

using namespace Nauticle;
using namespace ProLog;

void pmScript::print() const {
	pLogger::headerf<LBL>("Runtime script");
	pLogger::titlef<LMA>("Script");
	pLogger::logf<YEL>("        file: ");
	pLogger::logf<NRM>("%s\n", file_name.c_str());
	pLogger::logf<YEL>("        condition: "); condition->print(); pLogger::line_feed(1);
	pLogger::footerf<LBL>();
}

void pmScript::set_file_name(std::string const& fn) {
	file_name = fn;
}

void pmScript::set_condition(std::shared_ptr<pmExpression> cnd) {
	condition = cnd;
}

void pmScript::update() {
	if(condition->evaluate(0)[0]) {
		system(("bash "+file_name).c_str());
	}
}

std::shared_ptr<pmScript> pmScript::clone() const {
    return std::make_shared<pmScript>(*this);
}
