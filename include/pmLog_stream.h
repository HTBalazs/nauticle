#ifndef _LOG_STREAM_H_
#define _LOG_STREAM_H_

#include "prolog/pLogger.h"

class pmLog_stream {
	std::unique_ptr<pTimer> timer;
public:
	pmLog_stream();
	void print_step_info(int const& steps, float const& dt, int const& substeps, int const& all_steps, float const& current_time, float const& progress) const;
	void print_substep_info(float const& dt, int const& substeps) const;
	void print_finish(bool const& confirm) const;
	void print_start() const;
};

#endif // _LOG_STREAM_H_