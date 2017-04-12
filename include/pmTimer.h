/////////////////////////////////////////////////////////////////////////////
// Name:        pmTimer.h
// Purpose:     pmTimer class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _TIMER_H_
#define _TIMER_H_

#include <time.h>

/** This class manages the timer. Measures time between calls of tic() and tac().
// 	It can also estimate the finish date of the simulation.
*/
class pmTimer final {
	time_t start;
	time_t estimated_finish;
	clock_t start_tick;
	clock_t end_tick;
	bool ticked=false;
public:
	pmTimer();
	~pmTimer()=default;
	void tic();
	void tac();
	double get_difference() const;
	struct tm get_start() const;
	struct tm get_estimated_finish(double const& progress);
	struct tm get_current() const;
};

#endif //_TIMER_H_