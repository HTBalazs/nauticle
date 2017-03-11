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

#include "pmLog_stream.h"

using namespace pLogger;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmLog_stream::pmLog_stream() {
	timer = std::unique_ptr<pTimer>{new pTimer{}};
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the information relating to the current step.
/////////////////////////////////////////////////////////////////////////////////////////
void pmLog_stream::print_step_info(int const& steps, float const& dt, int const& substeps, int const& all_steps, float const& current_time, float const& progress) {
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
	float difference = timer->get_difference().count();
	float fps = difference==0?0:(float)substeps/difference;
	struct tm est_end = timer->get_estimated_finish(progress);
	logf<WHT>(" %5i   %1.3e      %5i      %6i   %.3f   %06.2f   %02i.%02i.%4i %02i:%02i\n", steps, (double)dt, substeps, all_steps, (double)current_time, (double)fps, est_end.tm_mday, est_end.tm_mon+1, est_end.tm_year+1900, est_end.tm_hour, est_end.tm_min);
	timer->tic();
	counter++;
	num_steps++;
	num_total_steps=all_steps;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Prints the substep information.
/////////////////////////////////////////////////////////////////////////////////////////
void pmLog_stream::print_substep_info(float const& dt, int const& substeps) const {
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
	logf<LGN>("     Number of result files      :   %i\n", num_steps+1);
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