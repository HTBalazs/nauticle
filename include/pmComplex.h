/*
    Copyright 2016 Balazs Toth
    This file is part of LEMPS.

    LEMPS is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LEMPS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LEMPS.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://BalazsToth@bitbucket.org/BalazsToth/lemps
*/
    
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