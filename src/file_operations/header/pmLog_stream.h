/*
    Copyright 2016-2019 Balazs Toth
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

#ifndef _LOG_STREAM_H_
#define _LOG_STREAM_H_

#include "prolog/pLogger.h"

namespace Nauticle {
    /** This class performs logging procedures based on ProLog
    */
    class pmLog_stream {
    	std::unique_ptr<ProLog::pTimer> timer;
        int num_steps=0;
        int num_total_steps=0;
    public:
        pmLog_stream();
    	pmLog_stream(int const& num_start);
    	void print_step_info(double const& dt, int const& substeps, int const& all_steps, double const& current_time, double const& simulated_time);
    	void print_substep_info(double const& dt, int const& substeps) const;
    	void print_finish(bool const& confirm) const;
    	void print_start() const;
    };
}

#endif // _LOG_STREAM_H_