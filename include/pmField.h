/////////////////////////////////////////////////////////////////////////////
// Name:        pmField.h
// Purpose:     pmField class.
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _FIELD_H_
#define _FIELD_H_

#include <string>
#include "pmTerm.h"
#include "pmSort.h"

/** An object of this class can hold a field of scalar, vector or tensor above any particle
//  cloud. No assignment to any particle system is required but sorting is always performed when
//  the particle system sorting in the same workspace is triggered. The field optionally stores
//  a copy of the field data in the previous step. Current and previous data is managed automatically
//  when the two_step option is on.
*/
class pmField : public pmTerm {
protected:
	std::vector<pmTensor> current_value;
	std::vector<pmTensor> previous_value;
	bool two_step=false;
protected:
	virtual std::shared_ptr<pmExpression> clone_impl() const override;
public:
	pmField()=delete;
	pmField(std::string const& n, int const& size, pmTensor const& value=pmTensor{0});
	pmField(std::string const& n, std::vector<pmTensor> const& value);
	pmField(pmField const&);
	pmField(pmField&&);
	pmField& operator=(pmField const&);
	pmField& operator=(pmField&&);
	virtual ~pmField() override {}
	void printv() const override;
	pmTensor evaluate(int const& i, Eval_type=current) const override;
	virtual void set_value(pmTensor const& value, int const& i=0) override;
	pmTensor get_value(int const& i) const override;
	int get_field_size() const override;
	std::string get_type() const override;
	void double_steps(bool const& use_double_steps) override;
	void push_back(pmTensor const& obj);
	bool is_double_steps() const;
	std::shared_ptr<pmField> clone() const;
	virtual void sort_field(std::vector<int>& sorted_idx);
	virtual void set_number_of_nodes(size_t const& N);
	virtual void write_to_string(std::ostream& os) const override;
	virtual pmTensor interpolate(pmTensor const& position) const;
};

inline std::ostream& operator<<(std::ostream& os, pmField const* obj) {
	obj->write_to_string(os);
	return os;
}

#endif //_FIELD_H_