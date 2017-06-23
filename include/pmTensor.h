/*
    Copyright 2016-2017 Balazs Toth
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
    
#ifndef _TENSOR_H_
#define _TENSOR_H_

#include <iostream>
#include <cmath>
#include "prolog/pLogger.h"
#include "nauticle_constants.h"

namespace Nauticle {
	/** This class represents a 0th, 1st or 2nd order tensor of arbitrary shape.
	//  This general mathematical object helps to build equations of scalars, vectors
	//  or tensors with automatic conversions.
	*/
	class pmTensor final { 
		int rows;
		int columns;
		double* elements;
		bool scalar;
	public:
		pmTensor();
		pmTensor(double const& s);
		pmTensor(int const& r, int const& c, double const& init=0, bool const& sc=true);
		pmTensor(pmTensor const& other);
		pmTensor(pmTensor&& other);
		static pmTensor Tensor(int const& num_components);
		static pmTensor Tensor11();
		static pmTensor Tensor12();
		static pmTensor Tensor21();
		static pmTensor Tensor13();
		static pmTensor Tensor31();
		static pmTensor Tensor22();
		static pmTensor Tensor33();
		pmTensor& operator=(pmTensor const& other);
		pmTensor& operator=(pmTensor&& other);
		pmTensor& operator+=(pmTensor const& other);
		pmTensor& operator-=(pmTensor const& other);
		pmTensor& operator*=(pmTensor const& other);
		pmTensor& operator/=(pmTensor const& other);
		pmTensor& operator++();
		pmTensor& operator--();
		pmTensor operator++(int);
		pmTensor operator--(int);
		double operator()(int const& i, int const& j) const;
		double operator()(int const& i) const;
		double& operator[](int const& i);
		double const& operator[](int const& i) const;
		~pmTensor();
		int get_numrows() const;
		int get_numcols() const;
		bool is_column() const;
		bool is_row() const;
		bool is_vector() const;
		bool is_scalar() const;
		bool is_empty() const;
		bool is_zero() const;
		bool is_square() const;
		bool is_singular() const;
		double productum() const;
		double summation() const;
		int numel() const;
		pmTensor transpose() const;
		pmTensor trace() const;
		void print() const;
		pmTensor copy() const;
		pmTensor divide_term_by_term(pmTensor const& rhs) const;
		pmTensor multiply_term_by_term(pmTensor const& rhs) const;
		double norm() const;
		void fill(double const& scalar);
		pmTensor to_row() const;
		pmTensor to_column() const;
		static pmTensor make_tensor(int const& r, int const& c, double const& value);
		static pmTensor make_tensor(pmTensor const& base, double const& value);
		static pmTensor make_identity(int const& size);
		void write_to_string(std::ostream& os) const;
		pmTensor sub_tensor(int rmin, int rmax, int cmin, int cmax) const;
		pmTensor sub_tensor(int r, int c) const;
		pmTensor determinant() const;
		pmTensor adjugate() const;
		pmTensor inverse() const;
		void set_scalar(bool const& sc);
		pmTensor reflect(pmTensor const& guide) const;
		pmTensor append(int const& row, int const& col) const;
		bool is_integer() const;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of << operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmTensor const* obj) {
		obj->write_to_string(os);
		return os;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of << operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline std::ostream& operator<<(std::ostream& os, pmTensor const& obj) {
		obj.write_to_string(os);
		return os;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of + operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator+(pmTensor const& lhs, pmTensor const& rhs) {
		if(lhs.numel()==0) { return rhs; }
		if(rhs.numel()==0) { return lhs; }
		if(lhs.get_numrows()!=rhs.get_numrows() || lhs.get_numcols()!=rhs.get_numcols()) {
			pLogger::error_msgf("Unable to add tensors of different sizes or types.\n");
		}
		pmTensor tensor = pmTensor{lhs};
		for(int i=0; i<tensor.numel(); i++) {
			tensor[i] += rhs[i];
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of - operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator-(pmTensor const& lhs, pmTensor const& rhs) {
		if(lhs.numel()==0) { return rhs; }
		if(rhs.numel()==0) { return lhs; }
		if(lhs.get_numrows()!=rhs.get_numrows() || lhs.get_numcols()!=rhs.get_numcols()) {
			pLogger::error_msgf("Unable to subtract tensors of different sizes or types.\n");
		}
		pmTensor tensor = pmTensor{lhs};
		for(int i=0; i<tensor.numel(); i++) {
			tensor[i] -= rhs[i];
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of - operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator-(pmTensor const& rhs) {
		pmTensor tensor{rhs};
		for(int i=0; i<tensor.numel(); i++) {
			tensor[i] = -tensor[i];
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of + operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator+(pmTensor const& rhs) {
		return rhs;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of * operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator*(pmTensor const& lhs, double const& rhs) {
		pmTensor tensor{lhs};
		for(int i=0; i<lhs.numel(); i++) {
			tensor[i] *= rhs;
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of * operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator*(double const& lhs, pmTensor const& rhs) {
		return rhs*lhs;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of / operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator/(pmTensor const& lhs, double const& rhs) {
		pmTensor t{lhs};
		for(int i=0; i<lhs.numel(); i++) {
			t[i] = lhs[i]/rhs;
		}
		return t;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of * operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator*(pmTensor const& lhs, pmTensor const& rhs) {
		if(lhs.numel()==1 && rhs.numel()!=1) {
			return lhs[0]*rhs;
		}
		if(lhs.numel()!=1 && rhs.numel()==1) {
			return lhs*rhs[0];
		}
		if(lhs.numel()!=1 && rhs.numel()!=1) {
			if(lhs.get_numcols()!=rhs.get_numrows()) {
				pLogger::error_msgf("Unable to multiply these tensors.\n");
				return pmTensor{};
			}
		}
		int imax = lhs.get_numrows();
		int jmax = rhs.get_numcols();
		bool sc = !lhs.is_scalar() || !rhs.is_scalar() ? (imax*jmax==1?true:false) : true;
		pmTensor tensor{imax, jmax, 0, sc};
		for(int i=0; i<imax; i++) {
			for(int j=0; j<jmax; j++) {
				double sum = 0;
				for(int k=0; k<lhs.get_numcols(); k++) {
					sum += lhs(i,k)*rhs(k,j);
				}
				tensor[i*jmax+j] = sum;
			}
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of / operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator/(pmTensor const& lhs, pmTensor const& rhs) {
		if(rhs.is_scalar()) {
			return lhs/rhs[0];
		} else {
			pLogger::error_msgf("Unable to divide by a non-scalar.\n");
			return pmTensor{};
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of && operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator&&(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_scalar() || !rhs.is_scalar()) {
			pLogger::error_msgf("Logical \"and\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)(lhs[0] && rhs[0])};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of || operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator||(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_scalar() || !rhs.is_scalar()) {
			pLogger::error_msgf("Logical \"or\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)(lhs[0] || rhs[0])};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of != operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator!=(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_scalar() || !rhs.is_scalar()) {
			pLogger::error_msgf("Logical \"xor\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)(lhs[0] != rhs[0])};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of ! operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator!(pmTensor const& tensor) {
		if(!tensor.is_scalar()) {
			pLogger::error_msgf("Logical \"not\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)!tensor[0]};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of > operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator>(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_scalar() || !rhs.is_scalar()) {
			pLogger::error_msgf("Logical \"greater\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)(lhs[0] > rhs[0])};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of < operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor operator<(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_scalar() || !rhs.is_scalar()) {
			pLogger::error_msgf("Logical \"less\" works only with scalars.\n");
		}
		return pmTensor{1,1,(double)(lhs[0] < rhs[0])};
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of == operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline bool operator==(pmTensor const& lhs, pmTensor const& rhs) {
		if(lhs.numel() == rhs.numel()) {
			for(int i=0; i<lhs.numel(); i++) {
				if(lhs[i] != rhs[i]) {
					return false;
				}
			}
			return true;
		} else {
			return false;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of sin operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor sin(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::sin(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of cos operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor cos(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::cos(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of tan operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor tan(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::tan(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of cot operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor cot(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::cos(tmp[i])/std::sin(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of asin operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor asin(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::asin(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of acos operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor acos(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::acos(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of atan operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor atan(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::atan(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of acot operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor acot(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::acos(tmp[i])/std::asin(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of sinh operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor sinh(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::sinh(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of cosh operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor cosh(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::cosh(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of tanh operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor tanh(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::tanh(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of coth operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor coth(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = 1/std::tanh(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of min operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor min(pmTensor const& T1, pmTensor const& T2) {
		if(T1.get_numrows()!=T2.get_numrows() || T1.get_numcols()!=T2.get_numcols()) {
			pLogger::error_msgf("Tensor dimensions do not agree.\n");
		}
		pmTensor tensor{T1.get_numrows(), T1.get_numcols()};
		for(int i=0; i< T1.numel(); i++) {
			tensor[i] = std::min(T1[i], T2[i]);
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of max operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor max(pmTensor const& T1, pmTensor const& T2) {
		if(T1.get_numrows()!=T2.get_numrows() || T1.get_numcols()!=T2.get_numcols()) {
			pLogger::error_msgf("Tensor dimensions do not agree.\n");
		}
		pmTensor tensor{T1.get_numrows(), T1.get_numcols()};
		for(int i=0; i< T1.numel(); i++) {
			tensor[i] = std::max(T1[i], T2[i]);
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of mod operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor mod(pmTensor const& T1, pmTensor const& T2) {
		if(T1.get_numrows()!=T2.get_numrows() || T1.get_numcols()!=T2.get_numcols()) {
			pLogger::error_msgf("Tensor dimensions do not agree.\n");
		}
		pmTensor tensor{T1.get_numrows(), T1.get_numcols()};
		for(int i=0; i< T1.numel(); i++) {
			tensor[i] = (int)T1[i] % (int)T2[i];
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of abs operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor abs(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::abs(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of floor operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor floor(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::floor(tmp[i]+NAUTICLE_EPS);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of trunc operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor trunc(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::trunc(tmp[i]+NAUTICLE_EPS);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of round operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor round(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::round(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of sqrt operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor sqrt(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::sqrt(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of log operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor log(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::log(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of exp operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor exp(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = std::exp(tmp[i]);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of sgn operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor sgn(pmTensor const& tensor) {
		pmTensor tmp{tensor};
		for(int i=0; i<tmp.numel(); i++) {
			tmp[i] = (0.0 < tmp[i])-(tmp[i] < 0.0);
		}
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Implementation of pow operator for pmTensor.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor pow(pmTensor const& T1, pmTensor const& T2) {
		if(!T2.is_scalar()) {
			pLogger::error_msgf("Unable to evaluate expression with non-scalar power.");
		}
		if(T1.is_scalar()) {
			return pmTensor{(double)std::pow(T1[0],T2[0])};
		}
		int power = (int)T2[0];
		pmTensor tensor{T1};
		for(int i=0; i<power-1; i++) {
			tensor = tensor * T1;
		}
		return tensor;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the cross product of the given vectors.
	/////////////////////////////////////////////////////////////////////////////////////////
	inline pmTensor cross(pmTensor const& lhs, pmTensor const& rhs) {
		if(!lhs.is_column() || !rhs.is_column() || lhs.numel()<3 || rhs.numel()<3) {
			pLogger::error_msgf("Cross product requires [3 by 1] vectors.\n");
		}
		pmTensor tensor{3,1,0};
		tensor[0] = lhs[1]*rhs[2]-lhs[2]*rhs[1];
		tensor[1] = lhs[2]*rhs[0]-lhs[0]*rhs[2];
		tensor[2] = lhs[0]*rhs[1]-lhs[1]*rhs[0];
		return tensor;
	}
}

#endif //_TENSOR_H_
