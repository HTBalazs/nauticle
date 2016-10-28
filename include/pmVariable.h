/////////////////////////////////////////////////////////////////////////////
// Name:        pmVariable.h
// Purpose:     pmVariable class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <string>
#include "pmSingle.h"

/** This class represents a single-valued variable.
*/
class pmVariable final : public pmSingle {
protected:
	virtual std::shared_ptr<pmExpression> clone_impl() const override;
private:
	pmTensor previous_value;
public:
	pmVariable()=delete;
	pmVariable(std::string const& n, pmTensor const& v=pmTensor{0});
	pmVariable(pmVariable const&)=default;
	pmVariable& operator=(pmVariable const&)=default;
	virtual ~pmVariable() override {}
	pmTensor evaluate(int const&, Eval_type=current) const override;
	void set_value(pmTensor const& value, int const& i=0) override;
	std::shared_ptr<pmVariable> clone() const;
	virtual void write_to_string(std::ostream& os) const override;
};

inline std::ostream& operator<<(std::ostream& os, pmVariable const* obj) {														\
	obj->write_to_string(os);																					\
	return os;																									\
}


#endif //_VARIABLE_H_