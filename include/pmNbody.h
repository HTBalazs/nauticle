/////////////////////////////////////////////////////////////////////////////
// Name:        pmNbody.h
// Purpose:     pmNbody class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////


#ifndef _NBODY_H_
#define _NBODY_H_

#include <iostream>
#include <memory>
#include <array>
#include "pmInteraction.h"
#include "pmTensor.h"

class pmNbody : public pmInteraction<2> {
private:
	std::shared_ptr<pmExpression> clone_impl() const;
public:
	pmNbody() {}
	pmNbody(std::array<std::shared_ptr<pmExpression>,2> op);
	pmNbody(pmNbody const& other);
	pmNbody(pmNbody&& other);
	pmNbody& operator=(pmNbody const& other);
	pmNbody& operator=(pmNbody&& other);
	virtual ~pmNbody() {}
	void print() const override;
	pmTensor process(pmTensor const& A_i, pmTensor const& A_j, float const& rho_i, float const& rho_j, float const& m_i, float const& m_j, pmTensor const& r_ji, float const& d_ji, float const& W_ij) const;
	pmTensor evaluate(int const& i, Eval_type eval_type/*=current*/) const override;
	std::shared_ptr<pmNbody> clone() const;
	virtual void write_to_string(std::ostream& os) const override;
	int get_field_size() const override;
};

#endif // _NBODY_H_