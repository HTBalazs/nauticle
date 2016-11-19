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
    
#include "pmComplex.h"

pmComplex::pmComplex(pmComplex const& other) {
	this->real = other.real;
	this->imag = other.imag;
}
pmComplex& pmComplex::operator=(pmComplex const& other) {
	if(this!=&other) {
		this->real = other.real;
		this->imag = other.imag;
	}
	return *this;
}
pmComplex operator+(pmComplex const& lhs, pmComplex const& rhs) {
	pmComplex result;
	result.real = lhs.real+rhs.real;
	result.imag = lhs.imag+rhs.imag;
	return result;
}
pmComplex operator-(pmComplex const& lhs, pmComplex const& rhs) {
	pmComplex result;
	result.real = lhs.real-rhs.real;
	result.imag = lhs.imag-rhs.imag;
	return result;
}
pmComplex operator*(pmComplex const& lhs, pmComplex const& rhs) {
	pmComplex result;
	result.real = lhs.real*rhs.real - lhs.imag*rhs.imag;
	result.imag = lhs.real*rhs.imag + rhs.real*lhs.imag;
	return result;
}
pmComplex operator/(pmComplex const& lhs, pmComplex const& rhs) {
	pmComplex result;
	result = lhs*rhs.conjugate();
	float denominator = rhs.real*rhs.real+rhs.imag*rhs.imag;
	result.real/=denominator;
	result.imag/=denominator;
	return result;
}

pmComplex& pmComplex::operator+=(pmComplex const& other) {
	*this = *this + other;
	return *this;
}

pmComplex& pmComplex::operator-=(pmComplex const& other) {
	*this = *this - other;
	return *this;
}

pmComplex& pmComplex::operator*=(pmComplex const& other) {
	*this = *this * other;
	return *this;
}

pmComplex& pmComplex::operator/=(pmComplex const& other) {
	*this = *this / other;
	return *this;
}

bool operator==(pmComplex const& lhs, pmComplex const& rhs) {
	return lhs.real==rhs.real && lhs.imag==rhs.imag;
}

float pmComplex::Re() const {
	return real;
}
float pmComplex::Im() const {
	return imag;
}
void pmComplex::print() const {
	if(this->is_complex()) {
		pLogger::logf<COLOR>("(%g+%gi)",real,imag);
	}
	else {
		pLogger::logf<COLOR>("%g",real);
	}
}
bool pmComplex::is_complex() const {
	return imag!=0;
}
pmComplex pmComplex::conjugate() const {
	pmComplex result = *this;
	result.imag*=-1;
	return result;
}

