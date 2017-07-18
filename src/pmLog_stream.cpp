/*
    Copyright 2016-2017 Balazs Toth
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

#include "pmLog_stream.h"

using namespace pLogger;
using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmLog_stream::pmLog_stream() {
	timer = std::unique_ptr<pTimer>{new pTimer{}};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmLog_stream::pmLog_stream(int const& num_start) : pmLog_stream() {
	num_steps = num_start;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the information relating to the current step.
/////////////////////////////////////////////////////////////////////////////////////////
void pmLog_stream::print_step_info(double const& dt, int const& substeps, double const& current_time, double const& simulated_time) {
	static int counter = 51;

	if(counter==51) {
		footerf<MAG>();
		logf<MAG>("  | ");
		logf<LMA>("N#");
		logf<MAG>(" | ");
		logf<LMA>("Time step");
		logf<MAG>(" | ");
		logf<LMA>("Substeps");
		logf<MAG>(" | ");
		logf<LMA>("All steps");
		logf<MAG>(" |  ");
		logf<LMA>("Time");
		logf<MAG>(" |  ");
		logf<LMA>("ts/s");
		logf<MAG>("  |  ");
		logf<LMA>("Estimated Finish");
		logf<MAG>(" |\n");
		footerf<MAG>();
		counter = 0;
	}
	timer->tac();
	double difference = timer->get_difference().count();
	double fps = difference==0?0:(double)substeps/difference;
	struct tm est_end = timer->get_estimated_finish((double)current_time/simulated_time*100.0);
	num_total_steps+=substeps;
	logf<WHT>(" %5i   %1.3e      %5i      %6i   %.3f   %06.2f   %02i.%02i.%4i %02i:%02i\n", num_steps, (double)dt, substeps, num_total_steps, (double)current_time, (double)fps, est_end.tm_mday, est_end.tm_mon+1, est_end.tm_year+1900, est_end.tm_hour, est_end.tm_min);
	timer->tic();
	counter++;
	num_steps++;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the substep information.
/////////////////////////////////////////////////////////////////////////////////////////
void pmLog_stream::print_substep_info(double const& dt, int const& substeps) const {
	logf<MAG>(" %5i   %1.3e\n", substeps, dt);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the calculation summary.
/////////////////////////////////////////////////////////////////////////////////////////
void pmLog_stream::print_finish(bool const& confirm) const {
	struct tm finish = timer->get_current();
	struct tm start = timer->get_start();
	footerf<MAG>();
	logf<LMA>("                         System time: %02i.%02i.%4i %02i:%02i\n", finish.tm_mday, finish.tm_mon+1, finish.tm_year+1900, finish.tm_hour, finish.tm_min);
	footerf<MAG>();
	logf<LGN>("\n  Calculation is successfully finished.\n");
	footerf<LGN>();
	logf<LGN>("   Simulation summary:\n");
	logf<LGN>("     Start                       :   %02i.%02i.%4i %02i:%02i\n", start.tm_mday, start.tm_mon+1, start.tm_year+1900, start.tm_hour, start.tm_min);
	logf<LGN>("     End                         :   %02i.%02i.%4i %02i:%02i\n", finish.tm_mday, finish.tm_mon+1, finish.tm_year+1900, finish.tm_hour, finish.tm_min);
	logf<LGN>("     Number of result files      :   %i\n", num_steps);
	logf<LGN>("     Number of simulation steps  :   %i\n", num_total_steps);
	if(warning_counter==0) logf<LGN>("     Number of warning messages  :   %i\n", warning_counter);
	else logf<YEL>("     Number of warning messages  :   %i\n", warning_counter);
	logf<LGN>("     Log file                    :   \"%s\"\n", logfile.c_str());
	footerf<LGN>();
	if(confirm) {
		finish_prompt<LGN>();
		std::cin.get();
	} else {
		logf("\n");
	}
	exit(0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the simulation start information.
/////////////////////////////////////////////////////////////////////////////////////////
void pmLog_stream::print_start() const {
	struct tm start = timer->get_start();
	logf<LGN>("   Starting calculation...\n\n");
	footerf<MAG>();
	logf<LMA>("                         System time: %02i.%02i.%4i %02i:%02i\n", start.tm_mday, start.tm_mon+1, start.tm_year+1900, start.tm_hour, start.tm_min);
}