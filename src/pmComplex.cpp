/////////////////////////////////////////////////////////////////////////////
// Name:        pmComplex.cpp
// Purpose:     pmComplex class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

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

