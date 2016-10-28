/////////////////////////////////////////////////////////////////////////////
// Name:        pmDem.h
// Purpose:     pmDem class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _DEM_H_
#define _DEM_H_

#include "pmInteraction.h"
#include "prolog/pLogger.h"

/** This class implements the conventianal Discrete element method as 
//  through interactions between particles. 
*/
class pmDem : public pmInteraction<6> {
private:
	std::shared_ptr<pmExpression> clone_impl() const;
public:
	pmDem() {}
	pmDem(std::array<std::shared_ptr<pmExpression>,6> op);
	pmDem(pmDem const& other);
	pmDem(pmDem&& other);
	pmDem& operator=(pmDem const& other);
	pmDem& operator=(pmDem&& other);
	virtual ~pmDem() {}
	void print() const override;
	pmTensor evaluate(int const& i, Eval_type eval_type/*=current*/) const override;
	std::shared_ptr<pmDem> clone() const;
	virtual void write_to_string(std::ostream& os) const override;
};

inline std::ostream& operator<<(std::ostream& os, pmDem const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_DEM_H_