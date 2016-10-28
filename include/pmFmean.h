/////////////////////////////////////////////////////////////////////////////
// Name:        pmFmean.h
// Purpose:     pmFmean class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _FMEAN_H_
#define _FMEAN_H_

#include "pmInteraction.h"

class pmFmean : public pmInteraction<1> {
	std::shared_ptr<pmExpression> clone_impl() const;
public:
	pmFmean(std::shared_ptr<pmExpression>);
	pmFmean(pmFmean const& other);
	pmFmean(pmFmean&& other);
	pmFmean& operator=(pmFmean const& other);
	pmFmean& operator=(pmFmean&& other);
	std::shared_ptr<pmFmean> clone() const;
	void print() const;
	pmTensor evaluate(int const& i, Eval_type eval_type/*=current*/) const;
	int get_field_size() const override;
	bool is_assigned() const override;
	virtual void write_to_string(std::ostream& os) const override;
};

inline std::ostream& operator<<(std::ostream& os, pmFmean const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_FMEAN_H_