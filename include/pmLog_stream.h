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

    For more information please visit: https://bitbucket.org/lempsproject/
*/

#ifndef _LOG_STREAM_H_
#define _LOG_STREAM_H_

#include "prolog/pLogger.h"

class pmLog_stream {
	std::unique_ptr<pTimer> timer;
    int num_steps=0;
    int num_total_steps=0;
public:
	pmLog_stream();
	void print_step_info(int const& steps, float const& dt, int const& substeps, int const& all_steps, float const& current_time, float const& progress);
	void print_substep_info(float const& dt, int const& substeps) const;
	void print_finish(bool const& confirm) const;
	void print_start() const;
};

#endif // _LOG_STREAM_H_