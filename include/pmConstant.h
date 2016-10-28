/////////////////////////////////////////////////////////////////////////////
// Name:        pmconstant.h
// Purpose:     Defines the constant expression.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#include <string>
#include "pmSingle.h"

/** This class implements the constant expression in the expression tree. 
//  A constant can be named or anonymous. The value of a constant cannot be changed
//  after definition so the set_value function does not do anything.
*/

class pmConstant final : public pmSingle {
	bool hidden=false;
protected:
	virtual std::shared_ptr<pmExpression> clone_impl() const override;
public:
	pmConstant()=delete;
	pmConstant(std::string n, pmTensor const& value, bool const& hh=false);
	pmConstant(pmTensor const& value);
	pmConstant(pmConstant const&)=default;
	pmConstant& operator=(pmConstant const&)=default;
	virtual ~pmConstant() override {}
	void print() const override;
	std::shared_ptr<pmConstant> clone() const;
	void set_value(pmTensor const& value, int const& i=0) override;
	bool is_hidden() const override;
	virtual void write_to_string(std::ostream& os) const override;
};

inline std::ostream& operator<<(std::ostream& os, pmConstant const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_CONSTANT_H_