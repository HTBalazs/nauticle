/*
    Copyright 2016-2018 Balazs Toth
    This file is part of Nauticle.

    Nauticle is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Nauticle is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Nauticle.  If not, see <http://www.gnu.org/licenses/>.

    For more information please visit: https://bitbucket.org/nauticleproject/
*/

#include <iostream>
#include <cmath>
#include "pmKernel.h"
#include "nauticle_constants.h"

using namespace Nauticle;

/////////////////////////////////////////////////////////////////////////////////////////
/// Constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmKernel::pmKernel() {
	kernel_ptr = &pmKernel::Wp22220;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/////////////////////////////////////////////////////////////////////////////////////////
pmKernel::pmKernel(pmKernel const& other) {
	this->kernel_ptr = other.kernel_ptr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Copy assignment operator.
/////////////////////////////////////////////////////////////////////////////////////////
pmKernel& pmKernel::operator=(pmKernel const& other) {
	if(this!=&other) {
		this->kernel_ptr = other.kernel_ptr;
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Set kernel type of the object.
/////////////////////////////////////////////////////////////////////////////////////////
void pmKernel::set_kernel_type(size_t const& i, bool const& derivative) {
	if(derivative) {
		switch(i) {
			default:
			case 0: kernel_ptr = &pmKernel::dWp22210; break;
			case 1: kernel_ptr = &pmKernel::dWp22220; break;
			case 2: kernel_ptr = &pmKernel::dWp22230; break;
			case 3: kernel_ptr = &pmKernel::dWp32210; break;
			case 4: kernel_ptr = &pmKernel::dWp32220; break;
			case 5: kernel_ptr = &pmKernel::dWp32230; break;
			case 6: kernel_ptr = &pmKernel::dWp52210; break;
			case 7: kernel_ptr = &pmKernel::dWp52220; break;
			case 8: kernel_ptr = &pmKernel::dWp52230; break;
			case 9: kernel_ptr = &pmKernel::dWe21010; break;
			case 10: kernel_ptr = &pmKernel::dWe21020; break;
			case 11: kernel_ptr = &pmKernel::dWe21030; break;
		}
	} else {
		switch(i) {
			default:
			case 0: kernel_ptr = &pmKernel::Wp22210; break;
			case 1: kernel_ptr = &pmKernel::Wp22220; break;
			case 2: kernel_ptr = &pmKernel::Wp22230; break;
			case 3: kernel_ptr = &pmKernel::Wp32210; break;
			case 4: kernel_ptr = &pmKernel::Wp32220; break;
			case 5: kernel_ptr = &pmKernel::Wp32230; break;
			case 6: kernel_ptr = &pmKernel::Wp52210; break;
			case 7: kernel_ptr = &pmKernel::Wp52220; break;
			case 8: kernel_ptr = &pmKernel::Wp52230; break;
			case 9: kernel_ptr = &pmKernel::We21010; break;
			case 10: kernel_ptr = &pmKernel::We21020; break;
			case 11: kernel_ptr = &pmKernel::We21030; break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates kernel.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::evaluate(double const& distance, double const& cell_size) const {
	return (this->*kernel_ptr)(distance, cell_size);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::Wp2220_raw(double const& q) const {
	return (3.0/16.0*q*q-0.75*q+0.75);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::Wp3220_raw(double const& q) const {
	if(q<1) { return 1.0-3.0/2.0*q*q+3.0/4.0*q*q*q; }
	else { return 0.25*(2.0-q)*(2.0-q)*(2.0-q); }
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::Wp5220_raw(double const& q) const {
	double val = (1.0-q/2.0);
	val*=val; val*=val;
	return val*(2.0*q+1.0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::We2100_raw(double const& q) const {
	return exp(-q*q/4.0);
}


/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWp2220_raw(double const& q) const {
	return (q/2.0-1.0);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWp3220_raw(double const& q) const {
	if(q<1) { return (-4.0*q+3.0*q*q); }
	else { return -(2.0-q)*(2.0-q); }
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWp5220_raw(double const& q) const {
	double val = (1.0-q/2.0);
	return val*val*val*q;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWe2100_raw(double const& q) const {
	return We2100_raw(q);
}



/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::Wp22210(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 1.0/h;
	return coeff*Wp2220_raw(q);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::Wp22220(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 2.0/NAUTICLE_PI/h/h;
	return coeff*Wp2220_raw(q);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::Wp22230(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 5.0/4.0/NAUTICLE_PI/h/h/h;
	return coeff*Wp2220_raw(q);
}




/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWp22210(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 3.0/4.0/h/h;
	return coeff*dWp2220_raw(q);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWp22220(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 6.0/4.0/NAUTICLE_PI/h/h/h;
	return coeff*dWp2220_raw(q);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWp22230(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 15.0/16.0/NAUTICLE_PI/h/h/h/h;
	return coeff*dWp2220_raw(q);
}




/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::Wp32210(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 2.0/3.0/h;
	return coeff*Wp3220_raw(q);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::Wp32220(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 10.0/7.0/NAUTICLE_PI/h/h;
	return coeff*Wp3220_raw(q);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::Wp32230(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 1.0/NAUTICLE_PI/h/h/h;
	return coeff*Wp3220_raw(q);
}




/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWp32210(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 6.0/12.0/h/h;
	return coeff*dWp3220_raw(q);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWp32220(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 30.0/28.0/NAUTICLE_PI/h/h/h;
	return coeff*dWp3220_raw(q);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWp32230(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 3.0/4.0/NAUTICLE_PI/h/h/h/h;
	return coeff*dWp3220_raw(q);
}




/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::Wp52210(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 3.0/4.0/h;
	return coeff*Wp5220_raw(q);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::Wp52220(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 7.0/4.0/NAUTICLE_PI/h/h;
	return coeff*Wp5220_raw(q);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::Wp52230(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = 21.0/16.0/NAUTICLE_PI/h/h/h;
	return coeff*Wp5220_raw(q);
}




/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWp52210(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = -30.0/8.0/h/h;
	return coeff*dWp5220_raw(q);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWp52220(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = -70.0/8.0/NAUTICLE_PI/h/h/h;
	return coeff*dWp5220_raw(q);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWp52230(double const& distance, double const& cell_size) const {
	double const h = cell_size/2.0;
	double const q = distance/h;
	double const coeff = -105.0/16.0/NAUTICLE_PI/h/h/h/h;
	return coeff*dWp5220_raw(q);
}






/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::We21010(double const& distance, double const& sigma) const {
	double const coeff = 1.0/sigma/sqrt(2.0*NAUTICLE_PI);
	return coeff*We2100_raw(distance/sigma);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::We21020(double const& distance, double const& sigma) const {
	double const coeff = 1.0/2.0/NAUTICLE_PI/sigma/sigma;
	return coeff*We2100_raw(distance/sigma);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::We21030(double const& distance, double const& sigma) const {
	double const coeff = 1.0/std::pow(2.0*NAUTICLE_PI,1.5)/sigma/sigma/sigma;
	return coeff*We2100_raw(distance/sigma);
}




/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWe21010(double const& distance, double const& sigma) const {
	double const coeff = -distance/sigma/sigma/sigma/sqrt(2.0*NAUTICLE_PI);
	return coeff*dWe2100_raw(distance/sigma);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWe21020(double const& distance, double const& sigma) const {
	double const coeff = -distance/2.0/NAUTICLE_PI/sigma/sigma/sigma/sigma;
	return coeff*dWe2100_raw(distance/sigma);
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of the kernel function at the given distance.
/////////////////////////////////////////////////////////////////////////////////////////
double pmKernel::dWe21030(double const& distance, double const& sigma) const {
	double const coeff = -distance/sigma/sigma/sigma/sigma/sigma/std::pow(2.0*NAUTICLE_PI,1.5);
	return coeff*dWe2100_raw(distance/sigma);
}

#undef NAUTICLE_PI
