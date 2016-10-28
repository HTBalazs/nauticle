/////////////////////////////////////////////////////////////////////////////
// Name:        pmComplex.h
// Purpose:     pmComplex class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _COMPLEX_H_
#define _COMPLEX_H_

#include "prolog/pLogger.h"

/** This class implements a complex number.
*/
class pmComplex {
	float real;
	float imag;
public:
	pmComplex() : real{0}, imag{0} {}
	pmComplex(float const& re, float const& im) : real{re}, imag{im} {}
	pmComplex(float const& re) : real{re}, imag{0} {}
	pmComplex(pmComplex const& other);
	pmComplex& operator=(pmComplex const& other);
	pmComplex& operator+=(pmComplex const& other);
	pmComplex& operator-=(pmComplex const& other);
	pmComplex& operator*=(pmComplex const& other);
	pmComplex& operator/=(pmComplex const& other);
	float Re() const;
	float Im() const;
	void print() const;
	bool is_complex() const;
	pmComplex conjugate() const;
	friend pmComplex operator+(pmComplex const& lhs, pmComplex const& rhs);
	friend pmComplex operator-(pmComplex const& lhs, pmComplex const& rhs);
	friend pmComplex operator*(pmComplex const& lhs, pmComplex const& rhs);
	friend pmComplex operator/(pmComplex const& lhs, pmComplex const& rhs);
	friend bool operator==(pmComplex const& lhs, pmComplex const& rhs);
};

#endif //_COMPLEX_H_