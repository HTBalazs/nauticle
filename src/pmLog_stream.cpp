#include "pmLog_stream.h"

using namespace pLogger;

pmLog_stream::pmLog_stream() {
	timer = std::unique_ptr<pTimer>{new pTimer{}};
}
void pmLog_stream::print_step_info(int const& steps, float const& dt, int const& substeps, int const& all_steps, float const& current_time, float const& progress) const {
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
}
void pmLog_stream::print_substep_info(float const& dt, int const& substeps) const {
	logf<MAG>(" %5i   %1.3e\n", substeps, dt);
}
void pmLog_stream::print_finish(bool const& confirm) const {
	struct tm finish = timer->get_current();
	footerf<MAG>();
	logf<LMA>("                         System time: %02i.%02i.%4i %02i:%02i\n", finish.tm_mday, finish.tm_mon+1, finish.tm_year+1900, finish.tm_hour, finish.tm_min);
	footerf<MAG>();
	logf<LGN>("\n  Calculation is successfully finished.\n");
	if(confirm) {
		finish_prompt<LGN>();
		std::cin.get();
	} else {
		logf("\n");
	}
	exit(0);
}
void pmLog_stream::print_start() const {
	struct tm start = timer->get_start();
	logf<LGN>("   Starting calculation...\n\n");
	footerf<MAG>();
	logf<LMA>("                         System time: %02i.%02i.%4i %02i:%02i\n", start.tm_mday, start.tm_mon+1, start.tm_year+1900, start.tm_hour, start.tm_min);
}