/////////////////////////////////////////////////////////////////////////////
// Name:        pmKernel.h
// Purpose:     pmKernel class
// Author:      Balázs Tóth
// Modified by:
// Created:     10/07/2016
// Copyright:   (c) LEMPS-project
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <memory>
#include <cstdio>


/** This class contains the smoothing kernel implementations for 1, 2 and 3 dimensions.
//  An object of this class behaves as a smoohting kernel through the evaluate function.
//  The type of the kernel to use is set by set_kernel_type() member function.
//
// Kernel notations:
//  dWlnnnnn
//	12345678
//	
//	1: optional flag for derivative.
//	2: W (weight function).
//  3: can be one of the followings: p (polynomial), s (spline), e (exponential)
//	4: order of the polynomial or spline. It is zero for exponential kernel.
//  5: number of peices along the radius.
//  6: influence radius/smoothing radius.
//	7: Dimensions.
//	8: serial number.
*/
class pmKernel {
private:
	// float (pmKernel::*kernel_ptr)(float const&, float const&) const;
	typedef  float (pmKernel::*ptr)(float const&, float const&) const;
	ptr kernel_ptr;
	float Wp2220_raw(float const& q) const;
	float Wp3220_raw(float const& q) const;
	float Wp5220_raw(float const& q) const;
	float dWp2220_raw(float const& q) const;
	float dWp3220_raw(float const& q) const;
	float dWp5220_raw(float const& q) const;

	float Wp22210(float const& distance, float const& cell_size) const;
	float Wp22220(float const& distance, float const& cell_size) const;
	float Wp22230(float const& distance, float const& cell_size) const;
	float dWp22210(float const& distance, float const& cell_size) const;
	float dWp22220(float const& distance, float const& cell_size) const;
	float dWp22230(float const& distance, float const& cell_size) const;
	float Wp32210(float const& distance, float const& cell_size) const;
	float Wp32220(float const& distance, float const& cell_size) const;
	float Wp32230(float const& distance, float const& cell_size) const;
	float dWp32210(float const& distance, float const& cell_size) const;
	float dWp32220(float const& distance, float const& cell_size) const;
	float dWp32230(float const& distance, float const& cell_size) const;
	float Wp52210(float const& distance, float const& cell_size) const;
	float Wp52220(float const& distance, float const& cell_size) const;
	float Wp52230(float const& distance, float const& cell_size) const;
	float dWp52210(float const& distance, float const& cell_size) const;
	float dWp52220(float const& distance, float const& cell_size) const;
	float dWp52230(float const& distance, float const& cell_size) const;

public:
	enum kernel_type { Quadratic, Cubic, Wendland, Gaussian };
	pmKernel();
	pmKernel(pmKernel const& other);
	pmKernel& operator=(pmKernel const& other);
	void set_kernel_type(size_t const& i, bool const& derivative);
	float evaluate(float const& distance, float const& cell_size) const;
};

#endif //_KERNEL_H_