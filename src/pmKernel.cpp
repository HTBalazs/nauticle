/*
    Copyright 2017 Balazs Toth
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

    For more information please visit: https://bitbucket.org/lempsproject/
*/

#include <iostream>
#include "pmKernel.h"

#define constant_pi 3.14159265359f

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
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Evaluates kernel.
/////////////////////////////////////////////////////////////////////////////////////////
float pmKernel::evaluate(float const& distance, float const& cell_size) const {
	return (this->*kernel_ptr)(distance, cell_size);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
float pmKernel::Wp2220_raw(float const& q) const {
	return (3.0f/16.0f*q*q-0.75f*q+0.75f);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
float pmKernel::Wp3220_raw(float const& q) const {
	if(q<1) { return 1-3/2*q*q+3/4*q*q*q; }
	else { return 0.25*(2-q)*(2-q)*(2-q); }
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
float pmKernel::Wp5220_raw(float const& q) const {
	float val = (1-q/2);
	val*=val; val*=val;
	return val*(2*q+1);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
float pmKernel::dWp2220_raw(float const& q) const {
	return (q/2-1);
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
float pmKernel::dWp3220_raw(float const& q) const {
	if(q<1) { return (-4*q+3*q*q); }
	else { return -(2-q)*(2-q); }
}
/////////////////////////////////////////////////////////////////////////////////////////
/// Returns the value of kernel function without normalization coefficient.
/////////////////////////////////////////////////////////////////////////////////////////
float pmKernel::dWp5220_raw(float const& q) const {
	float val = (1-q/2);
	return val*val*val*q;
}




float pmKernel::Wp22210(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 1.0f/h;
	return coeff*Wp2220_raw(q);
}
float pmKernel::Wp22220(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 2.0f/constant_pi/h/h;
	return coeff*Wp2220_raw(q);
}
float pmKernel::Wp22230(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 5.0f/4.0f/constant_pi/h/h/h;
	return coeff*Wp2220_raw(q);
}

float pmKernel::dWp22210(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 3.0f/4.0f/h/h;
	return coeff*dWp2220_raw(q);
}
float pmKernel::dWp22220(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 6.0f/4.0f/constant_pi/h/h/h;
	return coeff*dWp2220_raw(q);
}
float pmKernel::dWp22230(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 15.0f/16.0f/constant_pi/h/h/h/h;
	return coeff*dWp2220_raw(q);
}


float pmKernel::Wp32210(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 2.0f/3.0f/h;
	return coeff*Wp3220_raw(q);
}
float pmKernel::Wp32220(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 10.0f/7.0f/constant_pi/h/h;
	return coeff*Wp3220_raw(q);
}
float pmKernel::Wp32230(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 1.0f/constant_pi/h/h/h;
	return coeff*Wp3220_raw(q);
}

float pmKernel::dWp32210(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 6.0f/12.0f/h/h;
	return coeff*dWp3220_raw(q);
}
float pmKernel::dWp32220(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 30.0f/28.0f/constant_pi/h/h/h;
	return coeff*dWp3220_raw(q);
}
float pmKernel::dWp32230(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 3.0f/4.0f/constant_pi/h/h/h/h;
	return coeff*dWp3220_raw(q);
}


float pmKernel::Wp52210(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 3.0f/4.0f/h;
	return coeff*Wp5220_raw(q);
}
float pmKernel::Wp52220(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 7.0f/4.0f/constant_pi/h/h;
	return coeff*Wp5220_raw(q);
}
float pmKernel::Wp52230(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = 21.0f/16.0f/constant_pi/h/h/h;
	return coeff*Wp5220_raw(q);
}


float pmKernel::dWp52210(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = -30/8/h/h;
	return coeff*dWp5220_raw(q);
}
float pmKernel::dWp52220(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = -70/8/constant_pi/h/h/h;
	return coeff*dWp5220_raw(q);
}
float pmKernel::dWp52230(float const& distance, float const& cell_size) const {
	float h = cell_size/2;
	float q = distance/h;
	float coeff = -105/16/constant_pi/h/h/h/h;
	return coeff*dWp5220_raw(q);
}

#undef constant_pi
