/////////////////////////////////////////////////////////////////////////////
// Name:        pmFmax.h
// Purpose:     pmFmax class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _FMAX_H_
#define _FMAX_H_

#include "pmInteraction.h"

class pmFmax : public pmInteraction<1> {
	std::shared_ptr<pmExpression> clone_impl() const;
public:
	pmFmax(std::shared_ptr<pmExpression>);
	pmFmax(pmFmax const& other);
	pmFmax(pmFmax&& other);
	pmFmax& operator=(pmFmax const& other);
	pmFmax& operator=(pmFmax&& other);
	std::shared_ptr<pmFmax> clone() const;
	void print() const;
	pmTensor evaluate(int const& i, Eval_type eval_type/*=current*/) const;
	int get_field_size() const override;
	bool is_assigned() const override;
	virtual void write_to_string(std::ostream& os) const override;
};

inline std::ostream& operator<<(std::ostream& os, pmFmax const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_FMAX_H_