/////////////////////////////////////////////////////////////////////////////
// Name:        pmTerm.h
// Purpose:     pmTerm class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _TERM_H_
#define _TERM_H_

#include <string>
#include "pmExpression.h"

/** This interface forms the base for single and field expressions. It holds a name.
*/
class pmTerm : public pmExpression {
protected:
	std::string name = "";
public:
	virtual ~pmTerm() {}
	virtual std::string get_name() const;
	virtual pmTensor get_value(int const& i=0) const=0;
	virtual void print() const override;
	virtual void printv() const=0;
	virtual void double_steps(bool const&) override {}
	virtual int get_field_size() const override;
	std::shared_ptr<pmTerm> clone() const;
	virtual void set_value(pmTensor const& value, int const& i=0)=0;
	bool is_assigned() const override;
	virtual bool is_hidden() const;
	virtual std::string get_type() const=0;
};

inline std::ostream& operator<<(std::ostream& os, pmTerm const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_TERM_H_