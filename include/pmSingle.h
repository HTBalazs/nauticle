/////////////////////////////////////////////////////////////////////////////
// Name:        pmSingle.h
// Purpose:     pmSingle class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _SINGLE_H_
#define _SINGLE_H_

#include <string>
#include "pmTerm.h"

/** This inteface forms the base for expressions of constant and variables.
*/
class pmSingle : public pmTerm {
protected:
	pmTensor current_value;
public:
	virtual ~pmSingle() override {}
	pmTensor get_value(int const& i=0) const override;
	virtual pmTensor evaluate(int const&, Eval_type=current) const override;
	void printv() const override;
	std::shared_ptr<pmSingle> clone() const;
	std::string get_type() const override;
};

inline std::ostream& operator<<(std::ostream& os, pmSingle const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_SINGLE_H_