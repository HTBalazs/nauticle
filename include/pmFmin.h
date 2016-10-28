/////////////////////////////////////////////////////////////////////////////
// Name:        pmFmin.h
// Purpose:     pmFmin class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _FMIN_H_
#define _FMIN_H_

#include "pmInteraction.h"

class pmFmin : public pmInteraction<1> {
	std::shared_ptr<pmExpression> clone_impl() const;
public:
	pmFmin(std::shared_ptr<pmExpression>);
	pmFmin(pmFmin const& other);
	pmFmin(pmFmin&& other);
	pmFmin& operator=(pmFmin const& other);
	pmFmin& operator=(pmFmin&& other);
	std::shared_ptr<pmFmin> clone() const;
	void print() const;
	pmTensor evaluate(int const& i, Eval_type eval_type/*=current*/) const;
	int get_field_size() const override;
	bool is_assigned() const override;
	virtual void write_to_string(std::ostream& os) const override;
};

inline std::ostream& operator<<(std::ostream& os, pmFmin const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_FMIN_H_