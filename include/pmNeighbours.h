/////////////////////////////////////////////////////////////////////////////
// Name:        pmNeighbours.h
// Purpose:     pmNeighbours class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _NEIGHBOURS_H_
#define _NEIGHBOURS_H_

#include "pmInteraction.h"
#include "prolog/pLogger.h"

class pmNeighbours : public pmInteraction<0> {
	std::shared_ptr<pmExpression> clone_impl() const;
public:
	pmNeighbours() {}
	pmNeighbours(std::array<std::shared_ptr<pmExpression>,0> op);
	pmNeighbours(pmNeighbours const& other);
	pmNeighbours(pmNeighbours&& other);
	pmNeighbours& operator=(pmNeighbours const& other);
	pmNeighbours& operator=(pmNeighbours&& other);
	virtual ~pmNeighbours() {}
	void print() const override;
	pmTensor evaluate(int const& i, Eval_type eval_type/*=current*/) const override;
	std::shared_ptr<pmNeighbours> clone() const;
	int get_field_size() const override;
	virtual void write_to_string(std::ostream& os) const override;
};

inline std::ostream& operator<<(std::ostream& os, pmNeighbours const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif // _NEIGHBOURS_H_