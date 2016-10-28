/////////////////////////////////////////////////////////////////////////////
// Name:        pmSph.h
// Purpose:     pmSph class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _SPH_H_
#define _SPH_H_

#include <memory>
#include "pmInteraction.h"
#include "prolog/pLogger.h"
#include "pmKernel.h"

/** This class implements the conventianal Discrete element method as 
//  through interactions between particles.
*/
template <size_t S>
class pmSph : public pmInteraction<S> {
protected:
	std::shared_ptr<pmKernel> kernel;
public:
	pmSph() {}
	virtual ~pmSph() {}
};

#endif //_SPH_H_
