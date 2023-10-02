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

#include "pmOutput.h"

using namespace Nauticle;
using namespace ProLog;

void pmOutput::set_file_name(std::string const& fn) {
	file_name = fn;
	system(("rm -rf "+file_name).c_str());
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the content of the pmOutput object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmOutput::print() const {
	pLogger::headerf<LBL>("Output");
	pLogger::titlef<LMA>("Data");
	pLogger::logf<YEL>("        file_name: ");
	pLogger::logf<NRM>("%s\n", file_name.c_str());
	pLogger::logf<YEL>("        condition: "); condition->print(); pLogger::line_feed(1);
	pLogger::logf<YEL>("        time: "); current_time->print(); pLogger::line_feed(1);
	pLogger::footerf<LBL>();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Sets variable for the interpolation results to be stored in.
/////////////////////////////////////////////////////////////////////////////////////////
void pmOutput::add_data(std::shared_ptr<pmExpression> expr) {
	data.push_back(expr);
}

void pmOutput::add_time(std::shared_ptr<pmExpression> tm) {
	current_time = tm;
}

void pmOutput::set_condition(std::shared_ptr<pmExpression> cond) {
	condition = cond;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Reads input if not read yet, and performs interpolation.
/////////////////////////////////////////////////////////////////////////////////////////
void pmOutput::update() {
	std::ofstream datafile;
	datafile.open(file_name.c_str(), std::ios::app);
	for(auto const& it:data) {
		datafile << current_time->evaluate(0)[0] << ' ';
		pmTensor d = it->evaluate(0);
		for(int i=0; i<d.numel(); i++) {
			datafile << d[i] << " ";
		}
	}
	datafile << std::endl;
	datafile.close();
}


